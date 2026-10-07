#include "keyboardmonitor.h"
#include "evdevreader.h"
#include "xkbhelper.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <xkbcommon/xkbcommon.h>

#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace {
constexpr int EV_KEY_LEFTSHIFT = 42;
constexpr int EV_KEY_RIGHTSHIFT = 54;
constexpr int EV_KEY_CAPSLOCK = 58;
constexpr int EV_KEY_RIGHTALT = 100;

// X11 keycode = evdev code + 8
constexpr xkb_keycode_t X11_CAPS = EV_KEY_CAPSLOCK + 8; // 66
constexpr xkb_keycode_t X11_RALT = EV_KEY_RIGHTALT + 8; // 108

bool isKeyboardDevice(int fd) {
  unsigned long evBits = 0;
  if (ioctl(fd, EVIOCGBIT(0, sizeof(evBits)), &evBits) < 0)
    return false;
  if (!(evBits & (1UL << EV_KEY)))
    return false;

  constexpr int N = (KEY_MAX / (8 * sizeof(unsigned long))) + 1;
  unsigned long keyBits[N] = {0};
  if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits) < 0)
    return false;

  auto hasKey = [&](int code) {
    return keyBits[code / (8 * sizeof(unsigned long))] &
           (1UL << (code % (8 * sizeof(unsigned long))));
  };
  return hasKey(KEY_A) && hasKey(KEY_Z) && hasKey(KEY_SPACE);
}
} // namespace

KeyboardMonitor::KeyboardMonitor(XkbHelper *xkb, QObject *parent)
    : QObject(parent) {
  detectKeySemantics(xkb);
  scanDevices();
}

KeyboardMonitor::~KeyboardMonitor() {
  for (auto *r : readers_)
    delete r;
}

void KeyboardMonitor::detectKeySemantics(XkbHelper *xkb) {
  if (!xkb || !xkb->isValid()) {
    qWarning()
        << "Нет XKB — считаем CapsLock=CapsLock и RAlt=AltGr по умолчанию";
    capsIsCapsLock_ = true;
    raltIsAltGr_ = true;
    return;
  }

  xkb_keysym_t capsSym = xkb->rawKeySymbol(X11_CAPS);
  capsIsCapsLock_ = (capsSym == XKB_KEY_Caps_Lock);
  qDebug() << "Caps Lock (X11 kc 66) → keysym" << capsSym
           << (capsIsCapsLock_ ? "(это действительно Caps Lock)"
                               : "(перебинжен, игнорируем)");

  xkb_keysym_t raltSym = xkb->rawKeySymbol(X11_RALT);
  raltIsAltGr_ =
      (raltSym == XKB_KEY_Alt_R || raltSym == XKB_KEY_ISO_Level3_Shift ||
       raltSym == XKB_KEY_Mode_switch);
  qDebug() << "RAlt (X11 kc 108) → keysym" << raltSym
           << (raltIsAltGr_ ? "(это AltGr)" : "(не AltGr, игнорируем)");
}

void KeyboardMonitor::scanDevices() {
  QDir dir("/dev/input");
  if (!dir.exists()) {
    qWarning() << "/dev/input не существует";
    return;
  }

  for (const QFileInfo &fi : dir.entryInfoList({"event*"}, QDir::System)) {
    QString path = fi.absoluteFilePath();

    int fd = ::open(path.toUtf8().constData(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
      qWarning() << "Нет доступа к" << path
                 << "(добавьте себя в группу 'input'?)";
      continue;
    }
    bool kb = isKeyboardDevice(fd);
    ::close(fd);
    if (!kb)
      continue;

    auto *r = new EvdevReader(path, this);
    if (!r->isOpen()) {
      delete r;
      continue;
    }

    connect(r, &EvdevReader::eventReceived, this, &KeyboardMonitor::onKeyEvent);
    readers_.append(r);
    qDebug() << "Клавиатура:" << path;
  }

  if (readers_.isEmpty())
    qWarning() << "Клавиатур не найдено";
}

void KeyboardMonitor::onKeyEvent(const struct input_event &ev) {
  bool modsChanged = false;

  switch (ev.code) {
  case EV_KEY_LEFTSHIFT:
    if (ev.value == 1 && !shiftL_) {
      shiftL_ = true;
      modsChanged = true;
    } else if (ev.value == 0 && shiftL_) {
      shiftL_ = false;
      modsChanged = true;
    }
    break;
  case EV_KEY_RIGHTSHIFT:
    if (ev.value == 1 && !shiftR_) {
      shiftR_ = true;
      modsChanged = true;
    } else if (ev.value == 0 && shiftR_) {
      shiftR_ = false;
      modsChanged = true;
    }
    break;
  case EV_KEY_RIGHTALT:
    // Обрабатываем только если в keymap RAlt действительно AltGr
    if (!raltIsAltGr_)
      return;
    if (ev.value == 1 && !altGr_) {
      altGr_ = true;
      modsChanged = true;
    } else if (ev.value == 0 && altGr_) {
      altGr_ = false;
      modsChanged = true;
    }
    break;
  case EV_KEY_CAPSLOCK:
    // Обрабатываем только если в keymap Caps Lock не перебинжен
    if (!capsIsCapsLock_)
      return;
    if (ev.value == 1) {
      state_.caps = !state_.caps;
      modsChanged = true;
    }
    break;
  default:
    return;
  }

  if (!modsChanged)
    return;

  state_.shift = shiftL_ || shiftR_;
  state_.altgr = altGr_;
  emit stateChanged(state_);
}

void KeyboardMonitor::setGroup(int g) {
  if (g != state_.group) {
    state_.group = g;
    emit stateChanged(state_);
  }
}

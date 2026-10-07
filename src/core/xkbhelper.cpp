#include "xkbhelper.h"
#include <QDebug>

#include <X11/XKBlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <xkbcommon/xkbcommon-x11.h>

XkbHelper::XkbHelper(QObject *parent) : QObject(parent) {
  display_ = XOpenDisplay(nullptr);
  if (!display_) {
    qWarning() << "Нет X-дисплея";
    return;
  }

  xcb_ = XGetXCBConnection(display_);
  if (!xcb_) {
    qWarning() << "Нет XCB соединения";
    return;
  }

  int op, ev, er, maj = 1, min = 0;
  if (!XkbQueryExtension(display_, &op, &ev, &er, &maj, &min)) {
    qWarning() << "XKB недоступен";
    return;
  }

  ctx_ = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  int32_t devId = xkb_x11_get_core_keyboard_device_id(xcb_);
  if (devId < 0) {
    qWarning() << "Нет устройства клавиатуры";
    return;
  }

  keymap_ = xkb_x11_keymap_new_from_device(ctx_, xcb_, devId,
                                           XKB_KEYMAP_COMPILE_NO_FLAGS);
  if (!keymap_) {
    qWarning() << "Не удалось получить keymap";
    return;
  }

  qstate_ = xkb_x11_state_new_from_device(keymap_, xcb_, devId);

  shiftMod_ = xkb_keymap_mod_get_index(keymap_, XKB_MOD_NAME_SHIFT);
  capsMod_ = xkb_keymap_mod_get_index(keymap_, XKB_MOD_NAME_CAPS);
  altgrMod_ = xkb_keymap_mod_get_index(keymap_, "Mod5");
  if (altgrMod_ == XKB_MOD_INVALID)
    altgrMod_ = xkb_keymap_mod_get_index(keymap_, "LevelThree");
}

XkbHelper::~XkbHelper() {
  if (qstate_)
    xkb_state_unref(qstate_);
  if (keymap_)
    xkb_keymap_unref(keymap_);
  if (ctx_)
    xkb_context_unref(ctx_);
  if (display_)
    XCloseDisplay(display_);
}

int XkbHelper::currentGroup() const {
  if (!display_)
    return 0;
  XkbStateRec s;
  if (XkbGetState(display_, XkbUseCoreKbd, &s) == Success)
    return s.group;
  return 0;
}

QString XkbHelper::symbolForKey(xkb_keycode_t kc, const ModState &mods) {
  if (!qstate_)
    return QString();

  xkb_mod_mask_t depressed = 0;
  xkb_mod_mask_t locked = 0;

  if (mods.shift && shiftMod_ != XKB_MOD_INVALID)
    depressed |= 1u << shiftMod_;
  if (mods.altgr && altgrMod_ != XKB_MOD_INVALID)
    depressed |= 1u << altgrMod_;
  if (mods.caps && capsMod_ != XKB_MOD_INVALID)
    locked |= 1u << capsMod_;

  xkb_state_update_mask(qstate_, depressed, 0, locked, 0, 0, mods.group);

  xkb_keysym_t sym = xkb_state_key_get_one_sym(qstate_, kc);
  if (sym == XKB_KEY_NoSymbol)
    return QString();

  char buf[64];
  int n = xkb_keysym_to_utf8(sym, buf, sizeof(buf));
  if (n <= 1)
    return QString();

  QString r = QString::fromUtf8(buf, n - 1);
  if (r.size() == 1 && r.at(0).unicode() < 0x20)
    return QString();
  return r;
}

QString XkbHelper::layoutName(int group) const {
  if (!keymap_)
    return "?";
  const char *name = xkb_keymap_layout_get_name(keymap_, group);
  if (name && *name)
    return QString::fromUtf8(name);
  return "?";
}

xkb_keysym_t XkbHelper::rawKeySymbol(xkb_keycode_t kc) const {
  if (!keymap_)
    return XKB_KEY_NoSymbol;
  const xkb_keysym_t *syms = nullptr;
  int n = xkb_keymap_key_get_syms_by_level(keymap_, kc, 0, 0, &syms);
  if (n > 0 && syms)
    return syms[0];
  return XKB_KEY_NoSymbol;
}

int XkbHelper::groupForLayoutName(const QString &shortName) const {
  if (!keymap_)
    return -1;
  int total = xkb_keymap_num_layouts(keymap_);
  for (int i = 0; i < total; ++i) {
    const char *name = xkb_keymap_layout_get_name(keymap_, i);
    if (name &&
        QString::fromUtf8(name).contains(shortName, Qt::CaseInsensitive))
      return i;
  }
  return -1;
}

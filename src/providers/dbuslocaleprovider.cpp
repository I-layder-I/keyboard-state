#include "dbuslocaleprovider.h"
#include "xkbhelper.h"
#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QDebug>

DbusLocaleProvider::DbusLocaleProvider(XkbHelper *xkb, QObject *parent)
    : ILayoutProvider(parent), xkb_(xkb) {}

DbusLocaleProvider::~DbusLocaleProvider() { stop(); }

bool DbusLocaleProvider::isAvailable() const {
  QDBusConnection bus = QDBusConnection::systemBus();
  if (!bus.isConnected())
    return false;
  // Проверяем, зарегистрирован ли сервис на шине
  return bus.interface()->isServiceRegistered("org.freedesktop.locale1");
}

void DbusLocaleProvider::start() {
  iface_ = new QDBusInterface(
      "org.freedesktop.locale1", "/org/freedesktop/locale1",
      "org.freedesktop.locale1", QDBusConnection::systemBus(), this);

  if (!iface_->isValid()) {
    qWarning() << "D-Bus locale1 interface not valid";
    delete iface_;
    iface_ = nullptr;
    return;
  }

  // Подписка на изменения свойств (сигнал PropertiesChanged)
  QDBusConnection::systemBus().connect(
      "org.freedesktop.locale1", "/org/freedesktop/locale1",
      "org.freedesktop.DBus.Properties", "PropertiesChanged", this,
      SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

  updateGroup();
}

void DbusLocaleProvider::stop() {
  if (iface_) {
    delete iface_;
    iface_ = nullptr;
  }
}

void DbusLocaleProvider::onPropertiesChanged(const QString &iface,
                                             const QVariantMap &changed,
                                             const QStringList &invalidated) {
  Q_UNUSED(iface);
  Q_UNUSED(invalidated);
  if (changed.contains("X11Layout")) {
    updateGroup();
  }
}

void DbusLocaleProvider::updateGroup() {
  if (!iface_ || !iface_->isValid() || !xkb_)
    return;

  QString layoutStr = iface_->property("X11Layout").toString();
  QStringList layouts = layoutStr.split(',', Qt::SkipEmptyParts);
  if (layouts.isEmpty())
    return;

  // Пробуем найти индекс по каждой раскладке из списка
  int group = -1;
  for (const QString &layout : layouts) {
    group = xkb_->groupForLayoutName(layout);
    if (group >= 0)
      break;
  }
  if (group < 0)
    group = 0; // fallback

  if (group != lastGroup_) {
    lastGroup_ = group;
    emit groupChanged(group);
  }
}

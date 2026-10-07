// dbuslocaleprovider.h
#pragma once
#include "ilayoutprovider.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QVariantMap>

class XkbHelper;

class DbusLocaleProvider : public ILayoutProvider {
  Q_OBJECT
public:
  explicit DbusLocaleProvider(XkbHelper *xkb, QObject *parent = nullptr);
  ~DbusLocaleProvider() override;

  bool isAvailable() const override;
  void start() override;
  void stop() override;
  QString name() const override { return "D-Bus (localed)"; }

private slots:
  void onPropertiesChanged(const QString &iface, const QVariantMap &changed,
                           const QStringList &invalidated);

private:
  void updateGroup();

  XkbHelper *xkb_ = nullptr;
  QDBusInterface *iface_ = nullptr;
  int lastGroup_ = -1;
};

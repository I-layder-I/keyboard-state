#include "layoutproviderfactory.h"
#include "dbuslocaleprovider.h"
#include "niriipc.h"
#include "xkbhelper.h"

#include <QDebug>

ILayoutProvider *LayoutProviderFactory::create(XkbHelper *xkb,
                                               QObject *parent) {
  // 1. Niri
  {
    auto *p = new NiriIPC(parent);
    if (p->isAvailable())
      return p;
    delete p;
  }

  // 2. D-Bus localed (GNOME, KDE, systemd-based)
  {
    auto *p = new DbusLocaleProvider(xkb, parent);
    if (p->isAvailable())
      return p;
    delete p;
  }

  // 3. Ничего не нашли
  return nullptr;
}

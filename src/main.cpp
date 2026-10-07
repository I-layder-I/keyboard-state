#include <QApplication>
#include <QDebug>

#include "ilayoutprovider.h"
#include "keyboardmonitor.h"
#include "keyboardwidget.h"
#include "layoutproviderfactory.h"
#include "xkbhelper.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  XkbHelper xkb;
  if (!xkb.isValid()) {
    qWarning("Не удалось инициализировать XKB");
    return 1;
  }

  KeyboardMonitor monitor(&xkb);
  if (!monitor.hasDevices())
    qWarning("Не удалось открыть /dev/input/event*.");

  KeyboardWidget w(&xkb);
  w.resize(1000, 360);
  w.setState(monitor.state());

  QObject::connect(&monitor, &KeyboardMonitor::stateChanged, &w,
                   &KeyboardWidget::setState);

  // Автовыбор провайдера раскладки
  ILayoutProvider *provider = LayoutProviderFactory::create(&xkb, &app);
  if (provider) {
    qDebug() << "Layout provider:" << provider->name();
    QObject::connect(provider, &ILayoutProvider::groupChanged, &monitor,
                     &KeyboardMonitor::setGroup);
    provider->start();
  } else {
    qWarning() << "Layout provider не найден, используем группу 0";
  }

  w.show();
  return app.exec();
}

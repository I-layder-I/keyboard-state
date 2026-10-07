#include "hyprlandprovider.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

HyprlandProvider::HyprlandProvider(QObject *parent) : ILayoutProvider(parent) {
  timer_ = new QTimer(this);
  timer_->setInterval(300);
  connect(timer_, &QTimer::timeout, this, &HyprlandProvider::poll);
}

bool HyprlandProvider::isAvailable() const {
  return !qEnvironmentVariable("HYPRLAND_INSTANCE_SIGNATURE").isEmpty();
}

void HyprlandProvider::start() {
  timer_->start();
  poll();
}

void HyprlandProvider::stop() { timer_->stop(); }

void HyprlandProvider::poll() {
  QProcess p;
  p.start("hyprctl", {"devices", "-j"});
  if (!p.waitForFinished(200))
    return;
  if (p.exitCode() != 0)
    return;

  QJsonDocument doc = QJsonDocument::fromJson(p.readAllStandardOutput());
  if (!doc.isObject())
    return;

  QJsonObject root = doc.object();
  QJsonArray keyboards = root["keyboards"].toArray();

  for (const auto &v : keyboards) {
    QJsonObject kbd = v.toObject();
    if (!kbd["main"].toBool(false))
      continue;

    int idx = kbd["active_layout_index"].toInt(-1);
    if (idx >= 0 && idx != lastIndex_) {
      lastIndex_ = idx;
      emit groupChanged(idx);
    }
    return;
  }
}

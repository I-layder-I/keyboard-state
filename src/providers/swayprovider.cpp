#include "swayprovider.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

SwayProvider::SwayProvider(QObject *parent) : ILayoutProvider(parent) {
  timer_ = new QTimer(this);
  timer_->setInterval(300);
  connect(timer_, &QTimer::timeout, this, &SwayProvider::poll);
}

bool SwayProvider::isAvailable() const {
  return !qEnvironmentVariable("SWAYSOCK").isEmpty();
}

void SwayProvider::start() {
  timer_->start();
  poll(); // сразу получить текущее состояние
}

void SwayProvider::stop() { timer_->stop(); }

void SwayProvider::poll() {
  QProcess p;
  p.start("swaymsg", {"-t", "get_inputs", "-r"});
  if (!p.waitForFinished(200))
    return;
  if (p.exitCode() != 0)
    return;

  QJsonDocument doc = QJsonDocument::fromJson(p.readAllStandardOutput());
  if (!doc.isArray())
    return;

  for (const auto &v : doc.array()) {
    QJsonObject obj = v.toObject();
    // Ищем клавиатуру с полем xkb_active_layout_index
    if (!obj.contains("xkb_active_layout_index"))
      continue;

    int idx = obj["xkb_active_layout_index"].toInt(-1);
    if (idx >= 0 && idx != lastIndex_) {
      lastIndex_ = idx;
      emit groupChanged(idx);
    }
    return; // берем первую найденную клавиатуру
  }
}

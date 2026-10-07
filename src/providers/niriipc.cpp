#include "niriipc.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

NiriIPC::NiriIPC(QObject *parent) : ILayoutProvider(parent) {
  timer_ = new QTimer(this);
  timer_->setInterval(300);
  connect(timer_, &QTimer::timeout, this, &NiriIPC::poll);
}

bool NiriIPC::isAvailable() const {
  return !qEnvironmentVariable("NIRI_SOCKET").isEmpty();
}

void NiriIPC::start() {
  timer_->start();
  poll(); // сразу получить текущее значение
}

void NiriIPC::stop() { timer_->stop(); }

void NiriIPC::poll() {
  QProcess p;
  p.start("niri", {"msg", "-j", "keyboard-layouts"});
  if (!p.waitForFinished(200))
    return;
  if (p.exitCode() != 0)
    return;

  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(p.readAllStandardOutput(), &err);
  if (err.error != QJsonParseError::NoError || !doc.isObject())
    return;

  QJsonObject obj = doc.object();
  if (!obj.contains("current_idx"))
    return;

  int idx = obj["current_idx"].toInt(-1);
  if (idx >= 0 && idx != lastIdx_) {
    lastIdx_ = idx;
    emit groupChanged(idx);
  }
}

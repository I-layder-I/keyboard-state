#pragma once
#include "ilayoutprovider.h"

#include <QProcess>
#include <QTimer>

class NiriIPC : public ILayoutProvider {
  Q_OBJECT
public:
  explicit NiriIPC(QObject *parent = nullptr);

  bool isAvailable() const override;
  void start() override;
  void stop() override;
  QString name() const override { return "Niri"; }

private slots:
  void poll();

private:
  QTimer *timer_ = nullptr;
  int lastIdx_ = -1;
};

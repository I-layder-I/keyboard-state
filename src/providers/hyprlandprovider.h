#pragma once
#include "ilayoutprovider.h"
#include <QProcess>
#include <QTimer>

class HyprlandProvider : public ILayoutProvider {
  Q_OBJECT
public:
  explicit HyprlandProvider(QObject *parent = nullptr);
  bool isAvailable() const override;
  void start() override;
  void stop() override;
  QString name() const override { return "Hyprland"; }

private slots:
  void poll();

private:
  QTimer *timer_ = nullptr;
  int lastIndex_ = -1;
};

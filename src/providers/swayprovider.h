#pragma once
#include "ilayoutprovider.h"
#include <QProcess>
#include <QTimer>

class SwayProvider : public ILayoutProvider {
  Q_OBJECT
public:
  explicit SwayProvider(QObject *parent = nullptr);
  bool isAvailable() const override;
  void start() override;
  void stop() override;
  QString name() const override { return "Sway"; }

private slots:
  void poll();

private:
  QTimer *timer_ = nullptr;
  int lastIndex_ = -1;
};

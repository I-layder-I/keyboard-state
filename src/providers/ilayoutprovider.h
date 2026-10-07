#pragma once
#include <QObject>
#include <QString>

class ILayoutProvider : public QObject {
  Q_OBJECT
public:
  explicit ILayoutProvider(QObject *parent = nullptr) : QObject(parent) {}
  virtual ~ILayoutProvider() = default;

  virtual bool isAvailable() const = 0;
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual QString name() const = 0;

signals:
  void groupChanged(int group);
};

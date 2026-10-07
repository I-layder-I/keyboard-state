#pragma once
#include <QObject>
#include <QSocketNotifier>
#include <linux/input.h>

class EvdevReader : public QObject {
  Q_OBJECT
public:
  explicit EvdevReader(const QString &path, QObject *parent = nullptr);
  ~EvdevReader();

  bool isOpen() const { return fd_ >= 0; }
  QString path() const { return path_; }

signals:
  void eventReceived(const struct input_event &ev);

private slots:
  void onReadyRead();

private:
  QString path_;
  int fd_ = -1;
  QSocketNotifier *notifier_ = nullptr;
};

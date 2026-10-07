#pragma once
#include "modstate.h"
#include <QObject>
#include <QVector>

class EvdevReader;
class XkbHelper;

class KeyboardMonitor : public QObject {
  Q_OBJECT
public:
  explicit KeyboardMonitor(XkbHelper *xkb, QObject *parent = nullptr);
  ~KeyboardMonitor();

  ModState state() const { return state_; }
  bool hasDevices() const { return !readers_.isEmpty(); }

public slots:
  void setGroup(int g);

signals:
  void stateChanged(const ModState &s);

private slots:
  void onKeyEvent(const struct input_event &ev);

private:
  void scanDevices();
  void detectKeySemantics(XkbHelper *xkb);

  QVector<EvdevReader *> readers_;
  ModState state_;

  bool shiftL_ = false;
  bool shiftR_ = false;
  bool altGr_ = false;

  // Семантика физических клавиш в текущей XKB-конфигурации
  bool capsIsCapsLock_ = false;
  bool raltIsAltGr_ = false;
};

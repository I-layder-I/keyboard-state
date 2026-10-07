#pragma once
#include "keyboardlayout.h"
#include "modstate.h"
#include <QWidget>

class XkbHelper;

class KeyboardWidget : public QWidget {
  Q_OBJECT
public:
  explicit KeyboardWidget(XkbHelper *xkb, QWidget *parent = nullptr);

public slots:
  void setState(const ModState &s);

protected:
  void paintEvent(QPaintEvent *) override;

private:
  void drawKey(QPainter &p, const QRectF &r, const KeyDef &k);

  XkbHelper *xkb_;
  Layout layout_;
  ModState mods_;
};

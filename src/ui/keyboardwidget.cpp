#include "keyboardwidget.h"
#include "xkbhelper.h"
#include <QPainter>

KeyboardWidget::KeyboardWidget(XkbHelper *xkb, QWidget *parent)
    : QWidget(parent), xkb_(xkb), layout_(makeLayout()) {
  setMinimumSize(900, 320);
  setWindowTitle("Раскладка клавиатуры");
}

void KeyboardWidget::setState(const ModState &s) {
  mods_ = s;
  update();
}

void KeyboardWidget::drawKey(QPainter &p, const QRectF &r, const KeyDef &k) {
  bool active = false;
  if (k.highlight == KeyDef::ShiftMod)
    active = mods_.shift;
  if (k.highlight == KeyDef::CapsMod)
    active = mods_.caps;
  if (k.highlight == KeyDef::AltGrMod)
    active = mods_.altgr;

  QColor bg = active ? QColor(90, 140, 220) : QColor(60, 60, 64);
  p.setBrush(bg);
  p.setPen(QPen(QColor(25, 25, 28), 1));
  p.drawRoundedRect(r, 6, 6);

  QString text;
  if (k.kc != 0 && xkb_) {
    text = xkb_->symbolForKey(k.kc, mods_);
    if (!text.isEmpty() && text.trimmed().isEmpty())
      text.clear();
  }
  if (text.isEmpty())
    text = k.label;

  QFont f = p.font();
  double px = qBound(8.0, r.height() * 0.42, 20.0);
  f.setPixelSize(int(px));
  f.setBold(active);
  p.setFont(f);
  p.setPen(Qt::white);
  p.drawText(r, Qt::AlignCenter, text);
}

void KeyboardWidget::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  p.fillRect(rect(), QColor(28, 28, 32));

  QString layout = xkb_ ? xkb_->layoutName(mods_.group) : "?";
  QFont lf = p.font();
  lf.setPixelSize(14);
  lf.setBold(true);
  p.setFont(lf);
  p.setPen(QColor(180, 180, 200));
  p.drawText(QRectF(0, 4, width(), 20), Qt::AlignCenter,
             QString("Layout: %1   Shift: %2   Caps: %3   AltGr: %4")
                 .arg(layout)
                 .arg(mods_.shift ? "on" : "off")
                 .arg(mods_.caps ? "on" : "off")
                 .arg(mods_.altgr ? "on" : "off"));

  const double topPad = 30.0;
  const double margin = 8.0;
  const double spacing = 4.0;
  const double totalUnits = 15.0;

  double availW = width() - 2 * margin;
  double availH = height() - topPad - margin;

  double unitW = availW / totalUnits;
  double unitH = (availH - spacing * (layout_.size() - 1)) / layout_.size();
  double u = qMin(unitW, unitH);

  // Прижимаем клавиатуру к верху, а не центрируем
  double y = topPad + 4.0;

  for (const Row &row : layout_) {
    double x = margin;
    for (const KeyDef &k : row) {
      QRectF r(x, y, u * k.width - spacing, u - spacing);
      drawKey(p, r, k);
      x += u * k.width;
    }
    y += u + spacing;
  }
}

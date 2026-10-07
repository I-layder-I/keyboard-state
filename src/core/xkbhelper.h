#pragma once
#include "modstate.h"
#include <QObject>
#include <QString>
#include <xkbcommon/xkbcommon.h>

struct xkb_context;
struct xkb_keymap;
struct xkb_state;
struct xcb_connection_t;
struct _XDisplay;
typedef struct _XDisplay Display;

class XkbHelper : public QObject {
  Q_OBJECT
public:
  explicit XkbHelper(QObject *parent = nullptr);
  ~XkbHelper();

  bool isValid() const { return keymap_ != nullptr; }

  int groupForLayoutName(const QString &shortName) const;

  int currentGroup() const;
  QString symbolForKey(xkb_keycode_t kc, const ModState &mods);
  QString layoutName(int group) const;

  // Keysym «как есть», без учёта модификаторов (group=0, level=0)
  xkb_keysym_t rawKeySymbol(xkb_keycode_t kc) const;

private:
  Display *display_ = nullptr;
  xcb_connection_t *xcb_ = nullptr;
  xkb_context *ctx_ = nullptr;
  xkb_keymap *keymap_ = nullptr;
  xkb_state *qstate_ = nullptr;

  xkb_mod_index_t shiftMod_ = XKB_MOD_INVALID;
  xkb_mod_index_t capsMod_ = XKB_MOD_INVALID;
  xkb_mod_index_t altgrMod_ = XKB_MOD_INVALID;
};

#pragma once

struct ModState {
  bool shift = false;
  bool caps = false;
  bool altgr = false;
  int group = 0;

  bool operator==(const ModState &o) const {
    return shift == o.shift && caps == o.caps && altgr == o.altgr &&
           group == o.group;
  }
  bool operator!=(const ModState &o) const { return !(*this == o); }
};

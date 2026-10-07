#pragma once
#include <QString>
#include <QVector>
#include <xkbcommon/xkbcommon.h>

struct KeyDef {
  enum Mod { NoMod, ShiftMod, CapsMod, AltGrMod };

  xkb_keycode_t kc;
  QString label;
  double width;
  Mod highlight;

  KeyDef() : kc(0), width(1.0), highlight(NoMod) {}

  KeyDef(xkb_keycode_t k, const QString &l, double w = 1.0, Mod h = NoMod)
      : kc(k), label(l), width(w), highlight(h) {}
};

using Row = QVector<KeyDef>;
using Layout = QVector<Row>;

// Префикс K_ гарантирует отсутствие конфликтов с именами Qt, локальными
// переменными и макросами (Q, L, S, D и т.п.).
namespace KC {
enum : xkb_keycode_t {
  K_Grave = 49,
  K_1 = 10,
  K_2 = 11,
  K_3 = 12,
  K_4 = 13,
  K_5 = 14,
  K_6 = 15,
  K_7 = 16,
  K_8 = 17,
  K_9 = 18,
  K_0 = 19,
  K_Minus = 20,
  K_Equal = 21,
  K_Backspace = 22,
  K_Tab = 23,
  K_Q = 24,
  K_W = 25,
  K_E = 26,
  K_R = 27,
  K_T = 28,
  K_Y = 29,
  K_U = 30,
  K_I = 31,
  K_O = 32,
  K_P = 33,
  K_LBracket = 34,
  K_RBracket = 35,
  K_Backslash = 51,
  K_Caps = 66,
  K_A = 38,
  K_S = 39,
  K_D = 40,
  K_F = 41,
  K_G = 42,
  K_H = 43,
  K_J = 44,
  K_K = 45,
  K_L = 46,
  K_Semicolon = 47,
  K_Apostrophe = 48,
  K_Enter = 36,
  K_LShift = 50,
  K_Z = 52,
  K_X = 53,
  K_C = 54,
  K_V = 55,
  K_B = 56,
  K_N = 57,
  K_M = 58,
  K_Comma = 59,
  K_Period = 60,
  K_Slash = 61,
  K_RShift = 62,
  K_LCtrl = 37,
  K_LSuper = 133,
  K_LAlt = 64,
  K_Space = 65,
  K_RAlt = 108,
  K_RCtrl = 105,
};
}

inline Layout makeLayout() {
  using K = KeyDef; // короткий алиас для типа

  Layout layout;

  // Ряд 1
  layout.append(
      Row{K(KC::K_Grave, "`", 1.0), K(KC::K_1, "1", 1.0), K(KC::K_2, "2", 1.0),
          K(KC::K_3, "3", 1.0), K(KC::K_4, "4", 1.0), K(KC::K_5, "5", 1.0),
          K(KC::K_6, "6", 1.0), K(KC::K_7, "7", 1.0), K(KC::K_8, "8", 1.0),
          K(KC::K_9, "9", 1.0), K(KC::K_0, "0", 1.0), K(KC::K_Minus, "-", 1.0),
          K(KC::K_Equal, "=", 1.0), K(KC::K_Backspace, "⌫", 2.0)});

  // Ряд 2
  layout.append(Row{K(KC::K_Tab, "Tab", 1.5), K(KC::K_Q, "Q"), K(KC::K_W, "W"),
                    K(KC::K_E, "E"), K(KC::K_R, "R"), K(KC::K_T, "T"),
                    K(KC::K_Y, "Y"), K(KC::K_U, "U"), K(KC::K_I, "I"),
                    K(KC::K_O, "O"), K(KC::K_P, "P"), K(KC::K_LBracket, "["),
                    K(KC::K_RBracket, "]"), K(KC::K_Backslash, "\\", 1.5)});

  // Ряд 3
  layout.append(Row{K(KC::K_Caps, "Caps", 1.75, KeyDef::CapsMod),
                    K(KC::K_A, "A"), K(KC::K_S, "S"), K(KC::K_D, "D"),
                    K(KC::K_F, "F"), K(KC::K_G, "G"), K(KC::K_H, "H"),
                    K(KC::K_J, "J"), K(KC::K_K, "K"), K(KC::K_L, "L"),
                    K(KC::K_Semicolon, ";"), K(KC::K_Apostrophe, "'"),
                    K(KC::K_Enter, "⏎", 2.25)});

  // Ряд 4
  layout.append(Row{K(KC::K_LShift, "Shift", 2.25, KeyDef::ShiftMod),
                    K(KC::K_Z, "Z"), K(KC::K_X, "X"), K(KC::K_C, "C"),
                    K(KC::K_V, "V"), K(KC::K_B, "B"), K(KC::K_N, "N"),
                    K(KC::K_M, "M"), K(KC::K_Comma, ","), K(KC::K_Period, "."),
                    K(KC::K_Slash, "/"),
                    K(KC::K_RShift, "Shift", 2.75, KeyDef::ShiftMod)});

  // Ряд 5
  layout.append(Row{K(KC::K_LCtrl, "Ctrl", 1.25),
                    K(KC::K_LSuper, "Super", 1.25), K(KC::K_LAlt, "Alt", 1.25),
                    K(KC::K_Space, "Space", 6.25),
                    K(KC::K_RAlt, "AltGr", 1.25, KeyDef::AltGrMod),
                    K(KC::K_RCtrl, "Ctrl", 1.25)});

  return layout;
}

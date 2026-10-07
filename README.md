# Keyboard State

Индикатор текущей раскладки и модификаторов клавиатуры для Linux.
Показывает виртуальную клавиатуру с символами, которые будут введены
при нажатии, с учётом **XKB-раскладки**, **Shift**, **Caps Lock** и **AltGr**.

<p align="center">
  <em>Виртуальная клавиатура обновляется в реальном времени:
  Shift переводит буквы в верхний регистр, AltGr показывает третий
  уровень раскладки, смена группы переключает весь набор символов.</em>
</p>

#### Писал иишкой, тк нужен был просмотрщик раскладки(аналог дворака но улучшеный) которую я учил, а постоянно держать открытой вкладку браузера с скрином - ну такое, так что вот. Мб потом перелопачу ручками код и сделаю нормально, а так пока юзабельно.
## Возможности

- Отслеживание **Shift**, **Caps Lock**, **AltGr** через `evdev`
  (работает независимо от того, какое окно в фокусе).
- Отслеживание активной группы XKB-раскладки через провайдеры:
  - **Niri** (`niri msg -j keyboard-layouts`)
  - **Sway** (`swaymsg -t get_inputs`)
  - **Hyprland** (`hyprctl devices -j`)
  - **D-Bus localed** (`org.freedesktop.locale1`)
- Отрисовка **полной физической клавиатуры** (ISO 105) с подсветкой
  активных модификаторов.
- Поддержка **кастомных раскладок** через `name[GroupN]` в XKB-файле.
- Правильная обработка **переопределённых клавиш** (например,
  `Caps Lock → Escape`): если физическая клавиша в XKB выдаёт не
  `Caps_Lock`, программа не будет считать её модификатором.
- Интерфейс на Qt6 без внешних зависимостей, кроме X11/XCB/XKB.

## Скриншот

```
┌───────────────────────────────────────────────────────────┐
│ Layout: Schtdvorak   Shift: on   Caps: off   AltGr: off   │
├───────────────────────────────────────────────────────────┤
│ ` 1 2 3 4 5 6 7 8 9 0 - = ⌫                                │
│ Tab  Q W E R T Y U I O P [ ] \                            │
│ Caps  A S D F G H J K L ; ' ⏎                             │
│ Shift  Z X C V B N M , . / Shift                          │
│ Ctrl Super Alt        Space        AltGr Ctrl             │
└───────────────────────────────────────────────────────────┘
```

## Зависимости

**Arch Linux:**

```bash
sudo pacman -S qt6-base libxcb libxkbcommon libxkbcommon-x11 \
               xcb-util xorg-xwayland
```

**Сборка:**

```bash
sudo pacman -S cmake gcc pkgconf
```

**Права на чтение `/dev/input/event*`:**

Пользователь должен состоять в группе `input`:

```bash
sudo usermod -aG input $USER
# затем перелогиниться
```

## Сборка и запуск

```bash
git clone https://github.com/USERNAME/keyboard-state.git
cd keyboard-state

mkdir build && cd build
cmake ..
make -j$(nproc)
./vkbd
```

## Поддерживаемые окружения

| Композитор  | Провайдер        | Зависимость                |
|-------------|------------------|----------------------------|
| Niri        | `NiriIPC`        | `niri` в `$PATH`           |
| Sway        | `SwayProvider`   | `swaymsg` в `$PATH`        |
| Hyprland    | `HyprlandProvider` | `hyprctl` в `$PATH`      |
| GNOME, KDE  | `DbusLocaleProvider` | `systemd` + D-Bus      |

Провайдер выбирается автоматически при старте по переменным окружения
(`NIRI_SOCKET`, `SWAYSOCK`, `HYPRLAND_INSTANCE_SIGNATURE`) и наличию
D-Bus-сервиса `org.freedesktop.locale1`.

## Кастомные раскладки

Если вы используете раскладку, имя которой в XKB совпадает с `us(basic)`
(например, кастомный Dvorak-вариант), задайте ей уникальное имя прямо
в файле символов:

```xkb
xkb_symbols "custom" {
    include "us(basic)"
    name[Group1] = "MyCustomLayout";   // ← добавить
    ...
};
```

Файл можно положить как в `~/.config/xkb/symbols/<name>`, так и в
системный `/usr/share/xkeyboard-config-2/symbols/<name>`.

После правки — перезапустить композитор (не просто reload config,
а именно рестарт сессии), затем проверить:

```bash
niri msg -j keyboard-layouts
# {"names":["MyCustomLayout","Russian"],"current_idx":0}
```

## Архитектура

```
src/
├── main.cpp
├── core/                       # работа с ядром и XKB
│   ├── modstate.h              # структура Shift/Caps/AltGr/group
│   ├── evdevreader.{h,cpp}     # асинхронное чтение /dev/input/event*
│   ├── keyboardmonitor.{h,cpp} # агрегация модификаторов
│   └── xkbhelper.{h,cpp}       # символы из xkb_keymap + текущая группа
├── providers/                  # источники активной раскладки
│   ├── ilayoutprovider.h       # абстрактный интерфейс
│   ├── layoutproviderfactory.{h,cpp}
│   ├── niriipc.{h,cpp}
│   ├── swayprovider.{h,cpp}
│   ├── hyprlandprovider.{h,cpp}
│   └── dbuslocaleprovider.{h,cpp}
└── ui/                         # отрисовка
    ├── keyboardlayout.h        # раскладка клавиш по физическим кодам
    └── keyboardwidget.{h,cpp}  # QWidget с paintEvent
```

**Поток данных:**

1. `EvdevReader` читает события клавиш из `/dev/input/event*`.
2. `KeyboardMonitor` отслеживает Shift/Caps/AltGr и группу раскладки.
3. Провайдер (`NiriIPC`/`SwayProvider`/...) сообщает смену группы.
4. `XkbHelper` для каждого keycode возвращает символ Unicode
   с учётом группы и модификаторов.
5. `KeyboardWidget` перерисовывает клавиатуру при каждом изменении.

## Ограничения

- **Wayland:** группа раскладки читается через IPC композитора,
  потому что Wayland не предоставляет глобального API для этого.
- **XWayland:** `XkbGetState()` в XWayland может не обновляться
  при смене раскладки — используйте провайдер своего композитора.
- **`/dev/input`:** требуется членство в группе `input`. Без этого
  модификаторы не будут отслеживаться, но UI всё равно запустится.

## Лицензия

MIT

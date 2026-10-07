cmake_minimum_required(VERSION 3.16)
project(vkbd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets DBus)
find_package(X11 REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
pkg_check_modules(XKBCOMMON_X11 REQUIRED xkbcommon-x11)

# ---------- Источники ----------
set(VKBD_SOURCES
    src/main.cpp
    src/core/evdevreader.cpp
    src/core/keyboardmonitor.cpp
    src/core/xkbhelper.cpp
    src/providers/layoutproviderfactory.cpp
    src/providers/niriipc.cpp
    src/providers/swayprovider.cpp
    src/providers/hyprlandprovider.cpp
    src/providers/dbuslocaleprovider.cpp
    src/ui/keyboardwidget.cpp)

# Заголовки с Q_OBJECT перечисляем явно — иначе AUTOMOC не сгенерирует для них
# moc-код.
set(VKBD_HEADERS
    src/providers/ilayoutprovider.h
    src/core/keyboardmonitor.h
    src/providers/niriipc.h
    src/providers/swayprovider.h
    src/providers/hyprlandprovider.h
    src/providers/dbuslocaleprovider.h
    src/ui/keyboardwidget.h)

add_executable(vkbd ${VKBD_SOURCES} ${VKBD_HEADERS})

# ---------- Пути включений ----------
target_include_directories(
  vkbd
  PRIVATE src
          src/core
          src/providers
          src/ui
          ${X11_INCLUDE_DIR}
          ${XKBCOMMON_INCLUDE_DIRS}
          ${XKBCOMMON_X11_INCLUDE_DIRS})

# ---------- Линковка ----------
target_link_libraries(
  vkbd
  PRIVATE Qt6::Widgets
          Qt6::DBus
          ${X11_LIBRARIES}
          X11-xcb
          ${XKBCOMMON_LIBRARIES}
          ${XKBCOMMON_X11_LIBRARIES}
          xcb)

cmake_minimum_required(VERSION 3.16)
project(kwin-effects-yaml)

set(KF_MIN_VERSION "6.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${ECM_MODULE_PATH}
)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    DBus
    Gui
    Widgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    KCMUtils
    WindowSystem
)

find_package(KWin REQUIRED)

find_package(KWinDBusInterface CONFIG REQUIRED)

file(WRITE "${CMAKE_BINARY_DIR}/kwin_prepaint_probe.cc"
"#include <effect/effect.h>
#include <chrono>
struct Probe : KWin::Effect {
    void prePaintScreen(KWin::ScreenPrePaintData&, std::chrono::milliseconds) override {}
};
int main() { return 0; }
")
try_compile(KWIN_PREPAINT_HAS_PRESENT_TIME
    SOURCES "${CMAKE_BINARY_DIR}/kwin_prepaint_probe.cc"
    LINK_LIBRARIES KWin::kwin Qt6::Core Qt6::Gui KF6::CoreAddons
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

# KWin 6.8 made Effect::paintWindow return bool ([[nodiscard]]); 6.6 returns void.
file(WRITE "${CMAKE_BINARY_DIR}/kwin_paintwindow_probe.cc"
"#include <effect/effect.h>
struct Probe : KWin::Effect {
    bool paintWindow(const KWin::RenderTarget&, const KWin::RenderViewport&, KWin::EffectWindow*,
        int, const KWin::Region&, KWin::WindowPaintData&) override { return true; }
};
int main() { return 0; }
")
try_compile(KWIN_PAINTWINDOW_RETURNS_BOOL
    SOURCES "${CMAKE_BINARY_DIR}/kwin_paintwindow_probe.cc"
    LINK_LIBRARIES KWin::kwin Qt6::Core Qt6::Gui KF6::CoreAddons
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)
message(STATUS "KWin paintWindow returns bool: ${KWIN_PAINTWINDOW_RETURNS_BOOL}")

add_subdirectory(src)

feature_summary(WHAT ALL)

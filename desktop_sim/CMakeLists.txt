cmake_minimum_required(VERSION 3.14)
project(cerberus_dashboard_sim C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# ─── Download LVGL v9.2.0 ─────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    lvgl
    GIT_REPOSITORY https://github.com/lvgl/lvgl.git
    GIT_TAG        v9.2.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(lvgl)

# ─── Stub out ARM-only assembly on non-ARM platforms ─────────────────────────
# LVGL custom.cmake uses GLOB_RECURSE *.S which includes ARM Helium and NEON
# assembly files. On x86/x64 these fail to assemble. We replace them with
# empty stubs (functionality is disabled anyway via LV_USE_DRAW_SW_ASM=NONE).
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|ARM|aarch64)")
    file(WRITE "${lvgl_SOURCE_DIR}/src/draw/sw/blend/helium/lv_blend_helium.S" "/* ARM Helium — disabled on non-ARM */\n")
    file(WRITE "${lvgl_SOURCE_DIR}/src/draw/sw/blend/neon/lv_blend_neon.S"    "/* ARM NEON   — disabled on non-ARM */\n")
endif()

# ─── Expose lv_conf.h to LVGL library itself (PUBLIC = propagated to all users)
# lvgl_SOURCE_DIR = .../build/_deps/lvgl-src/
# We add desktop_sim/ to its include path so it finds our lv_conf.h
target_include_directories(lvgl PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_definitions(lvgl PUBLIC
    LV_CONF_INCLUDE_SIMPLE
)

# ─── SDL2 ─────────────────────────────────────────────────────────────────────
find_package(SDL2 REQUIRED)

# ─── Source files ─────────────────────────────────────────────────────────────
add_executable(dashboard_sim
    main.cpp
    mock_data.cpp
    ../ui/dashboard_ui.c
    ../ui/font_noto_bold_48.c
    ../ui/font_noto_bold_64.c
    cerberus_logo_argb.c
)

# ─── Include paths ────────────────────────────────────────────────────────────
target_include_directories(dashboard_sim PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}          # lv_conf.h, mock_data.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../ui    # dashboard_ui.h
    ${lvgl_SOURCE_DIR}                  # enables #include "lvgl.h"
)

# ─── Link libraries ───────────────────────────────────────────────────────────
target_link_libraries(dashboard_sim PRIVATE
    lvgl                                # main LVGL library (propagates lv_conf include)
    ${SDL2_LIBRARIES}
    m                                   # math (sin, fabs, etc.)
)

# ─── Copy executable to project root for convenience ──────────────────────────
set_target_properties(dashboard_sim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

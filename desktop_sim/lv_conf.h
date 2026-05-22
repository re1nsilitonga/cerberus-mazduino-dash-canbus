/**
 * lv_conf.h — LVGL v9 configuration for Cerberus Desktop Simulator
 * Screen: 480x320, 32-bit color (SDL2 ARGB8888)
 */

/* clang-format off */
#if 1 /* Set to "1" to enable — DO NOT REMOVE this guard */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/
/* Color depth: 8, 16, 24, 32 */
#define LV_COLOR_DEPTH 32

/*=========================
   MEMORY SETTINGS
 *=========================*/
/* Size of LVGL heap in bytes. Increase if you see "out of memory" warnings. */
#define LV_MEM_SIZE (512 * 1024U)  /* 512 KB */

/* Use custom malloc/free. 0 = use built-in lv_mem */
#define LV_MEM_CUSTOM 0

/* Enable float support in lv_label_set_text_fmt("%f", ...) */
#define LV_USE_FLOAT 1

/*====================
   HAL SETTINGS
 *====================*/
/* Default display refresh period in milliseconds */
#define LV_DEF_REFR_PERIOD 16   /* ~60 fps */

/* Default Dots Per Inch */
#define LV_DPI_DEF 130

/*=====================
 * DRAW CONFIGURATION
 *====================*/
#define LV_DRAW_BUF_STRIDE_ALIGN 1
#define LV_DRAW_BUF_ALIGN        4

/* Number of default gradient stops */
#define LV_GRADIENT_MAX_STOPS 4

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/
/* Enable assertions — disable for release */
#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ           0

/*==================
 * LOGGING
 *==================*/
#define LV_USE_LOG 1
#if LV_USE_LOG
    /* Minimum log level: LV_LOG_LEVEL_TRACE/INFO/WARN/ERROR/USER/NONE */
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
    /* 1: use printf for logging */
    #define LV_LOG_PRINTF 1
    #define LV_LOG_USE_TIMESTAMP 0
    #define LV_LOG_USE_FILE_LINE 1
#endif

/*==================
 * FONTS
 *==================*/
/* Montserrat fonts — used in dashboard_ui.c */
#define LV_FONT_MONTSERRAT_8   0
#define LV_FONT_MONTSERRAT_10  0
#define LV_FONT_MONTSERRAT_12  1
#define LV_FONT_MONTSERRAT_14  1
#define LV_FONT_MONTSERRAT_16  1
#define LV_FONT_MONTSERRAT_18  0
#define LV_FONT_MONTSERRAT_20  1
#define LV_FONT_MONTSERRAT_22  0
#define LV_FONT_MONTSERRAT_24  1
#define LV_FONT_MONTSERRAT_26  0
#define LV_FONT_MONTSERRAT_28  0
#define LV_FONT_MONTSERRAT_30  0
#define LV_FONT_MONTSERRAT_32  1
#define LV_FONT_MONTSERRAT_34  0
#define LV_FONT_MONTSERRAT_36  0
#define LV_FONT_MONTSERRAT_38  0
#define LV_FONT_MONTSERRAT_40  0
#define LV_FONT_MONTSERRAT_42  0
#define LV_FONT_MONTSERRAT_44  0
#define LV_FONT_MONTSERRAT_46  0
#define LV_FONT_MONTSERRAT_48  1

/* Default font — used when no other font is specified */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* Enable built-in symbol font (icons) */
#define LV_FONT_UNSCII_8  0
#define LV_FONT_UNSCII_16 0

/* Allow fmt string in font names (e.g. lv_font_fmt_txt) */
#define LV_FONT_FMT_TXT_LARGE 0
#define LV_USE_FONT_SUBPX      0

/*===============================
 * ENABLE REQUIRED WIDGETS
 *==============================*/
#define LV_USE_ARC        1
#define LV_USE_BAR        1
#define LV_USE_BTN        1
#define LV_USE_BTNMATRIX  0
#define LV_USE_CALENDAR   0
#define LV_USE_CANVAS     0
#define LV_USE_CHART      0
#define LV_USE_CHECKBOX   0
#define LV_USE_COLORWHEEL 0
#define LV_USE_DROPDOWN   0
#define LV_USE_IMG        1
#define LV_USE_IMGBTN     0
#define LV_USE_KEYBOARD   0
#define LV_USE_LABEL      1
#define LV_USE_LED        1
#define LV_USE_LINE       1
#define LV_USE_LIST       0
#define LV_USE_MENU       0
#define LV_USE_METER      1
#define LV_USE_MSGBOX     0
#define LV_USE_ROLLER     0
#define LV_USE_SCALE      1
#define LV_USE_SLIDER     0
#define LV_USE_SPAN       0
#define LV_USE_SPINBOX    0
#define LV_USE_SPINNER    1
#define LV_USE_SWITCH     0
#define LV_USE_TABLE      0
#define LV_USE_TABVIEW    0
#define LV_USE_TEXTAREA   0
#define LV_USE_TILEVIEW   0
#define LV_USE_WIN        0

/*==================
 * THEMES
 *==================*/
#define LV_USE_THEME_DEFAULT    1
#if LV_USE_THEME_DEFAULT
    #define LV_THEME_DEFAULT_DARK 1        /* 1 = dark theme, 0 = light */
    #define LV_THEME_DEFAULT_GROW 1
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif

#define LV_USE_THEME_SIMPLE  1
#define LV_USE_THEME_MONO    0

/*==================
 * LAYOUTS
 *==================*/
#define LV_USE_FLEX  1
#define LV_USE_GRID  0

/*=====================
 * 3RD PARTY LIBRARIES
 *====================*/
/* File system interfaces (disable for desktop sim) */
#define LV_USE_FS_STDIO      0
#define LV_USE_FS_POSIX      0
#define LV_USE_FS_WIN32      0
#define LV_USE_FS_FATFS      0

/* PNG/BMP/JPG decoders */
#define LV_USE_PNG    0
#define LV_USE_BMP    0
#define LV_USE_SJPG   0
#define LV_USE_GIF    0
#define LV_USE_QRCODE 0
#define LV_USE_BARCODE 0

/* FreeType (disable for simplicity) */
#define LV_USE_FREETYPE 0

/*==================
 * DEMOS
 *==================*/
#define LV_USE_DEMO_WIDGETS        0
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
#define LV_USE_DEMO_BENCHMARK      0
#define LV_USE_DEMO_STRESS         0
#define LV_USE_DEMO_MUSIC          0

/*==================
 * OTHERS
 *==================*/
#define LV_USE_SNAPSHOT 0
#define LV_USE_MONKEY   0
#define LV_USE_GRIDNAV  0
#define LV_USE_FRAGMENT 0
#define LV_USE_IMGFONT  0
#define LV_USE_MSG      0
#define LV_USE_IME_PINYIN 0

/*==================
 * DISPLAY DRIVER
 *==================*/
/* SDL2 built-in driver — enables lv_sdl_window API */
#define LV_USE_SDL 0   /* We use manual SDL2 flush_cb for more control */

/*==================
 * PROFILER / PERF
 *==================*/
#define LV_USE_PROFILER 0
#define LV_USE_MEM_MONITOR 0

#endif /* LV_CONF_H */

#endif /* End of "Content enable" */

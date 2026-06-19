/**
 * lv_conf.h — LVGL v9 configuration for Cerberus ESP32 firmware
 * Screen: 480x320, ILI9488 via TFT_eSPI, RGB565 (16-bit)
 */

/* clang-format off */
#if 1 /* Set to "1" to enable — DO NOT REMOVE this guard */

#ifndef LV_CONF_H
#define LV_CONF_H

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif

/*====================
   COLOR SETTINGS
 *====================*/
#define LV_COLOR_DEPTH 16

/*=========================
   MEMORY SETTINGS
 *=========================*/
/* LVGL internal heap for objects/styles */
#define LV_MEM_SIZE (32 * 1024U)

#define LV_MEM_CUSTOM 0

#define LV_USE_FLOAT 1

/*====================
   HAL SETTINGS
 *====================*/
#define LV_DEF_REFR_PERIOD 33   /* ~30 fps, plenty for gauges */
#define LV_DPI_DEF 130

/* Note: LV_TICK_CUSTOM is an LVGL v8 mechanism and has no effect in v9.
 * The tick source is registered at runtime via lv_tick_set_cb(millis)
 * in LvglDisplay.cpp. */

/*=====================
 * DRAW CONFIGURATION
 *====================*/
#define LV_DRAW_BUF_STRIDE_ALIGN 1
#define LV_DRAW_BUF_ALIGN        4
#define LV_GRADIENT_MAX_STOPS 4

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/
#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ           0

/*==================
 * LOGGING
 *==================*/
#define LV_USE_LOG 0

/*==================
 * FONTS
 *==================*/
#define LV_FONT_MONTSERRAT_8   0
#define LV_FONT_MONTSERRAT_10  0
#define LV_FONT_MONTSERRAT_12  1
#define LV_FONT_MONTSERRAT_14  1
#define LV_FONT_MONTSERRAT_16  0
#define LV_FONT_MONTSERRAT_18  0
#define LV_FONT_MONTSERRAT_20  1
#define LV_FONT_MONTSERRAT_22  0
#define LV_FONT_MONTSERRAT_24  0
#define LV_FONT_MONTSERRAT_26  0
#define LV_FONT_MONTSERRAT_28  0
#define LV_FONT_MONTSERRAT_30  0
#define LV_FONT_MONTSERRAT_32  0
#define LV_FONT_MONTSERRAT_34  0
#define LV_FONT_MONTSERRAT_36  0
#define LV_FONT_MONTSERRAT_38  0
#define LV_FONT_MONTSERRAT_40  0
#define LV_FONT_MONTSERRAT_42  0
#define LV_FONT_MONTSERRAT_44  0
#define LV_FONT_MONTSERRAT_46  0
#define LV_FONT_MONTSERRAT_48  0

#define LV_FONT_DEFAULT &lv_font_montserrat_14

#define LV_FONT_UNSCII_8  0
#define LV_FONT_UNSCII_16 0

#define LV_FONT_FMT_TXT_LARGE    0
#define LV_USE_FONT_SUBPX        0
#define LV_USE_FONT_COMPRESSED   1   /* required for lv_font_conv generated fonts */

/*===============================
 * ENABLE REQUIRED WIDGETS
 *==============================*/
#define LV_USE_ARC        0
#define LV_USE_BAR        0
#define LV_USE_BTN        0
#define LV_USE_BTNMATRIX  0
#define LV_USE_CALENDAR   0
#define LV_USE_CANVAS     0
#define LV_USE_CHART      0
#define LV_USE_CHECKBOX   0
#define LV_USE_COLORWHEEL 0
#define LV_USE_DROPDOWN   0
#define LV_USE_IMAGE      1
#define LV_USE_IMAGEBUTTON 0
#define LV_USE_KEYBOARD   0
#define LV_USE_LABEL      1
#define LV_USE_LED        0
#define LV_USE_LINE       0
#define LV_USE_LIST       0
#define LV_USE_MENU       0
#define LV_USE_METER      0
#define LV_USE_MSGBOX     0
#define LV_USE_ROLLER     0
#define LV_USE_SCALE      0
#define LV_USE_SLIDER     0
#define LV_USE_SPAN       0
#define LV_USE_SPINBOX    0
#define LV_USE_SPINNER    0
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
    #define LV_THEME_DEFAULT_DARK 1
    #define LV_THEME_DEFAULT_GROW 0
    #define LV_THEME_DEFAULT_TRANSITION_TIME 0
#endif

#define LV_USE_THEME_SIMPLE  1
#define LV_USE_THEME_MONO    0

/*==================
 * LAYOUTS
 *==================*/
#define LV_USE_FLEX  0
#define LV_USE_GRID  0

/*=====================
 * 3RD PARTY LIBRARIES
 *====================*/
#define LV_USE_FS_STDIO      0
#define LV_USE_FS_POSIX      0
#define LV_USE_FS_WIN32      0
#define LV_USE_FS_FATFS      0

#define LV_USE_PNG    0
#define LV_USE_BMP    0
#define LV_USE_SJPG   0
#define LV_USE_GIF    0
#define LV_USE_QRCODE 0
#define LV_USE_BARCODE 0

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

#endif /*LV_CONF_H*/

#endif /*Disable LVGL*/

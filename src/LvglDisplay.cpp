#include "LvglDisplay.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "DisplayManager.h"

#define LVGL_DISPLAY_WIDTH  480
#define LVGL_DISPLAY_HEIGHT 320
#define LVGL_BUF_LINES      20

static lv_color_t lvglDrawBuf[LVGL_DISPLAY_WIDTH * LVGL_BUF_LINES];
static lv_display_t *lvglDisplay = nullptr;

static void lvglFlushCb(lv_display_t *disp, const lv_area_t *area, uint8_t *pxMap)
{
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  display.startWrite();
  display.setAddrWindow(area->x1, area->y1, w, h);
  display.pushPixels((uint16_t *)pxMap, w * h);
  display.endWrite();

  lv_display_flush_ready(disp);
}

void lvglDisplayInit()
{
  // LVGL's RGB565 buffer is byte-order swapped vs TFT_eSPI's native push order
  display.setSwapBytes(true);

  lv_init();

  lvglDisplay = lv_display_create(LVGL_DISPLAY_WIDTH, LVGL_DISPLAY_HEIGHT);
  lv_display_set_color_format(lvglDisplay, LV_COLOR_FORMAT_RGB565);
  lv_display_set_flush_cb(lvglDisplay, lvglFlushCb);
  lv_display_set_buffers(lvglDisplay, lvglDrawBuf, nullptr, sizeof(lvglDrawBuf),
                          LV_DISPLAY_RENDER_MODE_PARTIAL);
}

void lvglTaskHandler()
{
  lv_timer_handler();
}

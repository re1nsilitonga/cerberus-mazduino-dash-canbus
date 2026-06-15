#ifndef LVGL_DISPLAY_H
#define LVGL_DISPLAY_H

// Initialize LVGL core + display driver bound to the TFT_eSPI `display` object.
void lvglDisplayInit();

// Pump LVGL's timer/render handler. Call every loop iteration.
void lvglTaskHandler();

#endif // LVGL_DISPLAY_H

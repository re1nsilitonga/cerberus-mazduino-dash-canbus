/**
 * ui/dashboard_ui.h
 *
 * Interface LVGL dashboard — 7 sensor racing layout (480×320).
 * Kompatibel dengan output LVGL Pro Editor:
 *   - ganti dashboard_ui_init() dengan ui_init() dari export
 *   - update dashboard_ui_update() untuk map data ke widget yang di-export
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mock_data.h"   /* DashboardData */

void dashboard_ui_init(void);
void dashboard_ui_update(const DashboardData *data);

#ifdef __cplusplus
}
#endif

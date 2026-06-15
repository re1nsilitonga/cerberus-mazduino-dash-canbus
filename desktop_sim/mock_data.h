#pragma once

#include <stdbool.h>

/**
 * DashboardData — 7 sensor yang ditampilkan di desktop simulator.
 * Harus C-compatible (typedef struct) karena dipakai dari dashboard_ui.c (C file).
 */
typedef struct DashboardData {
    int   rpm;             /* 0 – 7000 RPM                */
    float coolant_temp;    /* ECT °C  (CAN 0x3E0 byte0-1) */
    float oil_pressure;    /* kPa gauge (CAN 0x361 byte2-3)*/
    float tps;             /* TPS %   (CAN 0x360 byte4-5) */
    int   gear;            /* 0=N, 1-6 (CAN 0x3E2 byte0)  */
    float iat;             /* IAT °C  (CAN 0x3E0 byte2-3) */
    float map_kpa;         /* MAP kPa (CAN 0x360 byte2-3) */
    float speed_kmh;       /* Vehicle speed km/h (derived) */
    bool  simulator_active;
} DashboardData;

#ifdef __cplusplus
extern "C" {
#endif

extern DashboardData g_dash_data;

void mock_data_init(void);
void mock_data_update(void);
void mock_set_mode(int mode); /* 0=off, 1=sweep, 2=idle, 3=driving, 4=redline */

#ifdef __cplusplus
}
#endif

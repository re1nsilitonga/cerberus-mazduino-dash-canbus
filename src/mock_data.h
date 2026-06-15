#pragma once

#include <stdbool.h>

/**
 * DashboardData — sensor snapshot consumed by ui/dashboard_ui.c.
 * Mirrors desktop_sim/mock_data.h so the same UI source compiles on ESP32.
 */
typedef struct DashboardData {
    int   rpm;             /* 0 - 7000 RPM */
    float coolant_temp;    /* ECT degC */
    float oil_pressure;    /* kPa gauge */
    float tps;             /* TPS % */
    int   gear;            /* 0=N, 1-6 */
    float iat;             /* IAT degC */
    float map_kpa;         /* MAP kPa */
    float speed_kmh;       /* Vehicle speed km/h */
    bool  simulator_active;
} DashboardData;

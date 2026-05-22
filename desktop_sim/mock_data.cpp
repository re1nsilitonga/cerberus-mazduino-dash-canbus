/**
 * mock_data.cpp — Simulasi data 7 sensor untuk desktop simulator.
 *
 * Semua sensor memiliki animasi sendiri menggunakan phase offset berbeda
 * sehingga tidak bergerak serentak (lebih realistis).
 *
 * Modes (tekan 0-4 di window simulator):
 *   0 = off/static
 *   1 = RPM sweep  (0-7000, semua sensor mengikuti)
 *   2 = idle       (~850 RPM, oil/coolant warm-up)
 *   3 = driving    (variasi kecepatan menengah)
 *   4 = redline    (5700+ RPM, suhu tinggi)
 */

#include "mock_data.h"
#include <cmath>
#include <cstdlib>

extern "C" DashboardData g_dash_data = {};

static int   sim_mode = 1;
static float tick     = 0.0f;   /* naik 0.05 per frame (~60fps) */

/* ── Helpers ──────────────────────────────────────────────────────────────── */

static float lerp(float a, float b, float t) { return a + (b - a) * t; }

/* Clamp float ke range [lo, hi] */
static float fclamp(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

/* Hitung gear dari RPM — simulasi shift point yang realistis */
static int rpm_to_gear(int rpm) {
    if (rpm < 100)  return 0;   /* N */
    if (rpm < 1500) return 1;
    if (rpm < 2800) return 2;
    if (rpm < 4000) return 3;
    if (rpm < 5200) return 4;
    if (rpm < 6200) return 5;
    return 6;
}

/* ── Mode update functions ────────────────────────────────────────────────── */

static void update_sweep(void) {
    /* RPM: 0 → 7000 sinusoidal */
    float rpm_norm = (sinf(tick * 0.6f) + 1.0f) * 0.5f; /* 0..1 */
    g_dash_data.rpm = (int)(rpm_norm * 7000.0f);

    /* TPS: naik seiring RPM, noise kecil */
    g_dash_data.tps = fclamp(rpm_norm * 100.0f + 5.0f * sinf(tick * 3.1f), 0, 100);

    /* MAP: inverse dari TPS (throttle buka = MAP tinggi di naturally-aspirated) */
    g_dash_data.map_kpa = fclamp(40.0f + rpm_norm * 62.0f + 4.0f * sinf(tick * 1.7f), 20, 105);

    /* Oil pressure: naik dengan RPM, 150–400 kPa */
    g_dash_data.oil_pressure = fclamp(150.0f + rpm_norm * 250.0f + 10.0f * sinf(tick * 2.3f), 80, 420);

    /* Coolant: warm-up lambat 70–90°C, slight oscillation */
    g_dash_data.coolant_temp = 75.0f + 15.0f * fabs(sinf(tick * 0.08f)) + 2.0f * sinf(tick * 1.1f);

    /* IAT: lambat, 28–48°C */
    g_dash_data.iat = 30.0f + 18.0f * fabs(sinf(tick * 0.05f)) + 1.5f * sinf(tick * 0.9f);

    /* Gear: berdasarkan RPM */
    g_dash_data.gear = rpm_to_gear(g_dash_data.rpm);
}

static void update_idle(void) {
    /* RPM: 800-950 dengan sedikit hunting */
    g_dash_data.rpm = (int)(875.0f + 75.0f * sinf(tick * 1.2f) + (float)(rand() % 30));

    g_dash_data.tps      = 3.0f + 2.0f * sinf(tick * 0.8f);
    g_dash_data.map_kpa  = 35.0f + 5.0f * sinf(tick * 1.0f);

    /* Oil pressure idle: rendah ~150-180 kPa */
    g_dash_data.oil_pressure = 155.0f + 25.0f * sinf(tick * 0.7f) + (float)(rand() % 10);

    /* Coolant warm-up: pelan naik ke 90°C */
    static float clt_base = 60.0f;
    if (clt_base < 88.0f) clt_base += 0.03f;
    g_dash_data.coolant_temp = clt_base + 2.0f * sinf(tick * 0.15f);

    /* IAT idle: rendah, naik pelan karena mesin panas */
    g_dash_data.iat  = 32.0f + 6.0f * sinf(tick * 0.06f);
    g_dash_data.gear = 0;  /* Neutral saat idle */
}

static void update_driving(void) {
    /* RPM: variasi menengah 1500-4500 dengan burst akselerasi */
    float drive_wave = sinf(tick * 0.35f);
    g_dash_data.rpm = (int)(3000.0f + 1500.0f * drive_wave + 200.0f * sinf(tick * 1.8f));
    g_dash_data.rpm = (g_dash_data.rpm < 1200) ? 1200 : g_dash_data.rpm;

    float rpm_norm = (g_dash_data.rpm - 1000.0f) / 4000.0f;

    /* TPS: ikuti akselerasi */
    g_dash_data.tps = fclamp(15.0f + 55.0f * fabs(drive_wave) + 8.0f * sinf(tick * 2.5f), 0, 100);

    /* MAP: 55-95 kPa */
    g_dash_data.map_kpa = fclamp(55.0f + 40.0f * fabs(drive_wave) + 3.0f * sinf(tick * 1.9f), 30, 100);

    /* Oil pressure: 230-360 kPa saat jalan */
    g_dash_data.oil_pressure = fclamp(230.0f + 130.0f * rpm_norm + 15.0f * sinf(tick * 1.3f), 150, 400);

    /* Coolant: stabil di 85-92°C dengan sedikit spike */
    g_dash_data.coolant_temp = 87.0f + 5.0f * sinf(tick * 0.12f) + 1.5f * sinf(tick * 2.1f);

    /* IAT: lebih panas saat jalan, 38-52°C */
    g_dash_data.iat  = 42.0f + 10.0f * sinf(tick * 0.07f) + 2.0f * sinf(tick * 1.1f);
    g_dash_data.gear = rpm_to_gear(g_dash_data.rpm);
}

static void update_redline(void) {
    /* RPM: 5500-7000, naik-turun cepat */
    g_dash_data.rpm = (int)(6200.0f + 800.0f * sinf(tick * 1.5f) + (float)(rand() % 100));

    g_dash_data.tps      = fclamp(85.0f + 15.0f * sinf(tick * 2.0f), 70, 100);
    g_dash_data.map_kpa  = fclamp(90.0f + 12.0f * sinf(tick * 1.7f), 75, 105);

    /* Oil pressure tinggi: 350-430 kPa */
    g_dash_data.oil_pressure = fclamp(380.0f + 50.0f * sinf(tick * 1.2f) + (float)(rand() % 20), 300, 450);

    /* Coolant panas: 95-108°C (warning zone!) */
    g_dash_data.coolant_temp = 98.0f + 10.0f * sinf(tick * 0.2f) + 2.0f * sinf(tick * 3.0f);

    /* IAT panas: 55-70°C */
    g_dash_data.iat  = 60.0f + 10.0f * sinf(tick * 0.1f) + 3.0f * sinf(tick * 1.5f);
    g_dash_data.gear = 5 + (int)(fabs(sinf(tick * 0.8f)) > 0.6f ? 1 : 0); /* 5 atau 6 */
}

/* ── Public API ───────────────────────────────────────────────────────────── */

extern "C" DashboardData g_dash_data;

extern "C" void mock_data_init(void) {
    sim_mode = 1;
    tick     = 0.0f;
    mock_data_update();
}

extern "C" void mock_set_mode(int mode) {
    sim_mode = mode;
}

extern "C" void mock_data_update(void) {
    tick += 0.05f;

    switch (sim_mode) {
        case 1: update_sweep();   break;
        case 2: update_idle();    break;
        case 3: update_driving(); break;
        case 4: update_redline(); break;
        default:
            /* Mode 0: data berhenti / static */
            g_dash_data.rpm          = 0;
            g_dash_data.coolant_temp = 20.0f;
            g_dash_data.oil_pressure = 0.0f;
            g_dash_data.tps          = 0.0f;
            g_dash_data.gear         = 0;
            g_dash_data.iat          = 25.0f;
            g_dash_data.map_kpa      = 101.3f;
            break;
    }

    g_dash_data.simulator_active = (sim_mode != 0);
}

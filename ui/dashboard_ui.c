/**
 * ui/dashboard_ui.c — Cerberus Dashboard LVGL v9
 *
 * Layout 480×320 px — black background, cyan values:
 *
 * ┌──────────────────────────────────────────────────────────┐
 * │ [■■■■■■■■■■■■][■■■][■■]  ← RPM bar (green/yellow/red)   │ 64px
 * │  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16               │
 * ├─────────────────┬──────────────────┬────────────────────┤
 * │ ECT °C          │       0          │           OIL kPa  │
 * │ 88              │      rpm         │               88   │
 * │                 │                  │           MAP kPa  │
 * │ IAT °C          │       0          │               88   │
 * │ 88              │      km/h        │            TPS %   │
 * │                 │                  │               88   │
 * └─────────────────┴──────────────────┴────────────────────┘
 */

#include "dashboard_ui.h"
#include "lvgl.h"
#include <stdio.h>
#include <stdbool.h>

/* Custom bold fonts — generated via lv_font_conv from NotoSans-Bold.ttf */
extern const lv_font_t font_noto_bold_48;
extern const lv_font_t font_noto_bold_64;

/* ── Color palette ─────────────────────────────────────────────────────────── */
#define C_BG          0x000000   /* pure black */
#define C_CYAN        0x00FFFF   /* all sensor values */
#define C_WHITE       0xFFFFFF   /* all labels */
#define C_SEG_GREEN   0x00DD00
#define C_SEG_YELLOW  0xEEEE00
#define C_SEG_RED     0xFF2200
#define C_DIM_GREEN   0x003300
#define C_DIM_YELLOW  0x333300
#define C_DIM_RED     0x2A0000

/* ── RPM bar geometry ──────────────────────────────────────────────────────── */
#define N_SEGS        17
#define SEG_W         23
#define SEG_H         36
#define SEG_GAP       4
#define SEG_Y         8
#define BAR_X_START   12
/* RPM range represented by the bar: 0-16 (x1000 rpm) */
#define RPM_MAX       16000
/* Colour zones: 0-11 green, 12-14 yellow, 15-16 red */
#define ZONE_YELLOW   12
#define ZONE_RED      15

/* ── Startup sweep ─────────────────────────────────────────────────────────── */
#define SWEEP_DURATION_MS  5000

typedef enum {
    UI_STATE_SWEEP,
    UI_STATE_NORMAL
} ui_state_t;

static ui_state_t g_state = UI_STATE_SWEEP;
static uint32_t   g_sweep_start;
static bool       g_sweep_started = false;

/* ── Widget handles ────────────────────────────────────────────────────────── */
static lv_obj_t *g_seg[N_SEGS];
static lv_obj_t *g_rpm_val;
static lv_obj_t *g_speed_val;
static lv_obj_t *g_ect_val;
static lv_obj_t *g_iat_val;
static lv_obj_t *g_oil_val;
static lv_obj_t *g_map_val;
static lv_obj_t *g_tps_val;

/* Bright / dim colour per segment */
static const uint32_t k_bright[N_SEGS] = {
    C_SEG_GREEN,  C_SEG_GREEN,  C_SEG_GREEN,  C_SEG_GREEN,  C_SEG_GREEN,
    C_SEG_GREEN,  C_SEG_GREEN,  C_SEG_GREEN,  C_SEG_GREEN,  C_SEG_GREEN,
    C_SEG_GREEN,  C_SEG_GREEN,
    C_SEG_YELLOW, C_SEG_YELLOW, C_SEG_YELLOW,
    C_SEG_RED,    C_SEG_RED
};
static const uint32_t k_dim[N_SEGS] = {
    C_DIM_GREEN,  C_DIM_GREEN,  C_DIM_GREEN,  C_DIM_GREEN,  C_DIM_GREEN,
    C_DIM_GREEN,  C_DIM_GREEN,  C_DIM_GREEN,  C_DIM_GREEN,  C_DIM_GREEN,
    C_DIM_GREEN,  C_DIM_GREEN,
    C_DIM_YELLOW, C_DIM_YELLOW, C_DIM_YELLOW,
    C_DIM_RED,    C_DIM_RED
};

/* ── Small helper: label kiri atas dengan warna & font ─────────────────────── */
static lv_obj_t *make_label(lv_obj_t *parent, int x, int y,
                             const char *text,
                             uint32_t color, const lv_font_t *font)
{
    lv_obj_t *l = lv_label_create(parent);
    lv_label_set_text(l, text);
    lv_obj_set_pos(l, x, y);
    lv_obj_set_style_text_color(l, lv_color_hex(color), 0);
    lv_obj_set_style_text_font(l, font, 0);
    return l;
}

/* ── dashboard_ui_init ─────────────────────────────────────────────────────── */
void dashboard_ui_init(void)
{
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(C_BG), 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(scr, 0, 0);

    /* ── RPM segmented bar ───────────────────────────────────────────────── */
    for (int i = 0; i < N_SEGS; i++) {
        lv_obj_t *s = lv_obj_create(scr);
        lv_obj_set_pos(s, BAR_X_START + i * (SEG_W + SEG_GAP), SEG_Y);
        lv_obj_set_size(s, SEG_W, SEG_H);
        lv_obj_set_style_bg_color(s, lv_color_hex(k_dim[i]), 0);
        lv_obj_set_style_bg_opa(s, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(s, 0, 0);
        lv_obj_set_style_radius(s, 3, 0);
        lv_obj_set_style_pad_all(s, 0, 0);
        lv_obj_clear_flag(s, LV_OBJ_FLAG_SCROLLABLE);
        g_seg[i] = s;
    }

    /* Numbers 0-16 under the bar — each number sits under 1 segment */
    for (int n = 0; n < N_SEGS; n++) {
        int cx = BAR_X_START + n * (SEG_W + SEG_GAP) + SEG_W / 2;
        lv_obj_t *nl = lv_label_create(scr);
        char buf[4];
        snprintf(buf, sizeof(buf), "%d", n);
        lv_label_set_text(nl, buf);
        lv_obj_set_pos(nl, cx - 4, SEG_Y + SEG_H + 4);
        lv_obj_set_style_text_color(nl, lv_color_hex(C_WHITE), 0);
        lv_obj_set_style_text_font(nl, &lv_font_montserrat_12, 0);
    }

    /* ── LEFT column: ECT + IAT ─────────────────────────────────────────── */
    /*  x=10, each sensor block height = 128 px starting from y=66 */

    /* ECT */
    make_label(scr, 10, 72,  "ECT \xc2\xb0""C", C_WHITE, &lv_font_montserrat_20);
    g_ect_val = make_label(scr, 10, 98, "--", C_CYAN, &font_noto_bold_48);

    /* IAT */
    make_label(scr, 10, 192, "IAT \xc2\xb0""C", C_WHITE, &lv_font_montserrat_20);
    g_iat_val = make_label(scr, 10, 218, "--", C_CYAN, &font_noto_bold_48);

    /* ── CENTER column: RPM (top) + Speed (bottom, aligned with IAT) ─────── */

    /* RPM — big number, top center, "rpm" label below it */
    g_rpm_val = lv_label_create(scr);
    lv_label_set_text(g_rpm_val, "0");
    lv_obj_set_style_text_color(g_rpm_val, lv_color_hex(C_CYAN), 0);
    lv_obj_set_style_text_font(g_rpm_val, &font_noto_bold_64, 0);
    lv_obj_set_width(g_rpm_val, 190);
    lv_obj_set_style_text_align(g_rpm_val, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(g_rpm_val, 150, 70);

    lv_obj_t *rpm_unit = lv_label_create(scr);
    lv_label_set_text(rpm_unit, "rpm");
    lv_obj_set_style_text_color(rpm_unit, lv_color_hex(C_WHITE), 0);
    lv_obj_set_style_text_font(rpm_unit, &lv_font_montserrat_20, 0);
    lv_obj_set_width(rpm_unit, 190);
    lv_obj_set_style_text_align(rpm_unit, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(rpm_unit, 150, 145);

    /* Speed — aligned with the IAT row, "km/h" label below it */
    g_speed_val = lv_label_create(scr);
    lv_label_set_text(g_speed_val, "0");
    lv_obj_set_style_text_color(g_speed_val, lv_color_hex(C_CYAN), 0);
    lv_obj_set_style_text_font(g_speed_val, &font_noto_bold_48, 0);
    lv_obj_set_width(g_speed_val, 190);
    lv_obj_set_style_text_align(g_speed_val, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(g_speed_val, 150, 192);

    lv_obj_t *speed_unit = lv_label_create(scr);
    lv_label_set_text(speed_unit, "km/h");
    lv_obj_set_style_text_color(speed_unit, lv_color_hex(C_WHITE), 0);
    lv_obj_set_style_text_font(speed_unit, &lv_font_montserrat_20, 0);
    lv_obj_set_width(speed_unit, 190);
    lv_obj_set_style_text_align(speed_unit, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(speed_unit, 150, 244);

    /* ── RIGHT column: OIL | MAP | TPS ─────────────────────────────────── */

    /* OIL */
    lv_obj_t *oil_lbl = lv_label_create(scr);
    lv_label_set_text(oil_lbl, "OIL kPa");
    lv_obj_set_style_text_color(oil_lbl, lv_color_hex(C_WHITE), 0);
    lv_obj_set_style_text_font(oil_lbl, &lv_font_montserrat_20, 0);
    lv_obj_align(oil_lbl, LV_ALIGN_TOP_RIGHT, -10, 70);

    g_oil_val = lv_label_create(scr);
    lv_label_set_text(g_oil_val, "--");
    lv_obj_set_style_text_color(g_oil_val, lv_color_hex(C_CYAN), 0);
    lv_obj_set_style_text_font(g_oil_val, &font_noto_bold_48, 0);
    lv_obj_align(g_oil_val, LV_ALIGN_TOP_RIGHT, -10, 96);

    /* MAP */
    lv_obj_t *map_lbl = lv_label_create(scr);
    lv_label_set_text(map_lbl, "MAP kPa");
    lv_obj_set_style_text_color(map_lbl, lv_color_hex(C_WHITE), 0);
    lv_obj_set_style_text_font(map_lbl, &lv_font_montserrat_20, 0);
    lv_obj_align(map_lbl, LV_ALIGN_TOP_RIGHT, -10, 160);

    g_map_val = lv_label_create(scr);
    lv_label_set_text(g_map_val, "--");
    lv_obj_set_style_text_color(g_map_val, lv_color_hex(C_CYAN), 0);
    lv_obj_set_style_text_font(g_map_val, &font_noto_bold_48, 0);
    lv_obj_align(g_map_val, LV_ALIGN_TOP_RIGHT, -10, 186);

    /* TPS */
    lv_obj_t *tps_lbl = lv_label_create(scr);
    lv_label_set_text(tps_lbl, "TPS %");
    lv_obj_set_style_text_color(tps_lbl, lv_color_hex(C_WHITE), 0);
    lv_obj_set_style_text_font(tps_lbl, &lv_font_montserrat_20, 0);
    lv_obj_align(tps_lbl, LV_ALIGN_TOP_RIGHT, -10, 248);

    g_tps_val = lv_label_create(scr);
    lv_label_set_text(g_tps_val, "--");
    lv_obj_set_style_text_color(g_tps_val, lv_color_hex(C_CYAN), 0);
    lv_obj_set_style_text_font(g_tps_val, &font_noto_bold_48, 0);
    lv_obj_align(g_tps_val, LV_ALIGN_TOP_RIGHT, -10, 274);

}

/* ── dashboard_ui_update ───────────────────────────────────────────────────── */
void dashboard_ui_update(const DashboardData *d)
{
    DashboardData sweep_data;

    if (g_state == UI_STATE_SWEEP) {
        /* ── Startup sweep: gauges animate 0 -> max -> 0 before showing real data ── */
        if (!g_sweep_started) {
            g_sweep_start = lv_tick_get();
            g_sweep_started = true;
        }
        uint32_t elapsed = lv_tick_elaps(g_sweep_start);
        if (elapsed >= SWEEP_DURATION_MS) {
            g_state = UI_STATE_NORMAL;
        } else {
            /* Triangular sweep: 0 -> max -> 0 across all gauges, eased for a
             * smoother feel (slow start/end, fast through the middle). */
            float frac = (float)elapsed / (float)SWEEP_DURATION_MS;
            float tri  = (frac < 0.5f) ? (frac * 2.0f) : ((1.0f - frac) * 2.0f);
            /* Smoothstep ease: 3x^2 - 2x^3 */
            float t = tri * tri * (3.0f - 2.0f * tri);

            sweep_data.rpm             = (int)(t * RPM_MAX);
            sweep_data.speed_kmh       = t * 240.0f;
            sweep_data.coolant_temp    = t * 120.0f;
            sweep_data.iat             = t * 80.0f;
            sweep_data.oil_pressure    = t * 300.0f;
            sweep_data.map_kpa         = t * 300.0f;
            sweep_data.tps             = t * 100.0f;
            sweep_data.gear            = 0;
            sweep_data.simulator_active = false;
            d = &sweep_data;
        }
    }

#ifdef DEMO_MODE
    /* ── Demo mode: simulate realistic driving loop after sweep ─────────── */
    if (g_state == UI_STATE_NORMAL) {
        static uint32_t demo_start = 0;
        if (demo_start == 0) demo_start = lv_tick_get();

        /* Gear table: shift RPM, speed range per gear */
        static const int   N_GEARS       = 6;
        static const int   RPM_IDLE      = 1200;
        static const int   RPM_SHIFT     = 15000;
        static const float SPEED_MAX[]   = { 35, 65, 100, 140, 180, 220 };
        static const float GEAR_TIME_MS  = 4000.0f;
        /* Total cycle: accelerate through 6 gears + decelerate back = 2 phases */
        float total_ms    = N_GEARS * GEAR_TIME_MS * 2.0f;
        float elapsed_demo = (float)lv_tick_elaps(demo_start);
        float cycle_pos   = elapsed_demo - total_ms * (int)(elapsed_demo / total_ms);

        float accel_ms = N_GEARS * GEAR_TIME_MS;
        bool  accel    = (cycle_pos < accel_ms);
        float phase_t  = accel ? cycle_pos : (accel_ms - (cycle_pos - accel_ms));
        if (phase_t < 0) phase_t = 0;

        int   gear_idx = (int)(phase_t / GEAR_TIME_MS);
        if (gear_idx >= N_GEARS) gear_idx = N_GEARS - 1;
        float in_gear  = (phase_t - gear_idx * GEAR_TIME_MS) / GEAR_TIME_MS;
        /* Smooth ease within gear */
        in_gear = in_gear * in_gear * (3.0f - 2.0f * in_gear);

        int   rpm_val    = RPM_IDLE + (int)(in_gear * (RPM_SHIFT - RPM_IDLE));
        float speed_lo   = (gear_idx > 0) ? SPEED_MAX[gear_idx - 1] : 0.0f;
        float speed_hi   = SPEED_MAX[gear_idx];
        float speed_val  = speed_lo + in_gear * (speed_hi - speed_lo);

        /* TPS follows RPM roughly */
        float tps_val    = 20.0f + in_gear * 70.0f;
        float map_val    = 30.0f + in_gear * 200.0f;
        float oil_val    = 150.0f + speed_val * 0.5f;

        sweep_data.rpm             = rpm_val;
        sweep_data.speed_kmh       = speed_val;
        sweep_data.coolant_temp    = 85.0f;
        sweep_data.iat             = 38.0f;
        sweep_data.oil_pressure    = oil_val;
        sweep_data.map_kpa         = map_val;
        sweep_data.tps             = tps_val;
        sweep_data.gear            = gear_idx + 1;
        sweep_data.simulator_active = false;
        d = &sweep_data;
    }
#endif

    if (!d) return;

    /* ── RPM bar ────────────────────────────────────────────────────────── */
    int active = (d->rpm * N_SEGS) / RPM_MAX;
    if (active > N_SEGS) active = N_SEGS;
    for (int i = 0; i < N_SEGS; i++) {
        lv_color_t c = (i < active) ? lv_color_hex(k_bright[i])
                                     : lv_color_hex(k_dim[i]);
        lv_obj_set_style_bg_color(g_seg[i], c, 0);
    }

    /* ── RPM ────────────────────────────────────────────────────────────── */
    {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", d->rpm);
        lv_label_set_text(g_rpm_val, buf);
    }

    /* ── Speed ──────────────────────────────────────────────────────────── */
    {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", (int)d->speed_kmh);
        lv_label_set_text(g_speed_val, buf);
    }

    /* ── ECT ────────────────────────────────────────────────────────────── */
    {
        char buf[8];
        snprintf(buf, sizeof(buf), "%.0f", (double)d->coolant_temp);
        lv_label_set_text(g_ect_val, buf);
    }

    /* ── IAT ────────────────────────────────────────────────────────────── */
    {
        char buf[8];
        snprintf(buf, sizeof(buf), "%.0f", (double)d->iat);
        lv_label_set_text(g_iat_val, buf);
    }

    /* ── OIL ────────────────────────────────────────────────────────────── */
    {
        char buf[8];
        snprintf(buf, sizeof(buf), "%.0f", (double)d->oil_pressure);
        lv_label_set_text(g_oil_val, buf);
    }

    /* ── MAP ────────────────────────────────────────────────────────────── */
    {
        char buf[8];
        snprintf(buf, sizeof(buf), "%.0f", (double)d->map_kpa);
        lv_label_set_text(g_map_val, buf);
    }

    /* ── TPS ────────────────────────────────────────────────────────────── */
    {
        char buf[8];
        snprintf(buf, sizeof(buf), "%.0f", (double)d->tps);
        lv_label_set_text(g_tps_val, buf);
    }
}

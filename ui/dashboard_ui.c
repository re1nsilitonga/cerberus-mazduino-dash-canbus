/**
 * ui/dashboard_ui.c — Cerberus Dashboard LVGL v9
 *
 * Layout 480×320 px — 7 sensor, dark racing theme:
 *
 * ┌──────────────────────────────────────────────────────────┐
 * │  ◆ CERBERUS                                    [SIM]    │ 36px
 * ├──────────────────────────────────────────────────────────┤
 * │                          │                              │
 * │   RPM  (arc 190×190)     │   G E A R                   │
 * │        [  3 5 0 0  ]     │        [  3  ]               │
 * │        [    RPM    ]     │                              │
 * │                          │                              │
 * ├──────────────────────────────────────────────────────────┤
 * │  CLT °C  │ OIL kPa │ TPS % │  IAT °C  │  MAP kPa      │
 * │    85    │   280    │  35   │    30    │    95          │
 * └──────────────────────────────────────────────────────────┘
 */

#include "dashboard_ui.h"
#include "lvgl.h"
#include <stdio.h>   /* snprintf */

/* Macro: format float ke string lalu set ke label — menghindari ketergantungan
 * pada LV_USE_FLOAT / lv_snprintf yang defaultnya tidak support %f */
#define SET_FLOAT(widget, fmt, val) \
    do { char _b[16]; snprintf(_b, sizeof(_b), fmt, (double)(val)); \
         lv_label_set_text(widget, _b); } while(0)

/* ── Palette ───────────────────────────────────────────────────────────────── */
#define C_BG        0x0C0C11
#define C_HEADER    0x131318
#define C_PANEL     0x16161E
#define C_BORDER    0x282835
#define C_ACCENT    0xE01A0A  /* Cerberus red */
#define C_DIM       0x70707E  /* dim grey — labels */
#define C_BRIGHT    0xF0F0F2  /* near-white — values */
#define C_ARC_BG    0x232330
#define C_ARC_FG    0xE01A0A
#define C_WARN      0xFF8800  /* orange — warning */
#define C_DANGER    0xFF2020  /* red — critical */
#define C_SIM       0xFFAA00  /* amber — sim badge */
#define C_GEAR_LOW  0x28E028  /* green — gear 1-2 */
#define C_GEAR_MID  0xF0D010  /* yellow — gear 3-4 */
#define C_GEAR_HIGH 0xFF5500  /* orange — gear 5-6 */

/* ── Widget handles ────────────────────────────────────────────────────────── */
static lv_obj_t *g_rpm_arc;
static lv_obj_t *g_rpm_val;
static lv_obj_t *g_gear_val;
static lv_obj_t *g_gear_label;   /* small "GEAR" label */
static lv_obj_t *g_clt_val;
static lv_obj_t *g_oil_val;
static lv_obj_t *g_tps_val;
static lv_obj_t *g_iat_val;
static lv_obj_t *g_map_val;
static lv_obj_t *g_sim_badge;

/* ── Helpers ───────────────────────────────────────────────────────────────── */

/* Buat panel kotak dengan border */
static lv_obj_t *panel_create(lv_obj_t *parent, int x, int y, int w, int h)
{
    lv_obj_t *p = lv_obj_create(parent);
    lv_obj_set_pos(p, x, y);
    lv_obj_set_size(p, w, h);
    lv_obj_set_scrollbar_mode(p, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(p, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(p, lv_color_hex(C_PANEL), 0);
    lv_obj_set_style_border_color(p, lv_color_hex(C_BORDER), 0);
    lv_obj_set_style_border_width(p, 1, 0);
    lv_obj_set_style_radius(p, 4, 0);
    lv_obj_set_style_pad_all(p, 0, 0);
    return p;
}

/* Buat label kecil (judul kolom) */
static lv_obj_t *small_label(lv_obj_t *parent, const char *text, lv_align_t align, int ox, int oy)
{
    lv_obj_t *l = lv_label_create(parent);
    lv_label_set_text(l, text);
    lv_obj_align(l, align, ox, oy);
    lv_obj_set_style_text_color(l, lv_color_hex(C_DIM), 0);
    lv_obj_set_style_text_font(l, &lv_font_montserrat_12, 0);
    return l;
}

/* ── Sensor bottom panel ────────────────────────────────────────────────────── */
/*
 * Setiap panel bawah (96×88 px):
 *   - Garis pemisah atas (1px)
 *   - Label nama sensor (12px, abu-abu, tengah atas)
 *   - Nilai sensor (24px, putih/warning, tengah)
 *   - Unit (12px, abu-abu, bawah)
 */
static lv_obj_t *bottom_panel_create(lv_obj_t *parent,
                                      int        x,
                                      const char *label,
                                      const char *unit)
{
    /* Container */
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_set_pos(panel, x, 0);
    lv_obj_set_size(panel, 96, lv_pct(100));
    lv_obj_set_scrollbar_mode(panel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(panel, lv_color_hex(C_PANEL), 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(C_BORDER), 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_radius(panel, 0, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);

    /* Label sensor */
    lv_obj_t *lbl = lv_label_create(panel);
    lv_label_set_text(lbl, label);
    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 8);
    lv_obj_set_style_text_color(lbl, lv_color_hex(C_DIM), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);

    /* Nilai sensor (return pointer untuk di-update) */
    lv_obj_t *val = lv_label_create(panel);
    lv_label_set_text(val, "--");
    lv_obj_align(val, LV_ALIGN_CENTER, 0, 4);
    lv_obj_set_style_text_color(val, lv_color_hex(C_BRIGHT), 0);
    lv_obj_set_style_text_font(val, &lv_font_montserrat_24, 0);

    /* Unit */
    lv_obj_t *unt = lv_label_create(panel);
    lv_label_set_text(unt, unit);
    lv_obj_align(unt, LV_ALIGN_BOTTOM_MID, 0, -6);
    lv_obj_set_style_text_color(unt, lv_color_hex(C_DIM), 0);
    lv_obj_set_style_text_font(unt, &lv_font_montserrat_12, 0);

    return val; /* caller menyimpan pointer ke value label */
}

/* ── dashboard_ui_init ─────────────────────────────────────────────────────── */
void dashboard_ui_init(void)
{
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(C_BG), 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(scr, 0, 0);

    /* ── Header (480×36) ─────────────────────────────────────────────────── */
    lv_obj_t *hdr = lv_obj_create(scr);
    lv_obj_set_pos(hdr, 0, 0);
    lv_obj_set_size(hdr, 480, 36);
    lv_obj_clear_flag(hdr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(hdr, lv_color_hex(C_HEADER), 0);
    lv_obj_set_style_border_width(hdr, 0, 0);
    lv_obj_set_style_radius(hdr, 0, 0);
    lv_obj_set_style_pad_all(hdr, 0, 0);

    /* Red accent line */
    lv_obj_t *accent = lv_obj_create(hdr);
    lv_obj_set_pos(accent, 0, 33);
    lv_obj_set_size(accent, 480, 3);
    lv_obj_set_style_bg_color(accent, lv_color_hex(C_ACCENT), 0);
    lv_obj_set_style_border_width(accent, 0, 0);
    lv_obj_set_style_radius(accent, 0, 0);

    /* Title */
    lv_obj_t *title = lv_label_create(hdr);
    lv_label_set_text(title, LV_SYMBOL_WARNING " CERBERUS");
    lv_obj_set_pos(title, 10, 9);
    lv_obj_set_style_text_color(title, lv_color_hex(C_ACCENT), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);

    /* SIM badge */
    g_sim_badge = lv_label_create(hdr);
    lv_label_set_text(g_sim_badge, "[ SIM ]");
    lv_obj_align(g_sim_badge, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_text_color(g_sim_badge, lv_color_hex(C_SIM), 0);
    lv_obj_set_style_text_font(g_sim_badge, &lv_font_montserrat_14, 0);
    lv_obj_add_flag(g_sim_badge, LV_OBJ_FLAG_HIDDEN);

    /* ── Vertical divider antara RPM dan GEAR panel ─────────────────────── */
    lv_obj_t *vdiv = lv_obj_create(scr);
    lv_obj_set_pos(vdiv, 238, 38);
    lv_obj_set_size(vdiv, 2, 186);
    lv_obj_set_style_bg_color(vdiv, lv_color_hex(C_BORDER), 0);
    lv_obj_set_style_border_width(vdiv, 0, 0);
    lv_obj_set_style_radius(vdiv, 0, 0);

    /* ── RPM panel (kiri, 234×186) ──────────────────────────────────────── */
    lv_obj_t *rpm_panel = panel_create(scr, 0, 38, 238, 186);

    /* Arc RPM */
    g_rpm_arc = lv_arc_create(rpm_panel);
    lv_obj_set_size(g_rpm_arc, 180, 180);
    lv_obj_align(g_rpm_arc, LV_ALIGN_CENTER, -4, 0);

    lv_arc_set_rotation(g_rpm_arc, 135);
    lv_arc_set_bg_angles(g_rpm_arc, 0, 270);
    lv_arc_set_range(g_rpm_arc, 0, 7000);
    lv_arc_set_value(g_rpm_arc, 0);
    lv_obj_clear_flag(g_rpm_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_opa(g_rpm_arc, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_bg_opa(g_rpm_arc, LV_OPA_0, 0);

    lv_obj_set_style_arc_color(g_rpm_arc, lv_color_hex(C_ARC_BG), LV_PART_MAIN);
    lv_obj_set_style_arc_width(g_rpm_arc, 13, LV_PART_MAIN);
    lv_obj_set_style_arc_color(g_rpm_arc, lv_color_hex(C_ARC_FG), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(g_rpm_arc, 13, LV_PART_INDICATOR);

    /* RPM value — besar di tengah arc */
    g_rpm_val = lv_label_create(rpm_panel);
    lv_label_set_text(g_rpm_val, "0");
    lv_obj_align(g_rpm_val, LV_ALIGN_CENTER, -4, -10);
    lv_obj_set_style_text_color(g_rpm_val, lv_color_hex(C_BRIGHT), 0);
    lv_obj_set_style_text_font(g_rpm_val, &lv_font_montserrat_32, 0);

    /* "RPM" unit kecil di bawah value */
    small_label(rpm_panel, "RPM", LV_ALIGN_CENTER, -4, 22);

    /* ── GEAR panel (kanan, 234×186) ────────────────────────────────────── */
    lv_obj_t *gear_panel = panel_create(scr, 242, 38, 238, 186);

    /* Label "GEAR" kecil di atas */
    g_gear_label = small_label(gear_panel, "GEAR", LV_ALIGN_TOP_MID, 0, 14);
    lv_obj_set_style_text_font(g_gear_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(g_gear_label, lv_color_hex(C_DIM), 0);

    /* Nomor gigi — BESAR (font 48), center panel */
    g_gear_val = lv_label_create(gear_panel);
    lv_label_set_text(g_gear_val, "N");
    lv_obj_align(g_gear_val, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_style_text_color(g_gear_val, lv_color_hex(C_GEAR_LOW), 0);
    lv_obj_set_style_text_font(g_gear_val, &lv_font_montserrat_48, 0);

    /* ── Horizontal divider antara main dan bottom strip ────────────────── */
    lv_obj_t *hdiv = lv_obj_create(scr);
    lv_obj_set_pos(hdiv, 0, 226);
    lv_obj_set_size(hdiv, 480, 2);
    lv_obj_set_style_bg_color(hdiv, lv_color_hex(C_ACCENT), 0);
    lv_obj_set_style_border_width(hdiv, 0, 0);
    lv_obj_set_style_radius(hdiv, 0, 0);

    /* ── Bottom strip (480×90) — 5 sensor ──────────────────────────────── */
    lv_obj_t *strip = lv_obj_create(scr);
    lv_obj_set_pos(strip, 0, 230);
    lv_obj_set_size(strip, 480, 90);
    lv_obj_set_scrollbar_mode(strip, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(strip, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(strip, lv_color_hex(C_BG), 0);
    lv_obj_set_style_border_width(strip, 0, 0);
    lv_obj_set_style_radius(strip, 0, 0);
    lv_obj_set_style_pad_all(strip, 0, 0);

    /* 5 panel: CLT | OIL | TPS | IAT | MAP */
    g_clt_val = bottom_panel_create(strip,   0, "CLT",  "\xc2\xb0""C");
    g_oil_val = bottom_panel_create(strip,  96, "OIL",  "kPa");
    g_tps_val = bottom_panel_create(strip, 192, "TPS",  "%");
    g_iat_val = bottom_panel_create(strip, 288, "IAT",  "\xc2\xb0""C");
    g_map_val = bottom_panel_create(strip, 384, "MAP",  "kPa");
}

/* ── dashboard_ui_update ───────────────────────────────────────────────────── */
void dashboard_ui_update(const DashboardData *d)
{
    if (!d) return;

    /* SIM badge */
    if (d->simulator_active)
        lv_obj_clear_flag(g_sim_badge, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_sim_badge, LV_OBJ_FLAG_HIDDEN);

    /* ── RPM ─────────────────────────────────────────────────────────────── */
    int rpm = d->rpm;
    if (rpm < 0)    rpm = 0;
    if (rpm > 7000) rpm = 7000;

    lv_arc_set_value(g_rpm_arc, rpm);
    lv_label_set_text_fmt(g_rpm_val, "%d", rpm);

    /* Warna RPM: putih → kuning → merah */
    lv_color_t c_rpm;
    if (rpm < 4500)      c_rpm = lv_color_hex(C_BRIGHT);
    else if (rpm < 6000) c_rpm = lv_color_hex(C_WARN);
    else                 c_rpm = lv_color_hex(C_DANGER);
    lv_obj_set_style_text_color(g_rpm_val, c_rpm, 0);

    /* Warna arc indicator ikut RPM */
    lv_color_t c_arc = (rpm < 5000) ? lv_color_hex(C_ARC_FG) :
                        (rpm < 6200) ? lv_color_hex(C_WARN) :
                                       lv_color_hex(C_DANGER);
    lv_obj_set_style_arc_color(g_rpm_arc, c_arc, LV_PART_INDICATOR);

    /* ── GEAR ────────────────────────────────────────────────────────────── */
    int gear = d->gear;
    char gear_str[4];
    if (gear <= 0)
        gear_str[0] = 'N', gear_str[1] = '\0';
    else
        snprintf(gear_str, sizeof(gear_str), "%d", gear > 6 ? 6 : gear);

    lv_label_set_text(g_gear_val, gear_str);

    /* Warna gear: hijau=1-2, kuning=3-4, oranye=5-6, putih=N */
    lv_color_t c_gear;
    if (gear <= 0)      c_gear = lv_color_hex(C_DIM);
    else if (gear <= 2) c_gear = lv_color_hex(C_GEAR_LOW);
    else if (gear <= 4) c_gear = lv_color_hex(C_GEAR_MID);
    else                c_gear = lv_color_hex(C_GEAR_HIGH);
    lv_obj_set_style_text_color(g_gear_val, c_gear, 0);

    /* ── CLT (Coolant Temp) ──────────────────────────────────────────────── */
    SET_FLOAT(g_clt_val, "%.0f", d->coolant_temp);
    lv_color_t c_clt = (d->coolant_temp > 105.0f) ? lv_color_hex(C_DANGER) :
                        (d->coolant_temp >  95.0f) ? lv_color_hex(C_WARN)   :
                                                     lv_color_hex(C_BRIGHT);
    lv_obj_set_style_text_color(g_clt_val, c_clt, 0);

    /* ── OIL pressure ────────────────────────────────────────────────────── */
    SET_FLOAT(g_oil_val, "%.0f", d->oil_pressure);
    lv_color_t c_oil = (d->oil_pressure < 100.0f && rpm > 500) ? lv_color_hex(C_DANGER) :
                        (d->oil_pressure < 150.0f && rpm > 500) ? lv_color_hex(C_WARN)   :
                                                                   lv_color_hex(C_BRIGHT);
    lv_obj_set_style_text_color(g_oil_val, c_oil, 0);

    /* ── TPS ─────────────────────────────────────────────────────────────── */
    SET_FLOAT(g_tps_val, "%.0f", d->tps);
    lv_obj_set_style_text_color(g_tps_val, lv_color_hex(C_BRIGHT), 0);

    /* ── IAT (Intake Air Temp) ───────────────────────────────────────────── */
    SET_FLOAT(g_iat_val, "%.0f", d->iat);
    lv_color_t c_iat = (d->iat > 65.0f) ? lv_color_hex(C_WARN) : lv_color_hex(C_BRIGHT);
    lv_obj_set_style_text_color(g_iat_val, c_iat, 0);

    /* ── MAP ─────────────────────────────────────────────────────────────── */
    SET_FLOAT(g_map_val, "%.0f", d->map_kpa);
    lv_obj_set_style_text_color(g_map_val, lv_color_hex(C_BRIGHT), 0);
}

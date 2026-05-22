/**
 * desktop_sim/main.cpp
 *
 * Cerberus Dashboard — Desktop Simulator
 * LVGL v9 + SDL2, 480×320 px (matches physical TFT)
 *
 * Keys:
 *   0 = data off / static zero
 *   1 = RPM sweep
 *   2 = idle (~850 RPM)
 *   3 = driving (1500-4000 RPM)
 *   4 = redline (5700+ RPM)
 *   ESC / Q = quit
 */

#include "lvgl.h"
#include "SDL2/SDL.h"
#include "mock_data.h"      /* C++ — fungsi dengan C++ linkage */
#include "dashboard_ui.h"   /* sudah punya ifdef __cplusplus extern "C" guard */

/* ── Display resolution (must match TFT) ──────────────────────────────────── */
#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320

/* ── SDL2 globals ─────────────────────────────────────────────────────────── */
static SDL_Window   *g_window   = nullptr;
static SDL_Renderer *g_renderer = nullptr;
static SDL_Texture  *g_texture  = nullptr;
static lv_display_t *g_display  = nullptr;

/* ── LVGL flush callback → SDL2 texture ──────────────────────────────────── */
static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    /* LVGL ARGB8888 = SDL BGRA32 on little-endian — both are 4 bytes/px */
    SDL_UpdateTexture(g_texture, nullptr, px_map, DISPLAY_WIDTH * 4);
    SDL_RenderCopy(g_renderer, g_texture, nullptr, nullptr);
    SDL_RenderPresent(g_renderer);
    lv_display_flush_ready(disp);
}

/* ── Entry point ──────────────────────────────────────────────────────────── */
int main(int /*argc*/, char ** /*argv*/)
{
    /* --- SDL2 init --- */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    g_window = SDL_CreateWindow(
        "Cerberus Dashboard Simulator — " __DATE__,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        DISPLAY_WIDTH, DISPLAY_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!g_window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return 1;
    }

    g_renderer = SDL_CreateRenderer(g_window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g_renderer) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        return 1;
    }

    /* ARGB8888 texture — matches LV_COLOR_DEPTH 32 */
    g_texture = SDL_CreateTexture(g_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        DISPLAY_WIDTH, DISPLAY_HEIGHT);
    if (!g_texture) {
        SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
        return 1;
    }

    /* --- LVGL init --- */
    lv_init();

    g_display = lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);

    /* Full-screen draw buffer — LVGL renders entire frame then we blit */
    static uint8_t draw_buf[DISPLAY_WIDTH * DISPLAY_HEIGHT * 4];
    lv_display_set_buffers(g_display, draw_buf, nullptr, sizeof(draw_buf),
                           LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(g_display, flush_cb);

    /* --- App init --- */
    mock_data_init();
    dashboard_ui_init();

    /* --- Main loop --- */
    bool running = true;
    uint32_t last_tick = SDL_GetTicks();

    while (running) {
        /* Process SDL events */
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_q:
                        running = false;
                        break;
                    case SDLK_0: mock_set_mode(0); break;
                    case SDLK_1: mock_set_mode(1); break;
                    case SDLK_2: mock_set_mode(2); break;
                    case SDLK_3: mock_set_mode(3); break;
                    case SDLK_4: mock_set_mode(4); break;
                    default: break;
                }
            }
        }

        /* Update mock data and push to UI */
        uint32_t now   = SDL_GetTicks();
        uint32_t delta = now - last_tick;
        last_tick = now;

        mock_data_update();
        dashboard_ui_update(&g_dash_data);

        /* Drive LVGL timer system */
        lv_tick_inc(delta);
        lv_timer_handler();

        /* Cap to ~60 fps */
        SDL_Delay(5);
    }

    /* --- Cleanup --- */
    SDL_DestroyTexture(g_texture);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
    return 0;
}

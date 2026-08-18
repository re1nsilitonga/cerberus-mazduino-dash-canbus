#include "WS2812Control.h"

#ifdef WS2812_PIN

#include <Adafruit_NeoPixel.h>

#ifndef WS2812_COUNT
#define WS2812_COUNT 8
#endif

// 8 LEDs mirror the 17-segment on-screen RPM bar (12 green / 3 yellow / 2 red)
// scaled down proportionally: 5 green, 2 yellow, 1 red.
#define WS_GREEN_COUNT  5
#define WS_YELLOW_COUNT 2
// remaining 1 LED is red

static Adafruit_NeoPixel ws(WS2812_COUNT, WS2812_PIN, NEO_GRB + NEO_KHZ800);

void ws2812Init()
{
    ws.begin();
    ws.setBrightness(80);
    ws.clear();
    ws.show();
}

void ws2812Update(int rpm, int rpmMax)
{
    if (rpm < 0) rpm = 0;
    if (rpmMax <= 0) rpmMax = 1;

    int active = (rpm * WS2812_COUNT) / rpmMax;
    if (active > WS2812_COUNT) active = WS2812_COUNT;

    for (int i = 0; i < WS2812_COUNT; i++) {
        if (i >= active) {
            ws.setPixelColor(i, 0);
            continue;
        }
        if (i < WS_GREEN_COUNT) {
            ws.setPixelColor(i, ws.Color(0, 255, 0));
        } else if (i < WS_GREEN_COUNT + WS_YELLOW_COUNT) {
            ws.setPixelColor(i, ws.Color(255, 150, 0));
        } else {
            ws.setPixelColor(i, ws.Color(255, 0, 0));
        }
    }
    ws.show();
}

#else

void ws2812Init() {}
void ws2812Update(int rpm, int rpmMax) { (void)rpm; (void)rpmMax; }

#endif // WS2812_PIN

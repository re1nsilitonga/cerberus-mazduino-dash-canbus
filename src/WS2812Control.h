#ifndef WS2812_CONTROL_H
#define WS2812_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

void ws2812Init();
// rpm/rpmMax drives how many of the 8 LEDs are lit, same proportion as the
// 17-segment RPM bar on screen (5 green, 2 yellow, 1 red).
void ws2812Update(int rpm, int rpmMax);

#ifdef __cplusplus
}
#endif

#endif // WS2812_CONTROL_H

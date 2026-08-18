#ifndef GEAR_DISPLAY_H
#define GEAR_DISPLAY_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

void gearDisplayInit();
void gearDisplayUpdate(int gear);

#ifdef __cplusplus
}
#endif

#endif // GEAR_DISPLAY_H

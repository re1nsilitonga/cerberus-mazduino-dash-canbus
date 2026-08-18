#include "GearDisplay.h"

// Pins and polarity are overridable per board via build_flags (see platformio.ini).
// Defaults below match the original esp32dev board (common cathode).
#ifndef GEAR_SEG_A
#define GEAR_SEG_A 13
#define GEAR_SEG_B 14
#define GEAR_SEG_C 21
#define GEAR_SEG_D 22
#define GEAR_SEG_E 25
#define GEAR_SEG_F 26
#define GEAR_SEG_G 27
#endif

#define SEG_A GEAR_SEG_A
#define SEG_B GEAR_SEG_B
#define SEG_C GEAR_SEG_C
#define SEG_D GEAR_SEG_D
#define SEG_E GEAR_SEG_E
#define SEG_F GEAR_SEG_F
#define SEG_G GEAR_SEG_G

static const uint8_t seg_pins[] = { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G };

/*  Segment encoding
 *
 *       A
 *      ───
 *   F │   │ B
 *      ─G─
 *   E │   │ C
 *      ───
 *       D
 *
 *  Bit order: gfedcba (bit 0 = A, bit 6 = G)
 */
static const uint8_t font[] = {
    /* 0 */ 0b0111111,
    /* 1 */ 0b0000110,
    /* 2 */ 0b1011011,
    /* 3 */ 0b1001111,
    /* 4 */ 0b1100110,
    /* 5 */ 0b1101101,
    /* 6 */ 0b1111101,
    /* 7 */ 0b0000111,
    /* 8 */ 0b1111111,
    /* 9 */ 0b1101111,
};

static void writeSegments(uint8_t bits)
{
    for (int i = 0; i < 7; i++) {
        bool on = (bits >> i) & 1;
#ifdef GEAR_COMMON_ANODE
        digitalWrite(seg_pins[i], on ? LOW : HIGH);
#else
        digitalWrite(seg_pins[i], on ? HIGH : LOW);
#endif
    }
}

void gearDisplayInit()
{
    for (int i = 0; i < 7; i++) {
        pinMode(seg_pins[i], OUTPUT);
#ifdef GEAR_COMMON_ANODE
        digitalWrite(seg_pins[i], HIGH); // off
#else
        digitalWrite(seg_pins[i], LOW);  // off
#endif
    }
    writeSegments(font[0]);
}

void gearDisplayUpdate(int gear)
{
    if (gear < 0) gear = 0;
    if (gear > 9) gear = 9;
    writeSegments(font[gear]);
}

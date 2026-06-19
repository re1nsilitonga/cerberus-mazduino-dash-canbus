#include "GearDisplay.h"

#define SEG_A 13
#define SEG_B 14
#define SEG_C 21
#define SEG_D 22
#define SEG_E 25
#define SEG_F 26
#define SEG_G 27

static const uint8_t seg_pins[] = { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G };

/*  Segment encoding (common cathode: HIGH = on)
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

/* N = segments A, B, C, E, F (no middle bar, no bottom bar) */
static const uint8_t font_N = 0b0110111;

static void writeSegments(uint8_t bits)
{
    for (int i = 0; i < 7; i++) {
        digitalWrite(seg_pins[i], (bits >> i) & 1);
    }
}

void gearDisplayInit()
{
    for (int i = 0; i < 7; i++) {
        pinMode(seg_pins[i], OUTPUT);
        digitalWrite(seg_pins[i], LOW);
    }
    writeSegments(font_N);
}

void gearDisplayUpdate(int gear)
{
    if (gear <= 0 || gear > 9)
        writeSegments(font_N);
    else
        writeSegments(font[gear]);
}

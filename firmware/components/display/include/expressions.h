#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <stdint.h>

typedef uint8_t Matrix8x8[8];

typedef Matrix8x8 Leye[2];
typedef Matrix8x8 Reye[2];
typedef Matrix8x8 Lmouth[4];
typedef Matrix8x8 Rmouth[4];
typedef Matrix8x8 Lnose[1];
typedef Matrix8x8 Rnose[1];

typedef struct {
    Leye left_eye;
    Reye right_eye;
    Lmouth left_mouth;
    Rmouth right_mouth;
    Lnose left_nose;
    Rnose right_nose;
} StaticFace_t;

typedef Matrix8x8 FaceBuffer[14];




#endif
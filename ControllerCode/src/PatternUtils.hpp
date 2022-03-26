#ifndef __INCLUDED_PATTERN_UTILS__
#define __INCLUDED_PATTERN_UTILS__


#include "FastLED.h"

#define MATRIX_POINT_COUNT 30
#define MATRIX_PERIOD 25

#define SPIRAL_PERIOD 25

class  PatternUtils {

public:

    static void RainbowBarf(CRGB* leds, size_t arraySize);

    static void MatrixAnimation(CHSV* hsv_leds, size_t arraySize);

    static void ScreenTest(CRGB* leds, size_t arraySize);

    static void Spiral(CHSV* hsv_leds, size_t arraySize);

    static void hsv_linfade(byte amount, CHSV* hsv_leds);

protected:

    struct point {
        int8_t x = 0;
        int8_t y = 0;
        bool alive = false;
    };

};


#endif
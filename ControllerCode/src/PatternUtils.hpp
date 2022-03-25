#ifndef __INCLUDED_PATTERN_UTILS__
#define __INCLUDED_PATTERN_UTILS__


#include "FastLED.h"

#define MATRIX_POINT_COUNT 15
#define MATRIX_FPS 30

class  PatternUtils {

public:

    static void RainbowBarf(CRGB* leds, size_t arraySize);

    static void MatrixAnimation(CHSV* hsv_leds, size_t arraySize);

protected:

    struct point {
        int8_t x = 0;
        int8_t y = 0;
        bool alive = false;
    };

};


#endif
#ifndef __INCLUDED_PATTERN_UTILS__
#define __INCLUDED_PATTERN_UTILS__

#include "AnimatedGIF.h"
#include "MatrixHelper.hpp"
#include "FastLED.h"
#include "GifDecoder.h"
#include "defs.hpp"
#include "gimpbitmap.h"

#define MATRIX_POINT_COUNT 30
#define MATRIX_PERIOD 25

#define SPIRAL_PERIOD 25

class CPatternUtils {
   public:
    static void Initialise(CRGB* rgb_leds, CHSV* hsv_leds, size_t screenWidth,
                           size_t screenHeight);

    static void RainbowBarf();

    static void MatrixAnimation();

    static void ScreenTest();

    static void Spiral();

    static void DisplayImage();

    static void hsv_linfade(byte amount);

   protected:
    struct point {
        int8_t x = 0;
        int8_t y = 0;
        bool alive = false;
    };

    static CRGB* _rgb_leds;
    static CHSV* _hsv_leds;
    static int _screenWidth;
    static int _screenHeight;

    static GifDecoder<X_NUM, Y_NUM, 12> _decoder;

    static void drawBitmap(uint16_t x, uint16_t y,
                           const gimp32x32bitmap* bitmap);
    static void drawPixel(uint16_t x, uint16_t y, CRGB pixel);

    // Writes the RGB array data to the display
    //
    static void ShowRGB() { FastLED.show(); }

    // Transfers the HSV data values into the RGB array and updates the display.
    static void ShowHSV() {
        for (unsigned int i = 0; i < NUM_LEDS; i++) {
            _rgb_leds[i] = _hsv_leds[i];
        }
        ShowRGB();
    }

    static void screenClearCallback(void) {
        // Clear the screen
        FastLED.clear(true);
    }
    static void updateScreenCallback(void) { FastLED.show(); }
    static void drawPixelCallback(int16_t x, int16_t y, uint8_t red,
                                  uint8_t green, uint8_t blue) {
        _rgb_leds[CMatrixHelper::XY(x, y)] = CRGB(red, green, blue);
    }
};

#endif
#ifndef __INCLUDED_PATTERN_UTILS__
#define __INCLUDED_PATTERN_UTILS__

#ifdef SD_CARD_SUPPORT
#include "AnimatedGIF.h"
#include "GifDecoder.h"
#include "gimpbitmap.h"
#endif

#include "defs.hpp"
#include "FastLED.h"
#include "MatrixHelper.hpp"
#include "Patterns.hpp"
#include "KeyDecoder.hpp"

#define MATRIX_POINT_COUNT 20
#define MATRIX_PERIOD 35

#define SPIRAL_PERIOD 0

class CPatternUtils {
   public:
    static void Initialise(CRGB* rgb_leds, CHSV* hsv_leds, size_t screenWidth,
                           size_t screenHeight);

    static void RainbowBarf();

    static void MatrixAnimation();

    static void ScreenTest();

    static void Spiral(uint8_t x_offset = 0, uint8_t y_offset = 0);

    static void Eyeball(uint16_t x_offset, uint16_t y_offset, CKeyDecoder::T_KEY_STATE_STRUCT activeKeys);
#ifdef SD_CARD_SUPPORT
    static void DisplayImage(int index);

    static void DisplayGif(int index);
#endif
    static void hsv_linfade(byte amount);

   protected:
    struct point {
        int8_t x = 0;
        int8_t y = 0;
        bool alive = false;
    };
    
    enum EYE_STATE {
        EYE_NORMAL,
        EYE_ANGRY,
        EYE_LOVE
    };

    enum MOUTH_STATE {
        MOUTH_NORMAL,
        MOUTH_CURIOUS,
        MOUTH_SHOCK
    };

    static CRGB* _rgb_leds;
    static CHSV* _hsv_leds;
    static int _screenWidth;
    static int _screenHeight;
#ifdef SD_CARD_SUPPORT
    static GifDecoder<X_NUM, Y_NUM, 12> _decoder;

    static char bmpStore[];
    static void drawBitmap(int16_t x, int16_t y, char* bitmap);
#endif
    
    static void drawPixel(int16_t x, int16_t y, CRGB pixel);
    static void drawSprite(const int16_t x, const int16_t y, const uint16_t x_dim, const uint16_t y_dim, void* sprite, CRGB pixel, bool flipHorizontal = false, bool flipVertical = false);
    static void applyMask(const int16_t x, const int16_t y, const uint16_t x_dim, const uint16_t y_dim, void* sprite, CRGB pixel, bool flipHorizontal = false, bool flipVertical = false);
    static bool getSpriteBit(const int16_t x, const int16_t y, const uint16_t x_dim, const uint16_t y_dim, void* sprite, bool flipHorizontal = false, bool flipVertical = false);

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
        _rgb_leds[CMatrixHelper::XY(x, (_screenHeight -1 ) - y)] = CRGB(red, green, blue);
    }
};

#endif
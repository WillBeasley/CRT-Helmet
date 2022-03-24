

#include "PatternUtils.hpp"
#include "MatrixHelper.hpp"
#include "defs.hpp"


void PatternUtils::RainbowBarf(CRGB* leds, size_t arraySize){
    uint32_t ms = millis();
    int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / X_NUM));
    int32_t xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / Y_NUM));

    uint8_t lineStartHue = ms / 65536;
    for( uint8_t y = 0; y < Y_NUM; y++) {
        lineStartHue += yHueDelta32 / 32768;
        uint8_t pixelHue = lineStartHue;      
        for( uint8_t x = 0; x < X_NUM; x++) {
            pixelHue += xHueDelta32 / 32768;
            leds[ CMatrixHelper::XY(x, y)]  = CHSV( pixelHue, 255, 255);
        }
    }

}

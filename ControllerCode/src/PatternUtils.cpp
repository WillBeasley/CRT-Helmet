

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

void PatternUtils::MatrixAnimation(CHSV* hsv_leds, size_t arraySize){
    static uint32_t prevTime = millis();
    static point points[MATRIX_POINT_COUNT];
    uint32_t entryTime = millis();

    const uint32_t timeDelta = entryTime - prevTime;

    // Make sure we wait the minimum interval before generating new frames
    //
    if (timeDelta < (1000/MATRIX_FPS)){
        return;
    }

    // Globally blead the colour to black
    for (int dot = 0; dot < NUM_LEDS; dot++) {
      hsv_leds[dot].value *= 0.8; // Should be reducing brightness down to zero, 
      hsv_leds[dot].saturation += (256 - hsv_leds[dot].saturation) * 0.5;
    }

    for (int i = 0; i < MATRIX_POINT_COUNT; i++) {
        if (points[i].alive) {
            points[i].y--;
            if (points[i].y >= 0) {
                hsv_leds[CMatrixHelper::XY(points[i].x, points[i].y)] = CHSV(105, 100, 255);
            }
            else {
                points[i].alive = false;
            }
        }
        else if (random8() < 32) {
            points[i].x = random8(X_NUM);
            points[i].y = Y_NUM;
            points[i].alive = true;
        }
    }

    // Make not of what millis() was at the start of this cycle. 
    // This will be used in the next cycle to throttle time passage
    //
    prevTime = entryTime;



}





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
    if (timeDelta < (MATRIX_PERIOD)){
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

void PatternUtils::ScreenTest(CRGB* leds, size_t arraySize){
    static bool toggle = false;
    static uint32_t prevTime = millis();
    uint32_t entryTime = millis();

    const uint32_t timeDelta = entryTime - prevTime;

    if (timeDelta < 100){
        return;
    }

    for(int i= 0; i < X_NUM; i++){
        if (toggle){
            leds[CMatrixHelper::XY(i, 0)] = CRGB::Black;
        }else{
            leds[CMatrixHelper::XY(i, 0)] = CRGB::White;
        }
    }

    toggle = !toggle;

    // Make not of what millis() was at the start of this cycle. 
    // This will be used in the next cycle to throttle time passage
    //
    prevTime = entryTime;

}

void PatternUtils::hsv_linfade(byte amount, CHSV* hsv_leds) { // improve with reference to led?
  for (int dot = 0; dot < NUM_LEDS; dot++) {
    if (hsv_leds[dot].value <= amount) {
      hsv_leds[dot].value = amount;
    }
    hsv_leds[dot].value = hsv_leds[dot].value - amount;
  }
}

void PatternUtils::Spiral(CHSV* hsv_leds, size_t arraySize){
    static uint32_t prevTime = millis();
    uint32_t entryTime = millis();
    const uint32_t timeDelta = entryTime - prevTime;

   static int x;
    static int y;
    static float magnitude;
    static float piOverFour = 3.14159265 / 4;
    static float rotation = 0;
    static float magnitudes[160];
    static int i = 0;
    static float n;
    static int progress = 0;
    static bool newCMD = true;
    if (newCMD){
        for (int dot = 0; dot < NUM_LEDS; dot++) {
            hsv_leds[dot].saturation = 200;
            hsv_leds[dot].value = 0;
        }

        for (float n = 1; n < 17; n += 0.1) {
            magnitudes[i] = pow(1.5, n/2);
            i++;
        }
    }
    newCMD = false;

    // Make sure we wait the minimum interval before generating new frames
    //
    if (timeDelta < (SPIRAL_PERIOD)){
        return;
    }

       hsv_linfade(8, hsv_leds);
   rotation += 0.05;
   if (rotation > (TAU)) {
       rotation -= TAU;
   }

    for (n = 0; n < 16; n += 0.1) {
        i = (i + 1) % 160;
        magnitude = magnitudes[i];
        x = magnitude * cos(n - rotation) + 19;
        y = magnitude * sin(n - rotation) + 14;

        if ((x >= 0) && (x < X_NUM) && (y >= 0) && (y < Y_NUM)) {
            if (hsv_leds[CMatrixHelper::XY(x,y)].value < 230) {
                hsv_leds[CMatrixHelper::XY(x,y)].value += 25;
            }
            else {
                hsv_leds[CMatrixHelper::XY(x,y)].value = 255;
            }
        hsv_leds[CMatrixHelper::XY(x,y)].hue = progress / 8 - n * 8;
        }
    }
    //CRGB rgb_leds[NUM_LEDS];
   // for(int i = 0; i < NUM_LEDS; i++){
    //    rgb_leds[i] = hsv_leds[i];
   // }

   // FastLED.show();
  //  hsv_linfade(8, hsv_leds);
  //  rotation += 0.05;
   // if (rotation > (TAU)) {
   //     rotation -= TAU;
   // }
    progress++;
   // delay(1);

    // Make not of what millis() was at the start of this cycle. 
    // This will be used in the next cycle to throttle time passage
    //
    prevTime = entryTime;

}



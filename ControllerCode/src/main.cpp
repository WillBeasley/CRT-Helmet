#include <Arduino.h>
#include "defs.hpp"
#include <FastLED.h>
#include "MatrixHelper.hpp"

// One big block of CRGB values for all the leds
CRGB leds[NUM_LEDS];

void setup() {
  // Setup code runs once at startup

  // If we have USB UART then start it
  //
  Serial.begin(115200);

  // Create the FastLed controllers, one per pin
  //
  FastLED.addLeds<WS2812B,CH1_PIN>(leds, CH1_OFFSET, CH1_NUM);
  FastLED.addLeds<WS2812B,CH2_PIN>(leds, CH2_OFFSET, CH2_NUM);
  FastLED.addLeds<WS2812B,CH3_PIN>(leds, CH3_OFFSET, CH3_NUM);
  FastLED.addLeds<WS2812B,CH4_PIN>(leds, CH4_OFFSET, CH4_NUM);
  FastLED.addLeds<WS2812B,CH5_PIN>(leds, CH5_OFFSET, CH5_NUM);
  FastLED.addLeds<WS2812B,CH6_PIN>(leds, CH6_OFFSET, CH6_NUM);
  FastLED.addLeds<WS2812B,CH7_PIN>(leds, CH7_OFFSET, CH7_NUM);
  FastLED.addLeds<WS2812B,CH8_PIN>(leds, CH8_OFFSET, CH8_NUM);
  FastLED.addLeds<WS2812B,CH9_PIN>(leds, CH9_OFFSET, CH9_NUM);
  FastLED.addLeds<WS2812B,CH10_PIN>(leds, CH10_OFFSET, CH10_NUM);

    
}

void loop() {
  // Initial loop code just turns each pixel on one by one. 
  for (int i=0; i < Y_NUM ; i++){
    for (int j=0; j < X_NUM ; j++){
      leds[CMatrixHelper::XY(j, i)] = CRGB::Red;
      FastLED.show();
      leds[CMatrixHelper::XY(j, i)] = CRGB::Black;
      delay(100);
    }
  }
}
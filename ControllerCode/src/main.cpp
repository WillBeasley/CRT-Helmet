#include <Arduino.h>

#define FASTLED_ESP32_I2S
#include <FastLED.h>

#define NUM_PINS 10
const int LED_COUNT[NUM_PINS] = { 4, 4, 4, 4, 3, 3, 3, 3};
const int PINS[NUM_PINS] = { 1, 2, 3, 4, 5, 6, 7, 8};
#define X_DIM 38
#define Y_DIM 28

#define NUM_LEDS (X_DIM * Y_DIM)


CRGB leds[NUM_LEDS];

void setup() {
  // put your setup code here, to run once:  
  int running_total = 0;
  FastLED.addLeds<WS2812B,1>(leds, running_total, LED_COUNT[0]);
  running_total += LED_COUNT[0];
  FastLED.addLeds<WS2812B,2>(leds, running_total, LED_COUNT[1]);
  running_total += LED_COUNT[1];
  FastLED.addLeds<WS2812B,3>(leds, running_total, LED_COUNT[2]);
  running_total += LED_COUNT[2];
  FastLED.addLeds<WS2812B,4>(leds, running_total, LED_COUNT[3]);
  running_total += LED_COUNT[3];
  FastLED.addLeds<WS2812B,5>(leds, running_total, LED_COUNT[4]);
  running_total += LED_COUNT[4];
  FastLED.addLeds<WS2812B,13>(leds, running_total, LED_COUNT[5]);
  running_total += LED_COUNT[5];
  FastLED.addLeds<WS2812B,14>(leds, running_total, LED_COUNT[6]);
  running_total += LED_COUNT[6];
  FastLED.addLeds<WS2812B,15>(leds, running_total, LED_COUNT[7]);
  running_total += LED_COUNT[7];
  
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
    FastLED.show();
    leds[i] = CRGB::Black;
    delay(100);
  }
}
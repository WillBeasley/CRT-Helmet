#include <Arduino.h>
#include "defs.hpp"
#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>
#include "MatrixHelper.hpp"
#include "ESPNowUtils.hpp"
#include "PatternUtils.hpp"

// One big block of CRGB values for all the leds
CRGB rgb_leds[NUM_LEDS];
CHSV hsv_leds[NUM_LEDS];

// Packet of infomation to be sent to main controller. Just send the keys no extra info.
ControlDataStruct controlData;


void setup() {
  // If we have USB UART then start it
  //
  Serial.begin(115200);

  // Initialise the ESPNow functionality to update the controlData struct with controller info
  memset(&controlData, 0, sizeof(controlData));
  ESPNowUtils::InitESPNow(&controlData);

  // Set software limits on current draw, this should be adjusted for your system
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 8000);

  // Fix at 50% brightness
  //FastLED.setBrightness(156);
    FastLED.setMaxRefreshRate(120);

  // Create the FastLed controllers, one per pin. See defs.hpp for pin definitions.
  //
  FastLED.addLeds<NEOPIXEL,CH1_PIN>(rgb_leds, CH1_OFFSET, CH1_NUM);
  FastLED.addLeds<NEOPIXEL,CH2_PIN>(rgb_leds, CH2_OFFSET, CH2_NUM);
  FastLED.addLeds<NEOPIXEL,CH3_PIN>(rgb_leds, CH3_OFFSET, CH3_NUM);
  FastLED.addLeds<NEOPIXEL,CH4_PIN>(rgb_leds, CH4_OFFSET, CH4_NUM);
  FastLED.addLeds<NEOPIXEL,CH5_PIN>(rgb_leds, CH5_OFFSET, CH5_NUM);
  FastLED.addLeds<NEOPIXEL,CH6_PIN>(rgb_leds, CH6_OFFSET, CH6_NUM);
  FastLED.addLeds<NEOPIXEL,CH7_PIN>(rgb_leds, CH7_OFFSET, CH7_NUM);
  FastLED.addLeds<NEOPIXEL,CH8_PIN>(rgb_leds, CH8_OFFSET, CH8_NUM);
  FastLED.addLeds<NEOPIXEL,CH9_PIN>(rgb_leds, CH9_OFFSET, CH9_NUM);
  FastLED.addLeds<NEOPIXEL,CH10_PIN>(rgb_leds, CH10_OFFSET, CH10_NUM);
  
  // Make sure we clear the display at startup just in case
  FastLED.clear();
    
}



void loop() {

    //PatternUtils::RainbowBarf(leds, sizeof(leds));

    PatternUtils::MatrixAnimation(hsv_leds, sizeof(hsv_leds));


    for(int i = 0; i < NUM_LEDS; i++){
        rgb_leds[i] = hsv_leds[i];
    }

    FastLED.show();

}
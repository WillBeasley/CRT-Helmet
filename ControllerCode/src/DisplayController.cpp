#include "DisplayController.hpp"

#include "PatternUtils.hpp"
#include "gimpbitmap.h"

// Intialise statics
CRGB CDisplayController::rgb_leds[NUM_LEDS];
CHSV CDisplayController::hsv_leds[NUM_LEDS];

uint32_t CDisplayController::activeKeys = 0u;
uint16_t CDisplayController::joyXPos = 0u;
uint16_t CDisplayController::joyYPos = 0u;
// Initialise the display controller
//
void CDisplayController::InitialiseController() {
    // Set software limits on current draw, this should be adjusted for your
    // system
    FastLED.setMaxPowerInVoltsAndMilliamps(LED_VOLTAGE, LED_POWER_mA);

    // Fix at 50% brightness
    FastLED.setBrightness(SCREEN_BRIGHTNESS);
    // FastLED.setMaxRefreshRate(60);

    // Create the FastLed controllers, one per pin. See defs.hpp for pin
    // definitions.
    //
    FastLED.addLeds<NEOPIXEL, CH1_PIN>(rgb_leds, CH1_OFFSET, CH1_NUM);
    FastLED.addLeds<NEOPIXEL, CH2_PIN>(rgb_leds, CH2_OFFSET, CH2_NUM);
    FastLED.addLeds<NEOPIXEL, CH3_PIN>(rgb_leds, CH3_OFFSET, CH3_NUM);
    FastLED.addLeds<NEOPIXEL, CH4_PIN>(rgb_leds, CH4_OFFSET, CH4_NUM);
    FastLED.addLeds<NEOPIXEL, CH5_PIN>(rgb_leds, CH5_OFFSET, CH5_NUM);
    FastLED.addLeds<NEOPIXEL, CH6_PIN>(rgb_leds, CH6_OFFSET, CH6_NUM);
    FastLED.addLeds<NEOPIXEL, CH7_PIN>(rgb_leds, CH7_OFFSET, CH7_NUM);
    FastLED.addLeds<NEOPIXEL, CH8_PIN>(rgb_leds, CH8_OFFSET, CH8_NUM);
    FastLED.addLeds<NEOPIXEL, CH9_PIN>(rgb_leds, CH9_OFFSET, CH9_NUM);
    FastLED.addLeds<NEOPIXEL, CH10_PIN>(rgb_leds, CH10_OFFSET, CH10_NUM);

    // Set the rgb array pointers
    //
    CPatternUtils::Initialise(rgb_leds, hsv_leds, X_NUM, Y_NUM);

    // Make sure we clear the display at startup just in case
    //
    FastLED.clear();
}

// Main state machine
//
void CDisplayController::ControllerMain(uint32_t ActiveKeys, uint16_t JoystickX,
                                        uint16_t JoystickY) {
    //CPatternUtils::Spiral();
    //CPatternUtils::DisplayImage(1);

    //delay (5000);

    //CPatternUtils::DisplayImage(0);

    //delay(5000);

    CPatternUtils::DisplayGif(0);

}

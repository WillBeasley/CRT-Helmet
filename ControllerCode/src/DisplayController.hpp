#ifndef __DISPLAY_CONTROLLER_H__
#define __DISPLAY_CONTROLLER_H__

#include <Arduino.h>
#include "defs.hpp"
#include "FastLED.h"
#include "KeyDecoder.hpp"

#ifndef X_NUM
#error SCREEN WIDTH MUST BE DEFINED
#endif

#ifndef Y_NUM
#error SCREEN HEIGHT MUST BE DEFINED
#endif

#ifndef SCREEN_BRIGHTNESS
#define SCREEN_BRIGHTNESS 128
#endif

#ifndef LED_VOLTAGE
#define LED_VOLTAGE 5
#endif

#ifndef LED_POWER_mA
#define LED_POWER_mA 8000
#endif

#ifndef REMOTE_THROTTLE_MS
#define REMOTE_THROTTLE_MS 100
#endif

class CDisplayController{
public:

    enum CONTROLLER_STATE{
        ANIMATION_MIN,
        SPIRAL,
        MATRIX,
        RAINBOW_BARF,
        SCREEN_TEST,
        ANIMATION_MAX,
        GAME_1,
        CONTROLLER_MAX
    };

    static void InitialiseController();

    static void ControllerMain(uint32_t ActiveKeys, uint16_t JoystickX, uint16_t JoystickY);

    static void UpdateState();

    static void UpdateBrightness();

    static void ProcessState();

protected:

    CRGB static rgb_leds[NUM_LEDS];
    CHSV static hsv_leds[NUM_LEDS];

    // Packet of infomation to be sent to main controller. Just send the keys no extra info.
    //
    static CKeyDecoder::T_KEY_STATE_STRUCT activeKeys;
    static uint16_t joyXPos;
    static uint16_t joyYPos;

    static int ControllerState;
    static int Brightness;

};

#endif
#ifndef __DISPLAY_CONTROLLER_H__
#define __DISPLAY_CONTROLLER_H__

#include <Arduino.h>
#include "defs.hpp"
#include "FastLED.h"

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

class CDisplayController{
public:
    enum CONTROLLER_STATE{
        ANIMATION,
        GAME        
    };

    enum ANIMATIONS_STATE{
        SPIRAL,
        MATRIX,
        RAINBOW_BARF,
        SCREEN_TEST
    };

    static void InitialiseController();

    static void ControllerMain(uint32_t ActiveKeys, uint16_t JoystickX, uint16_t JoystickY);

    static void AnimationStateMachine();

    static void GameStateMachine();

protected:

    CRGB static rgb_leds[NUM_LEDS];
    CHSV static hsv_leds[NUM_LEDS];

    // Packet of infomation to be sent to main controller. Just send the keys no extra info.
    //
    uint32_t static activeKeys;
    uint16_t static joyXPos;
    uint16_t static joyYPos;




};

#endif
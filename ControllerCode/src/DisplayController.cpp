#include "DisplayController.hpp"

#include "PatternUtils.hpp"
#include "gimpbitmap.h"
#include "KeyDecoder.hpp"
// Intialise statics
CRGB CDisplayController::rgb_leds[NUM_LEDS];
CHSV CDisplayController::hsv_leds[NUM_LEDS];

CKeyDecoder::T_KEY_STATE_STRUCT CDisplayController::activeKeys;
uint16_t CDisplayController::joyXPos = 0u;
uint16_t CDisplayController::joyYPos = 0u;

int CDisplayController::ControllerState = CDisplayController::ANIMATION_MIN + 1;

int CDisplayController::Brightness = SCREEN_BRIGHTNESS;

// Initialise the display controller
//
void CDisplayController::InitialiseController() {
    // Set software limits on current draw, this should be adjusted for your
    // system
    FastLED.setMaxPowerInVoltsAndMilliamps(LED_VOLTAGE, LED_POWER_mA);

    // Fix at 50% brightness
    FastLED.setBrightness(SCREEN_BRIGHTNESS);

    FastLED.setCorrection(TypicalSMD5050);
    FastLED.setDither(BINARY_DITHER);
    
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

    // Get a read on the state of all the keys
    //
    activeKeys = CKeyDecoder::GetButtonStates(ActiveKeys);
    joyXPos = JoystickX;
    joyYPos = JoystickY;

    // Determine if there has beem any updates to the brightness requested
    //
    UpdateBrightness();

    // Update what state we are in based off of key press
    //
    UpdateState();

    // Process the current state
    //
    ProcessState();
}

void CDisplayController::UpdateBrightness(){
    static CKeyDecoder::T_KEY_STATE_STRUCT LastStates;
    if (activeKeys.L && !LastStates.L){
        Brightness += 20;
    }

    if (activeKeys.P && !LastStates.P){
        Brightness -= 20;
    }

    if (Brightness > 255){
        Brightness = 255;
    }
    if (Brightness < 10){
        Brightness = 10;
    }

    FastLED.setBrightness(Brightness);
    LastStates = activeKeys;

}

void CDisplayController::UpdateState(){


    static CKeyDecoder::T_KEY_STATE_STRUCT lastKeyStates = activeKeys;
    int newControllerState = ControllerState;

    // Each game has its own keymap, so if we are in a "Game" State, there is only 1 button we want to monitor
    // If we are in a game, then we want to go to the initial animation state, opposite if not.
    //
    if (activeKeys.M && !lastKeyStates.M){
        if (ControllerState > ANIMATION_MAX && ControllerState < CONTROLLER_MAX){
            ControllerState = ANIMATION_MIN + 1;
        }else{
            ControllerState = ANIMATION_MAX + 1;
        }
        lastKeyStates = activeKeys;
        return;
    }
    
    // A Button - Move to the previous animation
    //
    if (activeKeys.A && !lastKeyStates.A){
            newControllerState--;
    }
    // B Button - Move to the next animation
    //
    else if (activeKeys.B && !lastKeyStates.B){
            newControllerState++;
        
    }
    else if (activeKeys.C && !lastKeyStates.C){
        newControllerState = SPIRAL;
    }
    else if (activeKeys.D && !lastKeyStates.D){
        newControllerState = MATRIX;
    }
    else if (activeKeys.E && !lastKeyStates.E){
        newControllerState = RAINBOW_BARF;
    }
    // Limit the switching to the animation range of states
    //
    if (newControllerState <= ANIMATION_MIN){
        newControllerState = ANIMATION_MIN + 1;
    }else if (newControllerState >= ANIMATION_MAX){
        newControllerState = ANIMATION_MAX-1;
    }

    lastKeyStates = activeKeys;
    ControllerState = newControllerState;
}

void CDisplayController::ProcessState(){
    switch (ControllerState){
        case RAINBOW_BARF:
            CPatternUtils::RainbowBarf();
        break;
        case MATRIX:
            CPatternUtils::MatrixAnimation();
        break;
        case SPIRAL:
            CPatternUtils::Spiral();
        break;
        default:
            ControllerState = ANIMATION_MIN + 1;
        break;
    }
}

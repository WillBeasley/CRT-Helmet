#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <esp_now.h>

#include "AnimatedGIF.h"
#include "DisplayController.hpp"
#include "ESPNowUtils.hpp"
#include "GifDecoder.h"
#include "MatrixHelper.hpp"
#include "PatternUtils.hpp"
#include "SDUtils.hpp"
#include "defs.hpp"
#include "gimpbitmap.h"

CRGB rgb_leds[NUM_LEDS];
CHSV hsv_leds[NUM_LEDS];

ControlDataStruct controlData;



void setup() {
    // If we have USB UART then start it
    //
    Serial.begin(115200);

    // Attempt to initialise SD hardware
    if (!CSDUtils::Initialise()){
        //return;
    }

    // Initialise the ESPNow functionality to update the controlData struct with
    // controller info
    memset(&controlData, 0, sizeof(controlData));
    ESPNowUtils::InitESPNow(&controlData);

    // We want to intialise the controller first, because we want to be able to
    // provide a pointer to where to send controller data
    //
    CDisplayController::InitialiseController();
}

void loop() {
    // Call into the controller main loop, pass in the latest control data.
    // Decouples the ESPNowUtils from CDisplayController.
    //
    CDisplayController::ControllerMain(
        controlData.ActiveKeys, controlData.JoyStickX, controlData.JoyStickY);
}
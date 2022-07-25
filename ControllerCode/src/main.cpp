#include "defs.hpp"
#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <esp_now.h>

#include "DisplayController.hpp"
#include "ESPNowUtils.hpp"
#include "MatrixHelper.hpp"
#include "PatternUtils.hpp"


#ifdef SD_CARD_SUPPORT
#include "SDUtils.hpp"
#include "AnimatedGIF.h"
#include "GifDecoder.h"
#include "gimpbitmap.h"
#endif


// Format of incoming ESP-NOW packets from the remote
ControlDataStruct controlData;


void setup() {
    // If we have USB UART then start it
    //
    Serial.begin(115200);

#ifdef SD_CARD_SUPPORT
    // Attempt to initialise SD hardware
    if (!CSDUtils::Initialise()){
        //return;
    }
#endif
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
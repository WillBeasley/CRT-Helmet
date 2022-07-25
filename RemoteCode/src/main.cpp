#include <Arduino.h>
#include <Keypad.h>
#include <WiFi.h>
#include <esp_now.h>

#include "pins_arduino.h"
#include "porting.h"



// Maximum transmission period of 50ms
static constexpr auto CommsTransmitRate = 50u;

// Wifi comms channel
static constexpr auto CommsChannel = 1u; 

// PCB LED for the controller
constexpr auto LED_PIN = 5u;
constexpr auto LED_OFF = HIGH;
constexpr auto LED_ON = LOW;

// Storage for ESP-NOW peer
esp_now_peer_info_t controller;

// Number of columns and rows for the keypad
const byte ROWS = 4;
const byte COLS = 4;

// Define buttons as 0x0-0x0F to make my life easier
char keys[ROWS][COLS] = {
    {0x0C, 0x0D, 0x0E, 0x0F},
    {0x08, 0x09, 0x0A, 0x0B},
    {0x04, 0x05, 0x06, 0x07},
    {0x00, 0x01, 0x02, 0x03}
};

// Define which pins to use for the keypad
byte colPins[COLS] = {25, 33, 32, 4};  
byte rowPins[ROWS] = {26, 27, 14, 12}; 

const auto JoystickSwitchPin = 18;
const auto JoystickXPin = A3;
const auto JoystickYPin = A6;

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Keep track of what keys have been pressed
uint32_t HeldKeys;

// Calibrated midpoints for the XY axis
static double JoystickXMid = 1800.0;
static double JoystickYMid = 1800.0;

// Keep track of when we last did a transmission, so we can restrict power usage
static long LastTransmissionTime;

// State of the PCB LED
static bool LedState = LED_ON;

// Used to form the structure of each ESP-NOW packet to be sent to the screen controller.
struct ControlDataStruct {
    uint32_t ActiveKeys;
    uint32_t JoyStickX;
    uint32_t JoyStickY;
};

// Packet of infomation to be sent to main controller. Just send the keys no
// extra info.
ControlDataStruct controlData;
ControlDataStruct controlDataOld;

// Init ESP Now with fallback
void InitESPNow() {
    WiFi.disconnect();
    if (esp_now_init() == ESP_OK) {
        debugln("ESPNow Init Success");
    } else {
        debugln("ESPNow Init Failed");
        // Simply Restart
        ESP.restart();
    }
}

// Config AP SSID
void configDeviceAP() {
    const char *SSID = "Remote_1";
    bool result = WiFi.softAP(SSID, "Remote_1_Password", CommsChannel, 0);
    if (!result) {
        debugln("AP Config failed.");
    } else {
        debugln("AP Config Success. Broadcasting with AP: " +
                       String(SSID));
    }
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    // debug("\r\nLast Packet Send Status:\t");
    // debugln(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" :
    // "Delivery Fail");
}

// Scan for controllers in AP mode
void ScanForControllers() {
    int8_t scanResults = WiFi.scanNetworks();
    // reset on each scan
    bool controllerFound = 0;
    memset(&controller, 0, sizeof(controller));

    debugln("");
    if (scanResults == 0) {
        debugln("No WiFi devices in AP Mode found");
    } else {
        debug("Found ");
        debug(scanResults);
        debugln(" devices ");
        for (int i = 0; i < scanResults; ++i) {
            // Print SSID and RSSI for each device found
            String SSID = WiFi.SSID(i);
            int32_t RSSI = WiFi.RSSI(i);
            String BSSIDstr = WiFi.BSSIDstr(i);


            delay(10);
            // Check if the current device starts with `Controller`
            if (SSID.indexOf("Controller") == 0) {
                // SSID of interest
                debugln("Found a Controller.");
                debug(i + 1);
                debug(": ");
                debug(SSID);
                debug(" [");
                debug(BSSIDstr);
                debug("]");
                debug(" (");
                debug(RSSI);
                debug(")");
                debugln("");
                // Get BSSID => Mac Address of the Controller
                int mac[6];
                if (6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x", &mac[0],
                                &mac[1], &mac[2], &mac[3], &mac[4], &mac[5])) {
                    for (int ii = 0; ii < 6; ++ii) {
                        controller.peer_addr[ii] = (uint8_t)mac[ii];
                    }
                }

                controller.channel = CommsChannel;  // pick a channel
                controller.encrypt = 0;        // no encryption

                controllerFound = 1;

                // We on
                break;
            }
        }
    }

    if (controllerFound) {
        debugln("Controller Found, processing..");
    } else {
        debugln("Controller Not Found, trying again.");
    }

    // clean up ram
    WiFi.scanDelete();
}


// Check if the controller is already paired.
// If not, pair
bool manageController() {
    if (controller.channel == CommsChannel) {

        // check if the peer exists
        if (esp_now_is_peer_exist(controller.peer_addr)) {
            // Controller already paired.
            return true;
        } else {
            // Controller not paired, attempt pair
            esp_err_t addStatus = esp_now_add_peer(&controller);
            if (addStatus == ESP_OK) {
                // Pair success
                debugln("Pair success");
                return true;
            } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
                // How did we get so far!!
                debugln("ESPNOW Not Init");
                return false;
            } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
                debugln("Invalid Argument");
                return false;
            } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
                debugln("Peer list full");
                return false;
            } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
                debugln("Out of memory");
                return false;
            } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
                debugln("Peer Exists");
                return true;
            } else {
                debugln("Not sure what happened");
                return false;
            }
        }
    } else {
        // No Controller found to process
        debugln("No controller found to process");
        return false;
    }
}

void updateJoystickPos() {
    
    // Constants for the minimum and maximum joystick positions
    //
    const double JoystickMin = 0.00;
    const double JoystickMax = 4095.00;

    // Read the raw values from the HAL
    //
    double rawX = static_cast<double>(analogRead(JoystickXPin));
    double rawY = static_cast<double>(analogRead(JoystickYPin));

    // Special invert of the X-Axis this is a dumb fix to make it so I dont have to update the CRT helmet code.
    //
    rawX = JoystickMax - rawX;

    double percentageX = 0.00;
    double percentageY = 0.00;


    // The target of this function is to get a value between 0-4095 (12 bit), but with a more linear
    // scaling than the raw value ( which will center around 1800 rather than 2048 for some reason)

    // Are we above or below the midpoint?
    //
    if (rawX > JoystickXMid){
        percentageX = (rawX - JoystickXMid) / (JoystickMax - JoystickXMid);
    }else{
        percentageX = -1 + ((rawX - JoystickMin) / (JoystickXMid - JoystickMin));
    }

    if (rawY > JoystickYMid){
        percentageY = (rawY - JoystickYMid) / (JoystickMax - JoystickYMid);
    }else{
        percentageY = -1 + ((rawY - JoystickMin) / (JoystickYMid - JoystickMin));
    }

    // Now we should have a normalised percentage +-100% for the joystick reading
    // But we want a 0-4095 (12 bit) so that the helmet controller knows what we are talking about.
    //

    // If you add 1, the scale becomes 0-2
    percentageX += 1.00;
    percentageY += 1.00;

    // If you divide by 2 the scale becomes 0-1
    percentageX /= 2;
    percentageY /= 2;

    // Multiply this value by the maximum
    percentageX *= 4095;
    percentageY *= 4095;

    // Multiply by 10 for no good reason other than rounding issues
    percentageX *= 10;
    percentageY *= 10;

    controlData.JoyStickX = ((uint16_t)percentageX) / 10;
    controlData.JoyStickY = ((uint16_t)percentageY) / 10;

}

void calibrateJoyStick(){

    // Read the raw values from the analog inputs.
    //
    JoystickXMid = static_cast<double>(analogRead(JoystickXPin));
    JoystickYMid = static_cast<double>(analogRead(JoystickYPin)); 

}

void setup() {
    debugBegin(115200);

    // Set device in AP mode to begin with
    WiFi.mode(WIFI_AP);
    // configure device AP mode
    configDeviceAP();
    // This is the mac address of the Controller in AP Mode
    debug("AP MAC: ");
    debugln(WiFi.softAPmacAddress());
    // Init ESPNow with a fallback logic
    InitESPNow();
    // Once ESPNow is successfully Init, we will register for send CB
    //
    // esp_now_register_send_cb(OnDataSent);

    // Mark the joystick switch pin as input
    pinMode(JoystickSwitchPin, INPUT_PULLUP);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LedState);

    HeldKeys = 0u;
}

void loop() {
    bool isPaired = false;

    // Check for a controller to connect to, update status if we are already
    // connected
    //
    if (controller.channel != CommsChannel) {
        ScanForControllers();
        LedState = !LedState;
        digitalWrite(LED_PIN, LedState);
        delay(100);
    } else {
        isPaired = manageController();
        digitalWrite(LED_PIN, LED_OFF);
    }

    if (isPaired) {
        // Fills kpd.key[ ] array with up-to 10 active keys.
        // Returns true if there are ANY active keys.
        if (kpd.getKeys()) {
            for (int i = 0; i < LIST_MAX; i++) {
                if (kpd.key[i].stateChanged) {
                    switch (
                        kpd.key[i].kstate) {  // Report active key state : IDLE,
                                              // PRESSED, HOLD, or RELEASED
                        case HOLD:
                            HeldKeys |= (1u << kpd.key[i].kchar);
                        case PRESSED:
                            // Set the bit in the ActiveKeys at the offset in
                            // the key
                            controlData.ActiveKeys |= (1u << kpd.key[i].kchar);
                            break;
                        case RELEASED:
                        case IDLE:
                            // De-assert bit
                            controlData.ActiveKeys &= ~(1 << kpd.key[i].kchar);
                            HeldKeys &= ~(1 << kpd.key[i].kchar);
                            break;
                    }
                }
            }
        }

        // Bit 0-15 are keypad switches
        // Bit 16 is joystick switch
        if (!digitalRead(JoystickSwitchPin)) {
            controlData.ActiveKeys |= (1u << 0x10);
        } else {
            controlData.ActiveKeys &= ~(1 << 0x10);
        }

        updateJoystickPos();

        // We have a special mode for re-calibrating the joystick
        // if key N and O are held down, then we are in "calibration mode" where
        // we will read the joystick min/max the midpoint will be chosen to be
        // where the stick is when the buttons are released
        if (((HeldKeys >> 13) && 0x01) && ((HeldKeys >> 14) && 0x01)) {
            calibrateJoyStick();
            return;
        }

        // If a button has been pressed, or we have hit the timeout send a new
        // frame
        if (controlData.ActiveKeys != controlDataOld.ActiveKeys ||
            ((millis()) > LastTransmissionTime + CommsTransmitRate)) {
            LastTransmissionTime = millis();
            const uint8_t *peer_addr = controller.peer_addr;
            esp_err_t result = esp_now_send(peer_addr, (uint8_t *)&controlData,
                                            sizeof(controlData));

        }

        controlDataOld = controlData;
    }

}  // End loop
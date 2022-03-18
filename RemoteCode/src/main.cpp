#include <Arduino.h>
#include <Keypad.h>
#include "pins_arduino.h"
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0
#define COMMS_RATE 50

esp_now_peer_info_t controller;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {0x0C, 0x0D, 0x0E, 0x0F},
  {0x08, 0x09, 0x0A, 0x0B},
  {0x04, 0x05, 0x06, 0x07,},
  {0x00, 0x01, 0x02, 0x03,},
};

byte colPins[COLS] = {25, 33, 32, 17}; //connect to the row pinouts of the kpd
byte rowPins[ROWS] = {26, 27, 14, 12}; //connect to the column pinouts of the kpd
#define JOY_SW_PIN 18
#define JOY_X A3
#define JOY_Y A6

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

long sendTime;

struct ControlDataStruct
{
  uint32_t ActiveKeys;
  int32_t JoyStickX;
  int32_t JoyStickY;
};

// Packet of infomation to be sent to main controller. Just send the keys no extra info.
ControlDataStruct controlData;
ControlDataStruct controlDataOld;

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  const char *SSID = "Remote_1";
  bool result = WiFi.softAP(SSID, "Remote_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Scan for controllers in AP mode
void ScanForControllers() {
  int8_t scanResults = WiFi.scanNetworks();
  // reset on each scan
  bool controllerFound = 0;
  memset(&controller, 0, sizeof(controller));

  Serial.println("");
  if (scanResults == 0) {
    Serial.println("No WiFi devices in AP Mode found");
  } else {
    Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      if (PRINTSCANRESULTS) {
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(SSID);
        Serial.print(" (");
        Serial.print(RSSI);
        Serial.print(")");
        Serial.println("");
      }
      delay(10);
      // Check if the current device starts with `Controller`
      if (SSID.indexOf("Controller") == 0) {
        // SSID of interest
        Serial.println("Found a Controller.");
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
        // Get BSSID => Mac Address of the Controller
        int mac[6];
        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            controller.peer_addr[ii] = (uint8_t) mac[ii];
          }
        }

        controller.channel = CHANNEL; // pick a channel
        controller.encrypt = 0; // no encryption

        controllerFound = 1;
        
        // We on
        break;
      }
    }
  }

  if (controllerFound) {
    Serial.println("Controller Found, processing..");
  } else {
    Serial.println("Controller Not Found, trying again.");
  }

  // clean up ram
  WiFi.scanDelete();
}

void deletePeer() {
  esp_err_t delStatus = esp_now_del_peer(controller.peer_addr);
  Serial.print("Controller Delete Status: ");
  if (delStatus == ESP_OK) {
    // Delete success
    Serial.println("Success");
  } else if (delStatus == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW Not Init");
  } else if (delStatus == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (delStatus == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
  }
}

// Check if the controller is already paired.
// If not, pair
bool manageController() {
  if (controller.channel == CHANNEL) {
    if (DELETEBEFOREPAIR) {
      deletePeer();
    }

    // check if the peer exists
    bool exists = esp_now_is_peer_exist(controller.peer_addr);
    if ( exists) {
      // Controller already paired.
      return true;
    } else {
      // Controller not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(&controller);
      if (addStatus == ESP_OK) {
        // Pair success
        Serial.println("Pair success");
        return true;
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        // How did we get so far!!
        Serial.println("ESPNOW Not Init");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        Serial.println("Invalid Argument");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        Serial.println("Peer list full");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        Serial.println("Out of memory");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        Serial.println("Peer Exists");
        return true;
      } else {
        Serial.println("Not sure what happened");
        return false;
      }
    }
  } else {
    // No Controller found to process
    Serial.println("No controller found to process");
    return false;
  }
}



void setup() {
  Serial.begin(115200);

  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Controller in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for send CB
  //
  esp_now_register_send_cb(OnDataSent);

  // Mark the joystick switch pin as input
  pinMode(JOY_SW_PIN, INPUT_PULLUP);

}


void loop() {

  bool isPaired = false;

  // Check for a controller to connect to, update status if we are already connected
  //
  if (controller.channel != CHANNEL){
    ScanForControllers();
  }else{
    isPaired = manageController();
  }

  if (isPaired){
    // Fills kpd.key[ ] array with up-to 10 active keys.
    // Returns true if there are ANY active keys.
    if (kpd.getKeys()){
      for (int i=0; i<LIST_MAX; i++){   
        if ( kpd.key[i].stateChanged ) {
          switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
            case HOLD:
            case PRESSED:
              // Set the bit in the ActiveKeys at the offset in the key
              controlData.ActiveKeys |= (1u << kpd.key[i].kchar);
              break;
            case RELEASED:
            case IDLE:
              // De-assert bit
              controlData.ActiveKeys &= ~(1 << kpd.key[i].kchar);
              break;
          }
        }
      }
    }

    // Bit 0-15 are keypad switches
    // Bit 16 is joystick switch
    controlData.ActiveKeys |= (!digitalRead(JOY_SW_PIN) << 0x10);

    controlData.JoyStickX = analogRead(JOY_X);
    controlData.JoyStickY = analogRead(JOY_Y);

    // If a button has been pressed, or we have hit the timeout send a new frame
    if (controlData.ActiveKeys != controlDataOld.ActiveKeys || ((millis() )> sendTime+COMMS_RATE) ){
      sendTime = millis();
      const uint8_t *peer_addr = controller.peer_addr;
      esp_err_t result = esp_now_send(peer_addr, (uint8_t*)&controlData, sizeof(controlData));

      #ifdef DEBUG_COMMS
      Serial.print("Send Status: ");
      if (result == ESP_OK) {
        Serial.println("Success");
      } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
        // How did we get so far!!
        Serial.println("ESPNOW not Init.");
      } else if (result == ESP_ERR_ESPNOW_ARG) {
        Serial.println("Invalid Argument");
      } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
        Serial.println("Internal Error");
      } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
        Serial.println("ESP_ERR_ESPNOW_NO_MEM");
      } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
        Serial.println("Peer not found.");
      } else {
        Serial.println("Not sure what happened");
      }
    #endif
    }

    controlDataOld = controlData;

  }


}  // End loop
#include "ESPNowUtils.hpp"
#include <esp_now.h>
#include <WiFi.h>
#include "defs.hpp"
#include "porting.h"

ControlDataStruct* ESPNowUtils::_ctrlPtr = NULL;


void ESPNowUtils::InitESPNow(ControlDataStruct* ctrlPtr) {
    _ctrlPtr = ctrlPtr;

    WiFi.mode(WIFI_AP);

    configDeviceAP();

    debug("AP MAC: "); debugln(WiFi.softAPmacAddress());

    WiFi.disconnect();

    if (esp_now_init() == ESP_OK) {
        debugln("ESPNow Init Success");
    }
    else {
        debugln("ESPNow Init Failed");
        // Retry InitESPNow, add a counte and then restart?
        // InitESPNow();
        // or Simply Restart
        ESP.restart();
    }

    esp_now_register_recv_cb(OnDataRecv);
}

void ESPNowUtils::configDeviceAP() {
  const char *SSID = "Controller_1";
  bool result = WiFi.softAP(SSID, "Controller_1_Password", CHANNEL, 0);
  if (!result) {
    debugln("AP Config failed.");
  } else {
    debugln("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

// callback when data is recv from Master
void ESPNowUtils::OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {

  // Straight up copy the data to struct
  if (data_len == sizeof(ControlDataStruct) && _ctrlPtr != NULL){
    memcpy(_ctrlPtr, data, data_len);
  }
}
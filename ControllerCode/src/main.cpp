#include <Arduino.h>
#include "defs.hpp"
#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>
#include "MatrixHelper.hpp"

// One big block of CRGB values for all the leds
CRGB leds[NUM_LEDS];

struct ControlDataStruct
{
  uint32_t ActiveKeys;
  int32_t JoyStickX;
  int32_t JoyStickY;
};

// Packet of infomation to be sent to main controller. Just send the keys no extra info.
ControlDataStruct controlData;

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
  const char *SSID = "Controller_1";
  bool result = WiFi.softAP(SSID, "Controller_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  // Straight up copy the data to struct
  if (data_len == sizeof(controlData)){
    memcpy(&controlData, data, data_len);
  }
}

void setup() {
  // Setup code runs once at startup

  // If we have USB UART then start it
  //
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);

  configDeviceAP();

  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());

  InitESPNow();

  esp_now_register_recv_cb(OnDataRecv);

  // Create the FastLed controllers, one per pin
  //
  FastLED.addLeds<WS2812B,CH1_PIN>(leds, CH1_OFFSET, CH1_NUM);
  FastLED.addLeds<WS2812B,CH2_PIN>(leds, CH2_OFFSET, CH2_NUM);
  FastLED.addLeds<WS2812B,CH3_PIN>(leds, CH3_OFFSET, CH3_NUM);
  FastLED.addLeds<WS2812B,CH4_PIN>(leds, CH4_OFFSET, CH4_NUM);
  FastLED.addLeds<WS2812B,CH5_PIN>(leds, CH5_OFFSET, CH5_NUM);
  FastLED.addLeds<WS2812B,CH6_PIN>(leds, CH6_OFFSET, CH6_NUM);
  FastLED.addLeds<WS2812B,CH7_PIN>(leds, CH7_OFFSET, CH7_NUM);
  FastLED.addLeds<WS2812B,CH8_PIN>(leds, CH8_OFFSET, CH8_NUM);
  FastLED.addLeds<WS2812B,CH9_PIN>(leds, CH9_OFFSET, CH9_NUM);
  FastLED.addLeds<WS2812B,CH10_PIN>(leds, CH10_OFFSET, CH10_NUM);

    
}

void loop() {
  // Initial loop code just turns each pixel on one by one. 
  for (int i=0; i < Y_NUM ; i++){
    for (int j=0; j < X_NUM ; j++){
      leds[CMatrixHelper::XY(j, i)] = CRGB::Red;
      FastLED.show();
      leds[CMatrixHelper::XY(j, i)] = CRGB::Black;
      delay(100);
    }
  }
}
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

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 10000);
  //FastLED.setBrightness(30);

  // Create the FastLed controllers, one per pin
  //
  FastLED.addLeds<NEOPIXEL,CH1_PIN>(leds, CH1_OFFSET, CH1_NUM);
  FastLED.addLeds<NEOPIXEL,CH2_PIN>(leds, CH2_OFFSET, CH2_NUM);
  FastLED.addLeds<NEOPIXEL,CH3_PIN>(leds, CH3_OFFSET, CH3_NUM);
  FastLED.addLeds<NEOPIXEL,CH4_PIN>(leds, CH4_OFFSET, CH4_NUM);
  FastLED.addLeds<NEOPIXEL,CH5_PIN>(leds, CH5_OFFSET, CH5_NUM);
  FastLED.addLeds<NEOPIXEL,CH6_PIN>(leds, CH6_OFFSET, CH6_NUM);
  FastLED.addLeds<NEOPIXEL,CH7_PIN>(leds, CH7_OFFSET, CH7_NUM);
  FastLED.addLeds<NEOPIXEL,CH8_PIN>(leds, CH8_OFFSET, CH8_NUM);
  FastLED.addLeds<NEOPIXEL,CH9_PIN>(leds, CH9_OFFSET, CH9_NUM);
  FastLED.addLeds<NEOPIXEL,CH10_PIN>(leds, CH10_OFFSET, CH10_NUM);
FastLED.clear();
    
}

void pride() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}
void DrawOneFrame( uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  uint8_t lineStartHue = startHue8;
  for( uint8_t y = 0; y < Y_NUM; y++) {
    lineStartHue += yHueDelta8;
    uint8_t pixelHue = lineStartHue;      
    for( uint8_t x = 0; x < X_NUM; x++) {
      pixelHue += xHueDelta8;
      leds[ CMatrixHelper::XY(x, y)]  = CHSV( pixelHue, 255, 255);
    }
  }
}

void loop() {
   uint32_t ms = millis();
    int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / X_NUM));
    int32_t xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / Y_NUM));
    DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
    if( ms < 5000 ) {
      FastLED.setBrightness( scale8( 100, (ms * 256) / 5000));
    } else {
      FastLED.setBrightness(255);
    }
    FastLED.show();
  // pride();
  // FastLED.show();  
  // Initial loop code just turns each pixel on one by one. 
  // for (int i=0; i < Y_NUM ; i++){
  //  for (int j=0; j < X_NUM ; j++){
  //    leds[CMatrixHelper::XY(j, i)] = CRGB::Red;
  //    FastLED.show();
  //    leds[CMatrixHelper::XY(j, i)] = CRGB::Black;
  //    //delay(100);
  //  }
  // }
  Serial.println(FastLED.getFPS());
  // for(int i = 0; i < NUM_LEDS; i++) {
  //   leds[i] = CRGB::Red;
  //   FastLED.show();
  //   leds[i] = CRGB::Black;
  //   delay(20);
  // }
  // Serial.print(controlData.ActiveKeys);
  // Serial.print("\t\t");
  // Serial.print(controlData.JoyStickX);
  // Serial.print("\t\t");
  // Serial.println(controlData.JoyStickY);
}
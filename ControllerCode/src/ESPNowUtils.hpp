#ifndef __INCLUDED_ESPNOW_UTILS__
#define __INCLUDED_ESPNOW_UTILS__

#include <Arduino.h>

struct ControlDataStruct
{
  uint32_t ActiveKeys;
  int32_t JoyStickX;
  int32_t JoyStickY;
};

class ESPNowUtils{
public:

static void InitESPNow(ControlDataStruct* ctrlPtr);

static void configDeviceAP();

static void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);

protected:

static ControlDataStruct* _ctrlPtr;

};
#endif
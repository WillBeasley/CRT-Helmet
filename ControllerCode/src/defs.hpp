#ifndef __defs_H__
#define __defs_H__

// FastLED I2S support enable
#define FASTLED_ESP32_I2S

// Array Dimensions
#define X_NUM 38
#define Y_NUM 28

// Pin usage definitions
#define CH1_PIN 4
#define CH2_PIN 16
#define CH3_PIN 17
#define CH4_PIN 5
#define CH5_PIN 18
#define CH6_PIN 19
#define CH7_PIN 21
#define CH8_PIN 22
#define CH9_PIN 12
#define CH10_PIN 14

// Channel LED Count ( Basically they all have 3 strips of 38 attached except for channel 9 and 10 with only 2 strips)
#define CH1_NUM (3 * X_NUM)
#define CH2_NUM (3 * X_NUM)
#define CH3_NUM (3 * X_NUM)
#define CH4_NUM (3 * X_NUM)
#define CH5_NUM (3 * X_NUM)
#define CH6_NUM (3 * X_NUM)
#define CH7_NUM (3 * X_NUM)
#define CH8_NUM (3 * X_NUM)
#define CH9_NUM (2 * X_NUM)
#define CH10_NUM (2 * X_NUM)

// Offsets from the base for each channel
#define CH1_OFFSET (0u)
#define CH2_OFFSET (CH1_NUM)
#define CH3_OFFSET (CH2_OFFSET + CH2_NUM)
#define CH4_OFFSET (CH3_OFFSET + CH3_NUM)
#define CH5_OFFSET (CH4_OFFSET + CH4_NUM)
#define CH6_OFFSET (CH5_OFFSET + CH5_NUM)
#define CH7_OFFSET (CH6_OFFSET + CH6_NUM)
#define CH8_OFFSET (CH7_OFFSET + CH7_NUM)
#define CH9_OFFSET (CH8_OFFSET + CH8_NUM)
#define CH10_OFFSET (CH9_OFFSET + CH9_NUM)

#define NUM_LEDS (X_NUM * Y_NUM)


#define CHANNEL 1



#endif
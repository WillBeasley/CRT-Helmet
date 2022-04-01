
#ifndef _SD_UTILS_HPP_
#define _SD_UTILS_HPP_

#include <Arduino.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       -
 *    D3       SS  GPIO5
 *    CMD      MOSI GPIO23
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      SCK GPIO18
 *    VSS      GND
 *    D0       MISO GPIO19
 *    D1       -
 */

#define MAX_GIFS 10
#define MAX_BMPS 10

class CSDUtils {
   public:
    // Init the SD interface
    static bool Initialise();

    static void GetGifs(fs::FS &fs);

    static void GetBMPs(fs::FS &fs);

    static void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
    static void createDir(fs::FS &fs, const char * path);
    static void removeDir(fs::FS &fs, const char * path);
    static void readFile(fs::FS &fs, const char * path);
    static void writeFile(fs::FS &fs, const char * path, const char * message);
    static void appendFile(fs::FS &fs, const char * path, const char * message);
    static void renameFile(fs::FS &fs, const char * path1, const char * path2);
    static void deleteFile(fs::FS &fs, const char * path);
    static void testFileIO(fs::FS &fs, const char * path);

protected:
    static String gif_paths[];
    static String bmp_paths[];
};

#endif

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

    static void GetGifs();

    static void GetBMPs();

    static bool readBitmap(int index, char* dst, size_t bufferSize);

    static void createDir(const char * path);

    static void openGif(String path);

    static void openGifByIndex(int index);

    static bool getGifPath(int index, String &path){
        if (index > MAX_GIFS || gif_paths[index] == ""){
            return false;
        }

        path = gif_paths[index];
        return true;
    }

    static bool getBMPPath(int index, String &path){
        if (index > MAX_BMPS || bmp_paths[index] == ""){
            return false;
        }

        path = bmp_paths[index];
        return true;
    }

    static bool fileSeekCallback(unsigned long position) {
        return file.seek(position);
    }

    static unsigned long filePositionCallback(void) {
        return file.position();
    }

    static int fileReadCallback(void) {
        return file.read();
    }

    static int fileReadBlockCallback(void * buffer, int numberOfBytes) {
        return file.read((uint8_t*)buffer, numberOfBytes);
    }

    static int fileSizeCallback(void) {
        return file.size();
    }


protected:
    static String gif_paths[];
    static String bmp_paths[];

    static File file;

    static int numberOfFiles;
};

#endif
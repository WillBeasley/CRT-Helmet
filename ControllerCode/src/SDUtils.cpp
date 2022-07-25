
#include "defs.hpp"

#ifdef SD_CARD_SUPPORT
#include "SDUtils.hpp"
#include "porting.h"


String CSDUtils::gif_paths[MAX_GIFS];

String CSDUtils::bmp_paths[MAX_BMPS];

File CSDUtils::file;

int CSDUtils::numberOfFiles=0;

bool CSDUtils::Initialise() {
    if (!SD.begin()) {
        debugln("Card Mount Failed");
        return false;
    }
    uint8_t cardType = SD.cardType();
    
    if (cardType == CARD_NONE) {
        debugln("No SD card attached");
        return false;
    }

    debug("SD Card Type: ");
    if (cardType == CARD_MMC) {
        debugln("MMC");
    } else if (cardType == CARD_SD) {
        debugln("SDSC");
    } else if (cardType == CARD_SDHC) {
        debugln("SDHC");
    } else {
        debugln("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    debug("SD Card Size: %lluMB\n", cardSize);

    // Make sure the file structure we want is there
    SD.mkdir("/data");
    SD.mkdir("/data/bitmaps");
    SD.mkdir("/data/gifs");

    GetGifs();
    GetBMPs();

    return true;
}

void CSDUtils::GetGifs(){

    File gifsDir = SD.open("/data/gifs");
    if (!gifsDir){
        return;
    }

    for(int i = 0; i < MAX_GIFS; i++){
        File file = gifsDir.openNextFile();
        if (!file){
            break;
        }
        gif_paths[i] = file.name();

    }

}

void CSDUtils::GetBMPs(){

    File bmpDir = SD.open("/data/bitmaps");
    if (!bmpDir){
        return;
    }

    for(int i = 0; i < MAX_GIFS; i++){
        File file = bmpDir.openNextFile();
        if (!file){
            break;
        }
        bmp_paths[i] = file.name();

    }

}



bool CSDUtils::readBitmap(int index, char* dst, size_t bufferSize){
    String path ="";
    if (!getBMPPath(index, path)){
        return false;
    }

    File target = SD.open(path);

    // Make sure that we have the file asked for, if not return -1 for error
    if (!target){
        target.close();
        return false;
    }

    target.seek(0x0A);

    char offset = target.read();
    
    // Is the target going to fit in the buffer
    if (target.size() - offset > bufferSize){
        target.close();
        return false;
    }
    
    target.seek(offset);
    
    size_t dataLen = target.readBytes(dst, bufferSize);

    target.close();
    // Copy to the target buffer;
    return dataLen;
}

void  CSDUtils::openGif(String path){
    file = SD.open(path);

    if (!file){
        debugln("Cannot open file");
    }

}

void CSDUtils::openGifByIndex(int index){
    
    String path = "";
    openGif(gif_paths[index]);

}


#endif


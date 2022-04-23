

#include "PatternUtils.hpp"

#include "MatrixHelper.hpp"

#include "SDUtils.hpp"

// Variable declarations
CRGB *CPatternUtils::_rgb_leds = NULL;
CHSV *CPatternUtils::_hsv_leds = NULL;
int CPatternUtils::_screenWidth = 0;
int CPatternUtils::_screenHeight = 0;

GifDecoder<X_NUM, Y_NUM, 12> CPatternUtils::_decoder;

// 38 x 28 plus 2 bytes per row is ((38 * 3) + 2) * 28
char CPatternUtils::bmpStore[((38 * 3) + 2) * 28];

void CPatternUtils::Initialise(CRGB *rgb_leds, CHSV *hsv_leds,
                               size_t screenWidth, size_t screenHeight) {
    _rgb_leds = rgb_leds;
    _hsv_leds = hsv_leds;
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;


    // Set callbacks from the decoder to write data into arrays
    _decoder.setScreenClearCallback(screenClearCallback);
    _decoder.setUpdateScreenCallback(updateScreenCallback);
    _decoder.setDrawPixelCallback(drawPixelCallback);

    _decoder.setFileSeekCallback(CSDUtils::fileSeekCallback);
    _decoder.setFilePositionCallback(CSDUtils::filePositionCallback);
    _decoder.setFileReadCallback(CSDUtils::fileReadCallback);
    _decoder.setFileReadBlockCallback(CSDUtils::fileReadBlockCallback);
    _decoder.setFileSizeCallback(CSDUtils::fileSizeCallback);

}

void CPatternUtils::DisplayImage(int index) {
    static int lastIndex = -1;
    if (lastIndex != index){
        CSDUtils::readBitmap(index, bmpStore, sizeof(bmpStore));
        lastIndex = index;
    }
    
    drawBitmap(0,0,bmpStore);
    ShowRGB();


    // if (CSDUtils::getPath(0, &path)){
    //     if (CSDUtils::readBitmap(path.c_str(), bmpBuffer, sizeof(bmpBuffer)) > 0){
    //         drawBitmap(0, 0, &battle_bmp);
    //         ShowRGB();
    //     }
    // }
    
}

void CPatternUtils::DisplayGif(int index){
    static int lastIndex = -1;
    if (lastIndex != index){
        CSDUtils::openGifByIndex(0);
        _decoder.startDecoding();
    }
    
    _decoder.decodeFrame();
    lastIndex = index;
}

void CPatternUtils::RainbowBarf() {
    uint32_t ms = millis();
    int32_t yHueDelta32 =
        ((int32_t)cos16(ms * (27 / 1)) * (350 / _screenWidth));
    int32_t xHueDelta32 =
        ((int32_t)cos16(ms * (39 / 1)) * (310 / _screenHeight));

    uint8_t lineStartHue = ms / 65536;
    for (uint8_t y = 0; y < _screenHeight; y++) {
        lineStartHue += yHueDelta32 / 32768;
        uint8_t pixelHue = lineStartHue;
        for (uint8_t x = 0; x < _screenWidth; x++) {
            pixelHue += xHueDelta32 / 32768;
            _hsv_leds[CMatrixHelper::XY(x, y)] = CHSV(pixelHue, 255, 255);
        }
    }

    ShowHSV();
}

void CPatternUtils::MatrixAnimation() {
    static uint32_t prevTime = millis();
    static point points[MATRIX_POINT_COUNT];
    uint32_t entryTime = millis();

    const uint32_t timeDelta = entryTime - prevTime;

    // Make sure we wait the minimum interval before generating new frames
    //
    if (timeDelta < (MATRIX_PERIOD)) {
        return;
    }

    // Globally blead the colour to black
    for (int dot = 0; dot < (_screenHeight * _screenWidth); dot++) {
        _hsv_leds[dot].value *=
            0.8;  // Should be reducing brightness down to zero,
        _hsv_leds[dot].saturation += (256 - _hsv_leds[dot].saturation) * 0.5;
    }

    for (int i = 0; i < MATRIX_POINT_COUNT; i++) {
        if (points[i].alive) {
            points[i].y--;
            if (points[i].y >= 0) {
                _hsv_leds[CMatrixHelper::XY(points[i].x, points[i].y)] =
                    CHSV(105, 100, 255);
            } else {
                points[i].alive = false;
            }
        } else if (random8() < 32) {
            points[i].x = random8(_screenWidth);
            points[i].y = _screenHeight;
            points[i].alive = true;
        }
    }

    // Make not of what millis() was at the start of this cycle.
    // This will be used in the next cycle to throttle time passage
    //
    prevTime = entryTime;

    ShowHSV();
}

void CPatternUtils::ScreenTest() {
    static bool toggle = false;
    static uint32_t prevTime = millis();
    uint32_t entryTime = millis();

    const uint32_t timeDelta = entryTime - prevTime;

    if (timeDelta < 100) {
        return;
    }

    for (int i = 0; i < _screenWidth; i++) {
        if (toggle) {
            _rgb_leds[CMatrixHelper::XY(i, 0)] = CRGB::Black;
        } else {
            _rgb_leds[CMatrixHelper::XY(i, 0)] = CRGB::White;
        }
    }

    toggle = !toggle;

    // Make not of what millis() was at the start of this cycle.
    // This will be used in the next cycle to throttle time passage
    //
    prevTime = entryTime;

    ShowRGB();
}

void CPatternUtils::hsv_linfade(byte amount) {
    for (int dot = 0; dot < NUM_LEDS; dot++) {
        if (_hsv_leds[dot].value <= amount) {
            _hsv_leds[dot].value = amount;
        }
        _hsv_leds[dot].value = _hsv_leds[dot].value - amount;
    }
}

void CPatternUtils::Spiral(uint8_t x_offset, uint8_t y_offset) {
    static uint32_t prevTime = millis();
    uint32_t entryTime = millis();
    const uint32_t timeDelta = entryTime - prevTime;

    static int x;
    static int y;
    static float magnitude;
    static float piOverFour = 3.14159265 / 4;
    static float rotation = 0;
    static float magnitudes[160];
    static int i = 0;
    static float n;
    static int progress = 0;
    static bool newCMD = true;
    if (newCMD) {
        for (int dot = 0; dot < NUM_LEDS; dot++) {
            _hsv_leds[dot].saturation = 200;
            _hsv_leds[dot].value = 0;
        }

        for (float n = 1; n < 17; n += 0.1) {
            magnitudes[i] = pow(1.5, n / 2);
            i++;
        }
    }
    newCMD = false;

    // Make sure we wait the minimum interval before generating new frames
    //
    if (timeDelta < (SPIRAL_PERIOD)) {
        return;
    }

    hsv_linfade(8);
    rotation += 0.05;
    if (rotation > (TAU)) {
        rotation -= TAU;
    }

    for (n = 0; n < 32; n += 0.1) {
        i = (i + 1) % 160;
        magnitude = magnitudes[i];
        x = magnitude * cos(n - rotation) + 19 + x_offset;
        y = magnitude * sin(n - rotation) + 14 + y_offset;

        if ((x >= 0) && (x < _screenWidth) && (y >= 0) && (y < _screenHeight)) {
            if (_hsv_leds[CMatrixHelper::XY(x, y)].value < 230) {
                _hsv_leds[CMatrixHelper::XY(x, y)].value += 25;
            } else {
                _hsv_leds[CMatrixHelper::XY(x, y)].value = 255;
            }
            _hsv_leds[CMatrixHelper::XY(x, y)].hue = progress / 8 - n * 8;
        }
    }
    // CRGB rgb_leds[NUM_LEDS];
    // for(int i = 0; i < NUM_LEDS; i++){
    //    rgb_leds[i] =_hsv_leds[i];
    // }

    // FastLED.show();
    //  hsv_linfade(8,_hsv_leds);
    //  rotation += 0.05;
    // if (rotation > (TAU)) {
    //     rotation -= TAU;
    // }
    progress++;
    // delay(1);

    // Make not of what millis() was at the start of this cycle.
    // This will be used in the next cycle to throttle time passage
    //
    prevTime = entryTime;

    ShowHSV();
}

void CPatternUtils::drawBitmap(uint16_t x, uint16_t y,
                               char *bitmap) {
    static uint32_t prevTime = millis();
    uint32_t entryTime = millis();
    const uint32_t timeDelta = entryTime - prevTime;

    // If not enought time has passed yet then return fast.
    if (timeDelta < 1000) {
        return;
    }

    for (unsigned int i = 0; i < _screenHeight; i++) {
        for (unsigned int j = 0; j < _screenWidth; j++) {
            // Read the CRGB data from the BMP stream
            CRGB pixel = {
                bitmap
                    [((i) * 116 ) + (3 * j) + 2], //_screenHeight - 1 -
                bitmap
                    [((i) * 116) + (3 * j) + 1],
                bitmap
                    [((i) * 116) + (3 * j) + 0]};

            // Set the pixel
            //
            drawPixel(x + j, (y + i), pixel);
        }
    }

    prevTime = entryTime;
}

void CPatternUtils::drawPixel(uint16_t x, uint16_t y, CRGB pixel) {
    // Set the FastLED CRGB pixel to the target pixel colour
    _rgb_leds[CMatrixHelper::XY(x, y)] = pixel;
}

void CPatternUtils::Eyeball(int8_t x_offset, int8_t y_offset, bool blink){


    // Fill the screen completely white
    //
    for (int i =0; i < NUM_LEDS; i++){
        _rgb_leds[i] = CRGB::Black; //WB change
    }

    drawSprite(5, 1, 28, 26, (void*)socketMask, CRGB::White);

    // Draw the Iris
    //
    drawSprite(12, 2, 14, 25, (void*)iris, CRGB::Blue);


    // Draw the Pupil
    //
    drawSprite(15, 7, 8, 14, (void*)pupil, CRGB::Black);

    // Draw the frame
    //
    applyMask(5, 1, 28, 26, (void*)socketMask, CRGB::Black);
    // Eliminate everything outside the frame
    //

    // Update the screem
    //
    ShowRGB();


}

void CPatternUtils::drawSprite(const uint16_t x, const uint16_t y, const uint16_t x_dim, const uint16_t y_dim, void* sprite, CRGB pixel){
    uint8_t (*spriteArray)[x_dim] = (uint8_t (*)[x_dim]) sprite;
    // Draw the sprite at the coordinates provided
    // Note: Sprites have an origin at top left, whilst our origin is in bottom left
    //
    for (int sprite_y = 0; sprite_y < y_dim; sprite_y++){
        for(int sprite_x = 0; sprite_x < x_dim; sprite_x++){
            // If the value is '1' then we need to colour in this pixel
            if (spriteArray[sprite_y][sprite_x] == 1){
                drawPixel(x + sprite_x, Y_NUM - (y + sprite_y) - 1, pixel);
            }
        }
    }
    
}

void CPatternUtils::applyMask(const uint16_t x, const uint16_t y, const uint16_t x_dim, const uint16_t y_dim, void* sprite, CRGB pixel){
    uint8_t (*spriteArray)[x_dim] = (uint8_t (*)[x_dim]) sprite;

    // Set all pixels that do not fall within the mask to "pixel"
    for (int sprite_y = 0; sprite_y < y_dim; sprite_y++){
        for(int sprite_x = 0; sprite_x < x_dim; sprite_x++){
            // If the value is '0' then we need to colour in this pixel
            if (spriteArray[sprite_y][sprite_x] == 0){
                drawPixel(x + sprite_x, Y_NUM - (y + sprite_y) - 1, pixel);
            }
        }
    }
}
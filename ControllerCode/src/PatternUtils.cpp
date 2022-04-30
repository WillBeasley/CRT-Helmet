

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

void CPatternUtils::drawBitmap(int16_t x, int16_t y,
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

void CPatternUtils::drawPixel(int16_t x, int16_t y, CRGB pixel) {
    // We cant draw pixels outside the borders so if it is then dont bother
    //
    if (x < 0 || y < 0 || x >= X_NUM || y >= Y_NUM){
        return;
    }

    // Set the FastLED CRGB pixel to the target pixel colour
    //
    _rgb_leds[CMatrixHelper::XY(x, y)] = pixel;
}

void CPatternUtils::Eyeball(uint16_t x_offset, uint16_t y_offset, CKeyDecoder::T_KEY_STATE_STRUCT activeKeys){

    static const uint8_t _AnchorLeftX = 4;
    static const uint8_t _AnchorLeftY = 14;
    static const uint8_t _AnchorRightX = 22;
    static const uint8_t _AnchorRightY = 14;
    static const uint8_t _PupilXOffset = 0;//5;
    static const uint8_t _PupilYOffset = 0;//4;
    static const int8_t _xOffsetMax = 4;
    static const int8_t _yOffsetMax = 4;

    static CKeyDecoder::T_KEY_STATE_STRUCT lastKeys = activeKeys;

    static EYE_STATE eyeState = EYE_NORMAL;
    static MOUTH_STATE mouthState = MOUTH_NORMAL;

    static bool blinkActive = false;
    static int blinkIndex = 0;

    if (activeKeys.I){
        eyeState = EYE_NORMAL;
    }
    if (activeKeys.J){
        eyeState = EYE_LOVE;
    }
    if (activeKeys.K){
        eyeState = EYE_ANGRY;
    }
    if (activeKeys.M){
        mouthState = MOUTH_NORMAL;
    }
    if (activeKeys.N){
        mouthState = MOUTH_CURIOUS;
    }
    if (activeKeys.O){
        mouthState = MOUTH_SHOCK;
    }
    
    // Clear the screen
    //
    for (int i =0; i < NUM_LEDS; i++){
        _rgb_leds[i] = CRGB::Black; //WB change
    }

    // Convert the input to a percentage between -1 and 1
    double x = (((double)x_offset) / 2047.5) - 1.0;
    double y = (((double)y_offset) / 2047.5) - 1.0;

    // Convert that percentage to an offset Fn(x) = x^3 is the mapping
    //
    double FnX = pow(x, 1) * 10 * _xOffsetMax;
    double FnY = pow(y, 1) * 10 * _yOffsetMax;

    // Obtain a pixel offset from this
    int32_t finalXOffset = (((int32_t)FnX) / 10);
    int32_t finalYOffset = (((int32_t)FnY) / 10);

    if (activeKeys.JoySW && !lastKeys.JoySW ){
        blinkActive = true;
        blinkIndex = 0;
    }
    
    // If we are blinking then we dont need to draw the eyes
    // 
    if (blinkActive){
        // There are 5 different states of blinking, and we cycle through these a few times over 7 steps
        // 
        switch (blinkIndex)
        {
            case 0:
            case 6:
            {
                // Draw the edges
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)blink_0_aura, CRGB(0x001000));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)blink_0_aura, CRGB(0x001000));

                // Draw the Eye
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)blink_0_colour, CRGB(0x00FF00));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)blink_0_colour, CRGB(0x00FF00));

                // Draw the Pupils
                //
                drawSprite(_AnchorLeftX + finalXOffset + _PupilXOffset + (finalXOffset/2), _AnchorLeftY + finalYOffset + _PupilYOffset + (finalYOffset/2), 12, 10, (void*)eye_pupil, CRGB::White);
                drawSprite(_AnchorRightX + finalXOffset + _PupilXOffset + (finalXOffset/2), _AnchorRightY + finalYOffset + _PupilYOffset + (finalYOffset/2), 12, 10, (void*)eye_pupil, CRGB::White);
                break;
            }
            case 1:
            case 5:
            {
                // Draw the edges
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)blink_1_aura, CRGB(0x001000));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)blink_1_aura, CRGB(0x001000));

                // Draw the Eye
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)blink_1_colour, CRGB(0x00FF00));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)blink_1_colour, CRGB(0x00FF00));

                // Draw the Pupils
                //
                drawSprite(_AnchorLeftX + finalXOffset + _PupilXOffset + (finalXOffset/2), _AnchorLeftY + finalYOffset + _PupilYOffset + (finalYOffset/2), 12, 10, (void*)eye_pupil, CRGB::White);
                drawSprite(_AnchorRightX + finalXOffset + _PupilXOffset + (finalXOffset/2), _AnchorRightY + finalYOffset + _PupilYOffset + (finalYOffset/2), 12, 10, (void*)eye_pupil, CRGB::White);
                break;
            }
            case 2:
            case 4:
            {
                // Draw the edges
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)blink_2_aura, CRGB(0x001000));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)blink_2_aura, CRGB(0x001000));

                // Draw the Eye
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)blink_2_colour, CRGB(0x00FF00));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)blink_2_colour, CRGB(0x00FF00));

                // Draw the Pupils
                //
                drawSprite(_AnchorLeftX + finalXOffset + _PupilXOffset + (finalXOffset/2), _AnchorLeftY + finalYOffset + _PupilYOffset + (finalYOffset/2), 12, 10, (void*)eye_pupil, CRGB::White);
                drawSprite(_AnchorRightX + finalXOffset + _PupilXOffset + (finalXOffset/2), _AnchorRightY + finalYOffset + _PupilYOffset + (finalYOffset/2), 12, 10, (void*)eye_pupil, CRGB::White);
                break;
            }
            case 3:
            {
                // Draw the edges
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)blink_3_aura, CRGB(0x001000));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)blink_3_aura, CRGB(0x001000));

                // Draw the Eye
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)blink_3_colour, CRGB(0x00FF00));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)blink_3_colour, CRGB(0x00FF00));

                break;
            }
        default:
            break;
        }

        blinkIndex++;
        delay(20);

        if (blinkIndex >= 7){
            blinkActive = false;
        }

    }else{    
        switch (eyeState)
        {
        case EYE_NORMAL:
            {
                // Draw the edges
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)eye_aura, CRGB(0x001000));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)eye_aura, CRGB(0x001000));

                // Draw the Eye
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)eye_colour, CRGB(0x00FF00));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)eye_colour, CRGB(0x00FF00));

                // Draw the Pupils
                //
                drawSprite(_AnchorLeftX + finalXOffset + _PupilXOffset + (finalXOffset/2), _AnchorLeftY + finalYOffset + _PupilYOffset + (finalYOffset/2), 12, 10, (void*)eye_pupil, CRGB::White);
                drawSprite(_AnchorRightX + finalXOffset + _PupilXOffset + (finalXOffset/2), _AnchorRightY + finalYOffset + _PupilYOffset + (finalYOffset/2), 12, 10, (void*)eye_pupil, CRGB::White);
                break;
            }        

        case EYE_ANGRY:
            {
                // Draw the edges
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)eye_aura_angry, CRGB(0x001000));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)eye_aura_angry, CRGB(0x001000), true);

                // Draw the Eye
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)eye_colour_angry, CRGB(0x00FF00));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)eye_colour_angry, CRGB(0x00FF00), true);

                // Draw the Pupils
                //
                drawSprite(_AnchorLeftX + finalXOffset + _PupilXOffset + (finalXOffset/2), _AnchorLeftY + finalYOffset + _PupilYOffset + (finalYOffset/2), 12, 10, (void*)eye_pupil, CRGB::White);
                drawSprite(_AnchorRightX + finalXOffset + _PupilXOffset + (finalXOffset/2), _AnchorRightY + finalYOffset + _PupilYOffset + (finalYOffset/2), 12, 10, (void*)eye_pupil, CRGB::White);
                break;
            }

        case EYE_LOVE:
            {
                // Draw the edges
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)eye_aura, CRGB(0x001000));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)eye_aura, CRGB(0x001000));

                // Draw the Eye
                //
                drawSprite(_AnchorLeftX + finalXOffset, _AnchorLeftY + finalYOffset, 12, 10, (void*)eye_colour, CRGB(0x00FF00));
                drawSprite(_AnchorRightX + finalXOffset, _AnchorRightY + finalYOffset, 12, 10, (void*)eye_colour, CRGB(0x00FF00));

                // Draw the Pupils
                //
                drawSprite(_AnchorLeftX + finalXOffset + _PupilXOffset + (finalXOffset/2), _AnchorLeftY + finalYOffset + _PupilYOffset + (finalYOffset/2), 12, 10, (void*)eye_pupil_love, CRGB::DeepPink);
                drawSprite(_AnchorRightX + finalXOffset + _PupilXOffset + (finalXOffset/2), _AnchorRightY + finalYOffset + _PupilYOffset + (finalYOffset/2), 12, 10, (void*)eye_pupil_love, CRGB::DeepPink);
                break;
            }
        
        default:
            break;
        }
    }
    
    switch (mouthState){
        case MOUTH_NORMAL:
        {
            drawSprite(14 + (finalXOffset), 6 + (finalYOffset), 10, 5, (void*)mouth_smile, CRGB(0x00FF00));
            break;
        }
        case MOUTH_CURIOUS:
        {
            drawSprite(14 + (finalXOffset), 6 + (finalYOffset), 10, 5, (void*)mouth_curious, CRGB(0x00FF00));
            break;
        }
        case MOUTH_SHOCK:
        {
            drawSprite(14 + (finalXOffset), 6 + (finalYOffset), 10, 5, (void*)mouth_shock, CRGB(0x00FF00));
            break;
        }
        default:
            break;
    }

    // Update the screem
    //
    ShowRGB();

    lastKeys = activeKeys;


}

void CPatternUtils::drawSprite(const int16_t x, const int16_t y, const uint16_t x_dim, const uint16_t y_dim, void* sprite, CRGB pixel, bool flipHorizontal, bool flipVertical){
    // Draw the sprite at the coordinates provided
    // Note: Sprites have an origin at top left, whilst our origin is in bottom left
    //
    for (int sprite_y = 0; sprite_y < y_dim; sprite_y++){
        for(int sprite_x = 0; sprite_x < x_dim; sprite_x++){
            // If the value is '1' then we need to colour in this pixel
            if (getSpriteBit(sprite_x, sprite_y, x_dim, y_dim, sprite, flipHorizontal, flipVertical)){
                drawPixel(x + sprite_x, (y + sprite_y) , pixel);
            }
        }
    }
    
}

void CPatternUtils::applyMask(const int16_t x, const int16_t y, const uint16_t x_dim, const uint16_t y_dim, void* sprite, CRGB pixel, bool flipHorizontal, bool flipVertical){
    
    // Set all pixels that do not fall within the mask to "pixel"
    for (int sprite_y = 0; sprite_y < y_dim; sprite_y++){
        for(int sprite_x = 0; sprite_x < x_dim; sprite_x++){
            // If the value is '0' then we need to colour in this pixel
            if (!getSpriteBit(sprite_x, sprite_y, x_dim, y_dim, sprite, flipHorizontal, flipVertical)){
                drawPixel(x + sprite_x, (y + sprite_y), pixel);
            }
        }
    }
}


bool CPatternUtils::getSpriteBit(const int16_t x, const int16_t y, const uint16_t x_dim, const uint16_t y_dim, void* sprite, bool flipHorizontal, bool flipVertical){
    // Recast pointer to 2d array access
    // 
    uint8_t (*spriteArray)[x_dim] = (uint8_t (*)[x_dim]) sprite;

    // Check offset bounds, if we are outside the bounds then return false
    //
    if (x < 0 || y < 0 || x >= x_dim || y >= y_dim){
        return false;
    }

    // Calculate the offsets into the array based on if we are flipping x,y access
    //
    int x_offset = flipHorizontal ? (x_dim - 1) - x : x;
    int y_offset = flipVertical == 0 ? (y_dim - 1) - y : y;

    // Return true if the "pixel" is set
    //
    return spriteArray[y_offset][x_offset] == 1;

}
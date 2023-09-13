#pragma once
#include <stdint.h>

#include <Utility/spBitmapPattern.h>
using namespace StarterPack::BitmapPattern;

namespace StarterPack {

namespace LCDBitmap {

//
// ARROWS
//

    static constexpr uint8_t up[8] = {        
        ________, // 🀆🀆🀆🀆🀆
        _____O__, // 🀆🀆🀫🀆🀆
        ____OOO_, // 🀆🀫🀫🀫🀆
        ___O_O_O, // 🀫🀆🀫🀆🀫
        _____O__, // 🀆🀆🀫🀆🀆
        _____O__, // 🀆🀆🀫🀆🀆
        _____O__, // 🀆🀆🀫🀆🀆
        ________  // 🀆🀆🀆🀆🀆
        // 0B00000, // 🀆🀆🀆🀆🀆
        // 0B00100, // 🀆🀆🀫🀆🀆
        // 0B01110, // 🀆🀫🀫🀫🀆
        // 0B10101, // 🀫🀆🀫🀆🀫
        // 0B00100, // 🀆🀆🀫🀆🀆
        // 0B00100, // 🀆🀆🀫🀆🀆
        // 0B00100, // 🀆🀆🀫🀆🀆
        // 0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t down[8] = {
        0B00000, // 🀆🀆🀆🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B10101, // 🀫🀆🀫🀆🀫
        0B01110, // 🀆🀫🀫🀫🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t upThick[8] = {
        0B00000, // 🀆🀆🀆🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B01110, // 🀆🀫🀫🀫🀆
        0B11111, // 🀫🀫🀫🀫🀫
        0B01110, // 🀆🀫🀫🀫🀆
        0B01110, // 🀆🀫🀫🀫🀆
        0B00000, // 🀆🀆🀆🀆🀆
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t downThick[8] = {
        0B00000, // 🀆🀆🀆🀆🀆
        0B01110, // 🀆🀫🀫🀫🀆
        0B01110, // 🀆🀫🀫🀫🀆
        0B11111, // 🀫🀫🀫🀫🀫
        0B01110, // 🀆🀫🀫🀫🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B00000, // 🀆🀆🀆🀆🀆
        0B00000  // 🀆🀆🀆🀆🀆
    };    
    static constexpr uint8_t cw[8] = {
        0B00000, // 🀆🀆🀆🀆🀆
        0B11111, // 🀫🀫🀫🀫🀫
        0B00001, // 🀆🀆🀆🀆🀫
        0B00101, // 🀆🀆🀫🀆🀫
        0B01001, // 🀆🀫🀆🀆🀫
        0B11111, // 🀫🀫🀫🀫🀫
        0B01000, // 🀆🀫🀆🀆🀆
        0B00100  // 🀆🀆🀫🀆🀆
    };
    static constexpr uint8_t ccw[8] = {
        0B00000, // 🀆🀆🀆🀆🀆
        0B11111, // 🀫🀫🀫🀫🀫
        0B10000, // 🀫🀆🀆🀆🀆
        0B10100, // 🀫🀆🀫🀆🀆
        0B10010, // 🀫🀆🀆🀫🀆
        0B11111, // 🀫🀫🀫🀫🀫
        0B00010, // 🀆🀆🀆🀫🀆
        0B00100  // 🀆🀆🀫🀆🀆
    };

//
// LOGIC
//

    static constexpr uint8_t logicRising[8] = {
        0B00000, // 🀆🀆🀆🀆🀆
        0B00111, // 🀆🀆🀫🀫🀫
        0B00100, // 🀆🀆🀫🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B11100, // 🀫🀫🀫🀆🀆
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t logicHigh[8] = {
        0B00000, // 🀆🀆🀆🀆🀆
        0B11111, // 🀫🀫🀫🀫🀫
        0B00000, // 🀆🀆🀆🀆🀆
        0B00000, // 🀆🀆🀆🀆🀆
        0B00000, // 🀆🀆🀆🀆🀆
        0B00000, // 🀆🀆🀆🀆🀆
        0B00000, // 🀆🀆🀆🀆🀆
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t logicFalling[8] = {
        0B00000, // 🀆🀆🀆🀆🀆
        0B11100, // 🀫🀫🀫🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B00111, // 🀆🀆🀫🀫🀫
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static const char logicLowChar = '_'; // underscore
        // 🀆🀆🀆🀆🀆
        // 🀆🀆🀆🀆🀆
        // 🀆🀆🀆🀆🀆
        // 🀆🀆🀆🀆🀆
        // 🀆🀆🀆🀆🀆
        // 🀆🀆🀆🀆🀆
        // 🀫🀫🀫🀫🀫
        // 🀆🀆🀆🀆🀆

//
// NUMBERS
//

    static constexpr uint8_t reversed0[8] = {
        0B11111, // 🀫🀫🀫🀫🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B10101, // 🀫🀆🀫🀆🀫
        0B10101, // 🀫🀆🀫🀆🀫
        0B10101, // 🀫🀆🀫🀆🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11111, // 🀫🀫🀫🀫🀫
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t reversed1[8] = {
        0B11111, // 🀫🀫🀫🀫🀫
        0B11011, // 🀫🀫🀆🀫🀫
        0B11011, // 🀫🀫🀆🀫🀫
        0B11011, // 🀫🀫🀆🀫🀫
        0B11011, // 🀫🀫🀆🀫🀫
        0B11011, // 🀫🀫🀆🀫🀫
        0B11111, // 🀫🀫🀫🀫🀫
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t reversed2[8] = {
        0B11111, // 🀫🀫🀫🀫🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11101, // 🀫🀫🀫🀆🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B10111, // 🀫🀆🀫🀫🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11111, // 🀫🀫🀫🀫🀫
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t reversed3[8] = {
        0B11111, // 🀫🀫🀫🀫🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11101, // 🀫🀫🀫🀆🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11101, // 🀫🀫🀫🀆🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11111, // 🀫🀫🀫🀫🀫
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t reversed4[8] = {
        0B11111, // 🀫🀫🀫🀫🀫
        0B10101, // 🀫🀆🀫🀆🀫
        0B10101, // 🀫🀆🀫🀆🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11101, // 🀫🀫🀫🀆🀫
        0B11101, // 🀫🀫🀫🀆🀫
        0B11111, // 🀫🀫🀫🀫🀫
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t reversed5[8] = {
        0B11111, // 🀫🀫🀫🀫🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B10111, // 🀫🀆🀫🀫🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11101, // 🀫🀫🀫🀆🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11111, // 🀫🀫🀫🀫🀫
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t reversed6[8] = {
        0B11111, // 🀫🀫🀫🀫🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B10111, // 🀫🀆🀫🀫🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B10101, // 🀫🀆🀫🀆🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11111, // 🀫🀫🀫🀫🀫
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t reversed7[8] = {
        0B11111, // 🀫🀫🀫🀫🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11101, // 🀫🀫🀫🀆🀫
        0B11101, // 🀫🀫🀫🀆🀫
        0B11101, // 🀫🀫🀫🀆🀫
        0B11101, // 🀫🀫🀫🀆🀫
        0B11111, // 🀫🀫🀫🀫🀫
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t reversed8[8] = {
        0B11111, // 🀫🀫🀫🀫🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B10101, // 🀫🀆🀫🀆🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B10101, // 🀫🀆🀫🀆🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11111, // 🀫🀫🀫🀫🀫
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t reversed9[8] = {
        0B11111, // 🀫🀫🀫🀫🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B10101, // 🀫🀆🀫🀆🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11101, // 🀫🀫🀫🀆🀫
        0B10001, // 🀫🀆🀆🀆🀫
        0B11111, // 🀫🀫🀫🀫🀫
        0B00000  // 🀆🀆🀆🀆🀆
    };

//
// MISC
//

    static const char pikachuFaceChar = 0b11101111;
        // 🀆🀫🀆🀫🀆
        // 🀆🀆🀆🀆🀆
        // 🀆🀫🀫🀫🀆
        // 🀫🀆🀆🀆🀫
        // 🀫🀆🀆🀆🀫
        // 🀫🀆🀆🀆🀫
        // 🀆🀫🀫🀫🀆
        // 🀆🀆🀆🀆🀆

    static constexpr uint8_t oneHundredA[8] = {
        0B10111, // 🀫🀆🀫🀫🀫 🀫🀫🀫🀆🀆
        0B10101, // 🀫🀆🀫🀆🀫 🀫🀆🀫🀆🀆
        0B10101, // 🀫🀆🀫🀆🀫 🀫🀆🀫🀆🀆
        0B10101, // 🀫🀆🀫🀆🀫 🀫🀆🀫🀆🀆
        0B10101, // 🀫🀆🀫🀆🀫 🀫🀆🀫🀆🀆
        0B10101, // 🀫🀆🀫🀆🀫 🀫🀆🀫🀆🀆
        0B10111, // 🀫🀆🀫🀫🀫 🀫🀫🀫🀆🀆
        0B00000  // 🀆🀆🀆🀆🀆 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t oneHundredB[8] = {
        0B11100, // 🀫🀫🀫🀆🀆
        0B10100, // 🀫🀆🀫🀆🀆
        0B10100, // 🀫🀆🀫🀆🀆
        0B10100, // 🀫🀆🀫🀆🀆
        0B10100, // 🀫🀆🀫🀆🀆
        0B10100, // 🀫🀆🀫🀆🀆
        0B11100, // 🀫🀫🀫🀆🀆
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t check[8] = {
        0B00000, // 🀆🀆🀆🀆🀆
        0B00001, // 🀆🀆🀆🀆🀫
        0B00010, // 🀆🀆🀆🀫🀆
        0B00010, // 🀆🀆🀆🀫🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B10100, // 🀫🀆🀫🀆🀆
        0B01000, // 🀆🀫🀆🀆🀆
        0B00000  // 🀆🀆🀆🀆🀆
    };
    static constexpr uint8_t checkUnderlined[8] = {
        0B00000, // 🀆🀆🀆🀆🀆
        0B00001, // 🀆🀆🀆🀆🀫
        0B00010, // 🀆🀆🀆🀫🀆
        0B10100, // 🀫🀆🀫🀆🀆
        0B01000, // 🀆🀫🀆🀆🀆
        0B00000, // 🀆🀆🀆🀆🀆
        0B11111, // 🀫🀫🀫🀫🀫
        0B00000  // 🀆🀆🀆🀆🀆
    };

    static constexpr uint8_t upYours[8] = {
        0B00000, // 🀆🀆🀆🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B00100, // 🀆🀆🀫🀆🀆
        0B01011, // 🀆🀫🀆🀫🀫
        0B11011, // 🀫🀫🀆🀫🀫
        0B11111, // 🀫🀫🀫🀫🀫
        0B01110, // 🀆🀫🀫🀫🀆
        0B00000  // 🀆🀆🀆🀆🀆
    };

}

}

#pragma once
#include <inttypes.h>

namespace StarterPack {

    struct windowPosition {
        uint8_t col = 0;
        uint8_t row = 0;
        uint8_t windowSize = 10;
    };

    struct editorSettings : windowPosition {

        uint8_t decimalPoint = '.';
        bool allowDecimal = false;
        bool allowNegative = false;

        // uint8_t col = 0;
        // uint8_t row = 0;
        // uint8_t windowSize = 10;
        uint8_t bufferLength = 0;

        void setPosition( uint8_t col, uint8_t row, uint8_t windowSize ) {
            this->col = col;
            this->row = row;
            this->windowSize = windowSize;
        }
    };

}

#pragma once
#include <inttypes.h>

#include <Utility/CharCycle.h>
#include <Utility/WindowedText.h>

namespace StarterPack {

    struct windowPosition {

        uint8_t col = 0;
        uint8_t row = 0;
        uint8_t windowSize = 10;

        void setWindow( uint8_t col, uint8_t row, uint8_t windowSize ) {
            this->col = col;
            this->row = row;
            this->windowSize = windowSize;
        }
        void setWindow( windowPosition &wPos ) {
            this->col = wPos.col;
            this->row = wPos.row;
            this->windowSize = wPos.windowSize;
        }
    };

    struct numericEditorSettings : windowPosition {

        uint8_t decimalPoint = '.';
        bool allowDecimal = false;
        bool allowNegative = false;
        bool useLeftRightArrowKeys = true;

        WindowedText::cursorRangeOptions cursorRangeOptions = WindowedText::cursorRangeOptions::WithinBufferPlusOne;

        // length including NULL terminator
        uint8_t bufferLength = 0;
    };

    struct alphaEditorSettings : windowPosition {

        CharCycle::CharSet characterSets = CharCycle::CharSet::UpperCase | CharCycle::CharSet::Space;
        const char *symbolList = nullptr;
        const char *prefixList = nullptr;
        bool allowChangeCase = true;
        bool useLeftRightArrowKeys = true;

        WindowedText::cursorRangeOptions cursorRangeOptions = WindowedText::cursorRangeOptions::WithinBufferPlusOne;

        // length including NULL terminator
        uint8_t bufferLength = 0;
    };

}

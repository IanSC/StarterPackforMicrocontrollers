#pragma once
#include <stdint.h>
#include <Arduino.h>

#include <LCDCharBank.h>
#include <LCD/LCDInterface.h>
#include <Utility/spRangeMap.h>

namespace StarterPack {

class LCDPointer {

        LCDCharBank *cBank;
        StarterPack::LCDInterface *lcd;

        int16_t min, max;     // input min/max
        uint8_t lcdMaxValue;  // lcd range 0-max

        bool leftToRight;     // min-->max or max<--min
        uint8_t startPos;     // position of min

        uint8_t dataRow;      // data row
        uint8_t dataColFrom;  // data column positions
        uint8_t dataColTo;

        uint8_t cOffset = 0;

        uint8_t prevCharPos = -1; // last pointer displayed position

    public:
    
        LCDPointer( StarterPack::LCDInterface &lcd, LCDCharBank *cBank = nullptr, uint8_t charIDOffset = 0 ) {
            this->lcd = &lcd;
            if ( cBank == nullptr )
                cBank = new LCDCharBank( lcd );
            this->cBank = cBank;
            this->cOffset = charIDOffset;
            if ( !cBank->exists( cOffset ) ) {
                cBank->add( 0+cOffset, p0 );
                cBank->add( 1+cOffset, p1 );
                cBank->add( 2+cOffset, p2 );
                cBank->add( 3+cOffset, p3 );
                cBank->add( 4+cOffset, p4 );
            }
        }

        void loadCharacters() {
            // or let user call to load different subset characters
            cBank->load( 0+cOffset, 1+cOffset, 2+cOffset, 3+cOffset, 4+cOffset );
        }

        void setMapping( int16_t min, int16_t max ) {
            this->min = min; this->max = max;
        }

        void setDataWindow( uint8_t row, uint8_t colFrom, uint colTo, bool leftToRight ) {
            dataRow = row; dataColFrom = colFrom; dataColTo = colTo;
            this->leftToRight = leftToRight;
            if ( leftToRight )
                startPos = colFrom;
            else
                startPos = colTo;
            lcdMaxValue = (colTo-colFrom+1)*5-1;
        }

        void displayLegend( uint8_t row ) {
            // 01234567890123456789
            //  |   |   |   |    |
            // <                  >
            if ( dataColFrom-1 >= 0 )
                lcd->writeAt( dataColFrom-1, row, 0x7F );
            if ( dataColTo+1 < lcd->maxColumns )
                lcd->writeAt( dataColTo+1, row, 0x7E );
            displayRaw( 0              , row, false );
            displayRaw( lcdMaxValue*1/4, row, false );
            displayRaw( lcdMaxValue*2/4, row, false );
            displayRaw( lcdMaxValue*3/4, row, false );
            displayRaw( lcdMaxValue    , row, false );
        }

        inline void display( int16_t value ) {
            displayCore( value, dataRow, false );
        }
        inline void display( int16_t value, uint8_t row ) {
            displayCore( value, row, false );
        }
        inline void move( int16_t value ) {
            displayCore( value, dataRow, true );
        }
        inline void move( int16_t value, uint8_t row ) {
            displayCore( value, row, true );
        }
        inline void hide() {
            hide( dataRow );
        }
        inline void hide( uint8_t row ) {
            if ( prevCharPos != -1 )
                lcd->writeAt( prevCharPos, row, ' ' );
        }

    private:

        void displayCore( int16_t value, uint8_t row, bool removePrevious ) {
            auto mappedValue = StarterPack::RangeMap::IntInt( value, min, max, 0, lcdMaxValue );
            // auto mappedValue = StarterPack::spMap( value, min, max, 0, lcdMaxValue );
            if ( mappedValue < 0 ) mappedValue = 0;
            if ( mappedValue > lcdMaxValue ) mappedValue = lcdMaxValue;
            // Serial.println( mappedValue );
            displayRaw( mappedValue, row, removePrevious );
        }

        void displayRaw( int16_t value, uint8_t row, bool removePrevious ) {
            uint8_t charPos, ch;
            computePosition( value, charPos, ch );
            cBank->writeAt( charPos, row, ch );
            if ( removePrevious ) {
                if ( prevCharPos != -1 && prevCharPos != charPos ) {
                    lcd->writeAt( prevCharPos, row, ' ' );
                }
                prevCharPos = charPos;
            }
        }

        void computePosition( int16_t value, uint8_t &charPos, uint8_t &ch ) {
            if ( leftToRight ) {
                charPos = startPos + value / 5;
                ch = ( value % 5 ) + cOffset;
            } else {
                charPos = startPos - value / 5;
                ch = ( 4 - value % 5 ) + cOffset;
            }
        }

    private:

        const uint8_t p0[8] = {
            0B10000, // 🀫🀆🀆🀆🀆
            0B10000, // 🀫🀆🀆🀆🀆
            0B10000, // 🀫🀆🀆🀆🀆
            0B10000, // 🀫🀆🀆🀆🀆
            0B10000, // 🀫🀆🀆🀆🀆
            0B10000, // 🀫🀆🀆🀆🀆
            0B10000, // 🀫🀆🀆🀆🀆
            0B10000  // 🀫🀆🀆🀆🀆
        };
        const uint8_t p1[8] = {
            0B01000, // 🀆🀫🀆🀆🀆
            0B01000, // 🀆🀫🀆🀆🀆
            0B01000, // 🀆🀫🀆🀆🀆
            0B01000, // 🀆🀫🀆🀆🀆
            0B01000, // 🀆🀫🀆🀆🀆
            0B01000, // 🀆🀫🀆🀆🀆
            0B01000, // 🀆🀫🀆🀆🀆
            0B01000  // 🀆🀫🀆🀆🀆
        };
        const uint8_t p2[8] = {
            0B00100, // 🀆🀆🀫🀆🀆
            0B00100, // 🀆🀆🀫🀆🀆
            0B00100, // 🀆🀆🀫🀆🀆
            0B00100, // 🀆🀆🀫🀆🀆
            0B00100, // 🀆🀆🀫🀆🀆
            0B00100, // 🀆🀆🀫🀆🀆
            0B00100, // 🀆🀆🀫🀆🀆
            0B00100  // 🀆🀆🀫🀆🀆
        };
        const uint8_t p3[8] = {
            0B00010, // 🀆🀆🀆🀫🀆
            0B00010, // 🀆🀆🀆🀫🀆
            0B00010, // 🀆🀆🀆🀫🀆
            0B00010, // 🀆🀆🀆🀫🀆
            0B00010, // 🀆🀆🀆🀫🀆
            0B00010, // 🀆🀆🀆🀫🀆
            0B00010, // 🀆🀆🀆🀫🀆
            0B00010  // 🀆🀆🀆🀫🀆
        };
       const uint8_t p4[8] = {
            0B00001, // 🀆🀆🀆🀆🀫
            0B00001, // 🀆🀆🀆🀆🀫
            0B00001, // 🀆🀆🀆🀆🀫
            0B00001, // 🀆🀆🀆🀆🀫
            0B00001, // 🀆🀆🀆🀆🀫
            0B00001, // 🀆🀆🀆🀆🀫
            0B00001, // 🀆🀆🀆🀆🀫
            0B00001  // 🀆🀆🀆🀆🀫
        };

};

}

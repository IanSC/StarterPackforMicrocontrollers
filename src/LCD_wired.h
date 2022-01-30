//  2022/01 - isc
//
//  Interface for Directly Wired LCD
//  --------------------------------
//  - uses <LiquidCrystal.h> behind the scenes
//  - need this to make it compatible with buffering
//
//  To Use
//
//      const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
//      LCD_wired lcd = LCD_wired( rs, en, d4, d5, d6, d7 );
//      ...
//      lcd.begin( 16, 2 );
//

#pragma once
#include <LiquidCrystal.h>
#include <LCDInterface.h>

namespace StarterPack {

class LCD_wired : public LCDInterface {

        LiquidCrystal *lcd;

    public:

        ~LCD_wired() {
            delete lcd;
        }

        inline LCD_wired( uint8_t rs, uint8_t rw, uint8_t enable,
        uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
            lcd = new LiquidCrystal( rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7 );
        }
        
        inline LCD_wired( uint8_t rs, uint8_t enable,
        uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
            lcd = new LiquidCrystal( rs, enable, d0, d1, d2, d3, d4, d5, d6, d7 );
        }
        
        inline LCD_wired( uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3 ) {
            lcd = new LiquidCrystal( rs, rw, enable, d0, d1, d2, d3 );
        }
        
        inline LCD_wired( uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3 ) {
            lcd = new LiquidCrystal( rs, enable, d0, d1, d2, d3 );
        }

        inline void setTimeoutInMs( uint16_t timeOut ) {}
        inline void setFrequency( uint32_t frequency ) {}
        
    //
    // BEGIN
    //
    private:

        // called in constructor, so no needed to call again
        inline void init( uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
        uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {
            lcd->init( fourbitmode, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7 );
        }

        charDotSize dotSize; // copy when initialized, to use for recovery

    public:
        
        inline void begin( uint8_t cols, uint8_t lines, charDotSize dotSize = charDotSize::size5x8 ) {
            this->maxColumns = cols;
            this->maxRows = lines;
            this->dotSize = dotSize;
            lcd->begin( cols, lines, (uint8_t) dotSize );
        }

    //
    // VERIFY / RECOVERY
    //

        // no way to detect errors
        // also if reconnected will still work unless power is removed
        // in which case will revert to 8-bit mode
        
        inline bool     verify()            { return true; }
        inline ERROR_NO verifyWithError()   { return 0; }
        inline bool     recoverIfHasError() { return false; }
        inline void     setRecoveryThrottleInMs( uint16_t delay ) {}

        void reset() {
            // no access to private variables, so need to keep data when created
            // but what are the chances it needs to be called again, so never mind
            // uint8_t fourbitmode = ( lcd->_displayfunction & LCD_8BITMODE ) != LCD_8BITMODE;
            // lcd->init( fourbitmode, lcd->_rs_pin, lcd->_rw_pin, lcd->_enable_pin,
            // lcd->_data_pins[0], lcd->_data_pins[1], lcd->_data_pins[2], lcd->_data_pins[3], 
            // lcd->_data_pins[4], lcd->_data_pins[5], lcd->_data_pins[6], lcd->_data_pins[7] );
            lcd->begin( maxColumns, maxRows, dotSize );
        }

    //
    // USER COMMANDS
    //
    public:

        inline void setCursor( uint8_t col, uint8_t row ) { lcd->setCursor( col, row ); }

        inline void backlightOn()    {} // not supported
        inline void backlightOff()   {}

        inline void clear()          { lcd->clear();     }
        inline void home()           { lcd->home();      }
        inline void displayOn()      { lcd->display();   }
        inline void displayOff()     { lcd->noDisplay(); }
        inline void cursorOn()       { lcd->cursor();    }
        inline void cursorOff()      { lcd->noCursor();  }
        inline void cursorBlinkOn()  { lcd->blink();     }
        inline void cursorBlinkOff() { lcd->noBlink();   }

        inline void scrollDisplayLeft()  { lcd->scrollDisplayLeft();  }
        inline void scrollDisplayRight() { lcd->scrollDisplayRight(); }
        inline void leftToRight()        { lcd->leftToRight();        }
        inline void rightToLeft()        { lcd->rightToLeft();        }
        inline void autoscrollOn()       { lcd->autoscroll();         }
        inline void autoscrollOff()      { lcd->noAutoscroll();       }

        inline void createChar( uint8_t location, uint8_t     charmap[] ) { lcd->createChar( location, charmap ); }
        inline void createChar( uint8_t location, const char *charmap   ) { lcd->createChar( location, (uint8_t*) charmap ); }

        inline void command( uint8_t value ) {        lcd->command( value ); }
        inline size_t write( uint8_t value ) { return lcd->write( value );   }

};

}

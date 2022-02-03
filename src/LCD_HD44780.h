//  2022/01 - isc
//
//  Interface for HD4480 LCD Driver
//  -------------------------------
//  - converted from my PIC days
//
//  To Use
//
//      const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
//      LCD_wired lcd = LCD_wired( rs, en, d4, d5, d6, d7 );
//      ...
//      lcd.begin( 16, 2 );

#pragma once
#include <LCDInterface.h>
using namespace StarterPack;

#include <Arduino.h>

// NOT FULLY TESTED: !!!
// - R/W MODE
// - 8-BIT PORT
// - WRITE ROUTINES

//
// define these before calling header
//
// #define LCD_READ_WRITE_MODE       // R/W pin of LCD is used, otherwise R/W pin is hardwired as write (low)
// #define LCD_USE_8BIT_PORT         // handle 8 bit data mode
// #define LCD_INCLUDE_READ_ROUTINES // even in R/W mode, option to include reading routines

namespace StarterPack {

class LCD_HD44780 : public LCDInterface {

    // // data port, in/out
    // #if defined( LCD_USE_8BIT_PORT )
    //     uint8_t LCD_DB0 = 0;
    //     uint8_t LCD_DB1 = 0;
    //     uint8_t LCD_DB2 = 0;
    //     uint8_t LCD_DB3 = 0;
    // #endif
    // uint8_t LCD_DB4 = 0;
    // uint8_t LCD_DB5 = 0;
    // uint8_t LCD_DB6 = 0;
    // uint8_t LCD_DB7 = 0;

    // // always outputs
    // uint8_t LCD_RS = 0;
    // uint8_t LCD_E  = 0;

    // #if defined( LCD_READ_WRITE_MODE )

    //     uint8_t LCD_RW = 0;

    //     bool dataPortIsOutput = true;
    
    //     void setDataPortToWrite( bool setAsOutput ) {
    //         if ( dataPortIsOutput == setAsOutput ) return;
    //         dataPortIsOutput = setAsOutput;
    //         if ( dataPortIsOutput ) {
    //             #if defined( LCD_USE_8BIT_PORT )
    //                 pinMode( LCD_DB0, OUTPUT );
    //                 pinMode( LCD_DB1, OUTPUT );
    //                 pinMode( LCD_DB2, OUTPUT );
    //                 pinMode( LCD_DB3, OUTPUT );
    //             #endif
    //             pinMode( LCD_DB4, OUTPUT );
    //             pinMode( LCD_DB5, OUTPUT );
    //             pinMode( LCD_DB6, OUTPUT );
    //             pinMode( LCD_DB7, OUTPUT );
    //         } else {
    //             #if defined( LCD_USE_8BIT_PORT )
    //                 pinMode( LCD_DB0, INPUT );
    //                 pinMode( LCD_DB1, INPUT );
    //                 pinMode( LCD_DB2, INPUT );
    //                 pinMode( LCD_DB3, INPUT );
    //             #endif
    //             pinMode( LCD_DB4, INPUT );
    //             pinMode( LCD_DB5, INPUT );
    //             pinMode( LCD_DB6, INPUT );
    //             pinMode( LCD_DB7, INPUT );
    //         }
    //     }

    //     inline void setWriteMode() {
    //         digitalWrite( LCD_RW, LOW );            
    //     }
    //     inline void setReadMode() {
    //         digitalWrite( LCD_RW, HIGH );            
    //     }

    // #else

    //     // write only, r/w of lcd hard wired to low
    //     #define setWriteMode()        ;
    //     #define setReadMode()         ;
    //     #define setDataPortToWrite(x) ;

    // #endif

public:

    // #if defined( LCD_USE_8BIT_PORT )
    //     #if defined( LCD_READ_WRITE_MODE )
    //         inline LCD_wired2( uint8_t rs, uint8_t rw, uint8_t enable,
    //         uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
    //             LCD_RS = rs; LCD_RW = rw; LCD_E = enable;
    //             LCD_DB0 = d0; LCD_DB1 = d1; LCD_DB2 = d2; LCD_DB3 = d3;
    //             LCD_DB4 = d4; LCD_DB5 = d5; LCD_DB6 = d6; LCD_DB7 = d7;
    //             initPort();
    //         }
    //     #else    
    //         inline LCD_wired2( uint8_t rs, uint8_t enable,
    //         uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
    //             LCD_RS = rs; LCD_E = enable;
    //             LCD_DB0 = d0; LCD_DB1 = d1; LCD_DB2 = d2; LCD_DB3 = d3;
    //             LCD_DB4 = d4; LCD_DB5 = d5; LCD_DB6 = d6; LCD_DB7 = d7;
    //             initPort();
    //         }
    //     #endif        
    // #else
    //     #if defined( LCD_READ_WRITE_MODE )
    //         inline LCD_wired2( uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
    //             LCD_RS = rs; LCD_RW = rw; LCD_E = enable;
    //             LCD_DB4 = d4; LCD_DB5 = d5; LCD_DB6 = d6; LCD_DB7 = d7;
    //             initPort();
    //         }
    //     #else
    //         inline LCD_wired( uint8_t rs, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
    //             LCD_RS = rs; LCD_E = enable;
    //             LCD_DB4 = d4; LCD_DB5 = d5; LCD_DB6 = d6; LCD_DB7 = d7;
    //             initPort();
    //         }
    //     #endif
    // #endif

    // inline void setTimeoutInMs( uint16_t timeOut ) {}
    // inline void setFrequency( uint32_t frequency ) {}

    // void initPort() {
    //     // initialize port directions
    //     pinMode( LCD_RS, OUTPUT );
    //     pinMode( LCD_E, OUTPUT );
    //     #if defined( LCD_READ_WRITE_MODE )
    //         pinMode( LCD_RW, OUTPUT );
    //         setDataPortToWrite( true );
    //     #else
    //         // default to data output
    //         #if defined( LCD_USE_8BIT_PORT )
    //             pinMode( LCD_DB0, OUTPUT );
    //             pinMode( LCD_DB1, OUTPUT );
    //             pinMode( LCD_DB2, OUTPUT );
    //             pinMode( LCD_DB3, OUTPUT );
    //         #endif
    //         pinMode( LCD_DB4, OUTPUT );
    //         pinMode( LCD_DB5, OUTPUT );
    //         pinMode( LCD_DB6, OUTPUT );
    //         pinMode( LCD_DB7, OUTPUT );
    //     #endif
    // }

    //
    // BEGIN
    //
    protected:

        static const uint8_t LCD_FUNCTION_MODE    = 0b00100000;
        static const uint8_t LCD_FUNCTION_8_BIT   = 0b00010000;
        static const uint8_t LCD_FUNCTION_4_BIT   = 0b00000000;
        static const uint8_t LCD_FUNCTION_1_LINE  = 0b00000000;
        static const uint8_t LCD_FUNCTION_2_LINES = 0b00001000;
        
        charDotSize dotSize; // copy when initialized, to use for recovery
        uint8_t rowAddress[4];

    public:
        
        inline void begin( uint8_t maxColumns, uint8_t maxRows, charDotSize dotSize = charDotSize::size5x8 ) {
            // initialize LCD
            //  - set number of bits
            //  - cursor movement and style
            //  - clear screen
                        
            this->maxColumns = maxColumns;
            this->maxRows = maxRows;
            this->dotSize = dotSize;

            // https://forum.arduino.cc/t/solved-16x4-lcd-characters-in-row-3-4-are-moved-to-the-right/62808/5
            // ddram address:
            // lines   address
            //         line0     line1     line2     line3     notes
            // 1       00-4F                                   80 characters
            // 2       00-27     40-67                         40 characters each
            // 4       00-[c-1]  40-[+c]   [c]-yy    xx-zz     ?? characters each
            rowAddress[0] = 0x00;
            rowAddress[1] = 0x40;
            rowAddress[2] = 0x00 + maxColumns;
            rowAddress[3] = 0x40 + maxColumns;

            unsigned char com = LCD_FUNCTION_MODE | dotSize;
            if ( maxRows > 1 )
                com = com | LCD_FUNCTION_2_LINES;
    
            // DELAY_30_mSec();
    
            #if defined( LCD_USE_8BIT_PORT )
                com = com | LCD_FUNCTION_8_BIT;
                command( com );
            #else
                com = com | LCD_FUNCTION_4_BIT;
                command( com );
                command( com );
            #endif
            
            // default for entryMode: move cursor to right, no screen shifting
            command( entryMode );
            
            // default for displayMode: display on, no cursor, no cursor blink
            command( displayMode );
            
            clear();
        }

    //
    // VERIFY / RECOVERY
    //

        // no way to detect errors
        // also if reconnected will still work unless power is removed
        // in which case will revert to 8-bit mode
        
        // inline bool     verify()            { return true; }
        // inline ERROR_NO verifyWithError()   { return 0; }
        // inline bool     recoverIfHasError() { return false; }
        // inline void     setRecoveryThrottleInMs( uint16_t delay ) {}

        // void reset() {
        //     begin( maxColumns, maxRows, dotSize );
        // }

    //
    // USER COMMANDS
    //
    protected:
    
        static const uint8_t LCD_CLEAR          = 0b00000001;
        static const uint8_t LCD_HOME           = 0b00000010;
        static const uint8_t LCD_SET_DDRAM_ADDR = 0b10000000;
        
    public:

        // https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller
        // https://html.alldatasheet.com/html-pdf/63673/HITACHI/HD44780/6019/24/HD44780.html
        
        inline void setCursor( uint8_t col, uint8_t row ) {
            command( LCD_SET_DDRAM_ADDR | ( rowAddress[row && 0b11] + col ) );
        }

        // inline void backlightOn()  {} // not supported
        // inline void backlightOff() {}
    
        inline void clear() {
            command( LCD_CLEAR );
            delayMicroseconds( 1520 - 37 ); // 1.52ms, command already has 37ms
        }
        inline void home() {
            command( LCD_HOME );
            delayMicroseconds( 1520 - 37 ); // 1.52ms, command already has 37ms
        }

    protected:
    
        static const uint8_t LCD_DISPLAY_MODE       = 0b00001000;
        static const uint8_t LCD_DISPLAY_ON         = 0b00000100;
        static const uint8_t LCD_DISPLAY_OFF        = 0b00000000;
        static const uint8_t LCD_DISPLAY_CURSOR_ON  = 0b00000010;
        static const uint8_t LCD_DISPLAY_CURSOR_OFF = 0b00000000;
        static const uint8_t LCD_DISPLAY_BLINK_ON   = 0b00000001;
        static const uint8_t LCD_DISPLAY_BLINK_OFF  = 0b00000000;

        // keep track of last update
        uint8_t displayMode = LCD_DISPLAY_MODE | LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR_OFF | LCD_DISPLAY_BLINK_OFF;
        
    public:
    
        inline void displayOn() {
            displayMode |= LCD_DISPLAY_ON;
            command( displayMode );
        }
        inline void displayOff() {
            displayMode &= ~LCD_DISPLAY_ON;
            command( displayMode );
        }
        inline void cursorOn() {
            displayMode |= LCD_DISPLAY_CURSOR_ON;
            command( displayMode );
            // or just: display ON, cursor ON, blink OFF
        }
        inline void cursorOff() {
            displayMode &= ~LCD_DISPLAY_CURSOR_ON;
            command( displayMode );
            // or just: display ON, cursor OFF, blink N/A
        }
        inline void cursorBlinkOn()  {
            displayMode |= LCD_DISPLAY_BLINK_ON;
            command( displayMode );
            // or just: display ON, cursor ON, blink ON
        }
        inline void cursorBlinkOff() {
            displayMode &= ~LCD_DISPLAY_BLINK_ON;
            command( displayMode );
            // or just: display ON, cursor ON, blink OFF
        }
        
    //
    // SCREEN SHIFT
    //
    protected:

        // https://html.alldatasheet.com/html-pdf/63673/HITACHI/HD44780/7023/28/HD44780.html
        // https://html.alldatasheet.com/html-pdf/63673/HITACHI/HD44780/7274/29/HD44780.html
        static const uint8_t LCD_SHIFT_MODE         = 0b00010000;
        static const uint8_t LCD_SHIFT_DISPLAY_ON   = 0b00001000;
        static const uint8_t LCD_SHIFT_DISPLAY_OFF  = 0b00000000;
        static const uint8_t LCD_SHIFT_LEFT         = 0b00000000;
        static const uint8_t LCD_SHIFT_RIGHT        = 0b00000100;

    public:
    
        inline void moveCursorLeft() {
            command( LCD_SHIFT_MODE | LCD_SHIFT_LEFT );
        }
        inline void moveCursorRight() {
            command( LCD_SHIFT_MODE | LCD_SHIFT_RIGHT );
        }    
        inline void scrollDisplayLeft() {
            command( LCD_SHIFT_MODE | LCD_SHIFT_DISPLAY_ON | LCD_SHIFT_LEFT );
        }
        inline void scrollDisplayRight() {
            command( LCD_SHIFT_MODE | LCD_SHIFT_DISPLAY_ON | LCD_SHIFT_RIGHT );
        }
        
    //
    // ENTRY MODE
    //
    protected:
    
        static const uint8_t LCD_ENTRY_MODE      = 0b00000100;
        static const uint8_t LCD_ENTRY_INCREMENT = 0b00000010;
        static const uint8_t LCD_ENTRY_DECREMENT = 0b00000000;
        static const uint8_t LCD_ENTRY_SHIFT_ON  = 0b00000001;
        static const uint8_t LCD_ENTRY_SHIFT_OFF = 0b00000000;

        // keep track of last user updates
        uint8_t entryMode = LCD_ENTRY_MODE | LCD_ENTRY_INCREMENT | LCD_ENTRY_SHIFT_OFF;
        
    public:
    
        inline void leftToRight() {
            entryMode |= LCD_ENTRY_INCREMENT;
            command( entryMode );
        }
        inline void rightToLeft() { 
            entryMode &= ~LCD_ENTRY_INCREMENT;
            command( entryMode );
        }
        inline void autoscrollOn() {
            entryMode |= LCD_ENTRY_SHIFT_ON;
            command( entryMode );
        }
        inline void autoscrollOff() {
            entryMode &= ~LCD_ENTRY_SHIFT_ON;
            command( entryMode );
        }
        
    //
    // CUSTOM CHARACTERS
    //
    protected:

        static const uint8_t LCD_SET_CGRAM_ADDR = 0b01000000;

    public:
    
        void createChar( uint8_t charID, const uint8_t charmap[] ) {
            // charID &= 0b111;
            // command( LCD_SET_CGRAM_ADDR | ( charID << 3 ) );
            // for( int i = 0 ; i < 8 ; i++ )
            //     write( (uint8_t) charmap[i] );
            createCharCore( charID, (const char *) charmap );
        }

        void createChar( uint8_t charID, const char *charmap ) {
            createCharCore( charID, charmap );
        }
        
    protected:

        void createCharCore( uint8_t charID, const char *charmap ) {
            // put custom character into charID
            // pattern = 8 characters, rightmost 5 bits is the on/off pattern
            //
            // ex. set custom character 0 as pointing arrow
            //  LCD_customChar(0, "\004\016\037\004\004\004\004\004");
            //  Char 0 =    --1--
            //              -111-
            //              11111
            //              --1--
            //              --1--
            //              --1--
            //              --1--
            //              --1--
            charID &= 0b111;
            command( LCD_SET_CGRAM_ADDR | ( charID << 3 ) );
            for( int i = 0 ; i < 8 ; i++ )
                write( (uint8_t) *charmap++ );
        }
    
};

}

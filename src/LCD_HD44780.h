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

namespace StarterPack {

class LCD_HD44780 : public LCDInterface {

    //
    // BEGIN
    //
    protected:

        static const uint8_t LCD_FUNCTION_MODE    = 0b00100000;
        static const uint8_t LCD_FUNCTION_8_BIT   = 0b00010000;
        static const uint8_t LCD_FUNCTION_4_BIT   = 0b00000000;
        static const uint8_t LCD_FUNCTION_1_LINE  = 0b00000000;
        static const uint8_t LCD_FUNCTION_2_LINES = 0b00001000;
        
        charDotSize dotSize; // copy when initialized, to use for recovery/reset
        uint8_t rowAddress[4];

    public:

        ~LCD_HD44780() {}

        void begin( uint8_t maxColumns, uint8_t maxRows, charDotSize dotSize = charDotSize::size5x8 ) {
            delay( 50 );
            beginCore( maxColumns, maxRows, dotSize );
        }

    protected:

        virtual void sendNibble( uint8_t nibble ) {
            // nibble on lower bits
        };

        inline void beginCore( uint8_t maxColumns, uint8_t maxRows, charDotSize dotSize = charDotSize::size5x8 ) {
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
            // 4       00 ...    40 ...    00+c ...  40+c ...
            // note: LiquidCrystal_I2C.h is incorrect (works for 20-column LCD only)
            //       LiquidCrystal.h is correct
            rowAddress[0] = 0x00;
            rowAddress[1] = 0x40;
            rowAddress[2] = 0x00 + maxColumns;
            rowAddress[3] = 0x40 + maxColumns;

            unsigned char com = LCD_FUNCTION_MODE | dotSize;
            if ( maxRows > 1 )
                com = com | LCD_FUNCTION_2_LINES;
    
            #if defined( LCD_USE_8BIT_PORT )
                com = com | LCD_FUNCTION_8_BIT;
                command( com );
                // delayMicroseconds(100);
            #else
                com = com | LCD_FUNCTION_4_BIT;

                // NOT WORKING
                // according to datasheet, send command 2x
                // used to work but failed ???
                //command( com ); command( com );

                // kinda works... but sometimes glitches
                // from power on okay, if resetting mpu, glitches
                // https://www.handsonembedded.com/lcd16x2-hd44780-tutorial-6/
                // https://exploreembedded.com/wiki/Interfacing_LCD_in_4-bit_mode_with_8051
                // sendNibble( 0x02 );

                // use this if you are having problems
                // original LiquidCrystal_I2C
                // sendNibble( 0x03 ); delayMicroseconds(4500);
                // sendNibble( 0x03 ); delayMicroseconds(4500);
                // sendNibble( 0x03 ); delayMicroseconds(150);
                // sendNibble( 0x02 );

                command( (0x03 << 4) | 0x03 ); command( (0x03 << 4) | 0x02 );
                
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

        // based on implementation

    //
    // USER COMMANDS
    //
    protected:
    
        static const uint8_t LCD_CLEAR          = 0b00000001;
        static const uint8_t LCD_HOME           = 0b00000010;
        static const uint8_t LCD_SET_DDRAM_ADDR = 0b10000000;
        
    public:

        // based on implementation:
        //     void backlightOn()  {}
        //     void backlightOff() {}

        // https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller
        // https://html.alldatasheet.com/html-pdf/63673/HITACHI/HD44780/6019/24/HD44780.html
        
        inline void setCursor( uint8_t col, uint8_t row ) {
            row = row % maxRows;
            command( LCD_SET_DDRAM_ADDR | ( rowAddress[row] + col ) );
        }
    
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
    
        inline void createChar( uint8_t charID, const uint8_t charmap[] ) {
            createCharCore( charID, (const char *) charmap );
        }

        inline void createChar( uint8_t charID, const char *charmap ) {
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

//  2022/01 - isc
//
//  Common Interface for LCD
//  ------------------------
//  - hardwired LCD (aka LiquidCrystal.h)
//  - i2c LCD       (aka LiquidCrystal_I2C.h)
//  - buffered hardwired LCD
//  - buffered i2c LCD
//
//  Why?
//  - easily switch between hardwired or i2c LCDs
//  - easily switch to buffered display
//  - recover from i2c errors
//
//  Functions
//
//      void begin( maxColumns, maxLines )
//      void reset()                         reset device to initial state
//
//      bool isBuffered()                    returns true if buffered
//      void displayAll()                    send all buffered changes immediately to screen
//
//  I2C Specific (ignored if wired LCD)
//    
//      void setTimeoutInMs( timeout )       set i2c bus timeout, ignored for wired lcd
//      void setFrequency( frequency )       set i2c bus frequency, ignored for wire lcd
//
//      bool verify()                        verify i2c status
//      ERROR_NO verifyWithError()           verify and get i2c error
//      bool recoverIfHasError()             recover from i2c errors
//      void setRecoveryThrottleInMs()       throttle between i2c error recoveries
//
//  Standard Functions
//
//      clear()                             clear screen
//      home()                              move cursor to home
//      setCursor( col, row )               move cursor position
//      backlightOn()      backlight()      turn backlight on/off
//      backlightOff()     noBacklight()
//      displayOn()        display()        turn display on/off
//      displayOff()       noDisplay()
//      cursorOn()         cursor()         show/hide cursor
//      cursorOff()        noCursor()
//      cursorBlinkOn()    blink()          blink cursor if shown
//      cursorBlinkOff()   noBlink()
//      scrollDisplayLeft()                 move screen contents to left
//      scrollDisplayRight()                move screen contents to right
//      leftToRight()                       move cursor to right after each character
//      rightToLeft()                       move cursor to left after each character
//      autoscrollOn()     autoscroll()     autoscroll screen on each character printed
//      autoscrollOff()    noAutoscroll()   turn off autoscroll
//      createChar( location, charmap[] )   create special characters
//      command( value )                    direct command to LCD
//      write( ch )                         write single character
//      print( ... )                        all capabilities in <Print.h>
//
//  Additional functions
//
//      printCharsN( ch, count )            print ch, several times
//                                          ex. lcd.printCharsN( ' ', 10 )
//
//      printAt( col, row, value )          print value at specified location
//                                          ex. lcd.print( 8, 0, "hello" );
//                                              lcd.print( 3, 1, 123 );
//
//      writeAt( col, row, char )           write special character or value at specified location
//                                          ex. lcd.writeAt( 10, 1, 0x01 );
//
//      printStrAtLine( row, string )       print string at specified line and clear out till end of line
//                                          ex. lcd.printLine( 0, "hello" );
//
//      printStrN( string, N )              print only first N characters of string
//
//      clearLine( row )                    clear out specified line
//                                          ex. lcd.clearLine( 1 );
//
//      printfAt( col, row, value, ... )    use printf() at specified location
//                                          ex. lcd.printfAt( 10, 1, "%s = %4d", "val", val );

#pragma once
#include <stdarg.h>
#include <stdio.h>
#include <Print.h>
#ifdef ESP32
    #include <string>
#endif

namespace StarterPack {

typedef uint8_t ERROR_NO;

class LCDInterface : public Print {

    public:

        virtual ~LCDInterface() {}

        uint8_t maxColumns = 0, maxRows = 0;

        // for i2c bus
        virtual void setTimeoutInMs( uint16_t timeOut ) = 0;
        virtual void setFrequency( uint32_t frequency ) = 0;
        
    //
    // BEGIN
    //
    public:

        enum charDotSize {
            size5x8  = 0x00,
            size5x10 = 0x04
        };

        virtual void begin( uint8_t cols, uint8_t lines, charDotSize dotSize = charDotSize::size5x8 ) = 0;

    //
    // VERIFY / RECOVERY
    //
    public:

        virtual bool verify() = 0;
        virtual ERROR_NO verifyWithError() = 0;
        virtual bool recoverIfHasError() = 0;
        virtual void setRecoveryThrottleInMs( uint16_t delay ) = 0;
        virtual void reset() = 0;

    //
    // BUFFERED
    //
    public:
    
        virtual bool isBuffered() { return false; }
        virtual void displayAll() {}

    //
    // USER COMMANDS
    //
    public:

        // clear screen
        virtual void clear() = 0;

        // restore shifted X origin
        // when using:
        // - scrollDisplayLeft();
        // - scrollDisplayRight();
        // - autoscroll();
        // origin of cursor X will be shifted
        // eg. setCursor(0,0) will not necessary be top-left of screen
        virtual void home() = 0;

        virtual void setCursor( uint8_t col, uint8_t row ) = 0;

        // turn backlight on/off
        virtual void backlightOn() = 0;
        virtual void backlightOff() = 0;
        // ... compatibility
        inline void backlight()   { backlightOn();  }
        inline void noBacklight() { backlightOff(); }

        // turn display on/off (contents are preserved)
        virtual void displayOn() = 0;
        virtual void displayOff() = 0;
        // ... compatibility
        inline void display()   { displayOn();  }
        inline void noDisplay() { displayOff(); }

        // display cursor with optional blink
        virtual void cursorOn() = 0;
        virtual void cursorOff() = 0;
        virtual void cursorBlinkOn() = 0;
        virtual void cursorBlinkOff() = 0;
        // ... compatibility
        inline void cursor()   { cursorOn();  }
        inline void noCursor() { cursorOff(); }
        inline void blink()    { cursorBlinkOn();  }
        inline void noBlink()  { cursorBlinkOff(); }

        // move cursor
        virtual void moveCursorRight() = 0;
        virtual void moveCursorLeft() = 0;

        // scroll the display without changing the RAM
        virtual void scrollDisplayLeft() = 0;
        virtual void scrollDisplayRight() = 0;

        // text flow direction: Left to Right, Right to Left
        virtual void leftToRight() = 0;
        virtual void rightToLeft() = 0;

        // scroll screen to left/right after a character is written
        // ex. lcd.autoscrollOn();
        //     lcd.setCursor(15,0);
        //     lcd.print( "hello" ); // as if right justified
        //     0123456789012345
        //               hello
        virtual void autoscrollOn() = 0;
        virtual void autoscrollOff() = 0;
        // ... compatibility
        inline void autoscroll()   { autoscrollOn();  }
        inline void noAutoscroll() { autoscrollOff(); }

        // create custom character in CGRAM, location: 0 to 7
        // ex. const uint8_t bitmapPattern[8] = {
        //         0B11111,
        //         0B10101,
        //         0B10101,
        //         0B11111,
        //         0B10101,
        //         0B10101,
        //         0B11111,
        //         0B00000
        //     };
        //     lcd.createChar( 0, bitmapPattern );
        //     lcd.setCursor(0,0);
        //     lcd.write(0);
        // note: reset cursor position after this command
        virtual void createChar( uint8_t charID, const uint8_t charmap[] ) = 0;
        virtual void createChar( uint8_t charID, const char *charmap ) = 0;

        // createChar with PROGMEM input
        // ex. const char bell[8] PROGMEM = {B00100,B01110,B01110,B01110,B11111,B00000,B00100,B00000};
        //     write( (uint8_t) pgm_read_byte_near( charmap++ ) );

        virtual void command( uint8_t value ) = 0;

    //
    // EXTENSIONS
    //
    public:

        // print ch several times
        inline void printCharsN( char ch, int8_t count ) {
            //#if defined(ESP32)
            //    print( std::string( count, ch ).c_str() );
            //#else
                while( count-- > 0 )
                    write( ch );
            //#endif
        }

        // print with location
        template <typename TType>
        inline void printAt( uint8_t col, uint8_t row, TType value ) {
            setCursor( col, row );
            print( value );
        }

        // write with location
        inline void writeAt( uint8_t col, uint8_t row, uint8_t byte ) {
            setCursor( col, row );
            write( byte );
        }

        /*
        template <typename TType>
        void printAtRow( uint8_t row, TType value ) {
            // can't tell how many characters to clear
            // don't know current cursor position to clear to EOL
            // ??? can't clear line 1st, due to flicker
            // ??? no print() to buffer only printf()
            setCursor( 0, row );
            print( value );
            // ... clear to end of line
        }
        */

        // print string at start of line then fill with space to end
        void printStrAtRow( uint8_t row, const char *str ) {
            if ( str == nullptr ) return;
            setCursor( 0, row );
            // what me worry? printf( "%.*s", maxColumns, str );
            print( str );
            int len = maxColumns - strlen( str );
            if ( len > 0 ) printCharsN( ' ', len );
        }

        // print first N characters of string
        // fill the rest with spacea
        void printStrN( const char *str, uint8_t N, bool clearSpace = false ) {
            int8_t spaceAfter = 0;
            if ( clearSpace ) {
                spaceAfter = N - strlen( str );
                if ( spaceAfter < 0 ) spaceAfter = 0;
            }
            if ( str != nullptr ) {
                while( N > 0 && *str != 0 ) {
                    print( *str++ );
                    N--;
                }
            }
            if ( clearSpace )
                printCharsN( ' ', spaceAfter );
        }

        // clear whole line
        void clearRow( uint8_t row ) {                
            setCursor( 0, row );
            printCharsN( ' ', maxColumns );
        }

        // can't use - cursor position not tracked
        // inline virtual uint8_t currentColumn() = 0;
        // void clearToEOL() {
        //     uint8_t c = maxColumns - currentColumn();
        //     if ( c > 0 )
        //         printCharsN( ' ', maxColumns - currentColumn() );
        // }

        #if defined(ESP32)
            template <typename ... Ts>
            void printfAt( uint8_t col, uint8_t row, const char *format, Ts ... ts ) {
                // https://stackoverflow.com/questions/205529/passing-variable-number-of-arguments-around
                // https://stackoverflow.com/questions/1056411/how-to-pass-variable-number-of-arguments-to-printf-sprintf
                // NOT WORKING ON ARDUINO
                setCursor( col, row );
                printf( format, ts ... );
            }
        #else
            void printfAt( uint8_t col, uint8_t row, const char *format, ... ) {
                char buffer[maxColumns+1];
                va_list args;
                va_start( args, format );
                vsnprintf( buffer, maxColumns, format, args );
                va_end( args );
                setCursor( col, row );
                print( buffer );
            }
        #endif

        void printfAtRow( uint8_t row, const char *format, ... ) {
            char buffer[maxColumns+1];
            va_list args;
            va_start( args, format );
            vsnprintf( buffer, maxColumns, format, args );
            va_end( args );
            printStrAtRow( row, buffer );
        }

    //
    // FULL SCREEN
    //

    void showLines( const char *row1, const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr ) {
        clear();
        if ( row1 != nullptr ) printStrAtRow( 0, row1 );
        if ( row2 != nullptr ) printStrAtRow( 1, row2 );
        if ( row3 != nullptr ) printStrAtRow( 2, row3 );
        if ( row4 != nullptr ) printStrAtRow( 3, row4 );
        //lcd.displayAll();
    }
    
    inline void show_12345678901234567890( const char *row1, const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr ) {
        // wtf for?
        //
        // myLCD.
        // show_12345678901234567890(
        //     "can this fit in 20 chars?",
        //     "obviously not" );
        showLines( row1, row2, row3, row4 );
    }

    inline void show_01234567890123456789( const char *row1, const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr ) {
        // wtf for?
        //
        // myLCD.
        // show_01234567890123456789(
        //     "what col# is X on?",
        //     "it's 13" );
        showLines( row1, row2, row3, row4 );
    }



    //
    // ALIAS
    //
    /*
    public:

        // not implemented ???
        //void printLeft();
        //void printRight();
        //void shiftIncrement();
        //void shiftDecrement();

        inline void cursor_on()  { cursor(); }
        inline void cursor_off() { noCursor(); }
        inline void blink_on()   { blink(); }
        inline void blink_off()  { noBlink(); }
        inline void load_custom_character( uint8_t char_num, uint8_t *rows ) { createChar( char_num, rows ); }
        void setBacklight( uint8_t new_val ) { if ( new_val ) backlight(); else noBacklight(); }
        void printstr( const char c[] ) {
            // This function is not identical to the function used for "real" I2C displays
            // it's here so the user sketch doesn't have to be changed 
            print( c );
        }
    */

    //
    // UNSUPPORTED (not implemented in this library)
    //
    /*
    public:

        // unsupported API functions
        #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wunused-parameter"
            uint8_t status()                                { return 0; }
            void    setContrast( uint8_t new_val )          {}
            uint8_t keypad()                                { return 0; }
            void    setDelay( int cmdDelay, int charDelay ) {}
            void    on()                                    {}
            void    off()                                   {} 
            uint8_t init_bargraph( uint8_t graphtype ) { return 0; }
            void    draw_horizontal_graph( uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end ) {}
            void    draw_vertical_graph( uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_row_end ) {}
        #pragma GCC diagnostic pop    
    */

};

}

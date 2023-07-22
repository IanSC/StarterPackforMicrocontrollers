//  2022/01 - isc
//
//  Interface for Directly Wired LCD
//  --------------------------------
//  - unified HD44780 with i2c version
//    only difference is how data is sent to screen
//  - converted from my PIC days
//  - need this to make it compatible with buffering
//  - compatible with <LiquidCrystal.h>
//
//  To Use
//
//      const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
//      LCD_wired lcd = LCD_wired( rs, en, d4, d5, d6, d7 );
//      ...
//      lcd.begin( 16, 2 );

#pragma once
#include <LCD/LCD_HD44780.h>

// NOT FULLY TESTED: !!!
// - R/W mode
// - 8-bit port
// - read routines
// WHY NOT? no one is going to use it anyway, everything's i2c

// define these before calling header (all unused funcs are stripped out)
//
// #define LCD_READ_WRITE_MODE       // R/W pin of LCD is used, otherwise R/W pin is hardwired as write (low)
// #define LCD_USE_8BIT_PORT         // handle 8 bit data mode
// #define LCD_INCLUDE_READ_ROUTINES // even in read-only mode, include reading routines

namespace StarterPack {

class LCD_wired : public LCD_HD44780 {

    // data port, in/out
    #if defined( LCD_USE_8BIT_PORT )
        uint8_t LCD_DB0 = 0;
        uint8_t LCD_DB1 = 0;
        uint8_t LCD_DB2 = 0;
        uint8_t LCD_DB3 = 0;
    #endif
    uint8_t LCD_DB4 = 0;
    uint8_t LCD_DB5 = 0;
    uint8_t LCD_DB6 = 0;
    uint8_t LCD_DB7 = 0;

    // always outputs
    uint8_t LCD_RS = 0;
    uint8_t LCD_E  = 0;

    #if defined( LCD_READ_WRITE_MODE )

        uint8_t LCD_RW = 0;

        bool dataPortIsOutput = true;
    
        void setDataPortToWrite( bool setAsOutput ) {
            if ( dataPortIsOutput == setAsOutput ) return;
            dataPortIsOutput = setAsOutput;
            if ( dataPortIsOutput ) {
                #if defined( LCD_USE_8BIT_PORT )
                    pinMode( LCD_DB0, OUTPUT );
                    pinMode( LCD_DB1, OUTPUT );
                    pinMode( LCD_DB2, OUTPUT );
                    pinMode( LCD_DB3, OUTPUT );
                #endif
                pinMode( LCD_DB4, OUTPUT );
                pinMode( LCD_DB5, OUTPUT );
                pinMode( LCD_DB6, OUTPUT );
                pinMode( LCD_DB7, OUTPUT );
            } else {
                #if defined( LCD_USE_8BIT_PORT )
                    pinMode( LCD_DB0, INPUT );
                    pinMode( LCD_DB1, INPUT );
                    pinMode( LCD_DB2, INPUT );
                    pinMode( LCD_DB3, INPUT );
                #endif
                pinMode( LCD_DB4, INPUT );
                pinMode( LCD_DB5, INPUT );
                pinMode( LCD_DB6, INPUT );
                pinMode( LCD_DB7, INPUT );
            }
        }

        inline void setWriteMode() {
            digitalWrite( LCD_RW, LOW );            
        }
        inline void setReadMode() {
            digitalWrite( LCD_RW, HIGH );            
        }

    #else

        // write only, r/w of lcd hard wired to low
        #define setWriteMode()        ;
        #define setReadMode()         ;
        #define setDataPortToWrite(x) ;

    #endif

public:

    #if defined( LCD_USE_8BIT_PORT )
        #if defined( LCD_READ_WRITE_MODE )
            inline LCD_wired( uint8_t rs, uint8_t rw, uint8_t enable,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
                LCD_RS = rs; LCD_RW = rw; LCD_E = enable;
                LCD_DB0 = d0; LCD_DB1 = d1; LCD_DB2 = d2; LCD_DB3 = d3;
                LCD_DB4 = d4; LCD_DB5 = d5; LCD_DB6 = d6; LCD_DB7 = d7;
                initPort();
            }
        #else    
            inline LCD_wired( uint8_t rs, uint8_t enable,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
                LCD_RS = rs; LCD_E = enable;
                LCD_DB0 = d0; LCD_DB1 = d1; LCD_DB2 = d2; LCD_DB3 = d3;
                LCD_DB4 = d4; LCD_DB5 = d5; LCD_DB6 = d6; LCD_DB7 = d7;
                initPort();
            }
        #endif        
    #else
        #if defined( LCD_READ_WRITE_MODE )
            inline LCD_wired( uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
                LCD_RS = rs; LCD_RW = rw; LCD_E = enable;
                LCD_DB4 = d4; LCD_DB5 = d5; LCD_DB6 = d6; LCD_DB7 = d7;
                initPort();
            }
        #else
            inline LCD_wired( uint8_t rs, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
                LCD_RS = rs; LCD_E = enable;
                LCD_DB4 = d4; LCD_DB5 = d5; LCD_DB6 = d6; LCD_DB7 = d7;
                initPort();
            }
        #endif
    #endif

    inline void setTimeoutInMs( uint16_t timeOut ) override {}
    inline void setFrequency( uint32_t frequency ) override {}

    void initPort() {
        // initialize port directions
        pinMode( LCD_RS, OUTPUT );
        pinMode( LCD_E, OUTPUT );
        #if defined( LCD_READ_WRITE_MODE )
            pinMode( LCD_RW, OUTPUT );
            setDataPortToWrite( true );
        #else
            // default to data output
            #if defined( LCD_USE_8BIT_PORT )
                pinMode( LCD_DB0, OUTPUT );
                pinMode( LCD_DB1, OUTPUT );
                pinMode( LCD_DB2, OUTPUT );
                pinMode( LCD_DB3, OUTPUT );
            #endif
            pinMode( LCD_DB4, OUTPUT );
            pinMode( LCD_DB5, OUTPUT );
            pinMode( LCD_DB6, OUTPUT );
            pinMode( LCD_DB7, OUTPUT );
        #endif
    }

    //
    // VERIFY / RECOVERY
    //
    public:

        // no way to detect errors
        // also if reconnected will still work unless power is removed
        // in which case will revert to 8-bit mode
        
        inline bool     verify()            override { return true; }
        inline ERROR_NO verifyWithError()   override { return 0; }
        inline bool     recoverIfHasError() override { return false; }
        inline void     setRecoveryThrottleInMs( uint16_t delay ) override {}

        void reset() override {
            begin( maxColumns, maxRows, dotSize );
        }

    //
    // USER COMMANDS
    //
    public:

        inline void backlightOn()  override {} // not supported
        inline void backlightOff() override {}

    //
    // CORE
    //
    public:

        void command( uint8_t com ) override {
            digitalWrite( LCD_RS, LOW );
            setWriteMode();
            setDataPortToWrite( true );
            LCD_SEND_COMMAND( com );
            delayMicroseconds( 37 );
        }

        size_t write( uint8_t ch ) override {
            digitalWrite( LCD_RS, HIGH );
            setWriteMode();
            setDataPortToWrite( true );
            LCD_SEND_COMMAND( ch );
            delayMicroseconds(37);
            return 1;
        }

    //
    // LOW LEVEL
    //
    private:

        inline void pulse() {
            // https://html.alldatasheet.com/html-pdf/63673/HITACHI/HD44780/12294/49/HD44780.html
            digitalWrite( LCD_E, HIGH );
            // pulse width - 450ns
            delayMicroseconds( 1 );
            digitalWrite( LCD_E, LOW );
            // write - setup time 195ns
            // read  - delay time 360ns
            delayMicroseconds( 1 );
        }

        #if defined( LCD_USE_8BIT_PORT )

            void LCD_SEND_COMMAND( uint8_t ch ) {
                // easier to convert to fastDigitalWrite()
                // digitalWrite( LCD_E, HIGH );
                // PORTB = ch;
                // assume HIGH==1, LOW==0
                digitalWrite( LCD_DB0, ch & 1 ); ch >>= 1;
                digitalWrite( LCD_DB1, ch & 1 ); ch >>= 1;
                digitalWrite( LCD_DB2, ch & 1 ); ch >>= 1;
                digitalWrite( LCD_DB3, ch & 1 ); ch >>= 1;
                digitalWrite( LCD_DB4, ch & 1 ); ch >>= 1;
                digitalWrite( LCD_DB5, ch & 1 ); ch >>= 1;
                digitalWrite( LCD_DB6, ch & 1 ); ch >>= 1;
                digitalWrite( LCD_DB7, ch & 1 );
                // digitalWrite( LCD_E, LOW );
                pulse();
            }
            
        #else
    
            void sendNibble( uint8_t nibble ) override {
                // send lower nibble
                digitalWrite( LCD_RS, LOW );
                setWriteMode();
                setDataPortToWrite( true );
                LCD_putNibble( nibble );
                pulse();
            }

            void LCD_putNibble( uint8_t ch ) {
                // easier to convert to fastDigitalWrite()
                // assume HIGH==1, LOW==0
                digitalWrite( LCD_DB4, ch & 1 ); ch >>= 1;
                digitalWrite( LCD_DB5, ch & 1 ); ch >>= 1;
                digitalWrite( LCD_DB6, ch & 1 ); ch >>= 1;
                digitalWrite( LCD_DB7, ch & 1 );
            }

            void LCD_SEND_COMMAND( uint8_t ch ) {
                
                // https://html.alldatasheet.com/html-pdf/63673/HITACHI/HD44780/8278/33/HD44780.html
                
                // upper nibble
                // digitalWrite( LCD_E, HIGH );
                LCD_putNibble( ch >> 4 );
                // digitalWrite( LCD_E, LOW );
                pulse();

                // lower nibble
                // digitalWrite( LCD_E, HIGH );
                LCD_putNibble( ch );
                // digitalWrite( LCD_E, LOW );
                pulse();
            };
            
        #endif

    //
    // READ ROUTINES
    //
    
        #if defined( LCD_READ_WRITE_MODE ) && defined( LCD_INCLUDE_READ_ROUTINES )
        
            uint8_t LCD_readCore() {
                
                // set to input
                setReadMode();
                setDataPortToWrite( false );

                uint8_t result = 0;
                #if defined( LCD_USE_8BIT_PORT )
                    // digitalWrite( LCD_E, HIGH );
                    // digitalWrite( LCD_E, LOW );
                    pulse();
                    // delay here ???
                    // result = PORTB;
                    if ( digitalRead( LCD_DB7 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB6 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB5 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB4 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB3 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB2 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB1 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB0 ) ) result |= 1;
                #else                                    
                    // digitalWrite( LCD_E, HIGH );
                    // digitalWrite( LCD_E, LOW );
                    pulse();
                    // delay here ???
                    if ( digitalRead( LCD_DB7 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB6 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB5 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB4 ) ) result |= 1; result <<= 1;
                    // digitalWrite( LCD_E, HIGH );
                    // digitalWrite( LCD_E, LOW );
                    pulse();
                    // delay here ???
                    if ( digitalRead( LCD_DB7 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB6 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB5 ) ) result |= 1; result <<= 1;
                    if ( digitalRead( LCD_DB4 ) ) result |= 1;                        
                #endif
                return result;
            }
                
            bool LCD_isBusy() {
                // https://html.alldatasheet.com/html-pdf/63673/HITACHI/HD44780/6019/24/HD44780.html
                digitalWrite( LCD_RS, LOW );
                // delayMicroseconds( 10 );
                uint8_t result = LCD_readCore();
                return ( result & 0b10000000 ) != 0;
            }

            // https://html.alldatasheet.com/html-pdf/63673/HITACHI/HD44780/7776/31/HD44780.html

            uint8_t LCD_readCGRAM( uint8_t addr ) {
                command( LCD_SET_CGRAM_ADDR + addr );
                digitalWrite( LCD_RS, HIGH );
                // delayMicroseconds( 10 );
                return LCD_readCore();
            }
            uint8_t LCD_readDDRAM( uint8_t addr ) {
                command( LCD_SET_DDRAM_ADDR + addr );
                digitalWrite( LCD_RS, HIGH );
                // delayMicroseconds( 10 );
                return LCD_readCore();
            }
            uint8_t LCD_readData() {
                // read succeeding data
                digitalWrite( LCD_RS, HIGH );
                // delayMicroseconds( 10 );
                return LCD_readCore();
            }
            
        #endif
    
};

}

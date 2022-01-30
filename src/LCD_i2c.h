//  2022/01 - isc
//
//  LCD I2C
//  -------
//  - based on <LiquidCrystal_I2C.h> (not needed anymore)
//    https://github.com/johnrickman/LiquidCrystal_I2C/blob/master/LiquidCrystal_I2C.h
//    https://github.com/johnrickman/LiquidCrystal_I2C/blob/master/LiquidCrystal_I2C.cpp
//  - cannot use the library directly because it is fixed to global variable "Wire"
//    and not all error conditions are checked, so this instead uses zz_TwoWireHelper()
//    which can recover from disconnected wires
//    eg. disconnect any/all VCC, GND, SDA, SCL
//        after reconnecting, communication will continue without rebooting
//
//  To Use
//
//      LCD_i2c lcd = LCD_i2c( WireX );
//
//  To Use As Buffered
//
//      LCD_i2c lcd = LCD_i2c( WireX );
//      LCDBuffered lcdBuffered = LCDBuffered( lcd );
//
//      lcdBuffered.print( "hello" );
//      lcdBuffered.updateAllNow();
//
//  To Recover
//
//      coid loop() {
//          ...
//          lcd.recoverIfHasError();
//      }

#pragma once
#include <LCDInterface.h>
#include <i2cHelper.h>

//
// 

        //  use    use    need
        //  Wire   0x27   begin()   usage
        //  ----   ----   -------   -----
        //  yes    yes    yes       zz_LCD_i2c lcd = zz_LCD_i2c()
        //  yes    no     yes       zz_LCD_i2c lcd = zz_LCD_i2c( i2cAddress )
        //  no     yes    yes       zz_LCD_i2c lcd = zz_LCD_i2c( wireX )
        //  no     no     yes       zz_LCD_i2c lcd = zz_LCD_i2c( wireX, i2cAddress )
        //  no     no     yes       zz_LCD_i2c lcd = zz_LCD_i2c( TwoWireHelper )
        //
        //  yes    yes    no        zz_LCD_i2c lcd = zz_LCD_i2c( cols, rows )
        //  yes    no     no        zz_LCD_i2c lcd = zz_LCD_i2c( cols, rows, i2cAddress )
        //  no     yes    no        zz_LCD_i2c lcd = zz_LCD_i2c( wireX, cols, rows )
        //  no     no     no        zz_LCD_i2c lcd = zz_LCD_i2c( wireX, cols, rows, i2cAddress )
        //  no     no     no        zz_LCD_i2c lcd = zz_LCD_i2c( TwoWireHelper, cols, rows )
        //
        //  use Wire     - use global "Wire" variable
        //  use 0x27     - default address if not specified
        //  need begin() - need to call begin( cols, rows ) after

namespace StarterPack {

class LCD_i2c : public LCDInterface {

        i2cHelper *_wireHelper;
        bool createdHelper = false;
        uint8_t _i2cAddress;

    public:

        ~LCD_i2c() {
            if ( createdHelper ) delete _wireHelper;
        }

        static const uint8_t i2cDefaultAddress = 0x27;

        // seems to need this for oled display ????
        // when clear() is called, cursor does not automatically move to (0,0)
        bool resetCursorOnClear = false;

        LCD_i2c( int16_t i2cAddress = -1 ) {
            // default to "Wire"
            init( Wire, i2cAddress );
        }
        LCD_i2c( TwoWire &wire, int16_t i2cAddress = -1 ) {
            // use specified wire
            init( wire, i2cAddress );
        }
        LCD_i2c( i2cHelper &wireHelper, int16_t i2cAddress = -1 ) {
            if ( i2cAddress == -1 )
                this->_i2cAddress = i2cDefaultAddress;
            else
                this->_i2cAddress = i2cAddress;
            _wireHelper = &wireHelper;
        }

        inline void setTimeoutInMs( uint16_t timeOut ) {
            _wireHelper->setTimeoutInMs( timeOut );
        }

        inline void setFrequency( uint32_t frequency ) {
            _wireHelper->setFrequency( frequency );
        }
        
//        LCD_i2c( uint8_t maxColumns, uint8_t maxRows, int16_t i2cAddress = -1 ) {
//            init( Wire, i2cAddress );
//            begin( maxColumns, maxRows );
//        }
//        LCD_i2c( TwoWire &wire, uint8_t maxColumns, uint8_t maxRows, int16_t i2cAddress = -1 ) {
//            init( wire, i2cAddress );
//            begin( maxColumns, maxRows );
//        }
//        LCD_i2c( zz_TwoWireHelper &wireHelper, uint8_t maxColumns, uint8_t maxRows ) {
//            _wireHelper = &wireHelper;
//            begin( maxColumns, maxRows );
//        }

    //
    // BEGIN
    //
    private:

        uint8_t _displaycontrol;
        uint8_t _displaymode;
        uint8_t _backlightStatus = PIN_BACKLIGHT;

        inline void init( TwoWire &wire, int16_t i2cAddress = -1 ) {
            if ( i2cAddress == -1 ) i2cAddress = i2cDefaultAddress;
            this->_i2cAddress = i2cAddress;
            _wireHelper = new i2cHelper( wire, i2cAddress );
            createdHelper = true;
        }

    public:

        void begin( uint8_t maxColumns, uint8_t maxRows, charDotSize dotSize = charDotSize::size5x8 ) {

            // When the display powers up, it is configured as follows:
            //
            // 1. Display clear
            // 2. Function set: 
            //    DL = 1; 8-bit interface data 
            //    N = 0; 1-line display 
            //    F = 0; 5x8 dot character font 
            // 3. Display on/off control: 
            //    D = 0; Display  off 
            //    C = 0; Cursor   off 
            //    B = 0; Blinking off 
            // 4. Entry mode set: 
            //    I/D = 1; Increment by 1
            //    S = 0; No shift 
            //
            // Note, however, that resetting the Arduino doesn't reset the LCD, so we
            // can't assume that it's in that state when a sketch starts (and the
            // LiquidCrystal constructor is called).

            // flags for function set
            // static const uint8_t lcd_8BITMODE = 0x10; // not applicable
            static const uint8_t lcd_4BITMODE = 0x00;
            static const uint8_t lcd_2LINE    = 0x08;
            static const uint8_t lcd_1LINE    = 0x00;
            //static const uint8_t lcd_5x10DOTS = 0x04;
            //static const uint8_t lcd_5x8DOTS  = 0x00;            

            this->maxColumns = maxColumns;
            this->maxRows = maxRows;
            this->dotSize = dotSize;
            
            // turn on backlight, so when recovering
            // will not blink off/on
            _backlightStatus = PIN_BACKLIGHT;

            // for some 1-line displays you can select a 10 pixel high font
            // leave up to user, just apply 5x10 if specified
            uint8_t _displayfunction = lcd_4BITMODE | dotSize;
            if ( maxRows > 1 )
                _displayfunction |= lcd_2LINE;
            else
                _displayfunction |= lcd_1LINE;

            /*
            _backlightStatus = PIN_NOBACKLIGHT;
            uint8_t _displayfunction = lcd_4BITMODE | lcd_1LINE; // | lcd_5x8DOTS;
            if ( maxRows > 1 )
                _displayfunction |= lcd_2LINE;

            // for some 1-line displays you can select a 10 pixel high font
            //if ( (dotSize != charDotSize::size5x8) && (maxRows == 1) )
            //    _displayfunction |= lcd_5x10DOTS;
            // up to the user to select
            _displayfunction |= dotSize;
            */

            // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
            // according to datasheet, we need at least 40ms after power rises above 2.7V
            // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
            delay( 5 ); 
        
            // Now we pull both RS and R/W low to begin commands
            expanderWrite( 0 );
            //noBacklight(); // reset expander and turn backlight off
            delay( 1000 );

            // put the LCD into 4 bit mode
            // this is according to the hitachi HD44780 datasheet
            // figure 24, pg 46
            
            // we start in 8bit mode, try to set 4 bit mode
            write4bits( 0x03 << 4 ); delayMicroseconds( 4500 ); // wait min 4.1ms
            write4bits( 0x03 << 4 ); delayMicroseconds( 4500 ); // second try, wait min 4.1ms
            write4bits( 0x03 << 4 ); delayMicroseconds( 150 );  // third go!
            write4bits( 0x02 << 4 );                            // finally, set to 4-bit interface
            command( lcd_FUNCTIONSET | _displayfunction );      // set # lines, font size, etc.
            
            // turn the display on with no cursor or blinking default
            _displaycontrol = 0; // lcd_DISPLAYON | lcd_CURSOROFF | lcd_BLINKOFF;
            displayOn();
            clear(); // clear it off
            
            // initialize to default text direction (for roman languages)
            _displaymode = lcd_ENTRYLEFT | lcd_ENTRYSHIFTDECREMENT;
                        
            command( lcd_ENTRYMODESET | _displaymode ); // set the entry mode            
            home();
            //backlight();
        }

    //
    // VERIFY / RECOVERY
    //
    private:

        charDotSize dotSize; // copy when initialized, to use for recovery

    public:
    
        bool verify() {
            return _wireHelper->verify( _i2cAddress );
        }
        ERROR_NO verifyWithError() {
            return _wireHelper->verifyWithError( _i2cAddress );
        }
        
        bool recoverIfHasError() {
            bool r = _wireHelper->recoverIfHasError( _i2cAddress );
            if ( r ) {
                // Serial.println( "LCD RECO" );
                reset();
            }
            return r;
        }
        inline void setRecoveryThrottleInMs( uint16_t delay ) {
            _wireHelper->recoveryThrottleInMs = delay;
            // _wireHelper->setRecoveryThrottleInMs( delay );
        }
        void reset() {
            // if VCC/GND was disconnected, LCD will revert to 8-bit data bus, so must reset
            begin( maxColumns, maxRows, dotSize );
        }
        
    //
    // WRITE
    //
    public:

        // #if defined(ARDUINO) && ARDUINO >= 100
            inline size_t write( uint8_t value ) {
                send( value, PIN_Rs );
                return 1;
            }
        // #else
        //     #include <WProgram.h>
        //     inline void write( uint8_t value ) {
        //         send( value, Rs );
        //     }
        // #endif

    //
    // USER COMMANDS
    //
    public:

        void clear() {
            command( lcd_CLEARDISPLAY ); // clear display, set cursor position to zero
            delayMicroseconds( 2000 );   // this command takes a long time!
            if ( resetCursorOnClear ) setCursor( 0, 0 ); // oled needs this ?
        }

        void home() {
            command( lcd_RETURNHOME  ); // set cursor position to zero
            delayMicroseconds( 2000 );  // this command takes a long time!
        }

        void setCursor( uint8_t col, uint8_t row ) {
            int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
            if ( row > maxRows ) row = maxRows - 1;
            command( lcd_SETDDRAMADDR | (col + row_offsets[row]) );
        }
        
        // Turn the (optional) backlight off/on
        void backlightOn() {
            _backlightStatus = PIN_BACKLIGHT;
            expanderWrite(0);
        }
        void backlightOff() {
            _backlightStatus = PIN_NOBACKLIGHT;
            expanderWrite(0);
        }

        // Turn the display on/off (quickly)
        void displayOn() {
            _displaycontrol |= lcd_DISPLAYON;
            command( lcd_DISPLAYCONTROL | _displaycontrol );
            // LCDSupport::TurnOn( _displayControl, LCDSupport::DisplayOn );
        }
        void displayOff() {
            _displaycontrol &= ~lcd_DISPLAYON;
            command( lcd_DISPLAYCONTROL | _displaycontrol );
            // LCDSupport::TurnOff( _displayControl, LCDSupport::DisplayOn );
        }

        // Turns the underline cursor on/off
        void cursorOn() {
            _displaycontrol |= lcd_CURSORON;
            command( lcd_DISPLAYCONTROL | _displaycontrol );
            // LCDSupport::TurnOn( _displayControl, LCDSupport::CursorOn );
            // command( LCD_DISPLAYCONTROL | LCDSupport::Convert( _displayControl ) );
        }
        void cursorOff() {
            _displaycontrol &= ~lcd_CURSORON;
            command( lcd_DISPLAYCONTROL | _displaycontrol );
            // LCDSupport::TurnOff( _displayControl, LCDSupport::CursorOn );
            // command( LCD_DISPLAYCONTROL | LCDSupport::Convert( _displayControl ) );
        }

        // Turn on and off the blinking cursor
        void cursorBlinkOn() {
            _displaycontrol |= lcd_BLINKON;
            command( lcd_DISPLAYCONTROL | _displaycontrol );
            // LCDSupport::TurnOn( _displayControl, LCDSupport::BlinkOn );
            // command( LCD_DISPLAYCONTROL | LCDSupport::Convert( _displayControl ) );
        }
        void cursorBlinkOff() {
            _displaycontrol &= ~lcd_BLINKON;
            command( lcd_DISPLAYCONTROL | _displaycontrol );
            // LCDSupport::TurnOff( _displayControl, LCDSupport::BlinkOn );
            // command( LCD_DISPLAYCONTROL | LCDSupport::Convert( _displayControl ) );
        }

        // These commands scroll the display without changing the RAM
        void scrollDisplayLeft() {
            command( lcd_CURSORSHIFT | lcd_DISPLAYMOVE | lcd_MOVELEFT );
        }
        void scrollDisplayRight() {
            command( lcd_CURSORSHIFT | lcd_DISPLAYMOVE | lcd_MOVERIGHT );
        }

        // text flow direction: Left to Right, Right to Left
        void leftToRight() {
            _displaymode |= lcd_ENTRYLEFT;
            command( lcd_ENTRYMODESET | _displaymode );
        }
        void rightToLeft() {
            _displaymode &= ~lcd_ENTRYLEFT;
            command( lcd_ENTRYMODESET | _displaymode );
        }

        // This will 'right justify' text from the cursor
        void autoscrollOn() {
            _displaymode |= lcd_ENTRYSHIFTINCREMENT;
            command( lcd_ENTRYMODESET | _displaymode );
        }

        // This will 'left justify' text from the cursor
        void autoscrollOff() {
            _displaymode &= ~lcd_ENTRYSHIFTINCREMENT;
            command( lcd_ENTRYMODESET | _displaymode );
        }

        // Allows us to fill the first 8 CGRAM locations
        // with custom characters
        void createChar( uint8_t location, uint8_t charmap[] ) {
            location &= 0x7; // we only have 8 locations 0-7
            command( lcd_SETCGRAMADDR | (location << 3) );
            for( int i = 0 ; i < 8 ; i++ ) {
                write( charmap[i] );
            }
        }

        // createChar with PROGMEM input
        // Example: 	const char bell[8] PROGMEM = {B00100,B01110,B01110,B01110,B11111,B00000,B00100,B00000};
        void createChar( uint8_t location, const char *charmap ) {
            location &= 0x7; // we only have 8 locations 0-7
            command( lcd_SETCGRAMADDR | (location << 3) );
            for( int i = 0 ; i < 8 ; i++ )
                write( pgm_read_byte_near( charmap++ ) );
        }

        inline void command( uint8_t value ) {
            send(value, 0);
        }

    //
    // CORE COMMANDS
    //
    private:

        // write either command or data
        void send( uint8_t value, uint8_t mode ) {
            uint8_t highnib = value & 0xF0;
            uint8_t lownib  = ( value << 4 ) & 0xF0;
            write4bits( highnib | mode );
            write4bits( lownib  | mode ); 
        }

        void write4bits( uint8_t value ) {
            expanderWrite( value );
            pulseEnable( value );
        }

        void expanderWrite( uint8_t _data ) {
            _wireHelper->writeOneByte_i2c( _i2cAddress, _data | _backlightStatus );
//            _wireHelper->writeOneByte( _data | _backlightStatus );
            // Wire.beginTransmission(_Addr);
            // printIIC((int)(_data) | _backlightval);
            // Wire.endTransmission();
        }

        void pulseEnable( uint8_t _data ) {
            _data = _data | _backlightStatus;
            expanderWrite( _data | PIN_En );  // En high
            delayMicroseconds( 1 );		      // enable pulse must be >450ns
            expanderWrite( _data & ~PIN_En ); // En low
            delayMicroseconds( 50 );          // commands need > 37us to settle
        }

    //
    // CONSTANTS
    //
    private:

        // https://www.youtube.com/watch?v=DXjogGsJlhU
        // https://components101.com/sites/default/files/component_pin/16x2-LCD-Pinout.png
        //
        //  PCF8574 I2C Expander        LCD Display
        //  --------------------        -----------
        //      1   GND                 GND           
        //      2   +5v                 VDD
        //      3   +5v pot             constrast
        //      4   bit 0               register select
        //      5   bit 1               read/write
        //      6   bit 2               enable
        //      7                       D0
        //      8                       D1
        //      9                       D2
        //     10                       D3
        //     11   bit 4               D4
        //     12   bit 5               D5
        //     13   bit 6               D6
        //     14   bit 7               D7
        //     15                       LED +
        //     16   bit 3               LED -
        //          transistor output
        //          40v 500mA NPN

        // i2c expander pins
        static const uint8_t PIN_Rs          = 0B00000001; // Register Select bit
        static const uint8_t PIN_Rw          = 0B00000010; // Read/Write bit
        static const uint8_t PIN_En          = 0B00000100; // Enable bit
        static const uint8_t PIN_BACKLIGHT   = 0B00001000; // flags for backlight control
        static const uint8_t PIN_NOBACKLIGHT = 0B00000000;

        // commands
        static const uint8_t lcd_CLEARDISPLAY   = 0x01;
        static const uint8_t lcd_RETURNHOME     = 0x02;
        static const uint8_t lcd_ENTRYMODESET   = 0x04;
        static const uint8_t lcd_DISPLAYCONTROL = 0x08;
        static const uint8_t lcd_CURSORSHIFT    = 0x10;
        static const uint8_t lcd_FUNCTIONSET    = 0x20;
        static const uint8_t lcd_SETCGRAMADDR   = 0x40;
        static const uint8_t lcd_SETDDRAMADDR   = 0x80;

        // flags for display entry mode
        static const uint8_t lcd_ENTRYRIGHT          = 0x00;
        static const uint8_t lcd_ENTRYLEFT           = 0x02;
        static const uint8_t lcd_ENTRYSHIFTINCREMENT = 0x01;
        static const uint8_t lcd_ENTRYSHIFTDECREMENT = 0x00;

        // flags for display on/off control
        static const uint8_t lcd_DISPLAYON  = 0x04;
        static const uint8_t lcd_DISPLAYOFF = 0x00;
        static const uint8_t lcd_CURSORON   = 0x02;
        static const uint8_t lcd_CURSOROFF  = 0x00;
        static const uint8_t lcd_BLINKON    = 0x01;
        static const uint8_t lcd_BLINKOFF   = 0x00;

        // flags for display/cursor shift
        static const uint8_t lcd_DISPLAYMOVE = 0x08;
        static const uint8_t lcd_CURSORMOVE  = 0x00;
        static const uint8_t lcd_MOVERIGHT   = 0x04;
        static const uint8_t lcd_MOVELEFT    = 0x00;

};

}

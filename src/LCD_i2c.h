//  2022/01 - isc
//
//  LCD I2C
//  -------
//  - unified HD44780 with wired version
//    only difference is how data is sent to screen
//  - pulsing based on <LiquidCrystal_I2C.h> (not needed anymore)
//    https://github.com/johnrickman/LiquidCrystal_I2C/blob/master/LiquidCrystal_I2C.h
//    https://github.com/johnrickman/LiquidCrystal_I2C/blob/master/LiquidCrystal_I2C.cpp
//  - cannot use the library directly because it is fixed to global variable "Wire"
//    and not all error conditions are checked, so this instead uses TwoWireHelper()
//    which can recover from disconnected wires
//    eg. disconnect any/all VCC, GND, SDA, SCL
//        after reconnecting, communication will continue without rebooting
//
//  To Use
//
//      LCD_i2c lcd = LCD_i2c( i2cAddress );
//      lcd.begin( 16, 2 );
//
//  To Recover
//
//      void loop() {
//          ...
//          lcd.recoverIfHasError();
//      }

#pragma once
#include <LCD_HD44780.h>
#include <i2cHelper.h>

namespace StarterPack {

class LCD_i2c : public LCD_HD44780 {

        i2cHelper *_wireHelper;
        bool deleteHelperAfter = false;
        uint8_t _i2cAddress;

        inline void init( TwoWire &wire, int16_t i2cAddress = -1 ) {
            if ( i2cAddress == -1 ) i2cAddress = i2cDefaultAddress;
            this->_i2cAddress = i2cAddress;
            _wireHelper = new i2cHelper( wire, i2cAddress );
            deleteHelperAfter = true;
        }

    public:

        static const uint8_t i2cDefaultAddress = 0x27;

        ~LCD_i2c() {
            if ( deleteHelperAfter ) delete _wireHelper;
        }

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

    //
    // VERIFY / RECOVERY
    //
    public:
    
        inline bool verify() {
            return _wireHelper->verify( _i2cAddress );
        }
        inline ERROR_NO verifyWithError() {
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
        }
        inline void reset() {
            // if VCC/GND was disconnected, LCD will revert to 8-bit data bus, so must reset
            begin( maxColumns, maxRows, dotSize );
        }

    //
    // USER COMMANDS
    //
    public:

        uint8_t _backlightStatus = PIN_BACKLIGHT;

    public:

        void backlightOn() {
            _backlightStatus = PIN_BACKLIGHT;
            expanderWrite( _backlightStatus );
        }
        void backlightOff() {
            _backlightStatus = PIN_NOBACKLIGHT;
            expanderWrite( _backlightStatus );
        }

    //
    // CORE
    //
    public:

        inline void command( uint8_t value ) {
            // RS = LOW
            send( value, 0 );
        }

        inline size_t write( uint8_t value ) {
            // RS = HIGH
            send( value, PIN_RS );
            return 1;
        }

    //
    // LOW LEVEL
    //
    private:

        void send( uint8_t value, uint8_t mode ) {
            uint8_t hiNibble = value & 0xF0;
            uint8_t loNibble = ( value << 4 ) & 0xF0;
            sendCore( hiNibble | mode );
            sendCore( loNibble | mode );
        }

        void sendCore( uint8_t value ) {
            value |= _backlightStatus;
            expanderWrite( value | PIN_EN ); // EN high - send together
            delayMicroseconds( 1 );          // enable pulse must be >450ns
            expanderWrite( value );          // EN low
            delayMicroseconds( 50 );         // commands need > 37us to settle
        }

        inline void expanderWrite( uint8_t data ) {
            _wireHelper->writeOneByte_i2c( _i2cAddress, data );
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
        static const uint8_t PIN_RS          = 0B00000001; // Register Select bit
        static const uint8_t PIN_RW          = 0B00000010; // Read/Write bit
        static const uint8_t PIN_EN          = 0B00000100; // Enable bit
        static const uint8_t PIN_BACKLIGHT   = 0B00001000; // flags for backlight control
        static const uint8_t PIN_NOBACKLIGHT = 0B00000000;

};

}

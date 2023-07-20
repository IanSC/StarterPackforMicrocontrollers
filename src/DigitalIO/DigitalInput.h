//  2023-07 isc
//
//  Digital Input
//  - used for digital input
//  - handles initialization, pullup, pulldown
//  
//      DigitalInput( pin, options = ACTIVE_LOW_PULLUP )
//
//          options = ACTIVE_HIGH     HIGH is considered pressed
//                    ACTIVE_LOW      LOW is considered pressed
//                    NO_PULLUP       no pullup
//                    WITH_PULLUP     with pullup to +VCC
//                    WITH_PULLDOWN   with pulldown to GND (ESP32 only)
//                    STANDARD             = ACTIVE_HIGH | NO_PULLUP    // just set as INPUT
//                    ACTIVE_LOW_PULLUP    = ACTIVE_LOW  | WITH_PULLUP, // best for Arduino
//                    ACTIVE_HIGH_PULLDOWN = ACTIVE_HIGH | WITH_PULLDOWN
//          activeDebounceTime        debounce time when key is pressed
//          inactiveDebounceTime      debounce time when key is released
//
//      uint8_t readRaw()       same as digitalRead()
//
//                              the following handles active high/low logic
//                              ex. if initialized active low, will return true if pin is LOW
//      bool    read()          returns state
//      bool    isOn()          true if active
//      bool    isOff()         true if not active

#pragma once

#include <Arduino.h>
#include <inttypes.h>

namespace StarterPack {

class DigitalInput {

    protected:

        uint8_t PIN;                    // pin number to read
        bool INACTIVE_STATE = false;    // read result to be considered not "active"

    public:
    
        virtual ~DigitalInput() {}

        enum class optionsEnum : uint8_t {
            ACTIVE_HIGH   = 0,
            ACTIVE_LOW    = 1,
            NO_PULLUP     = 0,
            WITH_PULLUP   = 2,
            WITH_PULLDOWN = 4,
            STANDARD             = ACTIVE_HIGH | NO_PULLUP,
            ACTIVE_LOW_PULLUP    = ACTIVE_LOW  | WITH_PULLUP, // best for Arduino
            ACTIVE_HIGH_PULLDOWN = ACTIVE_HIGH | WITH_PULLDOWN
        };
        
        // inline friend optionsEnum operator|(optionsEnum lhs, optionsEnum rhs) {
        //     return static_cast<optionsEnum>(
        //         static_cast<std::underlying_type<optionsEnum>::type>(lhs) |
        //         static_cast<std::underlying_type<optionsEnum>::type>(rhs)
        //     );
        // }
        // inline friend optionsEnum operator&(optionsEnum lhs, optionsEnum rhs) {
        //     return static_cast<optionsEnum>(
        //         static_cast<std::underlying_type<optionsEnum>::type>(lhs) &
        //         static_cast<std::underlying_type<optionsEnum>::type>(rhs)
        //     );
        // }
        inline friend optionsEnum operator | ( optionsEnum a, optionsEnum b ) {
            return static_cast<optionsEnum>( static_cast<uint8_t>(a) | static_cast<uint8_t>(b) );
        }
        inline friend optionsEnum operator & ( optionsEnum a, optionsEnum b ) {
            return static_cast<optionsEnum>( static_cast<uint8_t>(a) & static_cast<uint8_t>(b) );
        }
        inline friend optionsEnum& operator |= ( optionsEnum& a, optionsEnum b ) {
            a = a | b; return a;
        }

        DigitalInput( const uint8_t pin, optionsEnum options = optionsEnum::ACTIVE_LOW_PULLUP ) {
            PIN = pin;
            if ( ( options & optionsEnum::ACTIVE_LOW ) == optionsEnum::ACTIVE_LOW )
                INACTIVE_STATE = HIGH;
            else
                INACTIVE_STATE = LOW;
            if ( ( options & optionsEnum::WITH_PULLUP ) == optionsEnum::WITH_PULLUP )
                pinMode( PIN, INPUT_PULLUP );
            else if ( ( options & optionsEnum::WITH_PULLDOWN ) == optionsEnum::WITH_PULLDOWN )
                #if defined( ESP32 )
                    pinMode( PIN, INPUT_PULLDOWN );
                // #elif defined( ESP8266 )
                //     // only pin16 can have pulldown
                //     // ... just ignore
                //     if (pin == 16)
                //         pinMode( PIN, INPUT_PULLDOWN_16 );
                //     else
                //         pinMode( PIN, INPUT );
                #else
                    // #error DIGITAL INPUT PULLDOWN NOT SUPPORTED
                    pinMode( PIN, INPUT );
                #endif
            else
                pinMode( PIN, INPUT );
        }

    //
    // CALLBACK
    //

        // typedef int  (*intIntFunction) (int); // int function( int )
        // typedef void (*voidIntFunction)(int); // void function( int )
        // voidIntFunction onChange = nullptr;

        // inline void setOnChangeCallback( voidIntFunction onChange ) {
        //     this->onChange = onChange;
        // }
    // private:
        // bool lastValue = -1;

        // inline void handleOnChanged() {
        //     if ( value != lastValue ) {
        //         lastValue = value;
        //         if ( onChange != nullptr )
        //             onChange( value );
        //     }
        // }

    //
    // READ
    //
    public:

        inline uint8_t readRaw() {
            return digitalRead( PIN );
        }

        inline bool readLogicalRaw() {
            return ( digitalRead( PIN ) != INACTIVE_STATE );
        }

        virtual bool read() {
            // to be overridden with extra functionality
            // eg. debounce, repeat, etc.
            return ( readRaw() != INACTIVE_STATE );
        }

        inline bool isOn() {
            return read();
        }

        inline bool isOff() {
            return !read();
        }

};

}

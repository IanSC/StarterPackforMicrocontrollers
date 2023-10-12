//  2023-07 isc
//
//  Digital Input
//  - used for digital input
//  - handles initialization, pullup, pulldown
//  
//      DigitalInputRaw( pin, pullupOptions = Pull::None )
//
//          pullupOptions = None   no pullup
//                          Up     with pullup to +VCC
//                          Down   with pulldown to GND (ESP32 only)
//
//      uint8_t readRaw()       same as digitalRead()
//      bool    isOnRaw()       true if active
//      bool    isOffRaw()      true if not active

#pragma once

#include <Arduino.h>
#include <stdint.h>

#include <Utility/spMacros.h>

namespace StarterPack {

class DigitalInputRaw {

    protected:

        uint8_t PIN;                    // pin number to read
        // bool INACTIVE_STATE = false;    // read result to be considered not "active"

    public:
    
        DigitalInputRaw() {}

        // virtual ~DigitalInputRaw() {}

        // do not use class ???
        // enum class DigitalInputRaw::optionsEnum::WITH_PULLUP
        // enum       DigitalInputRaw::WITH_PULLUP
        enum class Pull : uint8_t {
            // ACTIVE_HIGH   = 0,
            // ACTIVE_LOW    = 1,
            None = 0,
            Up   = 1,
            Down = 2
            // STANDARD             = ACTIVE_HIGH | NO_PULLUP,
            // ACTIVE_LOW_PULLUP    = ACTIVE_LOW  | WITH_PULLUP, // best for Arduino
            // ACTIVE_HIGH_PULLDOWN = ACTIVE_HIGH | WITH_PULLDOWN
        };
        CLASS_ENUM_MANIPULATION(Pull);

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
        // inline friend Pull operator | ( Pull a, Pull b ) {
        //     return static_cast<Pull>( static_cast<uint8_t>(a) | static_cast<uint8_t>(b) );
        // }
        // inline friend Pull operator & ( Pull a, Pull b ) {
        //     return static_cast<Pull>( static_cast<uint8_t>(a) & static_cast<uint8_t>(b) );
        // }
        // inline friend Pull& operator |= ( Pull& a, Pull b ) {
        //     a = a | b; return a;
        // }

        DigitalInputRaw( const uint8_t pin, Pull pullupOptions = Pull::None ) {
            PIN = pin;
            if ( PIN == -1 ) return;
            // if ( ( options & optionsEnum::ACTIVE_LOW ) == optionsEnum::ACTIVE_LOW )
            //     INACTIVE_STATE = HIGH;
            // else
            //     INACTIVE_STATE = LOW;
            if ( ( pullupOptions & Pull::Up ) == Pull::Up )
                pinMode( PIN, INPUT_PULLUP );
            else if ( ( pullupOptions & Pull::Down ) == Pull::Down )
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

        inline int readRaw() {
            if ( PIN == -1 ) return LOW;
            auto r = digitalRead( PIN );
// Serial.print( "P" ); Serial.print( PIN ); Serial.print( " = " ); Serial.println( r );
            return r;
        }

        inline bool isOnRaw() {
            if ( PIN == -1 ) return false;
            return ( readRaw() == HIGH );
        }

        inline bool isOffRaw() {
            if ( PIN == -1 ) return true;
            return ( readRaw() == LOW );
        }

        // inline bool readLogicalRaw() {
        //     return ( readRaw() != INACTIVE_STATE );
        // }

        // virtual bool read() {
        //     // to be overridden with extra functionality
        //     // eg. debounce, repeat, etc.
        //     return ( readRaw() != INACTIVE_STATE );
        // }

        // inline bool isOn() {
        //     return read();
        // }

        // inline bool isOff() {
        //     return !read();
        // }

};

}

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
#include <stdint.h>

#include <DigitalIO/DigitalInputRaw.h>

// #include <Utility/spMacros.h>
#include <Utility/spEnum.h>

namespace StarterPack {

// https://stackoverflow.com/questions/8357240/how-to-automatically-convert-strongly-typed-enum-into-int
// template <class E>
// constexpr std::common_type_t<int, std::underlying_type_t<E>>
// enumToInteger(E e) {
//     return static_cast<std::common_type_t<int, std::underlying_type_t<E>>>(e);
// }

class DigitalInput : public DigitalInputRaw {

    protected:

        // // digitalRead() to be considered not "active"
        // bool MY_INACTIVE_STATE = LOW;

        // digitalRead() to be considered "active"
        bool ACTIVE_STATE = LOW;

    public:
    
        enum Init : uint8_t {
            PullNone   = static_cast<int>(DigitalInputRaw::Pull::None),
            PullUp     = static_cast<int>(DigitalInputRaw::Pull::Up),
            PullDown   = static_cast<int>(DigitalInputRaw::Pull::Down),
            ActiveHigh = 0,
            ActiveLow  = 4,
            Default            = ActiveHigh | PullNone,
            ActiveLowPullUp    = ActiveLow  | PullUp, // best for Arduino
            ActiveHighPullDown = ActiveHigh | PullDown
        };
        CLASS_ENUM_MANIPULATION( Init )

/*
        // redefine here, otherwise:
        //    DigitalInput( pin, DigitalInput::Active::High, DigitalInputRaw::Pull::Up )
        // instead of:
        //    DigitalInput( pin, DigitalInput::Active::High, DigitalInput   ::Pull::Up )
        enum class Pull : uint8_t {
            None = static_cast<int>(DigitalInputRaw::Pull::None),
            Up   = static_cast<int>(DigitalInputRaw::Pull::Up),
            Down = static_cast<int>(DigitalInputRaw::Pull::Down)
        };        

        enum class Active : uint8_t {
            High = 0,
            Low  = 1
        };

        CLASS_ENUM_MANIPULATION(Active);
        CLASS_ENUM_MANIPULATION(Init);
*/

        DigitalInput() {}

        DigitalInput( const uint8_t pin, Init options = Init::Default )
        : DigitalInputRaw( pin, static_cast<DigitalInputRaw::Pull>(options) ) {
            if ( ( options & Init::ActiveLow ) == Init::ActiveLow )
                // MY_INACTIVE_STATE = HIGH;
                ACTIVE_STATE = LOW;
            else
                // MY_INACTIVE_STATE = LOW;
                ACTIVE_STATE = HIGH;
        }

/*
        DigitalInput( const uint8_t pin, Active activeLogic, DigitalInput::Pull pullupOptions = DigitalInput::Pull::None )
        : DigitalInputRaw( pin, static_cast<DigitalInputRaw::Pull>(pullupOptions) ) {
            if ( ( activeLogic & Active::Low ) == Active::Low )
                INACTIVE_STATE = HIGH;
            else
                INACTIVE_STATE = LOW;
        }
*/

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
    // READ WITH LOGIC
    //
    public:

        inline bool readLogicalRaw() {
            return ( readRaw() == ACTIVE_STATE );
            // return ( readRaw() != MY_INACTIVE_STATE );
        }

        // virtual 
        bool read() {
            // to be overridden with extra functionality
            // eg. debounce, repeat, etc.
            // return ( readRaw() != MY_INACTIVE_STATE );
            return ( readRaw() == ACTIVE_STATE );
        }

        inline bool isOn() {
            return read();
        }

        inline bool isOff() {
            return !read();
        }

};

}

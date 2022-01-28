//  2022-01 isc
//
//  Digital IO
//  - used for digital input
//  - handles initialization, pullup, pulldown
//  - debouncing of state
//  - repeating keys
//  
//      DigitalIO( pin, options = ACTIVE_HIGH, activeDebounceTime = 50, uint16_t inactiveDebounceTime = 50 )
//
//          options = NONE            just set as input
//                    ACTIVE_HIGH     HIGH is considered pressed
//                    ACTIVE_LOW      LOW is considered pressed
//                    NO_PULLUP       no pullup
//                    WITH_PULLUP     with pullup to +VCC
//                    WITH_PULLDOWN   with pulldown to GND (ESP32 only)
//          activeDebounceTime        debounce time when key is pressed
//          inactiveDebounceTime      debounce time when key is released
//          
//      bool value                            last debounced value
//      int  readStatus()                     read button, debounce and keep, use "value" to read it
//      bool getStatus()                      read button, debounce then return (also stored in "value")
//
//      setActiveDebounceTimeInMs( time )     set debounce time for active state, default 50
//      setInactiveDebounceTimeInMs( time )   set debounce time for inactive state, default 50
//      setMinimumDebounceTimeInMs( time )    set minimum time of debouncing before allowed to be cancelled, default 50
//      setConfirmStateTimeInMs( time )       confirm state change is consistent for specified time
//
//      flagWaitForKeyup()                    suppress key presses, until key is released then pressed again
//      cancelDebouncing()                    stop debouncing current state
//                                            eg. signal already processed, stop debouncing if 
//
//      bool getRepeating()                   get intermitent true's while a key is held down
//      setRepeatDelayInMs( delay )           time before sending 1st repeated key
//      setRepeatRateInMs( rate )             time before sending subsequent keys
//
//      setOnChangeCallback( &handler )       specify callback if value has changed
//                                            applied during readStatus()/getStatus()
//                                            signature: void onChange( int )
//
//      Debouncer debouncer                   access to internal debouncer, see <Debouncer.h>

#pragma once
#include <stdint.h>
#include <Arduino.h>
#include <Debouncer.h>

namespace StarterPack {

class DigitalIO {

    typedef int  (*intIntFunction) (int); // int function( int )
    typedef void (*voidIntFunction)(int); // void function( int )

    private:

        uint8_t PIN;          // pin number to read
        uint8_t ACTIVE_STATE; // pin read result to be considered "active"

    public:
    
        enum optionsEnum {
            NONE          = 0,
            ACTIVE_HIGH   = 0,
            ACTIVE_LOW    = 1,
            NO_PULLUP     = 0,
            WITH_PULLUP   = 2,
            WITH_PULLDOWN = 4
        };
        
        DigitalIO( const uint8_t pin, int options = ACTIVE_HIGH, // WITH_PULLUP | ACTIVE_LOW,
        uint16_t activeDebounceTime = 50, uint16_t inactiveDebounceTime = 50 ) {
            PIN = pin;
            if ( ( options & optionsEnum::ACTIVE_LOW ) == optionsEnum::ACTIVE_LOW )
                ACTIVE_STATE = LOW;
            else
                ACTIVE_STATE = HIGH;
            if ( ( options & optionsEnum::WITH_PULLUP ) == optionsEnum::WITH_PULLUP )
                pinMode( PIN, INPUT_PULLUP );
            else if ( ( options & optionsEnum::WITH_PULLDOWN ) == optionsEnum::WITH_PULLDOWN )
                #if defined( ESP32 )
                    pinMode( PIN, INPUT_PULLDOWN );
                #else
                    pinMode( PIN, INPUT );
                #endif
            else
                pinMode( PIN, INPUT );
            debouncer.activeStatesDebounceInMs = activeDebounceTime;
            debouncer.inactiveStateDebounceInMs = inactiveDebounceTime;
            // initial values
            value = ( digitalRead( PIN ) == ACTIVE_STATE );
            lastValue = value;
            debouncer.setInitialValue( value );
        }

        inline void setOnChangeCallback( voidIntFunction onChange ) {
            this->onChange = onChange;
        }

    //
    // READ
    //
    public:

        inline void readStatus()  {
            readPinCore();
            handleOnChanged();
            // actualState = ( digitalRead( PIN ) == ACTIVE_STATE );
            // value = debouncer.debounce( actualState );
            // if ( value != lastValue && onChange != nullptr ) {
            //     lastValue = value;
            //     onChange( value );
            // }
        }

        inline bool getStatus() {
            readPinCore();
            handleOnChanged();
            // readStatus();
            return value;
        }

        // bool getRepeating() {
        //     // if user keeps button held down send signal intermitently
        //     // eg. selecting an option, by sending right key every few seconds
        //     //     as opposed to keep going right as in gamepad
        //     readStatus();
        //     return debouncer.getRepeating( value );
        // }

//
// BUTTONS
//

    private:

        voidIntFunction onChange = nullptr;
        bool lastValue = -1;

        inline void readPinCore()  {
            actualState = ( digitalRead( PIN ) == ACTIVE_STATE );
            value = debouncer.debounce( actualState );
        }

        inline void handleOnChanged() {
            if ( value != lastValue ) {
                lastValue = value;
                if ( onChange != NULL )
                    onChange( value );
            }
        }

    public:

        bool actualState;
        bool value;

        int getKeyDown() {
            readPinCore();
            value = debouncer.getKeyDown( value );
            handleOnChanged();
            // if ( value != lastValue ) {
            //     lastValue = value;
            //     if ( onChange != NULL )
            //         onChange( value );
            // }
            return value;
        }

        int getKeyUp() {
            readPinCore();
            value = debouncer.getKeyUp( value );
            handleOnChanged();
            // if ( value != lastValue ) {
            //     lastValue = value;
            //     if ( onChange != NULL )
            //         onChange( value );
            // }
            return value;
        }

        int getContinuous() {
            readPinCore();
            value = debouncer.getContinuous( value );
            handleOnChanged();
            // if ( value != lastValue ) {
            //     lastValue = value;
            //     if ( onChange != NULL )
            //         onChange( value );
            // }
            return value;
        }

        int getRepeating() {
            // if user keeps button held down send signal intermitently
            // eg. selecting an option, by sending right key every few seconds
            //     as opposed to keep going right as in gamepad
            readPinCore();
            value = debouncer.getRepeating( value );
            handleOnChanged();
            // if ( value != lastValue ) {
            //     lastValue = value;
            //     if ( onChange != NULL )
            //         onChange( value );
            // }
            return value;
        }



        // direct access to debouncer being used
        Debouncer debouncer;

        inline void setActiveDebounceTimeInMs  ( uint16_t time ) { debouncer.activeStatesDebounceInMs  = time; }
        inline void setInactiveDebounceTimeInMs( uint16_t time ) { debouncer.inactiveStateDebounceInMs = time; }
        inline void setMinimumDebounceInMs     ( uint16_t time ) { debouncer.minimumDebounceTimeInMs   = time; }
        inline void setConfirmStateTimeInMs    ( uint16_t time ) { debouncer.confirmStateTimeInMs      = time; }
        inline void flagWaitForKeyup()                   { debouncer.flagWaitForKeyup();      }
        inline void cancelDebouncing()                   { debouncer.cancelDebouncing();      }
        inline void setRepeatDelayInMs( uint16_t delay ) { debouncer.repeatDelayInMs = delay; }
        inline void setRepeatRateInMs( uint16_t rate )   { debouncer.repeatRateInMs = rate;   }

};

}

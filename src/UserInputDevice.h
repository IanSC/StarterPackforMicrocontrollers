#pragma once
#include <stdint.h>
#include <Debouncer.h>

namespace StarterPack {

class UserInputDevice {

    public:

        virtual ~UserInputDevice() {}

    //
    // KEYS
    //
    public:

        virtual char *getContinuousKeys( char *allowedKeys = nullptr ) = 0;

        virtual uint8_t getContinuousKey( char *allowedKeys = nullptr ) = 0;
        virtual uint8_t getKeyDown( char *allowedKeys = nullptr ) = 0;
        virtual uint8_t getKeyUp( char *allowedKeys = nullptr ) = 0;
        virtual uint8_t getRepeatingKey( char *allowedKeys = nullptr ) = 0;
        virtual uint8_t getRepeatingKeyExcept( uint8_t nonRepeatingKey, char *allowedKeys = nullptr ) = 0;
        virtual uint8_t getRepeatingKeyExcept( char *nonRepeatingKeys, char *allowedKeys = nullptr ) = 0;

    //
    // CORE
    //

        // // not debounced
        // virtual uint8_t readRaw() = 0;

        // return mapped key number, eg. '1', '2', '3'
        // return 0 if multiple keys pressed
        virtual uint8_t readMappedKey() = 0;

        // return string containing all mapped key values, eg. "123ABC"
        virtual char *readMappedKeyList() = 0;

    //
    // KEYMAP
    //
    public:

        inline void assignKeymap( const char *keyMap ) {
            this->keyMap = keyMap;
        }

    protected:

        const char *keyMap = nullptr;

        uint8_t getKeymap( const uint8_t scanCode ) {
            // 1-based
            // eg. scanCode=1, will give keyMap[0]
            //    scanCode     : 123456789...
            //    keyMap index : 012345678...
            if ( scanCode == 0 ) return 0;
            if ( keyMap == nullptr )
                return scanCode;
            if ( strlen( keyMap ) < scanCode )
                return scanCode;
            return keyMap[ scanCode-1 ];
        }

    //
    // BASIC
    //
    public:

        bool isKeyPressed( uint8_t key, bool issueWaitForKeyUp = true ) {
            if ( getContinuousKey() == key ) {
                if ( issueWaitForKeyUp )
                    flagWaitForKeyupSpecific( key );
                return true;
            }
            return false;
        }

        void waitUntilNothingIsPressed() {
            while( true ) {
//Serial.print( "." );
                if ( strlen( readMappedKeyList() ) == 0 ) {
//Serial.println( "nothing pressed" );
                    return;
                }
//                 if ( getContinuousKey() == 0 ) {
// Serial.println( "nothing pressed" );
//                     return;
//                 }
            }
        }

        uint8_t waitForAnyKeyPressed() {
            // wait for key, don't check if released already
            waitUntilNothingIsPressed();
            while ( true ) {
                uint8_t key = getKeyDown();
                if ( key != 0 ) return key;
            }
        }

        inline uint8_t waitForAnyKey() {
            // wait for key, then wait for its release
            waitUntilNothingIsPressed();
            while ( true ) {
                uint8_t key = getKeyUp();
                if ( key != 0 ) return key;
            }            
            // return getKeyUp();
            // auto key = waitForAnyKeyOnly();
            // waitUntilNothingIsPressed();
            // return key;
        }

        // uint8_t waitForAnyKey() {
        //     waitUntilNothingIsPressed();
        //     while ( true ) {
        //         uint8_t key = getKeyDown();
        //         if ( key != 0 ) return key;
        //     }
        // }

        // uint8_t waitForAnyKeyAndRelease() {
        //     auto key = waitForAnyKey();
        //     waitUntilNothingIsPressed();
        //     return key;
        // }

    //
    // DEBOUNCER
    //
    public:

        Debouncer debouncer;

        void setDebounceTimeInMs( uint16_t activeState = 50, uint16_t inactiveState = 50, uint16_t minimum = 50 ) {
            debouncer.useCustomSettings();
            Debouncer::Settings *s = debouncer.getSettings();
            s->activeStatesDebounceInMs = activeState;
            s->inactiveStateDebounceInMs = inactiveState;
            s->minimumDebounceTimeInMs = minimum;
        }

        void setConfirmStateTimeInMs( uint16_t confirmActiveStateTimeInMs = 0, uint16_t confirmInactiveStateTimeInMs = 0 ) {
            debouncer.useCustomSettings();
            Debouncer::Settings *s = debouncer.getSettings();
            s->confirmActiveStateTimeInMs = confirmActiveStateTimeInMs;
            s->confirmInactiveStateTimeInMs = confirmInactiveStateTimeInMs;
        }

        void setRepeatDelayAndRateInMs( uint16_t repeatDelay = 400, uint16_t repeatRate = 250 ) {
            debouncer.useCustomSettings();
            Debouncer::Settings *s = debouncer.getSettings();
            s->repeatDelayInMs = repeatDelay;
            s->repeatRateInMs = repeatRate;
        }

        void flagWaitForKeyup() { debouncer.flagWaitForKeyup(); }
        void cancelDebouncing() { debouncer.cancelDebouncing(); }

        virtual void flagWaitForKeyupSpecific( uint8_t key ) = 0;

};

}

#pragma once
#include <stdint.h>
#include <Debouncer.h>

namespace StarterPack {

class UserInputDeviceBase {

    public:

        virtual ~UserInputDeviceBase() {}

    //
    // KEYS
    //
    public:

        virtual char *getContinuousKeys( char *allowedKeys = nullptr ) = 0;

        virtual uint8_t getContinuousKey( char *allowedKeys = nullptr )  = 0;
        virtual uint8_t getKeyDown( char *allowedKeys = nullptr ) = 0;
        virtual uint8_t getRepeatingKey( char *allowedKeys = nullptr ) = 0;
        virtual uint8_t getRepeatingKeyExcept( uint8_t nonRepeatingKey, char *allowedKeys = nullptr ) = 0;
        virtual uint8_t getRepeatingKeyExcept( char *nonRepeatingKeys, char *allowedKeys = nullptr ) = 0;

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

        bool isKeyPressed( char key, bool issueWaitForKeyUp = true ) {
            if ( getContinuousKey() == key ) {
                if ( issueWaitForKeyUp )
                    flagWaitForKeyupSpecific( key );
                return true;
            }
            return false;
        }

        void waitUntilNothingIsPressed() {
            while( true ) {
                if ( getContinuousKey() == 0 ) return;
            }
        }

        void waitForAnyKey() {
            waitUntilNothingIsPressed();
            while ( true ) {
                if ( getKeyDown() != 0 ) return;
            }
        }

    //
    // DEBOUNCER
    //
    public:

        Debouncer debouncer;

        void setDebounceTimeInMs( uint16_t activeState = 50, uint16_t inactiveState = 50, uint16_t minimum = 50 ) {
            Debouncer::Settings *s = debouncer.getSettings();
            s->activeStatesDebounceInMs = activeState;
            s->inactiveStateDebounceInMs = inactiveState;
            s->minimumDebounceTimeInMs = minimum;
        }

        void flagWaitForKeyup() { debouncer.flagWaitForKeyup(); }
        void cancelDebouncing() { debouncer.cancelDebouncing(); }

        virtual void flagWaitForKeyupSpecific( uint8_t key ) = 0;

    protected:

        // virtual void flagWaitForKeyup( char *keysPressed ) = 0;

};

}

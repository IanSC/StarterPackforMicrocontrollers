#pragma once
#include <UserInputDevice.h>
#include <spUtility.h>

namespace StarterPack {

class UserInputDevice1Key : public UserInputDevice {

    public:

        ~UserInputDevice1Key() {} // need this ???

    //
    // KEYS FASTER VERSION - no combining of keys
    //
    protected:

        uint8_t allenKey( char *allowedKeys = nullptr ) {
            uint8_t key = readMappedKey(); // getKeymap( readRawKeyNumber() );
            if ( key == 0 ) return 0;
            if ( allowedKeys != nullptr && !isCharInString( key, allowedKeys ) )
                return 0;
            return key;
        }

    public:

        // return unmapped key number
        virtual uint8_t readMappedKey() = 0;

        // return string containing all mapped key values
        char *readMappedKeyList() {
            static char buffer[2] = " ";
            buffer[0] = readMappedKey();
            return buffer;
        }

        char *getContinuousKeys( char *allowedKeys = nullptr ) {
            char *keys = readMappedKeyList();
            if ( allowedKeys != nullptr && !isCharInString( keys[0], allowedKeys ) )
                keys[0] = 0;
            return keys;
        }

        uint8_t getContinuousKey( char *allowedKeys = nullptr ) {
            return debouncer.getContinuousKey( allenKey( allowedKeys ) );
        }

        uint8_t getKeyDown( char *allowedKeys = nullptr ) {
            return debouncer.getKeyDown( allenKey( allowedKeys ) );
        }

        uint8_t getKeyUp( char *allowedKeys = nullptr ) {
            return debouncer.getKeyUp( allenKey( allowedKeys ) );
        }

        uint8_t getRepeatingKey( char *allowedKeys = nullptr ) {
            return debouncer.getKeyUp( allenKey( allowedKeys ) );
        }

        uint8_t getRepeatingKeyExcept( uint8_t nonRepeatingKey, char *allowedKeys = nullptr ) {
            uint8_t key = debouncer.getRepeatingKey( allenKey( allowedKeys ) );
            if ( key == nonRepeatingKey ) debouncer.flagWaitForKeyup();
            return key;
        }

        uint8_t getRepeatingKeyExcept( char *nonRepeatingKeys, char *allowedKeys = nullptr ) {
            uint8_t key = debouncer.getRepeatingKey( allenKey( allowedKeys ) );
            if ( nonRepeatingKeys != nullptr && isCharInString( key, nonRepeatingKeys ) )
                debouncer.flagWaitForKeyup();
            return key;
        }

    //
    // DEBOUNCER
    //
    // public:

    //     virtual void flagWaitForKeyupMulti( char *keys ) = 0;

};

}

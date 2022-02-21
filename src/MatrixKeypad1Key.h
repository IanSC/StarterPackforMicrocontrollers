#pragma once
#include <stdint.h>
#include <Arduino.h>

#include <MatrixKeypadCore.h>
#include <UserInputDevice1Key.h>

namespace StarterPack {

class MatrixKeypad1Key : public MatrixKeypadCore, public UserInputDevice1Key {

    //
    // INIT
    //
    // public:

    //     MatrixKeypad() {}
    //     ~MatrixKeypad() {}
/*
    //
    // FASTER VERSION - no combining of keys
    //
    protected:

        uint8_t allenKey( char *allowedKeys = nullptr ) {
            uint8_t key = getKeymap( readRaw() ) ;
            if ( allowedKeys != nullptr && !isCharInString( key, allowedKeys ) )
                return 0;
            return key;
        }

    public:

        char *getContinuousKeys( char *allowedKeys = nullptr ) {
            static char buffer[2] = " ";
            buffer[0] = getContinuousKey();
            return buffer;
        }

        uint8_t getContinuousKey( char *allowedKeys = nullptr ) {
            return debouncer.getContinuousKey( allenKey( allowedKeys ) );
        }

        uint8_t getKeyDown( char *allowedKeys = nullptr ) {
            return debouncer.getKeyDown( allenKey( allowedKeys ) );
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
*/
    //
    // DEBOUNCER
    //
    public:

        void flagWaitForKeyupSpecific( uint8_t key ) { debouncer.flagWaitForKeyup(); }

    //
    // READ DEVICE
    //
    public:

        uint8_t readMappedKey() {
            readScanCode = 0;
            multiplePress = false;
            if ( sendPinCount == 0 || recvPinCount == 0 )
                return 0;
            readMatrixCore( 0, sendPinCount - 1 );
            if ( multiplePress )
                return 0;
            else
                return getKeymap( readScanCode );
        }
        
    protected:

        uint8_t readScanCode = 0;
        bool multiplePress = false;

        void recordScanCode( uint8_t scanCode ) {
            if ( readScanCode == 0 )
                readScanCode = scanCode + 1;
            else
                multiplePress = true;
        }

};

}

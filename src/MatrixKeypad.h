#pragma once
#include <stdint.h>
#include <Arduino.h>

#include <MatrixKeypadCore.h>
#include <UserInputDeviceMulti.h>

namespace StarterPack {

class MatrixKeypad : public MatrixKeypadCore, public UserInputDeviceMulti {

    //
    // INIT
    //
    // public:

    //     MatrixKeypadMulti() {}
    //     ~MatrixKeypadMulti() {}

    //
    // DEBOUNCER
    //     
    public:

        void flagWaitForKeyupSpecific( uint8_t key ) { debouncer.flagWaitForKeyup(); }
        void flagWaitForKeyupMulti( char *keysPressed ) { debouncer.flagWaitForKeyup(); }

    //
    // READ DEVICE
    //
    public:

        char *readMappedKeyList() {
            // record each key pressed into buffer
            keysPressed[0] = 0;
            if ( sendPinCount == 0 || recvPinCount == 0 )
                return keysPressed;
            readMatrixCore( 0, sendPinCount - 1 );
            return keysPressed;
        }
        
    protected:

        static const uint8_t MAX_KEYS = 5;
        char keysPressed[MAX_KEYS+1];

        void recordScanCode( uint8_t scanCode ) {
            uint8_t l = strlen( keysPressed );
            if ( l >= MAX_KEYS ) return;
            // uint8_t getKeymap( const uint8_t scanCode )
            keysPressed[l] = getKeymap( scanCode+1 );
            keysPressed[l+1] = 0;
        }

};

//
// TEST
//

    void TEST_matrixKeypad( uint8_t row, uint8_t col1, uint8_t col2, uint8_t col3, uint8_t col4 ) {
        // depending on device
        // some cannot detect multiple keypress
        // mine only gets 2
        pinMode( row, OUTPUT );
        digitalWrite( row, LOW );
        pinMode( col1, INPUT_PULLUP );
        pinMode( col2, INPUT_PULLUP );
        pinMode( col3, INPUT_PULLUP );
        pinMode( col4, INPUT_PULLUP );
        while( true ) {
            Serial.print( digitalRead( col1 ) );
            Serial.print( digitalRead( col2 ) );
            Serial.print( digitalRead( col3 ) );
            Serial.print( digitalRead( col4 ) );
            Serial.println();
            delay( 50 );
        }
    }

}

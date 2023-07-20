#pragma once

#include <MatrixKeypad/MatrixKeypadCore.h>
#include <InputHelper/InputMapper.h>
#include <InputHelper/InputCombiner.h>
#include <UserInterface/UserInterfaceKeyUpDown.h>

namespace StarterPack {

class newMatrixKeypad : public newMatrixKeypadCore,
                        public InputMapper, public InputCombiner,
                        public UserInterfaceKeyUpDown {

    // possible to use each functionality by composition instead of inheritance
    // but will need extra pointer
    // eg. create object of InputMapper, InputCombiner, ...
    //     will need extra pointer for each

    protected:

        typedef UserInterfaceKeyUpDown::KEY KEY;
        
    //
    // READ DEVICE
    //
    protected:

        void recordScanCode( uint8_t scanCode ) override {
            // scanCode = 0-based
            // newMatrixKeypadCore will call this with raw key numbers
            // map those number to logical values using InputMapper
            // save result into keysPressed
            uint8_t l = strlen( keysPressed );
            if ( l >= newMatrixKeypadCore::MAX_SIMULTANEOUS_KEYS ) return;
            keysPressed[l] = InputMapper::actionMapKey( scanCode+1 );
            keysPressed[l+1] = 0;
        }

    //
    // VIRTUAL OVERRIDES
    //
    public:

        KEY getNonDebouncedKey() override {

            // keysPressed[0] = 0;

            // result in keysPressed
            // ex. "U"   up only
            //     "L"   left only
            //     "UL"  up and left pressed
            auto keysPressed = newMatrixKeypadCore::readMatrix();

            // if (strlen(keysPressed)>0)
            //     Serial.println(keysPressed);

            // combine keys into single key
            // ex. "UL" --> '1'
            auto key = InputCombiner::actionCombineKeys(keysPressed);

            // debounce
            // key = InputDebouncerBase::debounce(key);

            return key;
        }

};

//
// TEST
//

    void TEST_matrixKeypad( uint8_t row, uint8_t col1, uint8_t col2=0, uint8_t col3=0, uint8_t col4=0 ) {
        // depending on device
        // some cannot detect multiple keypress
        // mine only gets 2
        pinMode( row, OUTPUT );
        digitalWrite( row, LOW );
        pinMode( col1, INPUT_PULLUP );
        if (col2!=0) pinMode( col2, INPUT_PULLUP );
        if (col3!=0) pinMode( col3, INPUT_PULLUP );
        if (col4!=0) pinMode( col4, INPUT_PULLUP );
        while( true ) {
            Serial.print( digitalRead( col1 ) );
            if (col2!=0) Serial.print( digitalRead( col2 ) );
            if (col3!=0) Serial.print( digitalRead( col3 ) );
            if (col4!=0) Serial.print( digitalRead( col4 ) );
            Serial.println();
            delay( 50 );
        }
    }

}
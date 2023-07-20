#pragma once

#include <MatrixKeypad/MatrixKeypadCore.h>
#include <InputHelper/InputMapper.h>
#include <InputHelper/InputCombiner.h>
#include <UserInterface/UserInterfaceRepeated.h>

namespace StarterPack {

class newMatrixKeypadRepeated : public newMatrixKeypadCore,
                                public InputMapper, public InputCombiner,
                                public UserInterfaceRepeated {

    // possible to use each functionality by composition instead of inheritance
    // but will need extra pointer
    // eg. create object of InputRepeater, ...
    //     will need extra pointer for each

    protected:

        typedef UserInterfaceRepeated::KEY KEY;
        static constexpr KEY INACTIVE_KEY = UserInterfaceRepeated::INACTIVE_KEY;

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

}

#pragma once
// #include <stdint.h>
// #include <Debouncer.h>
#include <UserInputDeviceBase.h>

namespace StarterPack {

class UserInputDevice1Key : UserInputDeviceBase {

    public:

        ~UserInputDevice1Key() {}

    //
    // KEYMAP
    //
    protected:

        uint8_t getKeymap( const uint8_t scanCode ) {
            if ( keyMap == nullptr )
                return scanCode + 1;
            if ( strlen( keyMap ) <= scanCode )
                return scanCode + 1;
            return keyMap[ scanCode ];
        }

};

}

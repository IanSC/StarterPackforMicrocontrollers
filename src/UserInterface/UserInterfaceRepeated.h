#pragma once

#include <inttypes.h>

#include <UserInterface/UserInterfaceKeyUpDown.h>
#include <InputHelper/InputRepeater.h>

namespace StarterPack {

class UserInterfaceRepeated : public UserInterfaceKeyUpDown,
                              public InputRepeater {

    public:

        typedef UserInterfaceKeyUpDown::KEY KEY;
        static constexpr KEY INACTIVE_KEY = UserInterfaceKeyUpDown::INACTIVE_KEY;

    //
    // VIRTUAL
    //
    public:

        KEY getRepeatingKey() {
            auto key = UserInterfaceKeyUpDown::getDebouncedKey();
            return InputRepeater::actionGetRepeatingKey( key );
        }

};

}
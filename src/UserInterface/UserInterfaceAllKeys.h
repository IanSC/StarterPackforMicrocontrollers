#pragma once

#include <stdint.h>

#include <UserInterface/UserInterfaceMultiClick.h>
#include <InputHelper/InputRepeater.h>

namespace StarterPack {

class UserInterfaceAllKeys : public UserInterfaceMultiClick,
                             public InputRepeater<UserInterfaceMultiClick::KEY> {

    public:

        typedef UserInterfaceMultiClick::KEY KEY;
        static constexpr KEY INACTIVE_KEY = UserInterfaceMultiClick::INACTIVE_KEY;

    public:

        KEY getRepeatingKey() {
            auto key = UserInterfaceMultiClick::getDebouncedKey();
            return InputRepeater::actionGetRepeatingKey( key );
        }

};

}
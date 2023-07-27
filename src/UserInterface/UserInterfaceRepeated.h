#pragma once

#include <inttypes.h>

#include <UserInterface/UserInterfaceDebounced.h>
#include <InputHelper/InputRepeater.h>

namespace StarterPack {

class UserInterfaceRepeated : public UserInterfaceDebounced,
                              public InputRepeater<UserInterfaceDebounced::KEY> {

    public:

        typedef UserInterfaceDebounced::KEY KEY;
        static constexpr KEY INACTIVE_KEY = UserInterfaceDebounced::INACTIVE_KEY;

    public:

        KEY getRepeatingKey() {
            auto key = UserInterfaceDebounced::getDebouncedKey();
            return InputRepeater::actionGetRepeatingKey( key );
        }

};

}
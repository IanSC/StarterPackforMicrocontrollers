#pragma once

#include <stdint.h>

#include <UserInterface/UserInterfaceDebounced.h>
#include <InputHelper/InputRepeater.h>

namespace StarterPack {

class UserInterfaceRepeated : virtual public InputKeySource,
                              public InputRepeater<UserInterfaceBasic::KEY> {

    // public:

    //     typedef UserInterfaceDebounced::KEY KEY;
    //     static constexpr KEY INACTIVE_KEY = UserInterfaceDebounced::INACTIVE_KEY;

    public:

        // virtual InputKeySource::KEY getStableKey() = 0;

        InputKeySource::KEY getRepeatingKey() {
            auto key = getStableKey();
            return InputRepeater::actionGetRepeatingKey( key );
        }

};

// class UserInterfaceDebouncedRepeated : public UserInterfaceDebounced,
//                               public InputRepeater<UserInterfaceDebounced::KEY> {

//     public:

//         typedef UserInterfaceDebounced::KEY KEY;
//         static constexpr KEY INACTIVE_KEY = UserInterfaceDebounced::INACTIVE_KEY;

//     public:

//         KEY getRepeatingKey() {
//             auto key = UserInterfaceDebounced::getDebouncedKey();
//             return InputRepeater::actionGetRepeatingKey( key );
//         }

// };

}
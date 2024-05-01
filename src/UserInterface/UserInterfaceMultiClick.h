#pragma once

#include <stdint.h>

#include <UserInterface/UserInterfaceDebounced.h>
#include <InputHelper/InputMultiClick.h>

namespace StarterPack {

class UserInterfaceMultiClick : virtual public InputKeySource,
                                public InputMultiClick<UserInterfaceBasic::KEY> {

    // public:

    //     typedef UserInterfaceDebounced::KEY KEY;
    //     static constexpr KEY INACTIVE_KEY = UserInterfaceDebounced::INACTIVE_KEY;

    public:

        // virtual InputKeySource::KEY getStableKey() = 0;

        InputKeySource::KEY getMultiClick_Key() {
            auto key = getStableKey();
            return InputMultiClick::actionGetMultiClickKey( key );
        }

        // add getters to highlight there's something interesting here
        inline uint8_t getMultiClick_Count() {
            return InputMultiClick::multiClick_count;
        }

        inline bool getMultiClick_IsLongPressed() {
            return InputMultiClick::multiClick_isLongPressed;
        }

        inline bool getMultiClick_IsRepeated() {
            return InputMultiClick::multiClick_isRepeated;
        }

};

// class UserInterfaceDebouncedMultiClick_XXX : public UserInterfaceDebounced,
//                                          public InputMultiClick<UserInterfaceDebounced::KEY> {

//     public:

//         typedef UserInterfaceDebounced::KEY KEY;
//         static constexpr KEY INACTIVE_KEY = UserInterfaceDebounced::INACTIVE_KEY;

//     public:

//         KEY getMultiClick_Key() {
//             auto key = UserInterfaceDebounced::getDebouncedKey();
//             return InputMultiClick::actionGetMultiClickKey( key );
//         }

//         // add getters to highlight there's something interesting here
//         inline uint8_t getMultiClick_Count() {
//             return InputMultiClick::multiClick_count;
//         }

//         inline bool getMultiClick_IsLongPressed() {
//             return InputMultiClick::multiClick_isLongPressed;
//         }

//         inline bool getMultiClick_IsRepeated() {
//             return InputMultiClick::multiClick_isRepeated;
//         }

// };

}
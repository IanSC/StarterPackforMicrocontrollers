#pragma once

#include <inttypes.h>

#include <UserInterface/UserInterfaceKeyUpDown.h>
#include <InputHelper/InputRepeater.h>

namespace StarterPack {

class UserInterfaceRepeated : public UserInterfaceKeyUpDown,
                              public InputRepeater<UserInterfaceKeyUpDown::KEY> {

    public:

        typedef UserInterfaceKeyUpDown::KEY KEY;
        static constexpr KEY INACTIVE_KEY = UserInterfaceKeyUpDown::INACTIVE_KEY;

    // //
    // // REPEATER SETTINGS
    // //
    // public:

    //     static inline InputRepeater::Settings getGlobalRepeaterSettings() {
    //         return InputRepeater<KEY>::globalRepeaterSettings;
    //     }

    //     inline InputRepeater::Settings *createRepeaterSettings() {
    //         auto s = new InputRepeater::Settings();
    //         InputRepeater::assignRepeaterSettings(s);
    //         return s;
    //     }

    //
    // VIRTUAL
    //
    public:

        // virtual KEY getDebouncedKey() = 0;

        KEY getRepeatingKey() {
            auto key = UserInterfaceKeyUpDown::getDebouncedKey();
            // auto key = getDebouncedKey();
            return InputRepeater::actionGetRepeatingKey( key );
        }

};

}
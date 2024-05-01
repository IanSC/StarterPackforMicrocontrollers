#pragma once

#include <stdint.h>

#include <UserInterface/UserInterfaceBasic.h>
#include <UserInterface/UserInterfaceMultiClick.h>
#include <UserInterface/UserInterfaceRepeated.h>
#include <InputHelper/InputRepeater.h>

namespace StarterPack {

class UserInterfaceAllKeys : public UserInterfaceBasic,
                             public UserInterfaceMultiClick,
                             public UserInterfaceRepeated {
                            //  public InputMultiClick<InputKeySource::KEY>,
                            //  public InputRepeater<InputKeySource::KEY> {

    // public:

    //     typedef UserInterfaceDebouncedMultiClick::KEY KEY;
    //     static constexpr KEY INACTIVE_KEY = UserInterfaceDebouncedMultiClick::INACTIVE_KEY;

    public:

        // virtual InputKeySource::KEY getStableKey() = 0;

        // InputKeySource::KEY getMultiClick_Key() {
        //     auto key = getStableKey();
        //     return InputMultiClick::actionGetMultiClickKey( key );
        // }

        // // add getters to highlight there's something interesting here
        // inline uint8_t getMultiClick_Count() {
        //     return InputMultiClick::multiClick_count;
        // }

        // inline bool getMultiClick_IsLongPressed() {
        //     return InputMultiClick::multiClick_isLongPressed;
        // }

        // inline bool getMultiClick_IsRepeated() {
        //     return InputMultiClick::multiClick_isRepeated;
        // }

        // InputKeySource::KEY getRepeatingKey() {
        //     auto key = getStableKey();
        //     return InputRepeater::actionGetRepeatingKey( key );
        // }

};

// class UserInterfaceAllKeys : public UserInterfaceMultiClick,
//                              public InputRepeater<UserInterfaceMultiClick::KEY> {

//     // public:

//     //     typedef UserInterfaceDebouncedMultiClick::KEY KEY;
//     //     static constexpr KEY INACTIVE_KEY = UserInterfaceDebouncedMultiClick::INACTIVE_KEY;

//     public:

//        InputKeySource::KEY getRepeatingKey() {
//             auto key = UserInterfaceMultiClick::getDebouncedKey();
//             return InputRepeater::actionGetRepeatingKey( key );
//         }

// };

// class UserInterfaceAllKeys : public UserInterfaceDebouncedMultiClick,
//                              public InputRepeater<UserInterfaceDebouncedMultiClick::KEY> {

//     public:

//         typedef UserInterfaceDebouncedMultiClick::KEY KEY;
//         static constexpr KEY INACTIVE_KEY = UserInterfaceDebouncedMultiClick::INACTIVE_KEY;

//     public:

//         KEY getRepeatingKey() {
//             auto key = UserInterfaceDebouncedMultiClick::getDebouncedKey();
//             return InputRepeater::actionGetRepeatingKey( key );
//         }

// };

}
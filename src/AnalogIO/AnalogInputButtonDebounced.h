#pragma once

#include <AnalogIO/AnalogInputButton.h>
#include <UserInterface/UserInterfaceKeyUpDown.h>

namespace StarterPack {

class AnalogInputButtonDebounced : public AnalogInputButton,
                                   public UserInterfaceKeyUpDown {

    protected:

        typedef UserInterfaceKeyUpDown::KEY KEY;
        static constexpr KEY INACTIVE_KEY = UserInterfaceKeyUpDown::INACTIVE_KEY;

    public:

        AnalogInputButtonDebounced( const uint8_t pin ) :
        AnalogInputButton(pin) {
        }

    //
    // VIRTUAL OVERRIDES
    //
    public:

        inline KEY getNonDebouncedKey() override {
            auto value = readSlot();
            return UserInterfaceKeyUpDown::actionDebounce(value);
        }

};

}

#pragma once

#include <DigitalIO/DigitalInput.h>
#include <UserInterface/UserInterfaceKeyUpDown.h>

namespace StarterPack {

class DigitalInputDebounced : public DigitalInput,
                              public UserInterfaceKeyUpDown {

    protected:

        typedef UserInterfaceKeyUpDown::KEY KEY;
        static constexpr KEY INACTIVE_KEY = UserInterfaceKeyUpDown::INACTIVE_KEY;

    public:

        DigitalInputDebounced( const uint8_t pin, optionsEnum options = optionsEnum::ACTIVE_LOW_PULLUP ) :
        DigitalInput(pin,options) {
        }

    //
    // VIRTUAL OVERRIDES
    //
    public:

        inline KEY getNonDebouncedKey() override {
            return DigitalInput::read();
        }

};

}

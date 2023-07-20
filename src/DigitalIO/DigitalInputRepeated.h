#pragma once

#include <DigitalIO/DigitalInput.h>
// #include <InputHelper/InputRepeater.h>
#include <UserInterface/UserInterfaceRepeated.h>

namespace StarterPack {

class DigitalInputRepeated : public DigitalInput,
                             public UserInterfaceRepeated {

    protected:

        typedef UserInterfaceRepeated::KEY KEY;
        static constexpr KEY INACTIVE_KEY = UserInterfaceRepeated::INACTIVE_KEY;

    public:

        DigitalInputRepeated( const uint8_t pin, optionsEnum options = optionsEnum::ACTIVE_LOW_PULLUP ) :
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

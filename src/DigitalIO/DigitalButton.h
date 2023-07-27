#pragma once

#include <DigitalIO/DigitalInput.h>

#include <UserInterface/UserInterfaceDebounced.h>
#include <UserInterface/UserInterfaceRepeated.h>
#include <UserInterface/UserInterfaceMultiClick.h>
#include <UserInterface/UserInterfaceAllKeys.h>

namespace StarterPack {

//
// DEBOUNCED
//
class DigitalButtonDB : public DigitalInput,
                        public UserInterfaceDebounced {

    public:

        DigitalButtonDB( const uint8_t pin, Init options = Init::None ) :
        DigitalInput(pin,options) {}

    public:

        inline UserInterfaceDebounced::KEY getNonDebouncedKey() override {
            return DigitalInput::read();
        }

};

//
// DEBOUNCED REPEATED
//
class DigitalButtonRP : public DigitalInput,
                        public UserInterfaceRepeated {

    public:

        DigitalButtonRP( const uint8_t pin, Init options = Init::None ) :
        DigitalInput(pin,options) {}

    public:

        inline UserInterfaceRepeated::KEY getNonDebouncedKey() override {
            return DigitalInput::read();
        }

};

//
// DEBOUNCED MULTI-CLICK
//
class DigitalButtonMC : public DigitalInput,
                        public UserInterfaceMultiClick {

    public:

        DigitalButtonMC( const uint8_t pin, Init options = Init::None ) :
        DigitalInput(pin,options) {}

    public:

        inline UserInterfaceMultiClick::KEY getNonDebouncedKey() override {
            return DigitalInput::read();
        }

};

//
// DEBOUNCED MULTI-CLICK WITH REAL REPEATED LOGIC
//
class DigitalButton : public DigitalInput,
                      public UserInterfaceAllKeys {

    public:

        DigitalButton( const uint8_t pin, Init options = Init::None ) :
        DigitalInput(pin,options) {}

    public:

        inline UserInterfaceAllKeys::KEY getNonDebouncedKey() override {
            return DigitalInput::read();
        }

};

}
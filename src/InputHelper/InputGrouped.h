#pragma once

#include <InputHelper/InputGroupedBase.h>

#include <UserInterface/UserInterfaceDebounced.h>
#include <UserInterface/UserInterfaceRepeated.h>
#include <UserInterface/UserInterfaceMultiClick.h>
#include <UserInterface/UserInterfaceAllKeys.h>

namespace StarterPack {

//
// DEBOUNCED
//
class InputGroupedDB : public InputGroupedBase,
                       public UserInterfaceDebounced {
  
    public:

        inline UserInterfaceDebounced::KEY getNonDebouncedKey() override {
            return InputGroupedBase::getNonDebouncedKey();
        }

};

//
// DEBOUNCED REPEATED
//
class InputGroupedRP : public InputGroupedBase,
                       public UserInterfaceRepeated {

    public:

        inline UserInterfaceRepeated::KEY getNonDebouncedKey() override {
            return InputGroupedBase::getNonDebouncedKey();
        }

};

//
// DEBOUNCED MULTI-CLICK
//
class InputGroupedMC : public InputGroupedBase,
                       public UserInterfaceMultiClick {

    public:

        inline UserInterfaceMultiClick::KEY getNonDebouncedKey() override {
            return InputGroupedBase::getNonDebouncedKey();
        }

};

//
// DEBOUNCED MULTI-CLICK WITH REAL REPEATED LOGIC
//
class InputGrouped : public InputGroupedBase,
                     public UserInterfaceAllKeys {

    public:

        inline UserInterfaceAllKeys::KEY getNonDebouncedKey() override {
            return InputGroupedBase::getNonDebouncedKey();
        }

};

}

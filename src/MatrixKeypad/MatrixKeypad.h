#pragma once

#include <MatrixKeypad/MatrixKeypadMapped.h>

#include <UserInterface/UserInterfaceDebounced.h>
#include <UserInterface/UserInterfaceRepeated.h>
#include <UserInterface/UserInterfaceMultiClick.h>
#include <UserInterface/UserInterfaceAllKeys.h>

namespace StarterPack {

//
// DEBOUNCED
//
class MatrixKeypadDB : public MatrixKeypadMapped,
                       public UserInterfaceDebounced {

    public:

        inline UserInterfaceDebounced::KEY getNonDebouncedKey() override {
            return MatrixKeypadMapped::getMappedKey();
        }

};

//
// DEBOUNCED REPEATED
//
class MatrixKeypadRP : public MatrixKeypadMapped,
                       public UserInterfaceRepeated {

    public:

        inline UserInterfaceRepeated::KEY getNonDebouncedKey() override {
            return MatrixKeypadMapped::getMappedKey();
        }

};

//
// DEBOUNCED MULTI-CLICK
//
class MatrixKeypadMC : public MatrixKeypadMapped,
                       public UserInterfaceMultiClick {

    public:

        inline UserInterfaceRepeated::KEY getNonDebouncedKey() override {
            return MatrixKeypadMapped::getMappedKey();
        }

};

//
// DEBOUNCED MULTI-CLICK WITH REAL REPEATED LOGIC
//
class MatrixKeypad : public MatrixKeypadMapped,
                     public UserInterfaceAllKeys {

    public:

        inline UserInterfaceAllKeys::KEY getNonDebouncedKey() override {
            return MatrixKeypadMapped::getMappedKey();
        }

};

}
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
                       public UserInterfaceBasic,
                       public UserInterfaceDebounced { public:
    inline InputKeySource::KEY getNonDebouncedKey() override {
        return MatrixKeypadMapped::getMappedKey();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() { }
};

//
// DEBOUNCED REPEATED
//
class MatrixKeypadRP : public MatrixKeypadMapped,
                       public UserInterfaceBasic,
                       public UserInterfaceDebounced,
                       public UserInterfaceRepeated { public:
    inline InputKeySource::KEY getNonDebouncedKey() override {
        return MatrixKeypadMapped::getMappedKey();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() { }
};

//
// DEBOUNCED MULTI-CLICK
//
class MatrixKeypadMC : public MatrixKeypadMapped,
                       public UserInterfaceBasic,
                       public UserInterfaceDebounced,
                       public UserInterfaceMultiClick { public:
    inline InputKeySource::KEY getNonDebouncedKey() override {
        return MatrixKeypadMapped::getMappedKey();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() { }
};

//
// DEBOUNCED MULTI-CLICK WITH REAL REPEATED LOGIC
//
class MatrixKeypad : public MatrixKeypadMapped,
                     public UserInterfaceDebounced,
                     public UserInterfaceAllKeys { public:
    inline InputKeySource::KEY getNonDebouncedKey() override {
        return MatrixKeypadMapped::getMappedKey();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() { }
};

// //
// // DEBOUNCED
// //
// class MatrixKeypadDB : public MatrixKeypadMapped,
//                        public UserInterfaceDebounced {

//     public:

//         inline KEY getNonDebouncedKey() override {
//             return MatrixKeypadMapped::getMappedKey();
//         }

// };

// //
// // DEBOUNCED REPEATED
// //
// class MatrixKeypadRP : public MatrixKeypadMapped,
//                        public UserInterfaceRepeated {

//     public:

//         inline KEY getNonDebouncedKey() override {
//             return MatrixKeypadMapped::getMappedKey();
//         }

// };

// //
// // DEBOUNCED MULTI-CLICK
// //
// class MatrixKeypadMC : public MatrixKeypadMapped,
//                        public UserInterfaceMultiClick {

//     public:

//         inline KEY getNonDebouncedKey() override {
//             return MatrixKeypadMapped::getMappedKey();
//         }

// };

// //
// // DEBOUNCED MULTI-CLICK WITH REAL REPEATED LOGIC
// //
// class MatrixKeypad : public MatrixKeypadMapped,
//                      public UserInterfaceAllKeys {

//     public:

//         inline KEY getNonDebouncedKey() override {
//             return MatrixKeypadMapped::getMappedKey();
//         }

// };

}
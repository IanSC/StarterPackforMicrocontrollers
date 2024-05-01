#pragma once

#include <InputHelper/InputGroupedBase.h>

#include <UserInterface/UserInterfaceBasic.h>
#include <UserInterface/UserInterfaceDebounced.h>
#include <UserInterface/UserInterfaceRepeated.h>
#include <UserInterface/UserInterfaceMultiClick.h>
#include <UserInterface/UserInterfaceAllKeys.h>

namespace StarterPack {

//
// DEBOUNCED
//
class InputGroupedDB : public InputGroupedBase,
                       public UserInterfaceBasic,
                       public UserInterfaceDebounced { public:

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return InputGroupedBase::getNonDebouncedKey();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() { InputGroupedBase::clearBuffers(); }
};

//
// DEBOUNCED REPEATED
//
class InputGroupedRP : public InputGroupedBase,
                       public UserInterfaceBasic,
                       public UserInterfaceDebounced,
                       public UserInterfaceRepeated { public:

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return InputGroupedBase::getNonDebouncedKey();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() { InputGroupedBase::clearBuffers(); }
};

//
// DEBOUNCED MULTI-CLICK
//
class InputGroupedMC : public InputGroupedBase,
                       public UserInterfaceBasic,
                       public UserInterfaceDebounced,
                       public UserInterfaceMultiClick { public:

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return InputGroupedBase::getNonDebouncedKey();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() { InputGroupedBase::clearBuffers(); }
};

//
// DEBOUNCED MULTI-CLICK WITH REAL REPEATED LOGIC
//
class InputGrouped : public InputGroupedBase,
                     public UserInterfaceDebounced,
                     public UserInterfaceAllKeys { public:

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return InputGroupedBase::getNonDebouncedKey();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() { InputGroupedBase::clearBuffers(); }
};

// //
// // DEBOUNCED
// //
// class InputGroupedDB : public InputGroupedBase,
//                        public UserInterfaceDebounced {
  
//     public:

//         inline UserInterfaceDebounced::KEY getNonDebouncedKey() override {
//             return InputGroupedBase::getNonDebouncedKey();
//         }

// };

// //
// // DEBOUNCED REPEATED
// //
// class InputGroupedRP : public InputGroupedBase,
//                        public UserInterfaceDebouncedRepeated {

//     public:

//         inline UserInterfaceDebouncedRepeated::KEY getNonDebouncedKey() override {
//             return InputGroupedBase::getNonDebouncedKey();
//         }

// };

// //
// // DEBOUNCED MULTI-CLICK
// //
// class InputGroupedMC : public InputGroupedBase,
//                        public UserInterfaceDebouncedMultiClick {

//     public:

//         inline UserInterfaceDebouncedMultiClick::KEY getNonDebouncedKey() override {
//             return InputGroupedBase::getNonDebouncedKey();
//         }

// };

// //
// // DEBOUNCED MULTI-CLICK WITH REAL REPEATED LOGIC
// //
// class InputGrouped : public InputGroupedBase,
//                      public UserInterfaceAllKeys {

//     public:

//         inline UserInterfaceAllKeys::KEY getNonDebouncedKey() override {
//             return InputGroupedBase::getNonDebouncedKey();
//         }

// };

}

#pragma once

#include <AnalogIO/AnalogButtonsMapped.h>

#include <UserInterface/UserInterfaceBasic.h>
#include <UserInterface/UserInterfaceDebounced.h>
#include <UserInterface/UserInterfaceRepeated.h>
#include <UserInterface/UserInterfaceMultiClick.h>
#include <UserInterface/UserInterfaceAllKeys.h>

namespace StarterPack {

//
// DEBOUNCED
//
class AnalogButtonDB : public AnalogButtonsMapped,
                       public UserInterfaceBasic,
                       public UserInterfaceDebounced { public:

    AnalogButtonDB( const int8_t pin ) : AnalogButtonsMapped(pin) { }

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return AnalogButtonsMapped::read();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() override { }
};

//
// DEBOUNCED REPEATED
//
class AnalogButtonRP : public AnalogButtonsMapped,
                       public UserInterfaceBasic,
                       public UserInterfaceDebounced,
                       public UserInterfaceRepeated { public:

    AnalogButtonRP( const int8_t pin ) : AnalogButtonsMapped(pin) { }

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return AnalogButtonsMapped::read();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() override { }
};

//
// DEBOUNCED MULTI-CLICK
//
class AnalogButtonsMC : public AnalogButtonsMapped,
                        public UserInterfaceBasic,
                        public UserInterfaceDebounced,
                        public UserInterfaceMultiClick { public:

    AnalogButtonsMC( const int8_t pin ) : AnalogButtonsMapped(pin) { }

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return AnalogButtonsMapped::read();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() override { }
};

//
// DEBOUNCED MULTI-CLICK WITH REAL REPEATED LOGIC
//
class AnalogButtons : public AnalogButtonsMapped,
                      public UserInterfaceDebounced,
                      public UserInterfaceAllKeys { public:

    AnalogButtons( const int8_t pin ) : AnalogButtonsMapped(pin) { }

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return AnalogButtonsMapped::read();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() override { }
};

// //
// // DEBOUNCED
// //
// class AnalogButtonDB : public AnalogButtonsMapped,
//                        public UserInterfaceDebounced {

//     public:

//         AnalogButtonDB( const int8_t pin ) : AnalogButtonsMapped(pin) { }

//     public:

//         inline UserInterfaceDebounced::KEY getNonDebouncedKey() override {
//             return AnalogButtonsMapped::read();
//             // auto value = AnalogButtonsMapped::read();
//             // return UserInterfaceDebounced::actionDebounce(value);
//         }

// };

// //
// // DEBOUNCED REPEATED
// //
// class AnalogButtonRP : public AnalogButtonsMapped,
//                        public UserInterfaceDebouncedRepeated {

//     public:

//         AnalogButtonRP( const int8_t pin ) : AnalogButtonsMapped(pin) { }

//     public:

//         inline UserInterfaceDebouncedRepeated::KEY getNonDebouncedKey() override {
//             return AnalogButtonsMapped::read();
//             // auto value = AnalogButtonsMapped::read();
//             // return UserInterfaceRepeated::actionGetRepeatingKey(value);
//         }

// };

// //
// // DEBOUNCED MULTI-CLICK
// //
// class AnalogButtonsMC : public AnalogButtonsMapped,
//                         public UserInterfaceDebouncedMultiClick {

//     public:

//         AnalogButtonsMC( const int8_t pin ) : AnalogButtonsMapped(pin) { }

//     public:

//         inline UserInterfaceDebouncedMultiClick::KEY getNonDebouncedKey() override {
//             return AnalogButtonsMapped::read();
//             // auto value = AnalogButtonsMapped::read();
//             // return UserInterfaceMultiClick::actionGetMultiClickKey(value);
//         }

// };

// //
// // DEBOUNCED MULTI-CLICK WITH REAL REPEATED LOGIC
// //
// class AnalogButtons : public AnalogButtonsMapped,
//                       public UserInterfaceAllKeys {

//     public:

//         AnalogButtons( const int8_t pin ) : AnalogButtonsMapped(pin) { }

//     public:

//         inline UserInterfaceAllKeys::KEY getNonDebouncedKey() override {
//             return AnalogButtonsMapped::read();
//             // auto value = AnalogButtonsMapped::read();
//             // return UserInterfaceMultiClick::actionGetMultiClickKey(value);
//         }

// };

}

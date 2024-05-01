#pragma once

#include <DigitalIO/DigitalInput.h>

#include <UserInterface/UserInterfaceBasic.h>
#include <UserInterface/UserInterfaceDebounced.h>
#include <UserInterface/UserInterfaceRepeated.h>
#include <UserInterface/UserInterfaceMultiClick.h>
#include <UserInterface/UserInterfaceAllKeys.h>

namespace StarterPack {

//
// DEBOUNCED
//
class DigitalButtonDB : public DigitalInput,
                        public UserInterfaceBasic,
                        public UserInterfaceDebounced { public:

    DigitalButtonDB( const int8_t pin, Init options = Init::Default )
        : DigitalInput(pin,options) {}
    DigitalButtonDB( const int8_t pin, Init options, 
    uint16_t stabilizationOnPressedMs, uint16_t stabilizationOnReleasedMs ) : DigitalInput( pin, options ) {
        setDebounceStabilizeTimeInMs( stabilizationOnPressedMs, stabilizationOnReleasedMs );
    }

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return DigitalInput::read();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() override { }
};

//
// DEBOUNCED REPEATED
//
class DigitalButtonRP : public DigitalInput,
                        public UserInterfaceBasic,
                        public UserInterfaceDebounced,
                        public UserInterfaceRepeated { public:

    DigitalButtonRP( const int8_t pin, Init options = Init::Default )
        : DigitalInput(pin,options) {}
    DigitalButtonRP( const int8_t pin, Init options, 
    uint16_t stabilizationOnPressedMs, uint16_t stabilizationOnReleasedMs ) : DigitalInput( pin, options ) {
        setDebounceStabilizeTimeInMs( stabilizationOnPressedMs, stabilizationOnReleasedMs );
    }

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return DigitalInput::read();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() override { }
};

//
// DEBOUNCED MULTI-CLICK
//
class DigitalButtonMC : public DigitalInput,
                        public UserInterfaceBasic,
                        public UserInterfaceDebounced,
                        public UserInterfaceMultiClick { public:

    DigitalButtonMC( const int8_t pin, Init options = Init::Default )
        : DigitalInput(pin,options) {}
    DigitalButtonMC( const int8_t pin, Init options, 
    uint16_t stabilizationOnPressedMs, uint16_t stabilizationOnReleasedMs ) : DigitalInput( pin, options ) {
        setDebounceStabilizeTimeInMs( stabilizationOnPressedMs, stabilizationOnReleasedMs );
    }

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return DigitalInput::read();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() override { }
};

//
// DEBOUNCED MULTI-CLICK WITH REAL REPEATED LOGIC
//
class DigitalButton : public DigitalInput,
                      public UserInterfaceDebounced,
                      public UserInterfaceAllKeys { public:

    DigitalButton( const int8_t pin, Init options = Init::Default )
        : DigitalInput( pin, options ) {}
    DigitalButton( const int8_t pin, Init options, 
    uint16_t stabilizationOnPressedMs, uint16_t stabilizationOnReleasedMs ) : DigitalInput( pin, options ) {
        setDebounceStabilizeTimeInMs( stabilizationOnPressedMs, stabilizationOnReleasedMs );
    }

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return DigitalInput::read();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UserInterfaceDebounced::getDebouncedKey();
    }
    inline void clearBuffers() override { }
};

// class DigitalButton : public DigitalInput,
//                       public UserInterfaceAllKeys {

//     public:

//         DigitalButton( const int8_t pin, Init options = Init::Default ) : DigitalInput( pin, options ) {}

//         DigitalButton( const int8_t pin, Init options, 
//         uint16_t stabilizationOnPressedMs, uint16_t stabilizationOnReleasedMs ) : DigitalInput( pin, options ) {
//             setStabilizationTimeInMs( stabilizationOnPressedMs, stabilizationOnReleasedMs );
//         }

//     public:

//         inline UserInterfaceAllKeys::KEY getNonDebouncedKey() override {
//             return DigitalInput::read();
//         }

// };

}

// -------------------------------
// StabilizedTime vs DebounceDelay
// -------------------------------
// DebounceDelay  - time to ignore any changes in signal after initially detected
// StabilizedTime - time duration with no signal change before being accepted
//
// eg. very noisy signal, that bounces around for 1000 ms
//     if only StabilizedTime is set to 1100ms
//     actual signal bounces around for 1000ms, then need to be stabilized for 1100ms
//     total 2100ms, too long
// eg. same noisy signal with DebounceDelay=1000ms and StabilizedTime=100ms
//     for the first 1000ms, any changes are ignored, then 100ms of stabilized signal
//     total 1100ms only
//
// BUT: if user changes their mind and releases the button, it will still wait for the delay
//      before processing another key, use skipDelayWait() or cancelDelayWait() to exit waiting
//
// -------------------
// Pressed vs Released
// -------------------
// if a button is pressed, there will be more noise vs releasing
// also for application that requires immediate stop

#pragma once

#include <Arduino.h>
#include <inttypes.h>

namespace StarterPack {

//
// SETTINGS
//

    // TO CHECK: if not referenced will be stripped away by compiler ???

    struct InputDebouncerSettings {
        uint16_t debounceDelayPressedInMs = 0;      // time delay before checking for stabilized signal
        uint16_t debounceDelayReleasedInMs = 0;
        uint16_t stabilizedTimePressedInMs = 50;    // debouncing keyDown time
        uint16_t stabilizedTimeReleasedInMs = 10;   // debouncing keyUp time
        // uint16_t minimumDebounceTimeInMs = 50;   // minimum debounce time before allowed to be cancelled 
    };

    static struct InputDebouncerSettings globalInputDebouncerSettings;

template<typename T>
class InputDebouncer {

    private:

        typedef T KEY;
        static constexpr KEY INACTIVE_KEY = 0;

    //
    // SETTINGS
    //
    public:

        // struct Settings {
        //     uint16_t debounceDelayPressedInMs = 0;      // time delay before checking for stabilized signal
        //     uint16_t debounceDelayReleasedInMs = 0;
        //     uint16_t stabilizedTimePressedInMs = 50;    // debouncing keyDown time
        //     uint16_t stabilizedTimeReleasedInMs = 10;   // debouncing keyUp time
        //     // uint16_t minimumDebounceTimeInMs = 50;      // minimum debounce time before allowed to be cancelled 
        // };

        // // note: each version will create a new settings
        // // eg. InputDebouncerBase<uint8_t>
        // //     InputDebouncerBase<int>
        // //     ... will have separate settings and occupy memory
        // //     but making one single global will 
        // static Settings globalDebouncerSettings;           // global single instance setting to save space

        // create reference so user can go: obj.globalDebouncerSettings->stabilizedTimePressedInMs = 50;
        static constexpr InputDebouncerSettings *globalDebouncerSettings = &StarterPack::globalInputDebouncerSettings;

        // actual settings per instance, by default points to global
        InputDebouncerSettings *debouncerSettings = &StarterPack::globalInputDebouncerSettings;

        InputDebouncerSettings *createDebouncerSettings(
        uint16_t stabilizedTimePressedInMs = 50,
        uint16_t stabilizedTimeReleasedInMs = 10,
        uint16_t debounceDelayPressedInMs = 0,
        uint16_t debounceDelayReleasedInMs = 0 ) {
            // create and use a new settings, caller should handle freeing memory
            auto s = new InputDebouncerSettings();
            this->stabilizedTimePressedInMs = stabilizedTimePressedInMs;
            this->stabilizedTimeReleasedInMs = stabilizedTimeReleasedInMs;
            this->debounceDelayPressedInMs = debounceDelayPressedInMs;
            this->debounceDelayReleasedInMs = debounceDelayReleasedInMs;
            this->debouncerSettings = s;
            return s;
        }

        void assignDebouncerSettings( InputDebouncerSettings &settings ) {
            // caller is still owner and should handle freeing memory
            this->debouncerSettings = &settings;
        }

        void assignDebouncerSettings( InputDebouncerSettings *settings ) {
            // caller is still owner and should handle freeing memory
            this->debouncerSettings = settings;
        }

    //
    // ACTION
    //
    private:

        enum class debounceModeEnum : uint8_t {
            Waiting,        // waiting for key
            TimeDelay,      // got 1st press, delay without any checking
            CheckStability, // wait for stabilization
            SteadyState     // stabilized but kept pressed
        };
        debounceModeEnum debounceMode = debounceModeEnum::Waiting;

        KEY debounceCandidateKey = INACTIVE_KEY;        // new key being debounced
        unsigned long debounceTimerStart = 0;           // time key was first detected or start of stability test

        KEY debounceLastApprovedKey = INACTIVE_KEY;     // last key debounced

    public:

        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;

        KEY actionDebounce(KEY key) {
            switch( debounceMode ) {

            case debounceModeEnum::Waiting:
                debounceTimerStart = millis();
                debounceCandidateKey = key;

                if (key == INACTIVE_KEY) {
                    if (debouncerSettings->debounceDelayReleasedInMs != 0) {
                        debounceMode = debounceModeEnum::TimeDelay;
                        DEBUG_TRACE( Serial.print("[DELAY1 ") );
                        DEBUG_TRACE( Serial.print(debounceCandidateKey) );
                        DEBUG_TRACE( Serial.print("]") );
                        // return lastApprovedKey;
                        break;
                    }
                } else {
                    if (debouncerSettings->debounceDelayPressedInMs != 0) {
                        debounceMode = debounceModeEnum::TimeDelay;
                        DEBUG_TRACE( Serial.print("[DELAY2 ") );
                        DEBUG_TRACE( Serial.print(debounceCandidateKey) );
                        DEBUG_TRACE( Serial.print("]") );
                        // return lastApprovedKey;
                        break;
                    }
                }
                DEBUG_TRACE( Serial.print("[STABILITY1 ") );
                DEBUG_TRACE( Serial.print(debounceCandidateKey) );
                DEBUG_TRACE( Serial.print("]") );
                debounceMode = debounceModeEnum::CheckStability;
                // return lastApprovedKey;
                break;

            case debounceModeEnum::TimeDelay:
                {
                    auto now = millis();
                    auto elapsed = now - debounceTimerStart;
                    if (key == INACTIVE_KEY) {
                        if (elapsed < debouncerSettings->debounceDelayReleasedInMs) {
                            // return lastApprovedKey;
                            break;
                        }
                    } else {
                        if (elapsed < debouncerSettings->debounceDelayPressedInMs) {
                            // return lastApprovedKey;
                            break;
                        }
                    }
                    DEBUG_TRACE( Serial.print("[STABILITY2 ") );
                    DEBUG_TRACE( Serial.print(debounceCandidateKey) );
                    DEBUG_TRACE( Serial.print("]") );
                    debounceMode = debounceModeEnum::CheckStability;
                    debounceTimerStart = now;
                }
                // return lastApprovedKey;
                break;

            case debounceModeEnum::CheckStability:
                if (key != debounceCandidateKey) {
                    debounceMode = debounceModeEnum::Waiting;
                    DEBUG_TRACE( Serial.print("[RESET ") );
                    DEBUG_TRACE( Serial.print(debounceCandidateKey) );
                    DEBUG_TRACE( Serial.print("-->") );
                    DEBUG_TRACE( Serial.print(key) );
                    DEBUG_TRACE( Serial.print("]") );
                    // return lastApprovedKey;
                    break;
                }

                {
                    auto elapsed = millis() - debounceTimerStart;
                    if (key == INACTIVE_KEY) {
                        if (elapsed < debouncerSettings->stabilizedTimeReleasedInMs) {
                            // return lastApprovedKey;
                            break;
                        }
                    } else {
                        if (elapsed < debouncerSettings->stabilizedTimePressedInMs) {
                            // return lastApprovedKey;
                            break;
                        }
                    }
                }
                DEBUG_TRACE( Serial.print("[STEADY ") );
                DEBUG_TRACE( Serial.print(debounceCandidateKey) );
                DEBUG_TRACE( Serial.print("]") );
                debounceMode = debounceModeEnum::SteadyState;
                debounceLastApprovedKey = debounceCandidateKey;
                // return lastApprovedKey;
                break;

            case debounceModeEnum::SteadyState:
                if (key != debounceLastApprovedKey) {
                    DEBUG_TRACE( Serial.print("[RELEASED ") );
                    DEBUG_TRACE( Serial.print(debounceLastApprovedKey) );
                    DEBUG_TRACE( Serial.print("-->") );
                    DEBUG_TRACE( Serial.print(key) );
                    DEBUG_TRACE( Serial.print("]") );
                    debounceMode = debounceModeEnum::Waiting;
                }
                // return lastApprovedKey;
                break;
            }
            return debounceLastApprovedKey;
        }

        #undef DEBUG_TRACE

        void skipDebounceDelay() {
            // proceed to stability check
            // why skip? just don't set it ???
            if (debounceMode == debounceModeEnum::TimeDelay) {
                debounceMode = debounceModeEnum::CheckStability;
                debounceTimerStart = millis();
            }
        }

        void cancelDebounceDelay() {
            // for other cases, user can just release or press another key
            if (debounceMode == debounceModeEnum::TimeDelay)
                debounceMode = debounceModeEnum::Waiting;
        }

    //
    // ADDITIONAL ACTIONS
    //
    public:

        KEY actionGetKeyDown( KEY value ) {
            // return key, if transitioning
            auto origApproved = debounceLastApprovedKey;
            auto newApproved = actionDebounce(value);
            if ( newApproved != origApproved ) {
            // Serial.print(origApproved);
            // Serial.print('.');
            // Serial.println(newApproved);
                if ( newApproved != INACTIVE_KEY )
                    return newApproved;
            }
            return INACTIVE_KEY;
        }

        KEY actionGetKeyUp( KEY value ) {
            // return key, if transitioning
            auto origApproved = debounceLastApprovedKey;
            auto newApproved = actionDebounce(value);
            if ( newApproved != origApproved ) {
                if ( newApproved == INACTIVE_KEY ) {
                    // key released
                    // return key that was released
                    return origApproved;
                }
            }
            return INACTIVE_KEY;
        }

};

// template <typename T>
// struct InputDebouncer<T>::Settings InputDebouncer<T>::globalDebouncerSettings;

}

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

template<typename T>
class InputDebouncer {

    private:

        typedef T KEY;
        static constexpr KEY INACTIVE_KEY = 0;

    //
    // SETTINGS
    //
    public:

        struct Settings {
            uint16_t debounceDelayPressedInMs = 0;      // time delay before checking for stabilized signal
            uint16_t debounceDelayReleasedInMs = 0;
            uint16_t stabilizedTimePressedInMs = 50;    // debouncing keyDown time
            uint16_t stabilizedTimeReleasedInMs = 10;   // debouncing keyUp time
            // uint16_t activeStatesDebounceInMs = 50;     // debouncing keyDown time
            // uint16_t inactiveStateDebounceInMs = 50;    // debouncing keyUp time
            // uint16_t minimumDebounceTimeInMs = 50;      // minimum debounce time before allowed to be cancelled 
            // uint16_t confirmActiveStateTimeInMs = 0;    // time delay to confirm keyDown time
            // uint16_t confirmInactiveStateTimeInMs = 0;  // time delay to confirm keyUp time
            // uint16_t repeatDelayInMs = 400;
            // uint16_t repeatRateInMs  = 250;
        };

        // note: each version will create a new settings
        // eg. InputDebouncerBase<uint8_t>
        //     InputDebouncerBase<int>
        //     ... will have separate settings and occupy memory
        static Settings defaultDebouncerSettings;           // global single instance setting to save space

        Settings *debouncerSettings = &defaultDebouncerSettings;

        void assignDebouncerSettings( Settings &settings ) {
            // caller is still owner
            // and should handle freeing memory
            this->debouncerSettings = &defaultDebouncerSettings;
        }

    //
    // ACTION
    //
    protected:

        enum class modeEnum : uint8_t {
            Waiting, TimeDelay, CheckStability, SteadyState
        };
        modeEnum mode = modeEnum::Waiting;

        KEY candidateKey = INACTIVE_KEY;        // new key being debounced
        unsigned long timerStart = 0;           // time key was first detected or start of stability test

        KEY lastApprovedKey = INACTIVE_KEY;     // last key debounced

    public:

        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;

        KEY actionDebounce(KEY key) {
            switch( mode ) {

            case modeEnum::Waiting:
                timerStart = millis();
                candidateKey = key;

                if (key == INACTIVE_KEY) {
                    if (debouncerSettings->debounceDelayReleasedInMs != 0) {
                        mode = modeEnum::TimeDelay;
                        DEBUG_TRACE( Serial.print("[DELAY1 ") );
                        DEBUG_TRACE( Serial.print(candidateKey) );
                        DEBUG_TRACE( Serial.print("]") );
                        // return lastApprovedKey;
                        break;
                    }
                } else {
                    if (debouncerSettings->debounceDelayPressedInMs != 0) {
                        mode = modeEnum::TimeDelay;
                        DEBUG_TRACE( Serial.print("[DELAY2 ") );
                        DEBUG_TRACE( Serial.print(candidateKey) );
                        DEBUG_TRACE( Serial.print("]") );
                        // return lastApprovedKey;
                        break;
                    }
                }
                DEBUG_TRACE( Serial.print("[STABILITY1 ") );
                DEBUG_TRACE( Serial.print(candidateKey) );
                DEBUG_TRACE( Serial.print("]") );
                mode = modeEnum::CheckStability;
                // return lastApprovedKey;
                break;

            case modeEnum::TimeDelay:
                {
                    auto now = millis();
                    auto elapsed = now - timerStart;
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
                    DEBUG_TRACE( Serial.print(candidateKey) );
                    DEBUG_TRACE( Serial.print("]") );
                    mode = modeEnum::CheckStability;
                    timerStart = now;
                }
                // return lastApprovedKey;
                break;

            case modeEnum::CheckStability:
                if (key != candidateKey) {
                    mode = modeEnum::Waiting;
                    DEBUG_TRACE( Serial.print("[RESET ") );
                    DEBUG_TRACE( Serial.print(candidateKey) );
                    DEBUG_TRACE( Serial.print("-->") );
                    DEBUG_TRACE( Serial.print(key) );
                    DEBUG_TRACE( Serial.print("]") );
                    // return lastApprovedKey;
                    break;
                }

                {
                    auto elapsed = millis() - timerStart;
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
                DEBUG_TRACE( Serial.print(candidateKey) );
                DEBUG_TRACE( Serial.print("]") );
                mode = modeEnum::SteadyState;
                lastApprovedKey = candidateKey;
                // return lastApprovedKey;
                break;

            case modeEnum::SteadyState:
                if (key != lastApprovedKey) {
                    DEBUG_TRACE( Serial.print("[RELEASED ") );
                    DEBUG_TRACE( Serial.print(lastApprovedKey) );
                    DEBUG_TRACE( Serial.print("-->") );
                    DEBUG_TRACE( Serial.print(key) );
                    DEBUG_TRACE( Serial.print("]") );
                    mode = modeEnum::Waiting;
                }
                // return lastApprovedKey;
                break;
            }
            return lastApprovedKey;
        }

        #undef DEBUG_TRACE

        void skipDelayWait() {
            // proceed to stability check
            // why skip? just don't set it ???
            if (mode == modeEnum::TimeDelay) {
                mode = modeEnum::CheckStability;
                timerStart = millis();
            }
        }

        void cancelDelayWait() {
            // for other cases, user can just release or press another key
            if (mode == modeEnum::TimeDelay)
                mode = modeEnum::Waiting;
        }

    //
    // ADDITIONAL ACTIONS
    //
    public:

        KEY actionGetKeyDown( KEY value ) {
            // return key, if transitioning
            auto origApproved = lastApprovedKey;
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
            auto origApproved = lastApprovedKey;
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

template <typename T>
struct InputDebouncer<T>::Settings InputDebouncer<T>::defaultDebouncerSettings;

}

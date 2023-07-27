// -------------------------------
// StabilizedTime vs DebounceDelay
// -------------------------------
// DebounceDelay  - time to ignore any changes in signal after initially detected
// StabilizedTime - time duration the signal does not change anymore before being accepted
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
// if a button is pressed, there will be more noise than when releasing
// also for application that requires immediate stop

#pragma once

#include <Arduino.h>
#include <inttypes.h>

#include <InputHelper/InputFilterInterface.h>

namespace StarterPack {

template<typename DATA_TYPE>
class InputDebouncer : public InputFilterInterface<DATA_TYPE>
{

    private:

        // typedef T DATA_TYPE;
        static constexpr DATA_TYPE INACTIVE_KEY = 0;

    //
    // FILTER BASE
    //
    public:
        inline DATA_TYPE actionApplyFilter( DATA_TYPE value ) override {
            return actionDebounce(value);
        }

    //
    // SETTINGS
    //
    public:

        // debounceDelayPressedInMs     time delay before checking for stabilized signal
        // debounceDelayReleasedInMs
        // stabilizedTimePressedInMs    debouncing keyDown time
        // stabilizedTimeReleasedInMs   debouncing keyUp time
        #if defined(SP_INPUTDEBOUNCER_DEBOUNCE_DELAY_PRESSED_MS)
            uint16_t debounceDelayPressedInMs = SP_INPUTDEBOUNCER_DEBOUNCE_DELAY_PRESSED_MS;
        #else
            uint16_t debounceDelayPressedInMs = 0;
        #endif
        #if defined(SP_INPUTDEBOUNCER_DEBOUNCE_DELAY_RELEASED_MS)
            uint16_t debounceDelayReleasedInMs = SP_INPUTDEBOUNCER_DEBOUNCE_DELAY_RELEASED_MS;
        #else
            uint16_t debounceDelayReleasedInMs = 0;
        #endif
        #if defined(SP_INPUTDEBOUNCER_STABILIZED_TIME_PRESSED_MS)
            uint16_t stabilizedTimePressedInMs = SP_INPUTDEBOUNCER_STABILIZED_TIME_PRESSED_MS;
        #else
            uint16_t stabilizedTimePressedInMs = 50;
        #endif
        #if defined(SP_INPUTDEBOUNCER_STABILIZED_TIME_RELEASED_MS)
            uint16_t stabilizedTimeReleasedInMs = SP_INPUTDEBOUNCER_STABILIZED_TIME_RELEASED_MS;
        #else
            uint16_t stabilizedTimeReleasedInMs = 10;
        #endif

        inline void setDebouncerDelayInMs(uint16_t whenPressed, uint16_t whenReleased) {
            debounceDelayPressedInMs  = whenPressed;
            debounceDelayReleasedInMs = whenReleased;
        }

        inline void setStabilizationTimeInMs(uint16_t whenPressed, uint16_t whenReleased) {
            stabilizedTimePressedInMs  = whenPressed;
            stabilizedTimeReleasedInMs = whenReleased;
        }

        void copyDebounceSettings(InputDebouncer &instance) {
            debounceDelayPressedInMs   = instance.debounceDelayPressedInMs;
            debounceDelayReleasedInMs  = instance.debounceDelayReleasedInMs;
            stabilizedTimePressedInMs  = instance.stabilizedTimePressedInMs;
            stabilizedTimeReleasedInMs = instance.stabilizedTimeReleasedInMs;
        }

        // example:
        //                                             keyDown     keyUp
        // device                      inactiveState   stability   stability   confirm   notes
        // -------------------------   -------------   ---------   ---------   -------   -----
        // push button (active high)   false           50          50          0
        // push button (active low)    true            50          50          0
        // analog keypad               [depends]       50          50          0         1
        // analog rotary switch        [depends]       50          50          50        2
        
        // 1. resistor network style keys wired to a single analog pin
        //    similar to dfrobot lcd keypad shield
        // 2. rotary switch wired with resistor network same as #1
        //    when switching there is small gap between contacts that is unconnected
        //    switching A->B will be: A --> unconnected --> B
        //    to eliminate the momentary disconnection, put a confirm time value
        //
        // note: analog values must be mapped to button number before feeding to debouncer

    //
    // ACTION
    //
    private:

        //  (A) pressed -->
        //  (B)            delay: start --> end -->
        //  (C)                                     stability: start --> end

        enum class debounceModeEnum : uint8_t {
            Waiting,        // waiting for key
            TimeDelay,      // got 1st press, delay without any checking
            CheckStability, // wait for stabilization
            SteadyState     // stabilized but kept pressed
        };
        debounceModeEnum debounceMode = debounceModeEnum::Waiting;

        DATA_TYPE debounceCandidateKey = INACTIVE_KEY;      // new key being debounced
        unsigned long debounceTimerStart = 0;               // time key was first detected or start of stability test

        DATA_TYPE debounceLastApprovedKey = INACTIVE_KEY;   // last key debounced

    public:

        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;

        DATA_TYPE actionDebounce(DATA_TYPE key) {
            switch( debounceMode ) {

            case debounceModeEnum::Waiting:
                debounceTimerStart = millis();
                debounceCandidateKey = key;

                if (key == INACTIVE_KEY) {
                    if (debounceDelayReleasedInMs != 0) {
                        debounceMode = debounceModeEnum::TimeDelay;
                        DEBUG_TRACE( Serial.print("[DELAY1 ") );
                        DEBUG_TRACE( Serial.print(debounceCandidateKey) );
                        DEBUG_TRACE( Serial.print("]") );
                        // return debounceLastApprovedKey;
                        break;
                    }
                } else {
                    if (debounceDelayPressedInMs != 0) {
                        debounceMode = debounceModeEnum::TimeDelay;
                        DEBUG_TRACE( Serial.print("[DELAY2 ") );
                        DEBUG_TRACE( Serial.print(debounceCandidateKey) );
                        DEBUG_TRACE( Serial.print("]") );
                        // return debounceLastApprovedKey;
                        break;
                    }
                }
                DEBUG_TRACE( Serial.print("[STABILITY1 ") );
                DEBUG_TRACE( Serial.print(debounceCandidateKey) );
                DEBUG_TRACE( Serial.print("]") );
                debounceMode = debounceModeEnum::CheckStability;
                // return debounceLastApprovedKey;
                break;

            case debounceModeEnum::TimeDelay:
                {
                    auto now = millis();
                    auto elapsed = now - debounceTimerStart;
                    if (key == INACTIVE_KEY) {
                        if (elapsed < debounceDelayReleasedInMs) {
                            // return debounceLastApprovedKey;
                            break;
                        }
                    } else {
                        if (elapsed < debounceDelayPressedInMs) {
                            // return debounceLastApprovedKey;
                            break;
                        }
                    }
                    DEBUG_TRACE( Serial.print("[STABILITY2 ") );
                    DEBUG_TRACE( Serial.print(debounceCandidateKey) );
                    DEBUG_TRACE( Serial.print("]") );
                    debounceMode = debounceModeEnum::CheckStability;
                    debounceTimerStart = now;
                }
                // return debounceLastApprovedKey;
                break;

            case debounceModeEnum::CheckStability:
                if (key != debounceCandidateKey) {
                    debounceMode = debounceModeEnum::Waiting;
                    DEBUG_TRACE( Serial.print("[RESET ") );
                    DEBUG_TRACE( Serial.print(debounceCandidateKey) );
                    DEBUG_TRACE( Serial.print("-->") );
                    DEBUG_TRACE( Serial.print(key) );
                    DEBUG_TRACE( Serial.print("]") );
                    // return debounceLastApprovedKey;
                    break;
                }

                {
                    auto elapsed = millis() - debounceTimerStart;
                    if (key == INACTIVE_KEY) {
                        if (elapsed < stabilizedTimeReleasedInMs) {
                            // return debounceLastApprovedKey;
                            break;
                        }
                    } else {
                        if (elapsed < stabilizedTimePressedInMs) {
                            // return debounceLastApprovedKey;
                            break;
                        }
                    }
                }
                DEBUG_TRACE( Serial.print("[STEADY ") );
                DEBUG_TRACE( Serial.print(debounceCandidateKey) );
                DEBUG_TRACE( Serial.print("]") );
                debounceMode = debounceModeEnum::SteadyState;
                debounceLastApprovedKey = debounceCandidateKey;
                // return debounceLastApprovedKey;
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
                // return debounceLastApprovedKey;
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

        DATA_TYPE actionGetKeyDown( DATA_TYPE value ) {
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

        DATA_TYPE actionGetKeyUp( DATA_TYPE value ) {
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

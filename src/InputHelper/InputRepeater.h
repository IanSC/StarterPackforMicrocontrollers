// Send repeating keys if button is kept pressed.
// Keys are pulsed.
// Returns value, then INACTIVE, then value, then INACTIVE, while being called.
//
// ex. user keeps the button on hold
//     delay=400ms, rate=100ms
//     auto B = obj.actionGetRepeatingKey(true);
//
//     timeMs   B
//     0        true
//     ...      false
//     400      true
//     ...      false
//     500      true
//     ...
//
//  InputRepeater r;
//  while(1) {
//      auto key = r.actionGetRepeatingKey( digitalRead(PIN) );
//      if (key != 0) Serial.println("X");
//  }

#pragma once

#include <Arduino.h>
#include <stdint.h>

#include <Utility/spStr.h>

#include <InputHelper/InputFilterInterface.h>
#include <InputHelper/InputKeyFilter.h>

namespace StarterPack {

template<typename DATA_TYPE>
class InputRepeater : public InputFilterInterface<DATA_TYPE>,
                      public InputKeyFilterCore<DATA_TYPE> {

    private:

        // typedef T DATA_TYPE;
        static constexpr DATA_TYPE INACTIVE_KEY = 0;

    //
    // FILTER BASE
    //
    public:
        inline DATA_TYPE actionApplyFilter( DATA_TYPE value ) override {
            return actionGetRepeatingKey(value);
        }

    //
    // SETTINGS
    //
    public:

        // repeatDelayInMs   time before 1st repeat
        // repeatRateInMs    time between succeeding repeats
        #if defined(SP_INPUTREPEATER_REPEAT_DELAY_MS)
            uint16_t repeatDelayInMs = SP_INPUTREPEATER_REPEAT_DELAY_MS;
        #else
            uint16_t repeatDelayInMs = 400;
        #endif
        #if defined(SP_INPUTREPEATER_REPEAT_RATE_MS)
            uint16_t repeatRateInMs = SP_INPUTREPEATER_REPEAT_RATE_MS;
        #else
            uint16_t repeatRateInMs = 250;
        #endif

        inline void setRepeatSettingsInMs(uint16_t repeatDelay, uint16_t repeatRate) {
            repeatDelayInMs = repeatDelayInMs;
            repeatRateInMs = repeatRateInMs;
        }

        void copyRepeatSettings(InputRepeater &instance) {
            repeatDelayInMs = instance.repeatDelayInMs;
            repeatRateInMs  = instance.repeatRateInMs;
        }

    //
    // KEY FILTERS
    //
    // private:

    //     const char *allowedToRepeatKeys = nullptr;
    //     const char *cannotRepeatKeys = nullptr;

    //     bool allowedToRepeat( DATA_TYPE key ) {
    //         if ( allowedToRepeatKeys != nullptr ) {
    //             if ( Str::findCharacter( key, allowedToRepeatKeys ) )
    //                 return true;
    //             return false;
    //         }
    //         if ( cannotRepeatKeys != nullptr ) {
    //             if ( Str::findCharacter( key, cannotRepeatKeys ) )
    //                 return false;
    //             return true;
    //         }
    //         return true;
    //     } 

    //     DATA_TYPE filterKey( DATA_TYPE key ) {
    //         if ( allowedToRepeatKeys != nullptr ) {
    //             if ( Str::findCharacter( key, allowedToRepeatKeys ) )
    //                 return key;
    //             return INACTIVE_KEY;
    //         }
    //         if ( cannotRepeatKeys != nullptr ) {
    //             if ( Str::findCharacter( key, cannotRepeatKeys ) )
    //                 return INACTIVE_KEY;
    //             return key;
    //         }
    //         return key;
    //     } 

    public:

        inline void setAllowedToRepeatKeys( const char *list ) {
            InputKeyFilterCore<DATA_TYPE>::setAllowedKeys( list );
            // allowedToRepeatKeys = list;
        }

        inline void setCannotRepeatKeys( const char *list ) {
            InputKeyFilterCore<DATA_TYPE>::setDisallowedKeys( list );
            // cannotRepeatKeys = list;
        }

    //
    // ACTION
    //
    private:

        enum class repeatModeEnum : uint8_t {
            Idle,         // waiting for key
            SentFirstKey, // waiting for [repeatDelay] to go into Repeating
            Repeating,    // send every [repeatRate]
            WaitForKeyUp  // do not process anything until key is released
        };
        repeatModeEnum repeatMode = repeatModeEnum::Idle;

        unsigned long repeatLastActionTime;
        DATA_TYPE repeatTargetKey = -1;

    public:

        DATA_TYPE actionGetRepeatingKey( DATA_TYPE current ) {
            auto key = actionGetRepeatingKeyCore( current );
            return key;
            // if ( key == INACTIVE_KEY ) return INACTIVE_KEY;
            // return waitForKeyup( key );
        }

    private:

        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;

        DATA_TYPE actionGetRepeatingKeyCore( DATA_TYPE current ) {

            // (1) key released...
            if ( current == INACTIVE_KEY ) {
                repeatMode = repeatModeEnum::Idle;
                return current;
            }

            // (2) wait for key up but haven't got it, checked in (1)
            if ( repeatMode == repeatModeEnum::WaitForKeyUp )
                return INACTIVE_KEY;

            // (3) pressed another key
            if ( current != repeatTargetKey ) {
                DEBUG_TRACE( Serial.println( "REPEAT: different key" ) );
                repeatMode = repeatModeEnum::Idle;
            }

            switch( repeatMode ) {

            case repeatModeEnum::Idle:
                DEBUG_TRACE( Serial.print( "A" ) );
                DEBUG_TRACE( Serial.print( current ) );
                // DEBUG_TRACE( SerialPrintfln( "1st: %d", current ) );

                if ( !InputKeyFilterCore<DATA_TYPE>::isKeyAllowed(current) ) {
                    // send key, but do not repeat
                    repeatMode = repeatModeEnum::WaitForKeyUp;
                    // flagWaitForKeyup();
                } else {
                    repeatMode = repeatModeEnum::SentFirstKey;
                    repeatTargetKey = current;
                    repeatLastActionTime = millis();
                }
                return current;

            case repeatModeEnum::SentFirstKey: {

                    uint32_t now = millis();
                    if ( now - repeatLastActionTime >= repeatDelayInMs ) {
                        repeatMode = repeatModeEnum::Repeating;
                        repeatLastActionTime = now;
                        DEBUG_TRACE( Serial.print( "B" ) );
                        DEBUG_TRACE( Serial.print( current ) );
                        // DEBUG_TRACE( SerialPrintfln( "2nd: %d", current ) );
                        return current;
                    } else {
                        // still waiting for repeat delay to kick in
                        return INACTIVE_KEY;
                    }
                }
                // break;
                
            case repeatModeEnum::Repeating: {

                    uint32_t now = millis();
                    if ( now - repeatLastActionTime >= repeatRateInMs ) {
                        DEBUG_TRACE( Serial.print( "C" ) );
                        DEBUG_TRACE( Serial.print( current ) );
                        // DEBUG_TRACE( SerialPrintfln( "Nth: %d", current ) );
                        repeatLastActionTime = now;
                        return current;
                    } else {
                        // still waiting for next repeat cycle
                        return INACTIVE_KEY;
                    }
                }
                // break;

            case repeatModeEnum::WaitForKeyUp:
                // satisfy warning
                // return INACTIVE_KEY;
                break;

            }
            // ... just to remove warning
            // DEBUG_TRACE( Serial.println( "WTF ???" ) );
            return INACTIVE_KEY;
        }

    //
    // WAIT FOR KEYUP
    //
    public:

        inline void skipRepeatingCurrentKey() {
            // keep reporting inactive, until real inactive has been detected
            // eg. do not send that a button is pressed again
            //     until it is released and pressed again
            // ex. left arrow can be repeated
            //     but ENTER key cannot
            repeatMode = repeatModeEnum::WaitForKeyUp;
        }

    // //
    // // WAIT FOR KEYUP
    // //
    // private:

    //     bool waitForKeyupFlag = false;

    //     DATA_TYPE waitForKeyup( DATA_TYPE key ) {
    //         if ( !waitForKeyupFlag )
    //             return key;

    //         // note: consider only inactive states after debouncing, otherwise false inactive
    //         //       hence check only after all operations
    //         // eg. input states:  0001010101111111 
    //         //     debounce of 1:    ========        do not consider 0's here
    //         //     check for 0:              ======= begin checking for 0's here
    //         //     ignore 0's while still debouncing 1's
    //         if ( key == INACTIVE_KEY ) {
    //             DEBUG_TRACE( Serial.print( "got keyup: " ) );
    //             DEBUG_TRACE( Serial.println( key ) );
    //             waitForKeyupFlag = false;
    //             repeatMode = repeatModeEnum::Idle;
    //         } else {
    //             DEBUG_TRACE( Serial.print( "w" ) );
    //         }
    //         return INACTIVE_KEY;
    //     }

    // public:

    //     inline void flagWaitForKeyup() {
    //         // keep reporting inactive, until real inactive has been detected
    //         // eg. do not send that a button is pressed again
    //         //     until it is released and pressed again
    //         // ex. left arrow can be repeated
    //         //     but ENTER key cannot
    //         waitForKeyupFlag = true;
    //     }

        #undef DEBUG_TRACE

};

// template <typename T>
// struct InputRepeater<T>::Settings InputRepeater<T>::globalRepeaterSettings;

}

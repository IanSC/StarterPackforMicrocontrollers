// send repeating keys if button is kept pressed

#pragma once

#include <Arduino.h>
#include <inttypes.h>

namespace StarterPack {

class InputRepeater {

    private:

        typedef uint8_t KEY;
        static constexpr KEY INACTIVE_KEY = 0;

    // DERIVED CLASS
    //
    //      newMatrixKeypad::KEY readRepeated() {
    //          // read debounced value, then feed to repeater
    //          auto value = read();
    //          return InputRepeater::getRepeatingKey( value );
    //      }

    //
    // SETTINGS
    //
    public:

        struct Settings {
            // uint16_t activeStatesDebounceInMs = 50;     // debouncing keyDown time
            // uint16_t inactiveStateDebounceInMs = 50;    // debouncing keyUp time
            // uint16_t minimumDebounceTimeInMs = 50;      // minimum debounce time before allowed to be cancelled 
            // uint16_t confirmActiveStateTimeInMs = 0;    // time delay to confirm keyDown time
            // uint16_t confirmInactiveStateTimeInMs = 0;  // time delay to confirm keyUp time
            uint16_t repeatDelayInMs = 400;     // time before 1st repeat
            uint16_t repeatRateInMs  = 250;     // time between succeeding repeats
        };

        static Settings defaultRepeaterSettings;            // global single instance setting to save space

        Settings *repeaterSettings = &defaultRepeaterSettings;

        void assignRepeaterSettings( Settings &settings ) {
            // caller is still owner
            // and should handle freeing memory
            this->repeaterSettings = &settings;
        }

    //
    // ACTION
    //
    public:

        KEY inactiveValue = INACTIVE_KEY; // value that is considered not pressed

    protected:

        enum class repeatModeStates : uint8_t {
            Idle,
            SentFirstKey, // waiting for [repeatDelay] to go into Repeating
            Repeating     // send every [repeatRate]
        };
        repeatModeStates repeatMode = repeatModeStates::Idle;

        uint32_t lastRepeatActionTime;
        KEY keyBeingRepeated = -1;

    public:

        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;

        KEY actionGetRepeatingKey( KEY current ) {

            // key released...
            if ( current == inactiveValue ) {
                repeatMode = repeatModeStates::Idle;
                return current;
            }

            // pressed another key
            if ( current != keyBeingRepeated ) {
                DEBUG_TRACE( Serial.println( "REPEAT: different key" ) );
                repeatMode = repeatModeStates::Idle;
            }

            switch( repeatMode ) {

            case repeatModeStates::Idle:

                repeatMode = repeatModeStates::SentFirstKey;
                keyBeingRepeated = current;
                lastRepeatActionTime = millis();
                DEBUG_TRACE( Serial.print( "A" ) );
                DEBUG_TRACE( Serial.print( current ) );
                // DEBUG_TRACE( SerialPrintfln( "1st: %d", current ) );
                return current;

            case repeatModeStates::SentFirstKey: {

                    uint32_t now = millis();
                    if ( now - lastRepeatActionTime >= repeaterSettings->repeatDelayInMs ) {
                        repeatMode = repeatModeStates::Repeating;
                        lastRepeatActionTime = now;
                        DEBUG_TRACE( Serial.print( "B" ) );
                        DEBUG_TRACE( Serial.print( current ) );
                        // DEBUG_TRACE( SerialPrintfln( "2nd: %d", current ) );
                        return current;
                    } else {
                        // still waiting for repeat delay to kick in
                        return inactiveValue;
                    }
                }
                break;
                
            case repeatModeStates::Repeating: {

                    uint32_t now = millis();
                    if ( now - lastRepeatActionTime >= repeaterSettings->repeatRateInMs ) {
                        DEBUG_TRACE( Serial.print( "C" ) );
                        DEBUG_TRACE( Serial.print( current ) );
                        // DEBUG_TRACE( SerialPrintfln( "Nth: %d", current ) );
                        lastRepeatActionTime = now;
                        return current;
                    } else {
                        // still waiting for next repeat cycle
                        return inactiveValue;
                    }
                }
                break;

            }
            // ... just to remove warning
            DEBUG_TRACE( Serial.println( "WTF ???" ) );
            return inactiveValue;
        }

        #undef DEBUG_TRACE

};

InputRepeater::Settings InputRepeater::defaultRepeaterSettings;

}

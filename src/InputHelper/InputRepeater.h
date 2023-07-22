// send repeating keys if button is kept pressed

#pragma once

#include <Arduino.h>
#include <inttypes.h>

namespace StarterPack {

//
// SETTINGS
//

    struct InputRepeaterSettings {
        uint16_t repeatDelayInMs = 400;      // time before 1st repeat
        uint16_t repeatRateInMs  = 250;      // time between succeeding repeats
    };

    static struct InputRepeaterSettings globalInputRepeaterSettings;  // global single instance setting to save space

template<typename T>
class InputRepeater {

    private:

        typedef T KEY;
        static constexpr KEY INACTIVE_KEY = 0;

    //
    // SETTINGS
    //
    public:

        // struct Settings {
        //     uint16_t repeatDelayInMs = 400;      // time before 1st repeat
        //     uint16_t repeatRateInMs  = 250;      // time between succeeding repeats
        // };

        // static Settings globalRepeaterSettings;  // global single instance setting to save space

        // Settings *repeaterSettings = &globalRepeaterSettings;

        // create reference so user can go: obj.globalDebouncerSettings->stabilizedTimePressedInMs = 50;
        static constexpr InputRepeaterSettings *globalRepeaterSettings = &StarterPack::globalInputRepeaterSettings;

        // actual settings per instance, by default points to global
        InputRepeaterSettings *repeaterSettings = &StarterPack::globalInputRepeaterSettings;

        InputRepeaterSettings *createRepeaterSettings(uint16_t repeatDelayInMs=400, uint16_t repeatRateInMs=250) {
            // create and use a new settings, caller should handle freeing memory
            auto s = new InputRepeaterSettings();
            s->repeatDelayInMs = repeatDelayInMs;
            s->repeatRateInMs = repeatRateInMs;
            this->repeaterSettings = s;
            return s;
        }

        void assignRepeaterSettings( InputRepeaterSettings &settings ) {
            // caller is still owner and should handle freeing memory
            this->repeaterSettings = &settings;
        }

        void assignRepeaterSettings( InputRepeaterSettings *settings ) {
            // caller is still owner and should handle freeing memory
            this->repeaterSettings = settings;
        }

    //
    // ACTION
    //
    private:

        enum class repeatModeEnum : uint8_t {
            Idle,         // waiting for key
            SentFirstKey, // waiting for [repeatDelay] to go into Repeating
            Repeating     // send every [repeatRate]
        };
        repeatModeEnum repeatMode = repeatModeEnum::Idle;

        uint32_t repeatLastActionTime;
        KEY repeatTargetKey = -1;

    public:

        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;

        KEY actionGetRepeatingKey( KEY current ) {

            // key released...
            if ( current == INACTIVE_KEY ) {
                repeatMode = repeatModeEnum::Idle;
                return current;
            }

            // pressed another key
            if ( current != repeatTargetKey ) {
                DEBUG_TRACE( Serial.println( "REPEAT: different key" ) );
                repeatMode = repeatModeEnum::Idle;
            }

            switch( repeatMode ) {

            case repeatModeEnum::Idle:

                repeatMode = repeatModeEnum::SentFirstKey;
                repeatTargetKey = current;
                repeatLastActionTime = millis();
                DEBUG_TRACE( Serial.print( "A" ) );
                DEBUG_TRACE( Serial.print( current ) );
                // DEBUG_TRACE( SerialPrintfln( "1st: %d", current ) );
                return current;

            case repeatModeEnum::SentFirstKey: {

                    uint32_t now = millis();
                    if ( now - repeatLastActionTime >= repeaterSettings->repeatDelayInMs ) {
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
                break;
                
            case repeatModeEnum::Repeating: {

                    uint32_t now = millis();
                    if ( now - repeatLastActionTime >= repeaterSettings->repeatRateInMs ) {
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
                break;

            }
            // ... just to remove warning
            DEBUG_TRACE( Serial.println( "WTF ???" ) );
            return INACTIVE_KEY;
        }

        #undef DEBUG_TRACE

};

// template <typename T>
// struct InputRepeater<T>::Settings InputRepeater<T>::globalRepeaterSettings;

}

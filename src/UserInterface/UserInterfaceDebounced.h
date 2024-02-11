#pragma once

#include <stdint.h>

#include <InputHelper/InputDebouncer.h>

// add filter ??? just use 0 for key map
// #include <InputHelper/InputKeyFilter.h>

namespace StarterPack {

// class UserInterfaceDebounced : public InputDebouncer<uint8_t>
class UserInterfaceDebounced : public InputDebouncer<char>
                              //,public InputKeyFilter<uint8_t>
                              {

    public:

        // typedef uint8_t KEY;
        typedef char KEY;
        static constexpr KEY INACTIVE_KEY = 0;

    //
    // ???
    //
    public:

        void cancelDebouncing() {}

    //
    // VIRTUAL
    //
    public:

        virtual KEY getNonDebouncedKey() = 0;

    //
    // BASIC
    //
    public:

        KEY getDebouncedKey() {
            auto key = getNonDebouncedKey();
            // key = InputKeyFilter.actionFilterKey(key);
            return InputDebouncer::actionDebounce(key);
        }

        KEY getKeyDown() {
            auto key = getNonDebouncedKey();
            return InputDebouncer::actionGetKeyDown(key);
        }

        KEY getKeyUp() {
            auto key = getNonDebouncedKey();
            return InputDebouncer::actionGetKeyUp(key);
        }

    //
    // EXTENDED
    //
    public:

        bool isKeyPressed( KEY key ) {
            // return ( getNonDebouncedKey() == key );
            return ( getDebouncedKey() == key );
        }

        void waitUntilNothingIsPressed() {
            while ( true ) {
                if ( getNonDebouncedKey() == INACTIVE_KEY && getDebouncedKey() == INACTIVE_KEY )
                    return;
                // if ( getDebouncedKey() == INACTIVE_KEY )
                //     return;
                delay(10);
                // if ( getNonDebouncedKey() == INACTIVE_KEY )
                //     return;
            }
        }

        KEY waitForAnyKeyPressed() {
            // wait for key, don't check if released already
            waitUntilNothingIsPressed();
            // auto t = millis();
            while ( true ) {
                // if ( millis() - t > 1000 ) {
                //     Serial.print('+');
                //     t = millis();
                // }
                KEY key = getKeyDown();
                if ( key != INACTIVE_KEY ) return key;
                delay(10);
            }
        }

        KEY waitForAnyKeyReleased() {
            // wait for key, then wait for its release
            waitUntilNothingIsPressed();
            while ( true ) {
                KEY key = getKeyUp();
                if ( key != INACTIVE_KEY ) return key;
                delay(10);
            }
        }

};

}

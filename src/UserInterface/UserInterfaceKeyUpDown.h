#pragma once

#include <inttypes.h>

#include <InputHelper/InputDebouncer.h>

namespace StarterPack {

class UserInterfaceKeyUpDown : public InputDebouncer<uint8_t> {

    public:

        typedef uint8_t KEY;
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

    // //
    // // DEBOUNCER SETTINGS
    // //
    // public:

    //     static inline InputDebouncer::Settings getGlobalDebouncerSettings() {
    //         return InputDebouncer<KEY>::globalDebouncerSettings;
    //     }

    //     inline InputDebouncer<uint8_t>::Settings *createDebouncerSettings() {
    //         auto s = new InputDebouncer<uint8_t>::Settings();
    //         InputDebouncer::assignDebouncerSettings(s);
    //         return s;
    //     }

    //
    // BASIC
    //
    public:

        // bool first = true;

        KEY getDebouncedKey() {
            auto key = getNonDebouncedKey();
            // auto db = getDebouncer()->debounce(key);
            // if (first) {
            //     first = false;
            //     Serial.print(key);
            //     Serial.print('.');
            //     Serial.println(db);
            // }
            // return db;
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
            return ( getNonDebouncedKey() == key );
        }

        void waitUntilNothingIsPressed() {
            while ( true ) {
                if ( getNonDebouncedKey() == INACTIVE_KEY )
                    return;
            }
        }

        KEY waitForAnyKeyPressed() {
            // wait for key, don't check if released already
            waitUntilNothingIsPressed();
            while ( true ) {
                KEY key = getKeyDown();
                if ( key != INACTIVE_KEY ) return key;
            }
        }

        KEY waitForAnyKeyReleased() {
            // wait for key, then wait for its release
            waitUntilNothingIsPressed();
            while ( true ) {
                KEY key = getKeyUp();
                if ( key != INACTIVE_KEY ) return key;
            }
        }

};

}

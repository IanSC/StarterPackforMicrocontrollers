#pragma once

#include <stdint.h>

#include <InputHelper/InputKeySource.h>
#include <spWDT.h>

// add filter ??? just use 0 for key map
// #include <InputHelper/InputKeyFilter.h>

namespace StarterPack {

// virtual ancestor: https://stackoverflow.com/questions/1878055/c-common-ancestor-problem-what-is-it

// class UserInterfaceDebounced : public InputDebouncer<uint8_t>
class UserInterfaceBasic : virtual public InputKeySource
                              //,public InputKeyFilter<uint8_t>
                              {

    // public:

    //     // typedef uint8_t KEY;
    //     typedef InputKeySource::KEY KEY;
    //     static constexpr KEY INACTIVE_KEY = InputKeySource::INACTIVE_KEY;

    //
    // BASIC
    //
    public:

        // virtual KEY getDebouncedKey() = 0;

        // KEY getDebouncedKey() {
        //     auto key = getNonDebouncedKey();
        //     // key = InputKeyFilter.actionFilterKey(key);
        //     return InputDebouncer::actionDebounce(key);
        // }

        // KEY getKeyDown() {
        //     return getNonDebouncedKey();
        // }

        // KEY getKeyUp() {
        //     return getNonDebouncedKey();
        // }

        // void clearBuffers() {
        //     InputKeySource::clearBuffers();
        // }

    //
    // EXTENDED
    //
    public:

        bool isKeyPressed( KEY key ) {
            // return ( getNonDebouncedKey() == key );
            clearBuffers();
            return ( getStableKey() == key );
        }

        void waitUntilNothingIsPressed() {
            clearBuffers();
            while ( true ) {
                if ( getNonDebouncedKey() == INACTIVE_KEY && getStableKey() == INACTIVE_KEY )
                    return;
                // if ( getDebouncedKey() == INACTIVE_KEY )
                //     return;
                feedTheDog();
                // delay( 10 );
                // if ( getNonDebouncedKey() == INACTIVE_KEY )
                //     return;
            }
        }

        KEY waitForAnyKeyPressed() {
            // wait for key, don't check if released already
            waitUntilNothingIsPressed();
            while ( true ) {
                KEY key = getStableKey();
                if ( key != INACTIVE_KEY ) return key;
                feedTheDog();
            }
        }

        KEY waitForAnyKeyReleased() {
            // wait for key, then wait for its release
            KEY key = waitForAnyKeyPressed();
            waitUntilNothingIsPressed();
            return key;
            // KEY key;
            // while ( true ) {
            //     KEY key = getStableKey();
            //     if ( key != INACTIVE_KEY ) return key;
            //     feedTheDog();
            //     // delay( 10 );
            // }
        }

};

}

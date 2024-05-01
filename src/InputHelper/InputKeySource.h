#pragma once

// #include <stdint.h>

namespace StarterPack {

class InputKeySource {

    public:

        // typedef uint8_t KEY;
        typedef char KEY;
        static constexpr KEY INACTIVE_KEY = 0;

    //
    // VIRTUAL
    //
    public:

        virtual KEY getNonDebouncedKey() = 0;
        virtual KEY getStableKey() = 0;

        // PROBLEM:  uart input is buffered
        //           user sends bunch of keys
        //           when app is ready to check, outdated keystrokes will be processed
        // POSSIBLE: just read last, might miss quick strokes
        // SOLUTION: add clear buffers
        virtual void clearBuffers() = 0;

        // PROBLEM:  some input has debounce and holding period
        //           but after processing must stop the holding period immediately
        //           to get the latest state
        // SOLUTION: add function to clear it, even if there it is not applicable to some
        // NOTE: not sure if even needed but it was used a lot in "air-motion" ???
        virtual void clearDebouncedState() = 0;

};

}

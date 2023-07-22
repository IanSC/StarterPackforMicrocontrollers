#pragma once

#include <inttypes.h>

// #include <Arduino.h>
// #include <stdint.h>
// #include <stdarg.h>
// #include <string.h>

namespace StarterPack { namespace Bit {

    template <typename TType>
    // uint8_t countBits( TType input ) {
    uint8_t countOnes( TType input ) {
        // count number of "1" bits
        uint8_t bits = 0;
        while( input != 0 ) {
            if ( (input & 1) == 1 )
                bits++;
            input >>= 1;
        }
        return bits;
    }

    uint8_t findFirstActiveBit( uint32_t input ) {
    // uint8_t first1BitOccurence( uint32_t input ) {
        // find first bit from LSB to MSB that is turned on
        // ex. f( 0010 ) ==> 2      2nd bit is on
        //     f( 1100 ) ==> 3      3rd bit is on, first one found
        if ( input == 0 ) return 0;
        #if defined(__GNUC__)
            // https://stackoverflow.com/a/31393298
            // https://stackoverflow.com/questions/28166565/detect-gcc-as-opposed-to-msvc-clang-with-macro
            // https://www.geeksforgeeks.org/builtin-functions-gcc-compiler/
            return __builtin_ctzl( input ) + 1;
        #else
            uint8_t r = 1;
            while( ( input & 1 ) == 0 ) {
                input >>= 1;
                r++;
            }
            return r;
        #endif
    }

}}

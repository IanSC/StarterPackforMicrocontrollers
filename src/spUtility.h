#pragma once
#include <stdarg.h>

namespace StarterPack {

//
// MAP CONVERTERS
//

    // template <typename TType>
    // TType spMapCore( TType value, TType inputMin, TType inputMax, TType outputMin, TType outputMax ) {
    //     // from WMath.cpp - but fixed
    //     const TType run = inputMax - inputMin;
    //     if( divisor == 0 ) {
    //         log_e( "spMap(): invalid input range, min == max" );
    //         return -1; //AVR returns -1, SAM returns 0
    //     }
    //     const TType rise = outputMax - outputMin;
    //     const TType delta = value - inputMin;
    //     return ( delta * rise ) / run + outputMin;
    // }

    int spMap( int value, int inputMin, int inputMax, int outputMin, int outputMax ) {
        int run = inputMax - inputMin;
        if ( run == 0 ) return -1;
        return ( (long)(value - inputMin) * (long)(outputMax - outputMin) ) / run + outputMin;
    }

    long spMapLong( long value, long inputMin, long inputMax, long outputMin, long outputMax ) {
        int run = inputMax - inputMin;
        if ( run == 0 ) return -1;
        return ( (long long)(value - inputMin) * (long long)(outputMax - outputMin) ) / run + outputMin;
    }

    inline float spMapFloat( float value, float inputMin, float inputMax, float outputMin, float outputMax ) {
        float run = inputMax - inputMin;
        if ( run == 0 ) return -1;
        return ( (value - inputMin) * (outputMax - outputMin) ) / run + outputMin;
    }

    template <typename TType>
    TType spLimit( TType value, TType min, TType max ) {
        if ( value < min ) return min;
        if ( value > max ) return max;
        return value;
    }

//
// SERIAL
//

    void SerialPrintCharsN( char ch, uint8_t count ) {
        while( count-- > 0 )
            Serial.print( ch );
    }

    void SerialPrintf( const char *format, ... ) {
        const uint8_t maxChars = 100;
        char buffer[maxChars+1];
        va_list args;
        va_start( args, format );
        vsnprintf(buffer, maxChars, format, args);
        va_end( args );
        Serial.print( buffer );
    }

    void SerialPrintStrN( const char *str, uint8_t N ) {
        // print first N characters of string
        while( N > 0 && *str != 0 ) {
            Serial.print( *str++ );
            N--;
        }
    }

//
// STRING
//

    bool isCharInString( uint8_t key, char *list ) {
        if ( list == nullptr ) return false;
        return ( strchr( list, key ) != nullptr );
    }

//
// BITS
//

    template <typename TType>
    uint8_t countBits( TType input ) {
        // count number of "1" bits
        uint8_t bits = 0;
        while( input != 0 ) {
            if ( (input & 1) == 1 )
                bits++;
            input >>= 1;
        }
        return bits;
    }

    uint8_t first1BitOccurence( uint32_t input ) {
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

}

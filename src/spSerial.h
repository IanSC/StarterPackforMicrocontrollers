#pragma once

#include <Arduino.h>
// #include <stdint.h>
// #include <stdarg.h>
// #include <string.h>

namespace StarterPack {

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

    void SerialPrintfln( const char *format, ... ) {
        // how to pass to SerialPrintf() without template
        // since it does not work in Arduino
        const uint8_t maxChars = 100;
        char buffer[maxChars+1];
        va_list args;
        va_start( args, format );
        vsnprintf(buffer, maxChars, format, args);
        va_end( args );
        Serial.println( buffer );
    }

    void SerialPrintStrN( const char *str, uint8_t N ) {
        // print first N characters of string
        while( N > 0 && *str != 0 ) {
            Serial.print( *str++ );
            N--;
        }
    }

    void SerialPrintBinary(uint8_t value, char trueValue='1', char falseValue='0') {
        auto bitCheck = B10000000;
        for (int bit=0 ; bit<8 ; bit++) {
            if ((value & bitCheck) == 0)
                Serial.print(falseValue);
            else
                Serial.print(trueValue);
            bitCheck = bitCheck >> 1;
        }
    }

    void SerialPrintBinary(uint16_t value, char trueValue='1', char falseValue='0') {
        auto bitCheck = B10000000;
        for (int bit=0 ; bit<16 ; bit++) {
            if ((value & bitCheck) == 0)
                Serial.print(falseValue);
            else
                Serial.print(trueValue);
            bitCheck = bitCheck >> 1;
        }
    }

    void SerialPrintlnBinary(uint8_t value, char trueValue='1', char falseValue='0') {
        SerialPrintBinary(value, trueValue, falseValue);
        Serial.println();
    }

    void SerialPrintlnBinary(uint16_t value, char trueValue='1', char falseValue='0') {
        SerialPrintBinary(value, trueValue, falseValue);
        Serial.println();
    }

}

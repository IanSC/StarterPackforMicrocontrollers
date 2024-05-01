#pragma once

#include <Arduino.h>
// #include <stdint.h>
// #include <stdarg.h>
// #include <string.h>

namespace StarterPack {

//
// BASIC
//

    void SerialPrintCharsN( char ch, uint8_t repeatCount ) {
        while( repeatCount-- > 0 )
            Serial.print( ch );
    }

    void SerialPrintStrN( const char *str, uint8_t lengthToPrint ) {
        // print first N characters of string
        while( lengthToPrint > 0 && *str != 0 ) {
            Serial.print( *str++ );
            lengthToPrint--;
        }
    }

//
// PRINTF
//

    namespace _INTERNAL {
        void SerialPrintfCore( const char *format, va_list args ) {
            // https://stackoverflow.com/questions/3530771/passing-variable-arguments-to-another-function-that-accepts-a-variable-argument
            // how to pass to SerialPrintf() without template
            // since it does not work in Arduino
            const uint8_t maxChars = 100;
            char buffer[maxChars];
            vsnprintf( buffer, maxChars, format, args );
            Serial.print( buffer );
        }
    }

    void SerialPrintf( const char *format, ... ) {
        va_list args;
        va_start( args, format );
        _INTERNAL::SerialPrintfCore( format, args );
        va_end( args );
        // TO CALL vsnprintf() DIRECTLY:
        // void func( const char *format, va_list args ):
        //      const uint8_t maxChars = 100;
        //      char buffer[maxChars+1];
        //      va_list args;
        //      va_start( args, format );
        //      vsnprintf(buffer, maxChars, format, args);
        //      va_end( args );
    }

    void SerialPrintfln( const char *format, ... ) {
        // https://stackoverflow.com/questions/3530771/passing-variable-arguments-to-another-function-that-accepts-a-variable-argument
        va_list args;
        va_start( args, format );
        _INTERNAL::SerialPrintfCore( format, args );
        va_end( args );
        Serial.println();
    }

//
// PADDED
//

    namespace _INTERNAL {
        void SerialPrintPaddedCore( uint8_t length, bool rightPadding, const char *format, va_list args ) {
            const uint8_t maxChars = 100;
            char buffer[maxChars];
            vsnprintf( buffer, maxChars, format, args );

            // https://stackoverflow.com/questions/276827/string-padding-in-c
            // max = %-255s
            char formatBuffer[7] = "%-\0\0\0\0";  // left = %10s / right = %-10s
            char *ptr = formatBuffer + 1;         // skip '-' sign
            if ( rightPadding ) ptr++;            // include '-' sign        
            snprintf( ptr, 4, "%d", length );
            strcat( formatBuffer, "s" );
            SerialPrintf( formatBuffer, buffer );
        }
    }

    void SerialPrintPadLeft( uint8_t length, const char *format, ... ) {
        va_list args;
        va_start( args, format );
        _INTERNAL::SerialPrintPaddedCore( length, false, format, args );
        va_end( args );
    }

    void SerialPrintPadRight( uint8_t length, const char *format, ... ) {
        va_list args;
        va_start( args, format );
        _INTERNAL::SerialPrintPaddedCore( length, true, format, args );
        va_end( args );
    }

//
// NUMERIC
//

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

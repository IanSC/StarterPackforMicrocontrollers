#pragma once

namespace StarterPack {

template <typename TType>
TType spMap( TType value, TType inputMin, TType inputMax, TType outputMin, TType outputMax ) {
    // from WMath.cpp
    const TType divisor = inputMax - inputMin;
    if( divisor == 0 ) {
        log_e( "spMap(): invalid input range, min == max" );
        return -1; //AVR returns -1, SAM returns 0
    }
    const TType dividend = outputMax - outputMin;
    const TType delta = value - inputMin;
    return ( delta * dividend + (divisor / 2) ) / divisor + outputMin;
}

template <typename TType>
TType spLimit( TType value, TType min, TType max ) {
    if ( value < min ) return min;
    if ( value > max ) return max;
    return value;
}

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

}

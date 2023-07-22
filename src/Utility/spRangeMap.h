#pragma once

// #include <Arduino.h>
// #include <stdint.h>
// #include <stdarg.h>
// #include <string.h>

namespace StarterPack { namespace RangeMap {

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

    template<typename T, typename U>
    void Limit(T& value, U min, U max) {
        if (min <= max) {
            if (value<min) value=min;
            if (value>max) value=max;
        } else {
            if (value<max) value=max;
            if (value>min) value=min;
        }
    }

    // int spMap( int value, int inputMin, int inputMax, int outputMin, int outputMax ) {
    int IntInt( int value, int inputMin, int inputMax, int outputMin, int outputMax ) {
        int run = inputMax - inputMin;
        if ( run == 0 ) return -1;
        auto r = ( (long)(value - inputMin) * (long)(outputMax - outputMin) ) / run + outputMin;
        Limit(r, outputMin, outputMax);
        return r;
    }

    // long spMapLong( long value, long inputMin, long inputMax, long outputMin, long outputMax ) {
    long LongLong( long value, long inputMin, long inputMax, long outputMin, long outputMax ) {
        int run = inputMax - inputMin;
        if ( run == 0 ) return -1;
        auto r = ( (long long)(value - inputMin) * (long long)(outputMax - outputMin) ) / run + outputMin;
        Limit(r, outputMin, outputMax);
        return r;
    }

    inline float IntFloat( int value, int inputMin, int inputMax, int outputMin, int outputMax ) {
    // inline float spMapFloat( int value, int inputMin, int inputMax, int outputMin, int outputMax ) {
        float run = inputMax - inputMin;
        if ( run == 0 ) return -1;
        auto r = (float) ( (value - inputMin) * (outputMax - outputMin) ) / run + outputMin;
        Limit(r, outputMin, outputMax);
        return r;
    }

    // inline float spMapFloat( float value, float inputMin, float inputMax, float outputMin, float outputMax ) {
    inline float FloatFloat( float value, float inputMin, float inputMax, float outputMin, float outputMax ) {
        float run = inputMax - inputMin;
        if ( run == 0 ) return -1;
        auto r = ( (value - inputMin) * (outputMax - outputMin) ) / run + outputMin;
        Limit(r, outputMin, outputMax);
        return r;
    }

    // template <typename TType>
    // TType spLimit( TType value, TType min, TType max ) {
    //     if ( value < min ) return min;
    //     if ( value > max ) return max;
    //     return value;
    // }

}}
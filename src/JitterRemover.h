#pragma once
#include <stdint.h>
#include <Arduino.h>

// #define DEBUG_DIAGNOSE

class JitterRemover {

    public:

        ~JitterRemover() {
            destroyJitter();
        }

    protected:

        typedef int16_t valueType;    // make it same as AnalogIO
        // static const int16_t valueTypeMin = (-INT_MAX-1);
        // static const int16_t valueTypeMax =  INT_MAX;
        // valueType min = std::numeric_limits<int16_t>::max();
        // valueType max = std::numeric_limits<int16_t>::min();
        static const int16_t valueTypeMin = std::numeric_limits<valueType>::max();
        static const int16_t valueTypeMax = std::numeric_limits<valueType>::min();

        // bool jitterEnabled = false; // just test jBucket
        uint8_t jMaxSlots = 9;        // max number of slots
        valueType 
        //float
        *jBucket = nullptr; // storage
        int8_t jSlot = 0;             // current slot
        uint32_t jSum = 0;            // sum of stored entries
        bool jFillingMode = true;     // storage is filled, old values are to be removed already
        uint8_t jRangeTolerance;      // new value must exceed from the average to be considered relevant

        inline void destroyJitter() {
            if ( jBucket != nullptr ) {
                delete[] jBucket;
                jBucket = nullptr;
            }
        }

    public:

        void init( int maxSlots, int rangeTolerance ) {
            destroyJitter();
            jMaxSlots = maxSlots;
            jRangeTolerance = rangeTolerance;
            // jBucket = new float[maxSlots];
            jBucket = new valueType[maxSlots];
            // jitterEnabled = true;
        }

        inline void disableJitterRemoval() {
            // jitterEnabled = false;
            destroyJitter();
        }

        valueType removeJitter( valueType value ) {
            //  bucket   0   1   2   3
            //  1        +               add new entry
            //  2            +           add new entry
            //  3                +       add new entry
            //  4                    +   add new entry, fillingMode = false
            //  5        -               remove previous entry
            //           +               add new entry

            auto slotChanged = jSlot;

            if ( !jFillingMode ) {
                // remove last added enty
                jSum -= jBucket[jSlot];
            }
            jBucket[jSlot] = value;
            jSlot++;
            if ( jSlot >= jMaxSlots ) {
                // reached 3, reset to 0
                jSlot = 0;
                #if defined( DEBUG_DIAGNOSE )
                    if ( jFillingMode ) {
                        Serial.println( "STORAGE FILLED" );
                        delay( 1000 );
                    }
                #endif
                jFillingMode = false;
            }
            jSum += value;

            // at this point captured last X entries
            // get average of all entries
            // if difference vs new entry is close
            //    use average
            // ex. jitter 20 and 21, ... average will be 20.5
            //     if new value is 20 or 21, return (int) 20.5 = which is consistent
            float average;
            average = (float) jSum / (float) jMaxSlots;

            /*
            int result;
            if ( average == value) {
                #if defined( DEBUG_DIAGNOSE )
                    Serial.print( "= " );
                #endif
                result = value;
            } else if ( abs( average - (float) value ) < jRangeTolerance ) {
                #if defined( DEBUG_DIAGNOSE )
                    Serial.print( "X " );
                #endif
                // result = floor( average );
                result = round( average + 0.5 );
                // result = round( average );
                //jBucket[slotChanged] = average;
            } else {
                #if defined( DEBUG_DIAGNOSE )
                    Serial.print( "~ " );
                #endif
                result = value;
            }
            */

            int result;
            if ( average == value ) {
                // #if defined( DEBUG_DIAGNOSE )
                //     Serial.print( "= " );
                // #endif
                result = value;
            } else {
                //  condition: 50, 51, 50, 51, average 50.5
                //  actual   average      use
                //  50       50.5 below   50
                //  51       50.5 below   50
                //  50       50.5 above   51
                //  51       50.5 above   51                
                float diff = abs( average - (float) value );
                if ( diff < jRangeTolerance && !jFillingMode ) {
                    // near average, find min/max
                    valueType min = valueTypeMin; //std::numeric_limits<int16_t>::max();
                    valueType max = valueTypeMax; //std::numeric_limits<int16_t>::min();
                    for( int i = 0 ; i < jMaxSlots ; i++ ) {
                        if ( min > jBucket[i] ) min = jBucket[i];
                        if ( max < jBucket[i] ) max = jBucket[i];
                    }
                    // Serial.print( (long) valueTypeMax );
                    // Serial.print( " to " );
                    // Serial.println( (long) valueTypeMin );
                    // Serial.print( min );
                    // Serial.print( " to " );
                    // Serial.println( max );
                    float diffToMin = abs( average - min );
                    float diffToMax = abs( max - average );
                    // Serial.print( diffToMin );
                    // Serial.print( " to " );
                    // Serial.println( diffToMax );
                    if ( diffToMin == diffToMax )
                        result = ( min + max ) / 2;
                    else if ( diffToMin < diffToMax )
                        result = min;
                    else
                        result = max;
                    #if defined( DEBUG_DIAGNOSE )
                        Serial.print( "D " );
                    #endif
                } else {
                    #if defined( DEBUG_DIAGNOSE )
                        Serial.print( "= " );
                    #endif
                    result = value;
                }
            }
            // if ( abs( average - (float) value ) < jRangeTolerance ) {
            //     #if defined( DEBUG_DIAGNOSE )
            //         Serial.print( "X " );
            //     #endif
            //     // result = floor( average );
            //     result = round( average + 0.5 );
            //     // result = round( average );
            //     //jBucket[slotChanged] = average;
            // } else {
            //     #if defined( DEBUG_DIAGNOSE )
            //         Serial.print( "~ " );
            //     #endif
            //     result = value;
            // }

            #if defined( DEBUG_DIAGNOSE )
                Serial.print( jCounter );
                Serial.print( " : " );
                Serial.print( average );
                Serial.print( " : " );
                Serial.print( value );
                Serial.print( " : " );
                Serial.print( result );
                Serial.println();
                jCounter++;
            #endif

            return result;
        }

    public:

        #if defined( DEBUG_DIAGNOSE )
            uint32_t jCounter = 0;
            void testJitter() {
                delay( 1000 );
                Serial.println();
                Serial.println( "***** TEST Jitter 31/32 *****" );
                jCounter = 0;
                delay( 3000 );
                for( int i = 0 ; i < 300 ; i++ ) {
                    removeJitter( 50 ); delay(100);
                    removeJitter( 51 ); delay(100);
                }
                Serial.println();
                Serial.println( "***** TEST Jitter 31/31/32/32 *****" );
                delay( 3000 );
                jCounter = 0;
                for( int i = 0 ; i < 20 ; i++ ) {
                    removeJitter( 31 ); delay(100);
                    removeJitter( 31 ); delay(100);
                    removeJitter( 32 ); delay(100);
                    removeJitter( 32 ); delay(100);
                }
                Serial.println();
                Serial.println( "***** TEST Jitter 99/100 then all 99 *****" );
                delay( 3000 );
                jCounter = 0;
                for( int i = 0 ; i < 30 ; i++ ) {
                    removeJitter( 99 );
                    removeJitter( 100 );
                }
                for( int i = 0 ; i < 30 ; i++ ) {
                    removeJitter( 99 );
                    delay(100);
                }
                Serial.println();
                Serial.println( "***** TEST Jitter 99/100 then all 109 *****" );
                delay( 3000 );
                jCounter = 0;
                for( int i = 0 ; i < 30 ; i++ ) {
                    removeJitter( 99 );
                    removeJitter( 100 );
                }
                for( int i = 0 ; i < 30 ; i++ ) {
                    removeJitter( 100 );
                    delay(100);
                }
                Serial.println();
                delay( 3000 );
            }
        #endif
        
};

#if defined( DEBUG_DIAGNOSE )
    #undef DEBUG_DIAGNOSE
#endif

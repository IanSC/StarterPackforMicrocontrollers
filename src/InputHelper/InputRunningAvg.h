#pragma once

#include <Arduino.h>
#include <stdint.h>

#include <Utility/spMacros.h>

#include <InputHelper/InputFilterInterface.h>

// #define DEBUG_DIAGNOSE

namespace StarterPack {

template<typename DATA_TYPE, typename SUM_DATA_TYPE>
class InputRunningAvg : public InputFilterInterface<DATA_TYPE> {

    // SUM_DATA_TYPE - typename when adding DATA_TYPE's
    //                 eg. use int32_t to accumulate int16_t data

    // private:
    //
    //     typedef T DATA_TYPE;

    //
    // FILTER BASE
    //
    public:
        inline DATA_TYPE actionApplyFilter( DATA_TYPE value ) override {
            return actionComputeRunningAvg(value);
        }

    public:

        // InputRunningAvg() {
        //     // must call: setRunningAvgSlots()
        // }

        InputRunningAvg(uint8_t storageSlots, DATA_TYPE fluctuationRange=0) {
            setRunningAvgSlots(storageSlots);
            setFluctuationFilter(fluctuationRange);
        }

        virtual ~InputRunningAvg() {
            deleteStorage();
        }

    //
    // SETTINGS
    //
    private:

        // bool jitterEnabled = false;  // just test jBucket

        uint8_t    maxSlots = 0;          // number of storage slots
        DATA_TYPE *storage = nullptr;     // storage
        int8_t     currentSlot = 0;       // current storage slot
        bool       fillingMode = true;    // storage is still being filled, do not disaacard old values yet

        inline void deleteStorage() {
            if ( storage != nullptr ) {
                delete[] storage;
                storage = nullptr;
            }
            maxSlots = 0;
            currentSlot = 0;
            fillingMode = true;
            storageSum = 0;
        }

    //
    // SUM
    //
    private:

        // cannot simply use same datatype for sum
        // ex. for adc:
        //    8-bits,  too small
        //    16-bits, 4096 x 10 = 40000, too small
        // cannot just use 64-bits, might be too slow

        // just require user to provide it
        SUM_DATA_TYPE storageSum = 0;

        /*
        // ARDUINO: NOT WORKING
        // sum of stored entries
        // https://stackoverflow.com/questions/8243134/c-templates-select-different-type-based-on-value-of-template-parameter
        // if 8 or 16 bits, use 32 bit sum
        // otherwise use 64
        typedef typename std::conditional<
               std::is_same<DATA_TYPE,uint8_t>::value
            || std::is_same<DATA_TYPE, int8_t>::value
            || std::is_same<DATA_TYPE,uint16_t>::value
            || std::is_same<DATA_TYPE, int16_t>::value,
                int32_t
            ,
                int64_t
            >::type data_t;
        data_t   storageSum = 0;
        */

        // NOT WORKING:
        // #if DATATYPE==uint8_t || DATATYPE==uint16_t
        //     uint32_t storageSum;

    public:

        void setRunningAvgSlots( uint8_t storageSlots ) {
            deleteStorage();
            if (storageSlots < 2) {
                // 0 or 1 slot is useless anyway
                return;
            }
            maxSlots = storageSlots;
            storage = new DATA_TYPE[maxSlots];
            memset(storage,0,sizeof(DATA_TYPE)*maxSlots);
            // currentSlot = 0;
            // storageSum = 0;
            // fillingMode = true;
            // storageSum = 0;
        }

        inline void disableRunningAvg() {
            // jitterEnabled = false;
            deleteStorage();
        }

    //
    // ACTION
    //
    public:

        DATA_TYPE actionComputeRunningAvg( DATA_TYPE value ) {

            //  bucket   0   1   2   3
            //  1        +               add new entry
            //  2            +           add new entry
            //  3                +       add new entry
            //  4                    +   add new entry, fillingMode = false
            //  5        -               remove previous entry
            //           +               replace with new entry

            // auto slotChanged = rAvgCurrentSlot;

            if (storage == nullptr) return value;

            if ( !fillingMode ) {
                // SUM: remove last added entry
                storageSum -= storage[currentSlot];
            }
            // SUM: add new value
            storage[currentSlot] = value;
            storageSum += value;

            currentSlot++;
            if ( currentSlot >= maxSlots ) {
                // reached end, reset to 0
                currentSlot = 0;
                #if defined( DEBUG_DIAGNOSE )
                    if ( fillingMode ) {
                        Serial.println( "STORAGE FILLED" );
                        // delay( 1000 );
                    }
                #endif
                // rollover, fully filled
                fillingMode = false;
            }

            // at this point captured last X entries
            // get average of all entries
            // if difference vs new entry is close
            //    use average
            // ex. jitter 20 and 21, ... average will be 20.5
            //     if new value is 20 or 21, return (int) 20.5 = which is consistent

            float average;
            if (fillingMode) {
                // still filling, average over available data only
                average = (float) storageSum / currentSlot;
            } else {
                average = (float) storageSum / maxSlots;
            }

            #if defined(DEBUG_DIAGNOSE)
                int result;
                if ( average == value) {
                    #if defined( DEBUG_DIAGNOSE )
                        Serial.print( "= " );
                    #endif
                    result = value;
                } else if ( abs( average - (float) value ) < fluctuationRange ) {
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
            #endif

            // ADDITIONAL LOGIC:
            if (fluctuationRange != 0)
                average = removeFluctuation(average, value);

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

            return average;
        }

    //
    // RANGE TOLERANCE
    //    
    public:

        void setFluctuationFilter(DATA_TYPE fluctuationRange) {
            this->fluctuationRange = fluctuationRange;
        }

    private:

        // to apply fluctuation removal,
        // [newValue] must be within certain range from average
        DATA_TYPE fluctuationRange = 0;

        static constexpr DATA_TYPE MIN = SP_NUMLIMITS_MIN_OF(DATA_TYPE);
        static constexpr DATA_TYPE MAX = SP_NUMLIMITS_MAX_OF(DATA_TYPE);

        DATA_TYPE removeFluctuation(DATA_TYPE average, DATA_TYPE value) {

            // if signal alternates between 2 numbers, average will also fluctuate
            //    signal: 50, 51, 50, 51, average 50.5
            //    value   average      result
            //    50      below 50.5   50
            //    51      above 50.5   51
            // to handle it:
            //    cannot use floor/ceiling, if floor cannot get max, if ceiling cannot get min
            // so use extra logic to remove these fluctuations
            //    but only if the new value is within the average
            //    that tolerance is [fluctuationRange]

            // static const DATA_TYPE valueTypeMin = std::numeric_limits<DATA_TYPE>::max();
            // static const DATA_TYPE valueTypeMax = std::numeric_limits<DATA_TYPE>::min();

            DATA_TYPE result;
            if ( average == value ) {
                // #if defined( DEBUG_DIAGNOSE )
                //     Serial.print( "= " );
                // #endif
                result = value;
            } else {
                
                // find difference between new value and average
                float diff = abs( average - (float) value );

                if ( diff < fluctuationRange && !fillingMode ) {
                    // near average, find min/max

                    // find min/max within all samples
                    // DATA_TYPE min = valueTypeMin; //std::numeric_limits<int16_t>::max();
                    // DATA_TYPE max = valueTypeMax; //std::numeric_limits<int16_t>::min();

                    // ARDUINO: NOT WORKING
                    // DATA_TYPE findMin = std::numeric_limits<DATA_TYPE>::max();
                    // DATA_TYPE findMax = std::numeric_limits<DATA_TYPE>::min();
                    DATA_TYPE findMin = MAX;
                    DATA_TYPE findMax = MIN;

                    for( int i = 0 ; i < maxSlots ; i++ ) {
                        if ( findMin > storage[i] ) findMin = storage[i];
                        if ( findMax < storage[i] ) findMax = storage[i];
                    }
                    // Serial.print( (long) valueTypeMax );
                    // Serial.print( " to " );
                    // Serial.println( (long) valueTypeMin );
                    // Serial.print( "min/max: " );
                    // Serial.print( min );
                    // Serial.print( " to " );
                    // Serial.println( max );

                    // find diff of new value to min/max found
                    float diffToMin = abs( average - findMin );
                    float diffToMax = abs( findMax - average );
                    // Serial.print( "diff: " );
                    // Serial.print( diffToMin );
                    // Serial.print( " to " );
                    // Serial.println( diffToMax );

                    if ( diffToMin == diffToMax )
                        // result = round( (float) ( min + max ) / 2.0 );
                        result = average;
                    else if ( diffToMin < diffToMax )
                        // closer to min, use min
                        result = findMin;
                    else
                        // closer to max, use max
                        result = findMax;
                    #if defined( DEBUG_DIAGNOSE )
                        Serial.print( "D " );
                    #endif
                } else {
                    #if defined( DEBUG_DIAGNOSE )
                        Serial.print( "O " );
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

            return result;
        }

    public:

        #if defined( DEBUG_DIAGNOSE )
            uint32_t jCounter = 0;
            void TEST_runningAvg() {
                delay( 1000 );
                Serial.println();
                Serial.println( "***** TEST Jitter 31/32 *****" );
                jCounter = 0;
                delay( 3000 );
                for( int i = 0 ; i < 300 ; i++ ) {
                    actionComputeRunningAvg( 50 ); delay(100);
                    actionComputeRunningAvg( 51 ); delay(100);
                }
                Serial.println();
                Serial.println( "***** TEST Jitter 31/31/32/32 *****" );
                delay( 3000 );
                jCounter = 0;
                for( int i = 0 ; i < 20 ; i++ ) {
                    actionComputeRunningAvg( 31 ); delay(100);
                    actionComputeRunningAvg( 31 ); delay(100);
                    actionComputeRunningAvg( 32 ); delay(100);
                    actionComputeRunningAvg( 32 ); delay(100);
                }
                Serial.println();
                Serial.println( "***** TEST Jitter 99/100 then all 99 *****" );
                delay( 3000 );
                jCounter = 0;
                for( int i = 0 ; i < 30 ; i++ ) {
                    actionComputeRunningAvg( 99 );
                    actionComputeRunningAvg( 100 );
                }
                for( int i = 0 ; i < 30 ; i++ ) {
                    actionComputeRunningAvg( 99 );
                    delay(100);
                }
                Serial.println();
                Serial.println( "***** TEST Jitter 99/100 then all 100 *****" );
                delay( 3000 );
                jCounter = 0;
                for( int i = 0 ; i < 30 ; i++ ) {
                    actionComputeRunningAvg( 99 );
                    actionComputeRunningAvg( 100 );
                }
                for( int i = 0 ; i < 30 ; i++ ) {
                    actionComputeRunningAvg( 100 );
                    delay(100);
                }
                Serial.println();
                delay( 3000 );
            }
        #endif
        
};

}

#undef DEBUG_DIAGNOSE

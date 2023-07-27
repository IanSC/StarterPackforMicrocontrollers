//  Chain multiple InputRunningAvg to get better reults.
//
//  If signal is fluctuating fast, shorter slot will be able to handle it.
//  eg. got 50,51,50,51,50,51 ...
//             |<--50.5-->|<--50.5-->|
//
//  If signal is fluctuating slowly, a short slot won't be able to average it.
//  eg. got 50,50,50,50,51,51,51,51, ...
//             |<--50-->|<--51-->|
//
//  By chaining effectively getting [slot1*slot2], without too much memory.

#pragma once

#include <Arduino.h>
#include <stdint.h>
// #include <iostream>

#include <InputHelper/InputRunningAvg.h>
#include <InputHelper/InputFilterInterface.h>

namespace StarterPack {

template<typename DATA_TYPE, typename SUM_DATA_TYPE>
class InputRunningAvgChained : public InputFilterInterface<DATA_TYPE> {

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

        InputRunningAvgChained() {
            // call: setRunningAvgSlotsN( count, ... );
        }
        
        virtual ~InputRunningAvgChained() {
            delete[] filters;
        }

        // user convenience, don't know how to count number of args without so much extra code
        // https://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments/2124385#2124385
        // 1 to 10 entries
        InputRunningAvgChained( uint8_t r1 ) { setRunningAvgSlotsN( 1, r1 ); }
        InputRunningAvgChained( uint8_t r1, uint8_t r2 ) { setRunningAvgSlotsN( 2, r1, r2 ); }
        InputRunningAvgChained( uint8_t r1, uint8_t r2, uint8_t r3 ) { setRunningAvgSlotsN( 3, r1, r2, r3 ); }
        InputRunningAvgChained( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4 ) { setRunningAvgSlotsN( 4, r1, r2, r3, r4 ); }
        InputRunningAvgChained( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5 ) { setRunningAvgSlotsN( 5, r1, r2, r3, r4, r5 ); }
        InputRunningAvgChained( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6 ) { setRunningAvgSlotsN( 6, r1, r2, r3, r4, r5, r6 ); }
        InputRunningAvgChained( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6, uint8_t r7 ) { setRunningAvgSlotsN( 7, r1, r2, r3, r4, r5, r6, r7 ); }
        InputRunningAvgChained( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6, uint8_t r7, uint8_t r8 ) { setRunningAvgSlotsN( 8, r1, r2, r3, r4, r5, r6, r7, r8 ); }
        InputRunningAvgChained( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6, uint8_t r7, uint8_t r8, uint8_t r9 ) { setRunningAvgSlotsN( 9, r1, r2, r3, r4, r5, r6, r7, r8, r9 ); }
        InputRunningAvgChained( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6, uint8_t r7, uint8_t r8, uint8_t r9, uint8_t r10 ) { setRunningAvgSlotsN( 10, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10 ); }

    //
    // SETTINGS
    //
    private:

        InputRunningAvg<DATA_TYPE,SUM_DATA_TYPE> *filters = nullptr;
        uint8_t filterCount;

    public:

        void setRunningAvgSlotsN( uint8_t argCount, ... ) {
            // will cause va_arg() to give wrong result:
            //    assignRowsN( uint8_t argCount, int ... )
            filterCount = argCount;
            if ( filters != nullptr )
                delete[] filters;
            filters = new InputRunningAvg<DATA_TYPE,SUM_DATA_TYPE>[filterCount];

            va_list valist;
            va_start( valist, argCount );
            for ( uint8_t i = 0 ; i < filterCount; i++ ) {
                // must use int
                // https://www.avrfreaks.net/forum/variadic-function-question-uint8t-vs-uint16t
                filters[i].setRunningAvgSlots( va_arg( valist, int ) );
            }
            va_end( valist );
        }

        void setSlots( uint8_t slotNumber, uint8_t maxSlots ) {
            // change filter number slotNumber to maxSlots
            if (slotNumber >= filterCount) return;
            filters[slotNumber].setRunningAvgSlots(maxSlots);;
        }

        inline void disableRunningAvg(uint8_t slotNumber) {
            if (slotNumber >= filterCount) return;
            filters[slotNumber].disableRunningAvg();
        }

        void setFluctuationFilter(uint8_t slotNumber, DATA_TYPE fluctuationRange) {
            if (slotNumber >= filterCount) return;
            filters[slotNumber].setFluctuationFilter(fluctuationRange);            
        }

    //
    // ACTION
    //
    public:

        DATA_TYPE actionComputeRunningAvg( DATA_TYPE value ) {
            for (int i = 0; i < filterCount; i++) {
                value = filters[i].actionComputeRunningAvg(value);
            }
            return value;
        }

};

}

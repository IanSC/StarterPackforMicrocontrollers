//  2023/07 - isc
//
//  Analog Signal Slotter - map ADC value to slot number
//
//      initSlots( zeroValue, value1, value2, ..., value10, bool useGuard=true )
//          zeroValue     analog read value if nothing is pressed
//          value1        value for slot 1
//          ... value10   value for succeeding buttons
//                        max 10 slots
//          useGuard      last parameter, true to add dead-zone between data range
//          
//      initSlotsN( bool useGuard, entryCount, zeroValue, value1, ..., valueN )
//          use this if more than 10 slots
//          entryCount   number of entries (zeroValue should be included in count)
//          
//          ex. aIO.initButtons (           1022, 834, 642, 14, 228, 430, false ); // up to 10 slots, including zeroValue, 11 entries
//              aIO.initButtonsN( false, 6, 1022, 834, 642, 14, 228, 430 );        // 2nd param is number of total entries
//
//      uint8_t  getSlotCount()                 query number of slots assigned
//      int      getSlotValue( slotNo )         query value for specified slot
//
//      To determine values, run this sketch and press each button, record the values.
//          AnalogIO aIO = AnalogIO( A0 );
//          void setup() { Serial.begin( 9600 ); }
//          void loop() {
//              Serial.println( aIO.read() );
//              delay( 50 );
//          }
//      To use, run this sketch with result from above:
//          AnalogIO aIO = AnalogIO( A0 );
//          void setup() {
//              Serial.begin( 9600 );
//              // LCD Keypad Shield
//              aIO.initSlots( 1022, 834, 642, 14, 228, 430 );
//              // 1022   value if nothing is pressed, must be 1st entry   0 - result from readButton()
//              // 834    select button                                    1
//              // 642    left button                                      2
//              // 14     right button                                     3
//              // 228    up button                                        4
//              // 430    down button                                      5
//          }
//          void loop() {
//              Serial.println( aIO.readButton() );
//              delay( 50 );
//          }

#pragma once

#include <limits.h>
#include <inttypes.h>
#include <stdarg.h>

namespace StarterPack {

class InputSlotter {
    
    private:

        typedef uint8_t KEY;
        static const KEY INACTIVE_KEY = 0;

    //
    // INIT
    //
    protected:
    
        class slotRange { public:
            int from;
            int to;
            int value;
            KEY slot;
        };
        slotRange *slotRangeList = nullptr;
        uint8_t slotCount;

    public:

        ~InputSlotter() {
            if ( slotRangeList != nullptr )
                delete[] slotRangeList;
        }

        inline uint8_t getSlotCount() {
            return slotCount;
        }

        int getSlotValue( uint8_t slotNo ) {
            if ( slotNo < 0 || slotNo >= slotCount )
                return -1;
            return slotRangeList[slotNo].value;
        }

        // user convenience, don't know how to count number of args without so much extra code
        // https://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments/2124385#2124385
        // 1 to 10 buttons
        // zero - value if not pressed
        inline void initSlots( int zero, int v1, bool useGuard=true ) { initSlotsN( useGuard, 2, zero, v1 ); }
        inline void initSlots( int zero, int v1, int v2, bool useGuard=true ) { initSlotsN( useGuard, 3, zero, v1, v2 ); }
        inline void initSlots( int zero, int v1, int v2, int v3, bool useGuard=true ) { initSlotsN( useGuard, 4, zero, v1, v2, v3 ); }
        inline void initSlots( int zero, int v1, int v2, int v3, int v4, bool useGuard=true ) { initSlotsN( useGuard, 5, zero, v1, v2, v3, v4 ); }
        inline void initSlots( int zero, int v1, int v2, int v3, int v4, int v5, bool useGuard=true ) { initSlotsN( useGuard, 6, zero, v1, v2, v3, v4, v5 ); }
        inline void initSlots( int zero, int v1, int v2, int v3, int v4, int v5, int v6, bool useGuard=true ) { initSlotsN( useGuard, 7, zero, v1, v2, v3, v4, v5, v6 ); }
        inline void initSlots( int zero, int v1, int v2, int v3, int v4, int v5, int v6, int v7, bool useGuard=true ) { initSlotsN( useGuard, 8, zero, v1, v2, v3, v4, v5, v6, v7 ); }
        inline void initSlots( int zero, int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, bool useGuard=true ) { initSlotsN( useGuard, 9, zero, v1, v2, v3, v4, v5, v6, v7, v8 ); }
        inline void initSlots( int zero, int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, bool useGuard=true ) { initSlotsN( useGuard, 10, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9 ); }
        inline void initSlots( int zero, int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, int v10, bool useGuard=true ) { initSlotsN( useGuard, 11, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10 ); }

        // specify number of entries on 2nd argument
        // including value if nothing is pressed
        void initSlotsN( bool useGuard, int argCount, ... ) {
            // 2st arg is number of slots

            this->slotCount = argCount;
            if ( slotRangeList != nullptr )
                delete[] slotRangeList;
            slotRangeList = new slotRange[slotCount];

            va_list valist;
            va_start( valist, argCount );
            for ( int i = 0 ; i < slotCount; i++ ) {
                slotRangeList[i].value = va_arg( valist, int );
                slotRangeList[i].slot = i;
            }
            va_end( valist );

            // sort list: https://stackoverflow.com/questions/33067907/simple-program-to-sort-numbers-in-c
            for( int i = 0 ; i < slotCount-1 ; i++ ) {
                for( int k = 0 ; k < slotCount-1-i ; k++ ) {
                    if( slotRangeList[k].value > slotRangeList[k+1].value ) {
                        int temp = slotRangeList[k].value;
                        slotRangeList[k].value = slotRangeList[k+1].value;
                        slotRangeList[k+1].value = temp;
                        temp = slotRangeList[k].slot;
                        slotRangeList[k].slot = slotRangeList[k+1].slot;
                        slotRangeList[k+1].slot = temp;
                    }
                }
            }

            // GUARD: divide distance between target values by 3
            // 1/3 for previous value, 1/3 dead range, 1/3 for next value
            // ex. values     range
            //       0        -inf to 33
            //     100        66 to 133
            //     200        166 to +inf
            int divisor = useGuard ? 3 : 2;
            
            for ( int i = 0 ; i < slotCount; i++ ) {
                if ( i == 0 )
                    slotRangeList[0].from = INT_MIN;
                else
                    slotRangeList[i].from = slotRangeList[i].value - (slotRangeList[i].value-slotRangeList[i-1].value)/divisor;
                if ( i < slotCount-1 )
                    slotRangeList[i].to = slotRangeList[i].value + (slotRangeList[i+1].value-slotRangeList[i].value)/divisor;
                else
                    slotRangeList[i].to = INT_MAX;
            }

            // DEBUG
            // for ( int i = 0 ; i < slotCount; i++ )
            //     Serial.printf( "%d = [ %d ] = %d [ %d --> %d ]\n", i, slotRangeList[i].slot, slotRangeList[i].value, slotRangeList[i].from, slotRangeList[i].to );

            // // do initial read, otherwise debouncer will give wrong value 1st time
            // debouncer.setInitialValue( readMappedKey() );

            // // by default was set to false (assume = 0) already
            // //buttonDebouncer.inactiveState = inactiveButton;
        }

        // // do initial read, otherwise debouncer will give wrong value 1st time
        // debouncer.setInitialValue( readMappedKey() );

    //
    // ACTION
    //
    public:

        KEY actionFindSlot( int rawValue ) {
            for ( int i = 0 ; i < slotCount; i++ ) {
                if ( slotRangeList[i].from <= rawValue && rawValue <= slotRangeList[i].to ) {
                    return slotRangeList[i].slot;
                }
            }
            return INACTIVE_KEY;
        }

};

}

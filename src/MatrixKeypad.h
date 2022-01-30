#pragma once
#include <spUtility.h>
#include <spBitPackedBoolean.h>
using namespace StarterPack;

#include <stdint.h>
#include <stdarg.h>

#include <Arduino.h>

namespace StarterPack {

class MatrixKeypad {

        uint8_t rowCount = 0; uint8_t *rowPinList = nullptr;
        uint8_t colCount = 0; uint8_t *colPinList = nullptr;
        bool rowIsOutput = true;
        uint8_t outPinCount = 0; uint8_t *outPinList = nullptr;
        uint8_t inPinCount = 0; uint8_t *inPinList = nullptr;
        spBitPackedBoolean *bits = nullptr;

    public:

        ~MatrixKeypad() {
            if ( rowPinList != nullptr ) delete rowPinList;
            if ( colPinList != nullptr ) delete colPinList;
            if ( bits != nullptr ) delete bits;
            if ( scanCodeList != nullptr ) delete scanCodeList;
            if ( keyIdList != nullptr ) delete keyIdList;
        }

        // user convenience, don't know how to count number of args without so much extra code
        // https://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments/2124385#2124385
        // 1 to 10 pins
        inline void assignRows( uint8_t r1 ) { assignRowsN( 1, r1 ); }
        inline void assignRows( uint8_t r1, uint8_t r2 ) { assignRowsN( 2, r1, r2 ); }
        inline void assignRows( uint8_t r1, uint8_t r2, uint8_t r3 ) { assignRowsN( 3, r1, r2, r3 ); }
        inline void assignRows( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4 ) { assignRowsN( 4, r1, r2, r3, r4 ); }
        inline void assignRows( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5 ) { assignRowsN( 5, r1, r2, r3, r4, r5 ); }
        inline void assignRows( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6 ) { assignRowsN( 6, r1, r2, r3, r4, r5, r6 ); }
        inline void assignRows( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6, uint8_t r7 ) { assignRowsN( 7, r1, r2, r3, r4, r5, r6, r7 ); }
        inline void assignRows( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6, uint8_t r7, uint8_t r8 ) { assignRowsN( 8, r1, r2, r3, r4, r5, r6, r7, r8 ); }
        inline void assignRows( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6, uint8_t r7, uint8_t r8, uint8_t r9 ) { assignRowsN( 9, r1, r2, r3, r4, r5, r6, r7, r8, r9 ); }
        inline void assignRows( uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6, uint8_t r7, uint8_t r8, uint8_t r9, uint8_t r10 ) { assignRowsN( 10, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10 ); }

        void assignRowsN( uint8_t argCount, ... ) {
            // will cause va_arg() to give wrong result:
            //    assignRowsN( uint8_t argCount, int ... )
            rowCount = argCount;
            if ( rowPinList != nullptr )
                delete rowPinList;
            rowPinList = new uint8_t[rowCount];

            va_list valist;
            va_start( valist, argCount );
            for ( uint8_t i = 0 ; i < rowCount; i++ ) {
                // must use int
                // https://www.avrfreaks.net/forum/variadic-function-question-uint8t-vs-uint16t
                rowPinList[i] = va_arg( valist, int );
            }
            va_end( valist );
        }

        // 1 to 10 pins
        inline void assignColumns( uint8_t c1 ) { assignColumnsN( 1, c1 ); }
        inline void assignColumns( uint8_t c1, uint8_t c2 ) { assignColumnsN( 2, c1, c2 ); }
        inline void assignColumns( uint8_t c1, uint8_t c2, uint8_t c3 ) { assignColumnsN( 3, c1, c2, c3 ); }
        inline void assignColumns( uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4 ) { assignColumnsN( 4, c1, c2, c3, c4 ); }
        inline void assignColumns( uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5 ) { assignColumnsN( 5, c1, c2, c3, c4, c5 ); }
        inline void assignColumns( uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6 ) { assignColumnsN( 6, c1, c2, c3, c4, c5, c6 ); }
        inline void assignColumns( uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7 ) { assignColumnsN( 7, c1, c2, c3, c4, c5, c6, c7 ); }
        inline void assignColumns( uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7, uint8_t c8 ) { assignColumnsN( 8, c1, c2, c3, c4, c5, c6, c7, c8 ); }
        inline void assignColumns( uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7, uint8_t c8, uint8_t c9 ) { assignColumnsN( 9, c1, c2, c3, c4, c5, c6, c7, c8, c9 ); }
        inline void assignColumns( uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7, uint8_t c8, uint8_t c9, uint8_t c10 ) { assignColumnsN( 10, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10 ); }

        void assignColumnsN( uint8_t argCount, ... ) {
            colCount = argCount;
            if ( colPinList != nullptr )
                delete colPinList;
            colPinList = new uint8_t[colCount];

            va_list valist;
            va_start( valist, argCount );
            for ( uint8_t i = 0 ; i < colCount; i++ )
                colPinList[i] = va_arg( valist, int );
            va_end( valist );
        }

        inline void setOutputPinActive( uint8_t pin ) {
            pinMode( pin, OUTPUT );
            digitalWrite( pin, LOW );
        }

        inline void setOutputPinStandy( uint8_t pin ) {
            pinMode( pin, INPUT );
        }

        void begin() {

            // Debouncer::defaultSettings.activeStatesDebounceInMs = 1000;

            bits = new spBitPackedBoolean(); // rowCount * colCount );
            rowIsOutput = ( rowCount >= colCount );
            if ( rowIsOutput ) {
                outPinCount = rowCount; outPinList = rowPinList;
                inPinCount = colCount; inPinList = colPinList;
            } else {
                inPinCount = rowCount; inPinList = rowPinList;
                outPinCount = colCount; outPinList = colPinList;
            }
            for ( uint8_t i = 0 ; i < outPinCount; i++ )
                setOutputPinStandy( outPinList[i] );
            for ( uint8_t i = 0 ; i < inPinCount; i++ )
                pinMode( inPinList[i], INPUT_PULLUP );
        }

    void info() {
        Serial.print( "ROW = " );
        Serial.println( rowCount );
        for( int i = 0 ; i < rowCount ; i++ ) {
            Serial.println( rowPinList[i] );
        }
        Serial.print( "COL = " );
        Serial.println( colCount );
        for( int i = 0 ; i < colCount ; i++ ) {
            Serial.println( colPinList[i] );
        }
    }

    Debouncer myDb;

    uint8_t getContinuousKey() {
        return myDb.getContinuousKey( getKeymap( readRaw() ) );
    }

    uint8_t getKeyDown() {
        return myDb.getKeyDown( getKeymap( readRaw() ) );
    }

    uint8_t getKeyUp() {
        return myDb.getKeyUp( getKeymap( readRaw() ) );
    }

    uint8_t getRepeatingKey() {
        return myDb.getRepeatingKey( getKeymap( readRaw() ) );
    }

    uint32_t readRaw() {
        bits->reset();
        if ( outPinCount == 0 || inPinCount == 0 )
            return;
        readHelper( 0, outPinCount - 1, 0 );
        return bits->data;
        for( int i = 0 ; i < bits->slots ; i++ ) {
            if ( bits->get( i ) )
                Serial.print( "1" );
            else
                Serial.print( "0" );
        }
        Serial.println();
    }

    void readHelper( uint8_t from, uint8_t to, uint8_t step ) {
        bool active = IsAnyPressed( from, to );
        if ( active ) {
            if ( from == to ) {
                setOutputPinsStandy( from, to );
            } else {
                // divide to halfs
                uint8_t mid = ( to + from ) / 2;
                // put upper half on standy and read lower half
                setOutputPinsStandy( mid+1, to );
                readHelper( from, mid, step+1 );
                setOutputPinsStandy( from, mid );
                // read upper half
                readHelper( mid+1, to, step+1 );
                setOutputPinsStandy( mid+1, to );
            }
        } else {
            setOutputPinsStandy( from, to );
        }
    }

    void setOutputPinsStandy( uint8_t from, uint8_t to ) {
        // check just in case
        if ( from > to ) return;
        // SerialPrintf( "   output standy: %d-%d\n", from, to );
        for ( ; from <= to; from++ )
            setOutputPinStandy( outPinList[from] );
    }

    inline bool IsAnyPressed( uint8_t from, uint8_t to ) {
        // SerialPrintf( "   output active: %d-%d\n", from, to );
        for ( int i = from ; i <= to ; i++ )
            setOutputPinActive( outPinList[i] );
        // SerialPrintf( "   read inputs: %d-%d\n", 0, colCount );
        if ( from == to ) {
            // single output being checked, record results
            if ( rowIsOutput ) {
                for ( int i = 0 ; i < inPinCount ; i++ ) {
                    uint8_t scanCode = from * inPinCount + i;
                    bool state = digitalRead( inPinList[i] ) == LOW;
                    if ( state )
                        state = debounce( scanCode, state );
                    else
                        state = debounceExisting( scanCode, state );
                    if ( state ) bits->turnOn( scanCode );
                    // if ( digitalRead( inPinList[i] ) == LOW ) {
                    //     if ( debounce( keyId, true ) )
                    //         bits->turnOn( keyId );
                    //     // SerialPrintf( "   ON  = %d-%d\n", outPinList[from], inPinList[i] );
                    // } else {
                    //     debounceExisting( keyId, false );
                    //     // SerialPrintf( "   OFF = %d-%d\n", outPinList[from], inPinList[i] );
                    // }
                }
            } else {
                for ( int i = 0 ; i < inPinCount ; i++ ) {
                    uint8_t scanCode = i * outPinCount + from;
                    bool state = digitalRead( inPinList[i] ) == LOW;
                    if ( state )
                        state = debounce( scanCode, state );
                    else
                        state = debounceExisting( scanCode, state );
                    if ( state ) bits->turnOn( scanCode );
                    // if ( digitalRead( inPinList[i] ) == LOW ) {
                    //     if ( debounce( keyId, true ) )
                    //         bits->turnOn( keyId );
                    //     // SerialPrintf( "   ON  = %d-%d\n", inPinList[i], outPinList[from] );
                    // } else {
                    //     debounceExisting( keyId, false );
                    //     // SerialPrintf( "   OFF = %d-%d\n", inPinList[i], outPinList[from] );
                    // }
                }
            }
        } else {
            // multiple outputs being checked, if any is pressed
            // return immediately on 1st positive
            for ( int i = 0 ; i < inPinCount ; i++ ) {
                if ( digitalRead( inPinList[i] ) == LOW )
                    return true;
            }
            // debounce out from-to, in all pins
            if ( rowIsOutput ) {
                for ( int sendPin = from ; sendPin <= to ; sendPin++ ) {
                    for ( int recvPin = 0 ; recvPin <= inPinCount ; recvPin++ ) {
                        uint8_t scanCode = sendPin * inPinCount + recvPin;
                        bool state = debounceExisting( scanCode, false );
                        if ( state ) bits->turnOn( scanCode );
                    }
                }
            } else {
                for ( int sendPin = from ; sendPin <= to ; sendPin++ ) {
                    for ( int recvPin = 0 ; recvPin <= inPinCount ; recvPin++ ) {
                        uint8_t scanCode = recvPin * outPinCount + sendPin;
                        bool state = debounceExisting( scanCode, false );
                        if ( state ) bits->turnOn( scanCode );
                    }
                }
            }
        }

        return false;
    }

    //
    // KEYMAP
    //

    uint8_t keymapIndex = 0;
    uint8_t keyCount = 0;
    uint8_t *scanCodeList = nullptr;
    uint16_t *keyIdList = nullptr;

    void initKeymap( uint8_t keyCount ) {
        if ( scanCodeList != nullptr ) delete scanCodeList;
        if ( keyIdList != nullptr ) delete keyIdList;
        this->keyCount = keyCount;
        scanCodeList = new uint8_t[keyCount];
        keyIdList = new uint16_t[keyCount];
    }

    void addKeymap( uint8_t scanCode, uint16_t keyId ) {
        if ( keymapIndex >= keyCount ) return;
        scanCodeList[keymapIndex] = scanCode;
        keyIdList[keymapIndex] = keyId;
        keymapIndex++;
    }

    uint16_t getKeymap( const uint32_t &keyBitmap ) {
        if ( keyBitmap == 0 ) return 0;
        if ( countBits( keyBitmap ) > 1 ) return 0;
        // https://stackoverflow.com/a/31393298
        uint8_t bitPosition = __builtin_ctzl( keyBitmap );
        for ( int i = 0 ; i < keyCount ; i++ ) {
            if ( scanCodeList[i] == bitPosition ) {
                return keyIdList[i];
            }
        }
        return bitPosition + 1;
    }

    //
    // DUPLICATE KEY
    //

    // specify keys that can be duplicated
    // or the keymap of those keys

    //
    // DEBOUNCE
    //

    //  use 1 debouncer per scanCode:
    //  - too much memory
    //  - send to each debouncer
    //  - need to OR a lot of keys
    //  use fixed number debouncers rotated to those pressed
    //  - can't use this for musical keyboard ?

    static const uint8_t DEBOUNCER_COUNT = 3;
    Debouncer db[DEBOUNCER_COUNT];
    uint8_t dbScanCode[DEBOUNCER_COUNT];
    uint8_t dbPos = 0;

    bool debounce( uint8_t scanCode, bool state ) {
        for( int i = 0 ; i < DEBOUNCER_COUNT ; i++ ) {
            if ( dbScanCode[i] == scanCode ) {
                // bool xx =  db[i].debounce( state );
                // if ( key == 2 ) {
                //     Serial.print( "db1=" );
                //     // Serial.print( key );
                //     // Serial.print( "-" );
                //     Serial.println( xx );
                // }
                // return xx;
                return db[i].debounce( state );
            }
        }
        // not found
        uint8_t pos = dbPos;
        dbPos++;
        dbScanCode[pos] = scanCode;
        db[pos].setInitialValue( !state );
        return db[pos].debounce( state );
    }

    bool debounceExisting( uint8_t scanCode, bool state ) {
        for( int i = 0 ; i < DEBOUNCER_COUNT ; i++ ) {
            if ( dbScanCode[i] == scanCode ) {
                // bool xx = db[i].debounce( state );
                // if ( key == 2 ) {
                //     Serial.print( "db2=" );
                //     // Serial.print( key );
                //     // Serial.print( "-" );
                //     Serial.println( xx );
                // }
                // return xx;
                return db[i].debounce( state );
            }
            // if ( dbKey[i] == key )
            //     return db[i].debounce( state );
        }
        return state;
    }

};

}

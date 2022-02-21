#pragma once
#include <stdint.h>
#include <stdarg.h>
#include <Arduino.h>

#include <Debouncer.h>

namespace StarterPack {

class MatrixKeypadCore {

    // MAX 32 BUTTONS because of spBitPackedBoolean

    protected:

        uint8_t rowCount = 0; uint8_t *rowPinList = nullptr;
        uint8_t colCount = 0; uint8_t *colPinList = nullptr;
        bool sendViaRows = true;
        uint8_t sendPinCount = 0; uint8_t *sendPinList = nullptr;
        uint8_t recvPinCount = 0; uint8_t *recvPinList = nullptr;

        // bool driveHighScanning = true;
        uint8_t activeState = LOW;

    //
    // INIT
    //
    public:

        MatrixKeypadCore() {
            initMatrixDebouncers();
        }

        ~MatrixKeypadCore() {
            if ( rowPinList != nullptr ) delete rowPinList;
            if ( colPinList != nullptr ) delete colPinList;
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

        void begin( bool driveLowScanning = true, bool driveColumnsIfMorePins = true ) {
            // this->driveHighScanning = driveHighScanning;
            // default do driving low since Arduino only has PULLUP
            // if using ESP32 input only pins for receivers, must drive high
            if ( driveLowScanning )
                activeState = LOW;
            else
                activeState = HIGH;
            if ( driveColumnsIfMorePins && colCount > rowCount ) {
                sendPinCount = colCount; sendPinList = colPinList;
                recvPinCount = rowCount; recvPinList = rowPinList;
                sendViaRows = false;
            } else {
                sendPinCount = rowCount; sendPinList = rowPinList;
                recvPinCount = colCount; recvPinList = colPinList;
                sendViaRows = true;
            }
            setOutputPinsStandby( 0, sendPinCount - 1 );
            if ( driveLowScanning ) {
                // will set to LOW the send pins when scanning
                // pullup HIGH the receive pins
                for ( uint8_t i = 0 ; i < recvPinCount; i++ )
                    pinMode( recvPinList[i], INPUT_PULLUP );
            } else {
                // will set to HIGH the send pins when scanning
                // pulldown LOW the receive pins (if possible)
                // if no internal pulldown, must put externally
                //    otherwise it will be floating and would not work
                #if defined(ESP32)
                    for ( uint8_t i = 0 ; i < recvPinCount; i++ )
                        pinMode( recvPinList[i], INPUT_PULLDOWN );
                #else
                    for ( uint8_t i = 0 ; i < recvPinCount; i++ )
                        pinMode( recvPinList[i], INPUT );
                #endif
            }
        }

        void serialDebug() {
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

    //
    // READ DEVICE
    //
    protected:

        virtual void recordScanCode( uint8_t scanCode ) = 0;

        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;
        DEBUG_TRACE( uint8_t step = 3 );

        void readMatrixCore( uint8_t sendFrom, uint8_t sendTo ) {
            setOutputPinsActive( sendFrom, sendTo );
            bool active = scanInputs( sendFrom, sendTo );
            if ( active ) {
                DEBUG_TRACE( SerialPrintCharsN( ' ', step ) );
                DEBUG_TRACE( Serial.println( "ACTIVE" ) );
                if ( sendFrom == sendTo ) {
                    // already saved
                    setOutputPinsStandby( sendFrom, sendTo );
                } else {
                    // divide into halfs
                    uint8_t mid = ( sendTo + sendFrom ) / 2;
                    
                    DEBUG_TRACE( step += 3 );

                    // read lower half, put upper half on standy first
                    setOutputPinsStandby( mid+1, sendTo );
                    if ( sendFrom == mid ) {
                        scanInputs( sendFrom, mid );
                        setOutputPinsStandby( sendFrom, mid );
                    } else
                        readMatrixCore( sendFrom, mid );

                    // read upper half
                    if ( mid+1 == sendTo ) {
                        setOutputPinsActive( mid+1, sendTo );
                        scanInputs( mid+1, sendTo );
                        setOutputPinsStandby( mid+1, sendTo );
                    } else
                        readMatrixCore( mid+1, sendTo );
                    DEBUG_TRACE( step -= 3 );
                }
            } else {
                DEBUG_TRACE( SerialPrintCharsN( ' ', step ) );
                DEBUG_TRACE( Serial.println( "INACTIVE" ) );
                setOutputPinsStandby( sendFrom, sendTo );
            }
        }

        void setOutputPinsStandby( uint8_t sendFrom, uint8_t sendTo ) {
            // if ( sendFrom > sendTo ) return; // check just in case
            DEBUG_TRACE( SerialPrintCharsN( ' ', step ) );
            DEBUG_TRACE( SerialPrintf( "standby: %d-%d\n", sendFrom, sendTo ) );
            for ( ; sendFrom <= sendTo; sendFrom++ ) {
                pinMode( sendPinList[sendFrom], INPUT );
                digitalWrite( sendPinList[sendFrom], !activeState );
            }
        }

        void setOutputPinsActive( uint8_t sendFrom, uint8_t sendTo ) {
            // if ( sendFrom > sendTo ) return; // check just in case
            DEBUG_TRACE( SerialPrintCharsN( ' ', step ) );
            DEBUG_TRACE( SerialPrintf( "active: %d-%d\n", sendFrom, sendTo ) );
            for ( int sendPin = sendFrom ; sendPin <= sendTo ; sendPin++ ) {
                uint8_t p = sendPinList[sendPin];
                pinMode( p, OUTPUT );
                digitalWrite( p, activeState );
            }
        }

        bool scanInputs( uint8_t sendFrom, uint8_t sendTo ) {
            DEBUG_TRACE( SerialPrintCharsN( ' ', step ) );
            DEBUG_TRACE( Serial.println( "scanning" ) );
            if ( sendFrom == sendTo ) {
                // single send slot being checked, record results
                if ( sendViaRows ) {
                    for ( int recvPin = 0 ; recvPin < recvPinCount ; recvPin++ ) {
                        uint8_t scanCode = sendFrom * recvPinCount + recvPin;
                        bool state = ( digitalRead( recvPinList[recvPin] ) == activeState );
                        if ( state )
                            state = debounceMatrix( scanCode, state );
                        else
                            state = debounceMatrixExisting( scanCode, state );
                        if ( state ) {
                            DEBUG_TRACE( SerialPrintCharsN( ' ', step ) );
                            DEBUG_TRACE( SerialPrintf( "SET %d\n", scanCode ) );
                            recordScanCode( scanCode );
                        }
                    }
                } else {
                    for ( int recvPin = 0 ; recvPin < recvPinCount ; recvPin++ ) {
                        uint8_t scanCode = recvPin * sendPinCount + sendFrom;
                        bool state = ( digitalRead( recvPinList[recvPin] ) == activeState );
                        if ( state )
                            state = debounceMatrix( scanCode, state );
                        else
                            state = debounceMatrixExisting( scanCode, state );
                        if ( state ) recordScanCode( scanCode );
                    }
                }
            } else {
                // multiple send slot being checked, if any is pressed
                // return immediately for detailed checking
                for ( int recvPin = 0 ; recvPin < recvPinCount ; recvPin++ ) {
                    if ( digitalRead( recvPinList[recvPin] ) == activeState )
                        return true;
                }
            }
            return false;
        }

        #undef DEBUG_TRACE

    //
    // MATRIX DEBOUNCE
    //
    public:

        void setPhysicalButtonDebounceTimeInMs( uint16_t activeState = 50, uint16_t inactiveState = 50, uint16_t minimum = 50 ) {
            Debouncer::Settings *s = db[0].getSettings();
            s->activeStatesDebounceInMs = activeState;
            s->inactiveStateDebounceInMs = inactiveState;
            s->minimumDebounceTimeInMs = minimum;
        }

    protected:

        //  use 1 debouncer per scanCode:
        //  - too much memory
        //  use fixed number debouncers rotated to those pressed
        //  - can't use this for musical keyboard ?

        static const uint8_t DEBOUNCER_COUNT = 3;
        Debouncer db[DEBOUNCER_COUNT];
        uint8_t dbScanCode[DEBOUNCER_COUNT] = { 0xFF, 0xFF, 0xFF }; 
        uint8_t dbPos = 0;

        inline void initMatrixDebouncers() {
            // create custom settings for db[0]
            // then assign to all
            db[0].useCustomSettings();
            for( int i = 1 ; i < DEBOUNCER_COUNT ; i++ )
                db[i].assignSettings( *db[0].getSettings() );
        }

        bool debounceMatrix( uint8_t scanCode, bool state ) {
            for( int i = 0 ; i < DEBOUNCER_COUNT ; i++ ) {
                if ( dbScanCode[i] == scanCode ) {
                    
                    // if ( scanCode == 6 ) {
                    //     // StarterPack::Debouncer::modes modeB4 = db[i].mode;
                    //     int stateB4 = db[i].debouncedState;
                    //     // bool keyupFlagB4 = db[i].waitForKeyupFlag;
                    //     bool stateAfter = db[i].debounce( state );
                    //     if ( state && !stateAfter ) {
                    //         SerialPrintf( "T-->F %d\n", i );
                    //         // SerialPrintf( "  0Mode: %d\n", modeB4 );
                    //         SerialPrintf( "  0debouncedState: %d\n", stateB4 );
                    //         // SerialPrintf( "  0waitForKeyupFlag: %d\n", keyupFlagB4 );
                    //         // SerialPrintf( "  Mode: %d\n", db[i].mode );
                    //         SerialPrintf( "  debouncedState: %d\n", db[i].debouncedState );
                    //         // SerialPrintf( "  waitForKeyupFlag: %d\n", db[i].waitForKeyupFlag );
                    //     }
                    //     return stateAfter;
                    // }
                    
                    return db[i].debounce( state );
                }
            }
            // not found, add
            uint8_t pos = dbPos;
            dbPos++;
            if ( dbPos >= DEBOUNCER_COUNT ) dbPos = 0;
            dbScanCode[pos] = scanCode;
            db[pos].setInitialValue( state );
            return db[pos].debounce( state );
        }

        bool debounceMatrixExisting( uint8_t scanCode, bool state ) {
            for( int i = 0 ; i < DEBOUNCER_COUNT ; i++ ) {
                if ( dbScanCode[i] == scanCode ) {
                    return db[i].debounce( state );
                }
            }
            return state;
        }

};

}

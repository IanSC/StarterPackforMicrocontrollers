// 2023/07
//
//     +---+---+---+---+
//     | 1 | 2 | 3 | A |
//     +---+---+---+---+
//     | 4 | 5 | 6 | B |
//     +---+---+---+---+
//     | 7 | 8 | 9 | C |
//     +---+---+---+---+
//     | * | 0 | # | D |
//     +---+---+---+---+
//         ||||||||
//         rrrrcccc
//         12341234
//
//  example:
//      obj.assignRows(r1,r2,r3,r4);
//      obj.assignColumns(c1,c2,c3,c4);
//      obj.begin();
//      auto keys = obj.readMatrix();

#pragma once

#include <Arduino.h>
#include <inttypes.h>
#include <stdarg.h>

namespace StarterPack {

class MatrixKeypadBase {

    // protected:

    //     static constexpr int INACTIVE_STATE = 0;

    //
    // INIT
    //
    public:

        virtual ~MatrixKeypadBase() {
            if ( rowPinList != nullptr ) delete[] rowPinList;
            if ( colPinList != nullptr ) delete[] colPinList;
        }

    //
    // INPUTS
    //
    // - specify pins for rows and columns
    // - activeState = if pin will be turn HIGH or LOW for scanning 
    protected:

        uint8_t rowCount = 0; uint8_t *rowPinList = nullptr;
        uint8_t colCount = 0; uint8_t *colPinList = nullptr;
        bool sendViaRows = true;
        uint8_t sendPinCount = 0; uint8_t *sendPinList = nullptr;
        uint8_t recvPinCount = 0; uint8_t *recvPinList = nullptr;

        // pin will be turned HIGH or LOW during scanning
        uint8_t activeState = LOW;

    public:

        // user convenience, don't know how to count number of args without so much extra code
        // https://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments/2124385#2124385
        // NOT WORKING: int pins[] = { 2, 3, 4, 5 };
        //              keypad.assignRows( pins );
        // template<size_t rowCount>
        // static uint8_t assignRows( uint8_t (&rows)[rowCount] ) {
        //     // each call with a different parameter count will generate a new version
        //     // must put body separately to save space
        //     return assignRowsN( rowCount, rows );
        // }
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
                delete[] rowPinList;
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
                delete[] colPinList;
            colPinList = new uint8_t[colCount];

            va_list valist;
            va_start( valist, argCount );
            for ( uint8_t i = 0 ; i < colCount; i++ )
                colPinList[i] = va_arg( valist, int );
            va_end( valist );
        }

        void begin( bool driveLowScanning = true, bool driveColumnsIfMorePins = true ) {
            // default drive low since Arduino only has PULLUP
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
                // if no internal pulldown, must put external resistor to ground
                //    otherwise it will be floating and will not work
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
    public:

        char *readMatrix() {
            keysPressed[0] = 0;
            readMatrixCore( 0, sendPinCount - 1 );
            return keysPressed;
        }

    protected:

        static const uint8_t MAX_SIMULTANEOUS_KEYS = 5;
        char keysPressed[MAX_SIMULTANEOUS_KEYS+1] = "";

        virtual void recordScanCode( uint8_t scanCode ) {
            // scanCode = 0-based
            // this function will be called if a key press is detected
            // save result into keysPressed
            uint8_t l = strlen( keysPressed );
            if ( l >= MAX_SIMULTANEOUS_KEYS ) return;
            // uint8_t getKeymap( const uint8_t scanCode )
            keysPressed[l] = scanCode+1;
            keysPressed[l+1] = 0;
        }

        // other possible implementation using spBitPackedBoolean
        // spBitPackedBoolean:
        //      pro:  easier to debounce before mapping, combining, save time
        //      cons: limited to 32 keys due to spBitPackedBoolean
        // char *
        //      pro:  255 number of keys
        //      cons: difficult to debounce, so map, combine then debounce
        //      cons: limited number of simultaneous keys
        //
        //      StarterPack::spBitPackedBoolean result;
        //      void recordScanCode( uint8_t scanCode ) {
        //          // up to 32 only, limited by spBitPackedBoolean
        //          if (scanCode >= 32) return;
        //          result.turnOn(scanCode);
        //          ...
        //          InputDebouncer<uint32_t> debouncer;
        //          debouncer.debounce(result.data);
        //      }

        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;
        DEBUG_TRACE( uint8_t step = 3 );

        void readMatrixCore( uint8_t sendFrom, uint8_t sendTo ) {
            setOutputPinsActive( sendFrom, sendTo );
            bool active = scanInputs( sendFrom, sendTo );
            if ( active ) {
                // detected something active
                // scan half at a time to narrow down
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
                        // state = debouncer.debounce(state);
                        // if ( state )
                        //     state = debounceMatrix( scanCode, state );
                        // else
                        //     state = debounceMatrixExisting( scanCode, state );
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
                        // state = debouncer.debounce(state);
                        // if ( state )
                        //     state = debounceMatrix( scanCode, state );
                        // else
                        //     state = debounceMatrixExisting( scanCode, state );
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

};

}

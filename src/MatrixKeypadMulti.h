#pragma once
#include <stdint.h>
#include <Arduino.h>
// #include <spUtility.h>
// using namespace StarterPack;

// #include <stdarg.h>

// #include "Debouncer.h"

#include <spBitPackedBoolean.h>
#include <MatrixKeypadBase.h>
#include <UserInputDeviceMulti.h>

namespace StarterPack {

class MatrixKeypadMulti : public MatrixKeypadBase, public UserInputDeviceMulti {

    // MAX 32 BUTTONS because of spBitPackedBoolean

    // protected:

    //     uint8_t rowCount = 0; uint8_t *rowPinList = nullptr;
    //     uint8_t colCount = 0; uint8_t *colPinList = nullptr;
    //     bool sendViaRows = true;
    //     uint8_t sendPinCount = 0; uint8_t *sendPinList = nullptr;
    //     uint8_t recvPinCount = 0; uint8_t *recvPinList = nullptr;

    //     // bool driveHighScanning = true;
    //     uint8_t activeState = LOW;

    //
    // INIT
    //
    public:

        MatrixKeypadMulti() {
            // initMatrixDebouncers();
        }

        ~MatrixKeypadMulti() {
            // if ( rowPinList != nullptr ) delete rowPinList;
            // if ( colPinList != nullptr ) delete colPinList;
        }

/*
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
            // for ( uint8_t i = 0 ; i < sendPinCount; i++ )
            //     setOutputPinStandy( sendPinList[i] );
            if ( driveLowScanning ) {
                // will set LOW send pins
                // pullup HIGH receive pins
                for ( uint8_t i = 0 ; i < recvPinCount; i++ )
                    pinMode( recvPinList[i], INPUT_PULLUP );
            } else {
                // will set HIGH send pins 
                // pulldown LOW receive pins (if possible)
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
*/
    //
    // KEYMAP
    //
    
    private:

        // const char *keyMap = nullptr;

    public:

        // inline void assignKeymap( const char *keyMap ) {
        //     this->keyMap = keyMap;
        // }
/*
        uint8_t getKeymap( const uint32_t keyBitmap ) {
            if ( keyBitmap == 0 ) return 0;
            if ( countBits( keyBitmap ) > 1 ) return 0;
            // https://stackoverflow.com/a/31393298
            uint8_t bitPosition = __builtin_ctzl( keyBitmap );
            if ( keyMap == nullptr )
                return bitPosition + 1;
            if ( strlen( keyMap ) < bitPosition )
                return bitPosition + 1;
            // Serial.print( "bitPosition=" );
            // Serial.println( bitPosition );
            return keyMap[ bitPosition ];
        }

        char *getKeymaps( uint32_t keyBitmap ) {

            static char buffer[33];

            if ( keyBitmap == 0 ) return nullptr;
            if ( keyMap == nullptr ) {
                buffer[0] = getKeymap( keyBitmap );
                buffer[1] = 0;
                return buffer;
            }

            uint8_t mapLen = strlen( keyMap );
            char *p = buffer;
            uint8_t mapIndex = 0;

            // get keyMap for each bit pressed
            // combine as string
            while( keyBitmap != 0 ) {
                if ( mapIndex >= mapLen )
                    break;
                if ( (keyBitmap & 1) != 0 ) {
                    // key pressed, add keyMap
                    *p = keyMap[mapIndex];
                    p++;
                }
                mapIndex++;
                keyBitmap >>= 1;
            }
            *p = 0;
            return buffer;
        }
        */

/*
    //
    // KEYS
    //
    protected:

        uint8_t allenKey( char *allowedKeys = nullptr ) {
            uint32_t keyBitmap = readBitmapRaw();
            char *keyList = getKeymapsForMultiKey32( keyBitmap );
            return processKeysCore( allowedKeys, keyList, false );
        }

    public:

        char *getContinuousKeys( char *allowedKeys = nullptr ) {
            uint32_t keyBitmap = readBitmapRaw();
            char *keyList = getKeymapsForMultiKey32( keyBitmap );
            if ( allowedKeys != nullptr )
                removeDisallowedKeys( allowedKeys, keyList );
            return keyList;
        }

        uint8_t getContinuousKey( char *allowedKeys = nullptr ) {
            return debouncer.getContinuousKey( allenKey( allowedKeys ) );
            // faster but cannot combine keys:            
            // uint8_t key = debouncer.getContinuousKey( getKeymap( readRaw() ) );
            // if ( allowedKeys != nullptr && !isIncludedInList( key, allowedKeys ) )
            //     return 0;
            // return key;
        }

        uint8_t getKeyDown( char *allowedKeys = nullptr ) {
            return debouncer.getKeyDown( allenKey( allowedKeys ) );
            // uint8_t key = debouncer.getKeyDown( getKeymap( readRaw() ) );
            // if ( allowedKeys != nullptr && !isIncludedInList( key, allowedKeys ) )
            //     return 0;
            // return key;
        }

        uint8_t getKeyUp( char *allowedKeys = nullptr ) {
            return debouncer.getKeyUp( allenKey( allowedKeys ) );
            // uint8_t key = debouncer.getKeyUp( getKeymap( readRaw() ) );
            // if ( allowedKeys != nullptr && !isIncludedInList( key, allowedKeys ) )
            //     return 0;
            // return key;
        }

        uint8_t getRepeatingKey( char *allowedKeys = nullptr ) {
            return debouncer.getRepeatingKey( allenKey( allowedKeys ) );
            // uint8_t key = debouncer.getRepeatingKey( getKeymap( readRaw() ) );
            // if ( allowedKeys != nullptr && !isIncludedInList( key, allowedKeys ) )
            //     return 0;
            // return key;
        }

        uint8_t getRepeatingKeyExcept( uint8_t nonRepeatingKey, char *allowedKeys = nullptr ) {
            uint8_t key = debouncer.getRepeatingKey( allenKey( allowedKeys ) );
            if ( key == nonRepeatingKey )
                debouncer.flagWaitForKeyup();
            // uint8_t key = debouncer.getRepeatingKey( getKeymap( readRaw() ) );            
            // if ( allowedKeys != nullptr && !isIncludedInList( key, allowedKeys ) )
            //     return 0;
            // if ( nonRepeatingKeys != nullptr && isIncludedInList( key, nonRepeatingKeys ) )
            //     debouncer.fl();
            return key;
        }

        uint8_t getRepeatingKeyExcept( char *nonRepeatingKeys, char *allowedKeys = nullptr ) {
            uint8_t key = debouncer.getRepeatingKey( allenKey( allowedKeys ) );
            if ( nonRepeatingKeys != nullptr && isCharInString( key, nonRepeatingKeys ) )
                debouncer.flagWaitForKeyup();
            // uint8_t key = debouncer.getRepeatingKey( getKeymap( readRaw() ) );            
            // if ( allowedKeys != nullptr && !isIncludedInList( key, allowedKeys ) )
            //     return 0;
            // if ( nonRepeatingKeys != nullptr && isIncludedInList( key, nonRepeatingKeys ) )
            //     debouncer.fl();
            return key;
        }

*/

    //
    // FAST - no key filtering, no combining of keys
    //

        // char *getContinuousKeysFast() { return getKeymaps( readRaw() ); }
        // uint8_t getContinuousKeyFast() { return debouncer.getContinuousKey( getKeymap( readRaw() ) ); }
        // uint8_t getKeyDownFast() { return debouncer.getKeyDown( getKeymap( readRaw() ) ); }
        // uint8_t getRepeatingKeyFast() { return debouncer.getKeyUp( getKeymap( readRaw() ) ); }
        // uint8_t getRepeatingKeyExceptFast( uint8_t exception ) {
        //     uint8_t key = debouncer.getRepeatingKey( getKeymap( readRaw() ) );
        //     if ( key == exception ) debouncer.flagWaitForKeyup();
        //     return key;
        // }
        // virtual uint8_t getRepeatingKeyExceptFast( char *nonRepeatingKeys ) {
        //     uint8_t key = debouncer.getRepeatingKey( getKeymap( readRaw() ) );
        //     if ( nonRepeatingKeys != nullptr && isIncludedInList( key, nonRepeatingKeys ) )
        //         debouncer.flagWaitForKeyup();
        //     return key;            
        // }


    //
    // DEBOUNCER
    //
    // protected:       
    public:

        void flagWaitForKeyupSpecific( uint8_t key ) { debouncer.flagWaitForKeyup(); }
        void flagWaitForKeyupMulti( char *keysPressed ) { debouncer.flagWaitForKeyup(); }


    //
    // DEBOUNCER
    //
    //public:

        // Debouncer debouncer;

        // void setDebounceTimeInMs( uint16_t activeState = 50, uint16_t inactiveState = 50, uint16_t minimum = 50 ) {
        //     Debouncer::Settings *s = debouncer.getSettings();
        //     s->activeStatesDebounceInMs = activeState;
        //     s->inactiveStateDebounceInMs = inactiveState;
        //     s->minimumDebounceTimeInMs = minimum;
        // }

        // inline void flagWaitForKeyup() { debouncer.flagWaitForKeyup(); }
        // inline void cancelDebouncing() { debouncer.cancelDebouncing(); }
    
        // void flagWaitForKeyupSpecific( uint8_t key ) { debouncer.flagWaitForKeyup(); }
        // void flagWaitForKeyupMulti( char *keys ) { debouncer.flagWaitForKeyup(); }

    //
    // READ DEVICE
    //
    public:

        char *readKeyList() {
            // record each key pressed into buffer
            keysPressed[0] = 0;
            if ( sendPinCount == 0 || recvPinCount == 0 )
                return keysPressed;
            readMatrixCore( 0, sendPinCount - 1 );
            return keysPressed;
        }
        
    protected:

        static const uint8_t MAX_KEYS = 10;
        char keysPressed[MAX_KEYS+1];

        void recordScanCode( uint8_t scanCode ) {
            uint8_t l = strlen( keysPressed );
            if ( l >= MAX_KEYS ) return;
            // uint8_t getKeymap( const uint8_t scanCode ) 
            keysPressed[l] = getKeymap( scanCode+1 );
            keysPressed[l+1] = 0;
        }

/*
    //
    // READ DEVICE
    //
    public:

        uint32_t readBitmapRaw() {
            // record each key pressed into bitmap
            bits.reset();
            if ( sendPinCount == 0 || recvPinCount == 0 )
                return -1;
            readMatrixCore( 0, sendPinCount - 1 );
            return bits.data;
        }
        
    protected:

        spBitPackedBoolean bits;

        void recordScanCode( uint8_t scanCode ) {
            bits.turnOn( scanCode );
        }
*/

/*
        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;
        DEBUG_TRACE( uint8_t step = 0 );

        virtual void recordScanCode( uint8_t scanCode ) = 0;

        void readHelper( uint8_t sendFrom, uint8_t sendTo ) {
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
                        readHelper( sendFrom, mid );

                    // read upper half
                    if ( mid+1 == sendTo ) {
                        setOutputPinsActive( mid+1, sendTo );
                        scanInputs( mid+1, sendTo );
                        setOutputPinsStandby( mid+1, sendTo );
                    } else
                        readHelper( mid+1, sendTo );
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

        inline bool scanInputs( uint8_t sendFrom, uint8_t sendTo ) {
            DEBUG_TRACE( SerialPrintCharsN( ' ', step ) );
            DEBUG_TRACE( Serial.println( "scanning" ) );
            if ( sendFrom == sendTo ) {
                // single send slot being checked, record results
                if ( sendViaRows ) {
                    for ( int recvPin = 0 ; recvPin < recvPinCount ; recvPin++ ) {
                        uint8_t scanCode = sendFrom * recvPinCount + recvPin;
                        bool state = ( digitalRead( recvPinList[recvPin] ) == activeState );
                        if ( state )
                            state = debounce( scanCode, state );
                        else
                            state = debounceExisting( scanCode, state );
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
                            state = debounce( scanCode, state );
                        else
                            state = debounceExisting( scanCode, state );
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

    //
    // DEBOUNCE
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

        bool debounce( uint8_t scanCode, bool state ) {
            // return state;
            // Serial.println( scanCode );
            // return state;
            //if ( scanCode != 6 ) return state;
            for( int i = 0 ; i < DEBOUNCER_COUNT ; i++ ) {
                if ( dbScanCode[i] == scanCode ) {
                    / *
                    if ( scanCode == 6 ) {
                        // StarterPack::Debouncer::modes modeB4 = db[i].mode;
                        int stateB4 = db[i].debouncedState;
                        // bool keyupFlagB4 = db[i].waitForKeyupFlag;
                        bool stateAfter = db[i].debounce( state );
                        if ( state && !stateAfter ) {
                            SerialPrintf( "T-->F %d\n", i );
                            // SerialPrintf( "  0Mode: %d\n", modeB4 );
                            SerialPrintf( "  0debouncedState: %d\n", stateB4 );
                            // SerialPrintf( "  0waitForKeyupFlag: %d\n", keyupFlagB4 );
                            // SerialPrintf( "  Mode: %d\n", db[i].mode );
                            SerialPrintf( "  debouncedState: %d\n", db[i].debouncedState );
                            // SerialPrintf( "  waitForKeyupFlag: %d\n", db[i].waitForKeyupFlag );
                        }
                        return stateAfter;
                    }
                    * /
                    return db[i].debounce( state );
                }
            }
            // not found, add
            uint8_t pos = dbPos;
            dbPos++;
            if ( dbPos >= DEBOUNCER_COUNT ) dbPos = 0;
            dbScanCode[pos] = scanCode;
            db[pos].setInitialValue( state );
            // SerialPrintf( "DB Used %d for %d\n", pos, scanCode );
            // bool yy = db[pos].debounce( state );
            // if ( state && !yy ) {
            //     Serial.print( "T-->Fq " );
            //     Serial.println( pos );
            // }
            // return yy;
            return db[pos].debounce( state );
        }

        bool debounceExisting( uint8_t scanCode, bool state ) {
            // return state;
            for( int i = 0 ; i < DEBOUNCER_COUNT ; i++ ) {
                if ( dbScanCode[i] == scanCode ) {
                    // if ( scanCode == 6 ) {
                    //     bool xx = db[i].debounce( state );
                    //     if ( state && !xx)
                    //         Serial.println(xx);
                    //     return xx;
                    // }
                    return db[i].debounce( state );
                }
            }
            return state;
        }
*/
};

}

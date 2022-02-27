#pragma once
#include <stdint.h>
#include <Arduino.h>

//  spEncoder E( pinA, pinB, pinA_activeState, pinB_activeState );
//
//  void IRAM_ATTR encoderISRCaller() { E.ISR(); }
//
//  void setup() {
//      E.pinAB_openCollectorMode();
//      E.pinZ_openCollectorMode();
//      E.setDirection( cwIsPositive );
//      ... home
//      E.set( homeposition );
//      E.setupZSync( zPin, encoderPPR, syncValue, zPin_activeState );
//      E.enableZSync();
//      E.enableInterrupt( encoderISRCaller );
//      ...
//      ... = E.get();
//      E.increment();
//      E.decrement();
//      E.delta( value );
//      ...
//      E.disableZSync();
//      ...
//      E.disableInterrupt();
//      ... E.getMissingPulses();
//  }

namespace StarterPack {

    class Encoder {

        protected:

            uint8_t  pinA, pinB;
            uint8_t  pinA_activeState, pinB_activeState;

            portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

        public:

            Encoder( uint8_t pinA, uint8_t pinB, uint8_t pinA_activeState = HIGH, uint8_t pinB_activeState = HIGH ) {
                this->pinA = pinA; this->pinB = pinB;
                this->pinA_activeState = pinA_activeState; this->pinB_activeState = pinB_activeState;

                pinMode( pinA, INPUT );
                pinMode( pinB, INPUT );
            }

            void setPinAB_activeStates( uint8_t pinA_activeState = HIGH, uint8_t pinB_activeState = HIGH ) {
                this->pinA_activeState = pinA_activeState; this->pinB_activeState = pinB_activeState;
            }

            void pinAB_openCollectorMode() {
                // based on encoder output type (not sure if correct):
                // - voltage output, set a INPUT
                // - NPN open collector, positive ACTIVE, set as INPUT_PULLUP  (or external resistor)
                // - PNP open collector, negative ACTIVE, set as INPU_PULLDOWN (or external resitor, no pull down on Arduino)
                //
                // if active HIGH, pullup HIGH
                // if active LOW,  pullup LOW
                if ( pinA_activeState == HIGH )
                    pinMode( pinA, INPUT_PULLUP );
                else {
                    #if defined(ESP32)
                        pinMode( pinA, INPUT_PULLDOWN );
                    #else
                        // pinMode( pinA, INPUT );
                    #endif
                }
                if ( pinB_activeState == HIGH )
                    pinMode( pinB, INPUT_PULLUP );
                else {
                    #if defined(ESP32)
                        pinMode( pinB, INPUT_PULLDOWN );
                    #else
                        // pinMode( pinB, INPUT );
                    #endif
                }
            }

        //
        // DIRECTION
        //
        protected:

            // set CW = increasing positionn, CCW = decreasing
            int8_t directionMapCW [16] = { 0,  1, -1, 0, -1, 0, 0,  1,  1, 0, 0, -1, 0, -1,  1, 0 };
            int8_t directionMapCCW[16] = { 0, -1,  1, 0,  1, 0, 0, -1, -1, 0, 0,  1, 0,  1, -1, 0 };

            int8_t *directionMap = directionMapCW;
            bool   cwIsPositive = true;

        public:

            void setDirection( bool cwIsPositive ) {
                this->cwIsPositive = cwIsPositive;
                if ( cwIsPositive )
                    directionMap = directionMapCW;
                else
                    directionMap = directionMapCCW;
            }
            bool isDirectionCWPositive() {
                return cwIsPositive;
            }

        //
        // INTERRUPT
        //
        public:

            void enableInterrupt( void (*interruptCaller)() ) {
                // ex. spEncoder E( clkPin, dirPin );
                //     void IRAM_ATTR encoderISRCaller() { E.ISR(); }
                //     void setup() { E.attachClkInterrupt( encoderISRCaller ); }
                if ( interruptCaller == nullptr ) return;
                attachInterrupt( digitalPinToInterrupt( pinA ), interruptCaller, CHANGE );
                attachInterrupt( digitalPinToInterrupt( pinB ), interruptCaller, CHANGE );
                prevState = ( digitalRead( pinA ) << 3 ) | ( digitalRead( pinB ) << 2 );
            }

            void disableInterrupt() {
                detachInterrupt( digitalPinToInterrupt( pinA ) );
            }

        protected:

            uint8_t prevState = 0;
            volatile int32_t position = 0;

        public:

            void ISR() {

                // https://www.ia.omron.com/product/item/2382/
                // timing diagrams for hand controlled encoders: CLK => B, DT => A

                // === CW === >>>
                //               +-[A]----1--+           +--1-----1--+           +--1-----1--+
                //     (A)       |           |           |           |           |
                //          -[a]-+           +--0-----0--+           +--0-----0--+
                //                     +--1-----1--+           +--1-----1--+           +--1---
                //     (B)             |           |           |           |           |
                //          -[b]---[B]-+           +--0-----0--+           +--0-----0--+
                //               ^           ^     ^     ^     ^
                //               ABab -->    0111  0001  1000  1110
                //
                // === CCW === >>>
                //                     +--1-----1--+           +--1-----1--+           +--1---
                //     (A)             |           |           |           |           |
                //          -[a]---[A]-+           +--0-----0--+           +--0-----0--+
                //               +-[B]----1--+           +--1-----1--+           +--1-----1--+
                //     (B)       |           |           |           |           |
                //          -[b]-+           +--0-----0--+           +--0-----0--+
                //               ^           ^     ^     ^     ^
                //               ABab        1011  0010  0100  1101
                //
                //  AB - new state / ab - previous state
                //
                //    ABab   DIR   Value
                //    ----   ---   -----
                //  > 0000          0
                //    0001   cw     1
                //    0010   ccw   -1
                //    0011          0
                //  > 0100   ccw   -1
                //    0101          0
                //    0110          0
                //    0111   cw     1
                //  > 1000   cw     1
                //    1001          0
                //    1010          0
                //    1011   ccw   -1
                //  > 1100          0
                //    1101   ccw   -1
                //    1110   cw     1
                //    1111          0

                portENTER_CRITICAL_ISR( &mux );

                // move AB__ ==> __ab
                prevState >>= 2;

                // put in new AB
                prevState = prevState | ( digitalRead( pinA ) << 3 ) | ( digitalRead( pinB ) << 2 );

                if ( syncOnZ && ( prevState == 0b1110 || prevState == 0b1011 ) ) {

                    // based on Omron's, but most should be compatible with it
                    //
                    // === CW === >>>
                    //               +--1-----1--+           +--1-----1--+           +--1-----1--+
                    //     (A)       | [a]   [A] |           |           |           |
                    //          --0--+           +--0-----0--+           +--0-----0--+
                    //                     +--1-----1--+           +--1-----1--+           +--1---
                    //     (B)         [b] | [B]       |           |           |           |
                    //          --0-----0--+           +--0-----0--+           +--0-----0--+
                    //             +-------------------+
                    //     (Z)     |       ^           |
                    //          ---+       ^           +--------
                    //                     1110
                    // === CCW === >>>
                    //                     +--1-----1--+           +--1-----1--+           +--1---
                    //     (A)             | [a]   [A] |           |           |           |
                    //          --0-----0--+           +--0-----0--+           +--0-----0--+
                    //               +--1-----1--+           +--1-----1--+           +--1-----1--+
                    //     (B)       |       [b] | [B]       |           |           |
                    //          --0--+           +--0-----0--+           +--0-----0--+
                    //               +-----------------------+
                    //     (Z)       |           ^           |
                    //          -----+           ^           +--------
                    //                           1011
                    //
                    if ( digitalRead( zPin ) == zPin_activeState ) {
                        // home !!!
                        syncZCore();
                        portEXIT_CRITICAL_ISR( &mux );
                        return;
                    }
                }

                // -1, 0 or 1
                int8_t currentDirection = directionMap[ prevState ];

                position += currentDirection;

                portEXIT_CRITICAL_ISR( &mux );
            }
        
        //
        // DATA
        //
        public:

            int32_t get() {
                portENTER_CRITICAL( &mux );
                auto r = position;
                portEXIT_CRITICAL( &mux );
                return r;
            }

            void set( int32_t position ) {
                portENTER_CRITICAL( &mux );
                position = position;
                portEXIT_CRITICAL( &mux );
            }

            void delta( int32_t value ) {
                portENTER_CRITICAL( &mux );
                position += value;
                portEXIT_CRITICAL( &mux );
            }

            void increment() {
                portENTER_CRITICAL( &mux );
                position++;
                portEXIT_CRITICAL( &mux );
            }

            void decrement() {
                portENTER_CRITICAL( &mux );
                position--;
                portEXIT_CRITICAL( &mux );
            }

        //
        // Z SYNC
        //
        public:

            void setupZSync( uint8_t zPin, uint16_t encoderPPR, int32_t syncValue, uint8_t zPin_activeState = HIGH ) {
                // when Z in hit, encoder count must be syncValue +/- multiple of encoder PPR
                //
                //      sVal-2*PPR    sVal-1*PPR    sVal          sVal+1*PPR    sVal+2*PPR
                //   ---|-------------|-------------|-------------|-------------|-------------|---
                //      Z             Z             Z             Z             Z             Z
                //
                this->zPin = zPin;
                this->encoderPPR = encoderPPR;
                this->syncValue = syncValue;
                this->zPin_activeState = zPin_activeState;
                pinMode( zPin, INPUT );
                // portENTER_CRITICAL( &mux );
                // syncOnZ = true;
                // portEXIT_CRITICAL( &mux );
            }

            void setPinZ_activeStates( uint8_t zPin_activeState = HIGH ) {
                this->zPin_activeState = zPin_activeState;
            }

            void pinZ_openCollectorMode() {
                // same as pinAB_openCollectorMode()
                if ( zPin_activeState == HIGH )
                    pinMode( zPin, INPUT_PULLUP );
                else {
                    #if defined(ESP32)
                        pinMode( zPin, INPUT_PULLDOWN );
                    #else
                        // pinMode( zPin, INPUT );
                    #endif
                }
            }

            void enableZSync() {
                if ( syncOnZ ) return;
                portENTER_CRITICAL( &mux );
                syncOnZ = true;
                portEXIT_CRITICAL( &mux );
            }

            void disableZSync() {
                if ( !syncOnZ ) return;
                portENTER_CRITICAL( &mux );
                syncOnZ = false;
                portEXIT_CRITICAL( &mux );
            }

            uint32_t getMissingPulses() {
                portENTER_CRITICAL( &mux );
                auto r = missingPulses;
                portEXIT_CRITICAL( &mux );
                return r;
            }

        protected:

            bool     syncOnZ = false;
            uint8_t  zPin;
            uint8_t  zPin_activeState;
            uint16_t encoderPPR = 100;
            int32_t  syncValue = 0;

            uint32_t missingPulses = 0;

            inline void syncZCore() {
                //Serial.println( "sync" );
                if ( encoderPPR == 0 ) return;

                auto adjusted = position - syncValue;
                auto stray = adjusted % encoderPPR;
                if ( stray == 0 ) return;

                if ( stray >= 0 ) {
                    if ( stray >= ( encoderPPR >> 1 ) ) {
                        auto diff = encoderPPR - stray;
                        missingPulses += diff;
                        position += diff;
                    } else {
                        missingPulses += stray;
                        position -= stray;
                    }
                } else {
                    if ( stray <= -( encoderPPR >> 1 ) ) {
                        auto diff = encoderPPR + stray;
                        missingPulses += diff;
                        position -= diff;
                    } else {
                        missingPulses -= stray;
                        position -= stray;
                    }
                }
            }

        //
        // TEST
        //
        protected:

            friend void TEST_encoderTriggerSyncZ( Encoder &e );

    };

//
// TEST
//

    // void TEST_spEncoderCore( bool incFtoB, int32_t randomInitialPos, int32_t setPositionTo ) {
    //     spEncoder e( 0, 0 );

    //     Serial.print( "   TEST: " );
    //     Serial.print( incFtoB ? "inc" : "dec" );
    //     Serial.print( " / " );
    //     SerialPrintf( "%5d", randomInitialPos );
    //     Serial.print( " / " );
    //     SerialPrintf( "%5d", setPositionTo );

    //     e.setDirection( !incFtoB );
    //     // // e.reverseDirection = !incFtoB;

    //     e.set( randomInitialPos );      // SerialPrintfln( "\n      initial  = %d", e.get() );
    //     e.debugMoveBackward( 1000 );    // SerialPrintfln(   "      backward = %d", e.get() );
    //     e.set( setPositionTo );         // SerialPrintfln(   "      set      = %d", e.get() );
    //     e.debugMoveForward( 1000 );     // SerialPrintfln(   "      forward  = %d", e.get() );
    //     int32_t position = e.get();      SerialPrintf  (   "      position = %d", e.get() );
    //     if ( position != setPositionTo - 1000 )
    //         Serial.println( " ==> *** ERROR ***" );
    //     else
    //         Serial.println( " ==> PASSED" );
    // }

    // void TEST_spEncoder() {
    //     // --- USELESS TEST ALREADY ---
    //     Serial.println( "--- START ---" );
    //     TEST_spEncoderCore( true ,     0,  1000 );
    //     TEST_spEncoderCore( true ,  1000,  1000 );
    //     TEST_spEncoderCore( true ,   500,  1000 );
    //     TEST_spEncoderCore( true , -1000,  1000 );
    //     TEST_spEncoderCore( true ,  -500,  1000 );
    //     TEST_spEncoderCore( false,     0,  1000 );
    //     TEST_spEncoderCore( false,  1000,  1000 );
    //     TEST_spEncoderCore( false,   500,  1000 );
    //     TEST_spEncoderCore( false, -1000,  1000 );
    //     TEST_spEncoderCore( false,  -500,  1000 );
    //     TEST_spEncoderCore( true ,     0, -1000 );
    //     TEST_spEncoderCore( true ,  1000, -1000 );
    //     TEST_spEncoderCore( true ,   500, -1000 );
    //     TEST_spEncoderCore( true , -1000, -1000 );
    //     TEST_spEncoderCore( true ,  -500, -1000 );
    //     TEST_spEncoderCore( false,     0, -1000 );
    //     TEST_spEncoderCore( false,  1000, -1000 );
    //     TEST_spEncoderCore( false,   500, -1000 );
    //     TEST_spEncoderCore( false, -1000, -1000 );
    //     TEST_spEncoderCore( false,  -500, -1000 );
    //     Serial.println( "--- END ---" );
    // }

    void TEST_encoderTriggerSyncZ( Encoder &e ) {
        e.syncZCore();
    }

    void TEST_encoderZSyncCore( int32_t currentPosition, int32_t ppr, int32_t zSync, int32_t expected ) {
        Encoder e( 0, 0 );
        e.setupZSync( 0, ppr, zSync );
        e.set( currentPosition );
        TEST_encoderTriggerSyncZ( e );

        Serial.print( "   TEST: " );
        Serial.print( ppr );
        Serial.print( " / " );
        Serial.print( zSync );
        Serial.print( " : " );
        Serial.print( currentPosition );
        Serial.print( " ==> " );
        auto newValue = e.get();
        Serial.print( newValue );
        Serial.print( " / " );
        Serial.print( e.getMissingPulses() );
        if ( newValue != expected )
            Serial.print( " *** FAILED ***" );
        Serial.println();
    }

    void TEST_encoderZSync() {
        Serial.println( "--- START ---" );

        TEST_encoderZSyncCore(    0, 100, 0,    0 );
        TEST_encoderZSyncCore(    1, 100, 0,    0 );
        TEST_encoderZSyncCore(   -1, 100, 0,    0 );
        TEST_encoderZSyncCore(   99, 100, 0,  100 );
        TEST_encoderZSyncCore(  100, 100, 0,  100 );
        TEST_encoderZSyncCore(  101, 100, 0,  100 );
        TEST_encoderZSyncCore(  -99, 100, 0, -100 );
        TEST_encoderZSyncCore( -100, 100, 0, -100 );
        TEST_encoderZSyncCore( -101, 100, 0, -100 );
        TEST_encoderZSyncCore(   49, 100, 0,    0 );
        TEST_encoderZSyncCore(   50, 100, 0,  100 );
        TEST_encoderZSyncCore(   51, 100, 0,  100 );
        TEST_encoderZSyncCore(  -49, 100, 0,    0 );
        TEST_encoderZSyncCore(  -50, 100, 0, -100 );
        TEST_encoderZSyncCore(  -51, 100, 0, -100 );

        TEST_encoderZSyncCore( 300, 400, 0, 400 );
        TEST_encoderZSyncCore( 301, 400, 0, 400 );
        TEST_encoderZSyncCore( 299, 400, 0, 400 );
        TEST_encoderZSyncCore( 200, 400, 0, 400 );
        TEST_encoderZSyncCore( 201, 400, 0, 400 );
        TEST_encoderZSyncCore( 199, 400, 0,   0 );

        TEST_encoderZSyncCore( 300, 400, 50, 450 );
        TEST_encoderZSyncCore( 301, 400, 50, 450 );
        TEST_encoderZSyncCore( 299, 400, 50, 450 );
        TEST_encoderZSyncCore( 250, 400, 50, 450 );
        TEST_encoderZSyncCore( 251, 400, 50, 450 );
        TEST_encoderZSyncCore( 249, 400, 50,  50 );

        TEST_encoderZSyncCore( 300, 400, -50, 350 );
        TEST_encoderZSyncCore( 301, 400, -50, 350 );
        TEST_encoderZSyncCore( 299, 400, -50, 350 );
        TEST_encoderZSyncCore( 150, 400, -50, 350 );
        TEST_encoderZSyncCore( 151, 400, -50, 350 );
        TEST_encoderZSyncCore( 149, 400, -50, -50 );

        Serial.println( "--- END ---" );
    }

}

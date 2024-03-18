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

    class spEncoder {

        public:
        
            typedef int64_t countSigned;

        protected:

            uint8_t  pinA, pinB;
            uint8_t  pinA_activeState, pinB_activeState;

            portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

        public:

            enum class openCollectorType : uint8_t {
                // None,
                NPN = 0, // ACTIVE = positive
                PNP = 1  // ACTIVE = negative
            };

            spEncoder() {}
            void setup( uint8_t pinA, uint8_t pinB ) {
                this->pinA = pinA; this->pinB = pinB;
            }

            spEncoder( uint8_t pinA, uint8_t pinB ) {
                this->pinA = pinA; this->pinB = pinB;
                setPinAB_activeStates( HIGH, HIGH );
            }

            spEncoder( uint8_t pinA, uint8_t pinB, openCollectorType openCollector ) {
                this->pinA = pinA; this->pinB = pinB;
                setOpenCollectorInput( pinA, openCollector );
                setOpenCollectorInput( pinB, openCollector );
                this->pinA_activeState = getOpenCollectorActiveState( openCollector );
                this->pinB_activeState = getOpenCollectorActiveState( openCollector );
            }

            // spEncoder( uint8_t pinA, uint8_t pinB, uint8_t pinA_activeState = HIGH, uint8_t pinB_activeState = HIGH ) {
            //     this->pinA = pinA; this->pinB = pinB;
            //     this->pinA_activeState = pinA_activeState; this->pinB_activeState = pinB_activeState;

            //     pinMode( pinA, INPUT );
            //     pinMode( pinB, INPUT );
            //     setOpenCollectorInput( pinA, pinA_activeState );
            //     setOpenCollectorInput( pinB, pinB_activeState );
            // }

            void setPinAB_OpenCollectorType( openCollectorType openCollector ) {
                setOpenCollectorInput( pinA, openCollector );
                setOpenCollectorInput( pinB, openCollector );
                this->pinA_activeState = getOpenCollectorActiveState( openCollector );
                this->pinB_activeState = getOpenCollectorActiveState( openCollector );
            }
            openCollectorType getPinAB_OpenCollectorType() {
                return getOpenCollectorActiveState( pinA_activeState );
            }

            void setPinAB_activeStates( uint8_t pinA_activeState, uint8_t pinB_activeState ) {
                auto pinA_OC = pinA_activeState == HIGH ? openCollectorType::NPN : openCollectorType::PNP;
                auto pinB_OC = pinB_activeState == HIGH ? openCollectorType::NPN : openCollectorType::PNP;
                setOpenCollectorInput( pinA, pinA_OC );
                setOpenCollectorInput( pinB, pinB_OC );
                this->pinA_activeState = getOpenCollectorActiveState( pinA_OC );
                this->pinB_activeState = getOpenCollectorActiveState( pinB_OC );
            }
            uint8_t getPinAB_activeStates() {
                // assume: pinA_activeState = pinB_activeState
                return this->pinA_activeState;
            }

/*
            void pinAB_openCollectorMode() {
                setOpenCollectorInput( pinA, pinA_activeState );
                setOpenCollectorInput( pinB, pinB_activeState );
                // // based on encoder output type (not sure if correct):
                // // - voltage output, set a INPUT
                // // - NPN open collector, positive ACTIVE, set as INPUT_PULLUP  (or external resistor)
                // // - PNP open collector, negative ACTIVE, set as INPU_PULLDOWN (or external resitor, no pull down on Arduino)
                // //
                // // if active HIGH, pullup HIGH
                // // if active LOW,  pullup LOW
                // if ( c == HIGH )
                //     pinMode( pinA, INPUT_PULLUP );
                // else {
                //     #if defined(ESP32)
                //         pinMode( pinA, INPUT_PULLDOWN );
                //     #else
                //         // pinMode( pinA, INPUT );
                //     #endif
                // }
                // if ( pinB_activeState == HIGH )
                //     pinMode( pinB, INPUT_PULLUP );
                // else {
                //     #if defined(ESP32)
                //         pinMode( pinB, INPUT_PULLDOWN );
                //     #else
                //         // pinMode( pinB, INPUT );
                //     #endif
                // }
            }
*/

        protected:

            static uint8_t getOpenCollectorActiveState( openCollectorType openCollector ) {
                // based on encoder output type (not sure if correct):
                // - voltage output, set a INPUT
                // - NPN open collector, positive ACTIVE, set as INPUT_PULLUP  (or external resistor)
                // - PNP open collector, negative ACTIVE, set as INPUT_PULLDOWN (or external resitor, no pull down on Arduino)

                switch( openCollector ) {
                // case openCollectorType::None: break;
                case openCollectorType::NPN: return HIGH;
                case openCollectorType::PNP: return LOW;
                default: return HIGH;
                }
            }

            static openCollectorType getOpenCollectorActiveState( uint8_t activeState ) {
                if ( activeState == LOW )
                    return openCollectorType::PNP;
                else
                    return openCollectorType::NPN;
            }

            static void setOpenCollectorInput( uint8_t pin, openCollectorType openCollector ) {
                // based on encoder output type (not sure if correct):
                // - voltage output, set a INPUT
                // - NPN open collector, positive ACTIVE, set as INPUT_PULLUP  (or external resistor)
                // - PNP open collector, negative ACTIVE, set as INPUT_PULLDOWN (or external resitor, no pull down on Arduino)

                switch( openCollector ) {
                case openCollectorType::NPN:
                    // NPN open collector, positive ACTIVE, set as INPUT_PULLUP  (or external resistor)
                    #if defined(ESP32)
                        pinMode( pin, INPUT_PULLDOWN );
                    #else
                        // pinMode( pin, INPUT );
                    #endif
                    break;
                case openCollectorType::PNP:
                    // PNP open collector, negative ACTIVE, set as INPU_PULLDOWN (or external resitor, no pull down on Arduino)
                    pinMode( pin, INPUT_PULLUP );
                    break;
                // case openCollectorType::None:
                default:
                    pinMode( pin, INPUT );
                    break;
                }
            }

/*
            static void setOpenCollector( uint8_t pin, uint8_t activeState ) {
                // based on encoder output type (not sure if correct):
                // - voltage output, set a INPUT
                // - NPN open collector, positive ACTIVE, set as INPUT_PULLUP  (or external resistor)
                // - PNP open collector, negative ACTIVE, set as INPUT_PULLDOWN (or external resitor, no pull down on Arduino)
                //
                // if active HIGH, pullup HIGH
                // if active LOW,  pullup LOW

                if ( activeState == LOW ) {
                    // PNP open collector, negative ACTIVE, set as INPU_PULLDOWN (or external resitor, no pull down on Arduino)
                    pinMode( pin, INPUT_PULLUP );
                } else {
                    // NPN open collector, positive ACTIVE, set as INPUT_PULLUP  (or external resistor)
                    #if defined(ESP32)
                        pinMode( pin, INPUT_PULLDOWN );
                    #else
                        // pinMode( pin, INPUT );
                    #endif
                }

                // if ( activeState == HIGH )
                //     pinMode( pin, INPUT_PULLUP );
                // else {
                //     #if defined(ESP32)
                //         pinMode( pin, INPUT_PULLDOWN );
                //     #else
                //         // pinMode( pin, INPUT );
                //     #endif
                // }
            }
*/

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

            void setDirectionCwIsPositive( bool cwIsPositive ) {
                this->cwIsPositive = cwIsPositive;
                if ( cwIsPositive )
                    directionMap = directionMapCW;
                else
                    directionMap = directionMapCCW;
            }
            bool getDirectionCwIsPositive() {
                return this->cwIsPositive;
            }
            bool isDirectionCWPositive() {
                return cwIsPositive;
            }
            void reverseDirectionCWPositive() {
                setDirectionCwIsPositive( !cwIsPositive );
            }

        //
        // TARGET
        //
        public:
            typedef std::function<void(void)> cbCounterReachedDelegateF;
        protected:
            cbCounterReachedDelegateF cbCounterReachedF = nullptr;

        protected:

            bool triggerActive = false;
            countSigned triggerStopPosition;

        public:

            void setTriggerAt( countSigned target ) {
                triggerActive = true;
                triggerStopPosition = target;
            }

            bool isTriggerActive() {
                return triggerActive;
            }
            void cancelTrigger() {
                triggerActive = false;
            }

            inline void setTriggerCallback( cbCounterReachedDelegateF cb ) {
                cbCounterReachedF = cb; }

            // inline void setTriggerToFullStop( cbCounterReachedDelegateF cb ) {
            //     // use internal stop function
            //     // after stopping stepMotor speed is still not 0
            //     // use callback to set it
            //     cbCounterReachedF = cb;
            //     counter.setCounterReachedCallback( fullStopOnCounterReached );
            // }
            // inline void setTriggerCallback( spCounterHW::cbTargetReachedDelegateF cb ) {
            //     counter.setCounterReachedCallback( cb ); }
            // inline void setTriggerCallback( spCounterHW::cbTargetReachedDelegate cb ) {
            //     counter.setCounterReachedCallback( cb ); }
            // inline void clearTriggerCallback() {
            //     counter.clearCounterReachedCallback(); }

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
            volatile countSigned _position = 0;

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
                //  ABab, where AB - new state / ab - previous state
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
                prevState = prevState
                    | ( ( digitalRead( pinA ) == pinA_activeState ) << 3 )
                    | ( ( digitalRead( pinB ) == pinB_activeState ) << 2 );

                // -1, 0 or 1
                int8_t currentDirection = directionMap[ prevState ];
                _position += currentDirection;

                // same transition CW and CCW
                // in diagram when (B) rises/falls
                // - if keep going CW, no ZSync discrepancy
                // - if keep going CCW, no ZSync discrepancy
                // - if changing CW/CCW, has ZSync discrepancy
                //
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
                //  ABab ==> AB - new state / ab - previous state
                //

                // if ( syncOnZ && ( prevState == 0b0001 || prevState == 0b0100 ) ) {
                // if ( syncOnZ && ( prevState == 0b0111 || prevState == 0b1101 ) ) {
                // if ( syncOnZ && ( prevState == 0b1000 || prevState == 0b0010 ) ) {
                // if ( syncOnZ && ( prevState == 0b1110 || prevState == 0b1011 ) ) {

                if ( syncOnZ && ( prevState == 0b1110 || prevState == 0b1101 ) ) {

                    // transistion on a pulse
                    // going into *******
                    // - no ZSync discrepancy
                    //
                    // based on Omron's, but most should be compatible with it
                    //
                    // === CW === >>>      *******
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
                    // === CCW === >>>     *******
                    //                     +--1-----1--+           +--1-----1--+           +--1---
                    //     (A)         [a] | [A]       |           |           |           |
                    //          --0-----0--+           +--0-----0--+           +--0-----0--+
                    //               +--1-----1--+           +--1-----1--+           +--1-----1--+
                    //     (B)       | [b]   [B] |           |           |           |
                    //          --0--+           +--0-----0--+           +--0-----0--+
                    //               +-----------------------+
                    //     (Z)       |     ^                 |
                    //          -----+     ^                 +--------
                    //                     1101
                    //
                    //  ABab ==> AB - new state / ab - previous state
                    //
                    if ( digitalRead( zPin ) == zPin_activeState ) {
                        // home !!!
                        syncZCore();
                        // portEXIT_CRITICAL_ISR( &mux );
                        // return;
                    }
                }

                bool triggered = false;
                if ( triggerActive ) {
                    if ( _position == triggerStopPosition ) {
                        triggerActive = false;
                        triggered = true;
                    }
                }

                portEXIT_CRITICAL_ISR( &mux );

                if ( triggered && cbCounterReachedF != nullptr )
                    cbCounterReachedF();
            }
        
        //
        // DATA
        //
        public:

            countSigned get() {
                portENTER_CRITICAL( &mux );
                auto r = _position;
                portEXIT_CRITICAL( &mux );
                return r;
            }

            void set( countSigned position ) {
                portENTER_CRITICAL( &mux );
                this->_position = position;
                portEXIT_CRITICAL( &mux );
            }

            inline void reset() { set( 0 ); }

            void delta( countSigned value ) {
                portENTER_CRITICAL( &mux );
                _position += value;
                portEXIT_CRITICAL( &mux );
            }

            void increment() {
                portENTER_CRITICAL( &mux );
                _position++;
                portEXIT_CRITICAL( &mux );
            }

            void decrement() {
                portENTER_CRITICAL( &mux );
                _position--;
                portEXIT_CRITICAL( &mux );
            }

        //
        // Z SYNC
        //
        public:

            void setZPin( uint8_t zPin, bool activeState=HIGH ) {
                this->zPin = zPin;
                setPinZ_activeState( activeState );
            }

            void setZPin( uint8_t zPin, openCollectorType openCollector ) {
                this->zPin = zPin;
                setPinZ_OpenCollectorType( openCollector );
            }

            void setupZSync( uint16_t encoderPPR, int32_t syncValue ) {
                // when Z in hit, encoder count must be syncValue +/- multiple of encoder PPR
                //
                //      sVal-2*PPR    sVal-1*PPR    sVal          sVal+1*PPR    sVal+2*PPR
                //   ---|-------------|-------------|-------------|-------------|-------------|---
                //      Z             Z             Z             Z             Z             Z
                //
                this->encoderPPR = encoderPPR;
                this->syncValue = syncValue;
            }

            // void setupZSync( uint8_t zPin, uint16_t encoderPPR, int32_t syncValue ) {
            //     setupZSync( zPin, encoderPPR, syncValue, HIGH );
            // }

            void setupZSync_ORIG( uint8_t zPin, uint16_t encoderPPR, int32_t syncValue, openCollectorType openCollector ) {
                // when Z in hit, encoder count must be syncValue +/- multiple of encoder PPR
                //
                //      sVal-2*PPR    sVal-1*PPR    sVal          sVal+1*PPR    sVal+2*PPR
                //   ---|-------------|-------------|-------------|-------------|-------------|---
                //      Z             Z             Z             Z             Z             Z
                //
                this->zPin = zPin;
                this->encoderPPR = encoderPPR;
                this->syncValue = syncValue;
                setPinZ_OpenCollectorType( openCollector );
                // setOpenCollectorInput( zPin, openCollector );
                // this->zPin_activeState = getOpenCollectorActiveState( openCollector );
            }

            void setupZSync_ORIG( uint8_t zPin, uint16_t encoderPPR, int32_t syncValue, bool activeState ) {
                // when Z in hit, encoder count must be syncValue +/- multiple of encoder PPR
                //
                //      sVal-2*PPR    sVal-1*PPR    sVal          sVal+1*PPR    sVal+2*PPR
                //   ---|-------------|-------------|-------------|-------------|-------------|---
                //      Z             Z             Z             Z             Z             Z
                //
                this->zPin = zPin;
                this->encoderPPR = encoderPPR;
                this->syncValue = syncValue;
                setPinZ_activeState( activeState );
            }

            void setPPR( uint16_t encoderPPR ) {
                this->encoderPPR = encoderPPR;
            }
            uint16_t getPPR() {
                return encoderPPR;
            }

            void setPinZ_OpenCollectorType( openCollectorType openCollector ) {
                setOpenCollectorInput( zPin, openCollector );
                this->zPin_activeState = getOpenCollectorActiveState( openCollector );
            }
            openCollectorType getPinZ_OpenCollectorType() {
                return getOpenCollectorActiveState( zPin_activeState );
            }

            void setPinZ_activeState( uint8_t zPin_activeState ) {
                auto pinZ_OC = pinA_activeState == HIGH ? openCollectorType::NPN : openCollectorType::PNP;
                setOpenCollectorInput( zPin, pinZ_OC );
                this->zPin_activeState = getOpenCollectorActiveState( pinZ_OC );
            }
            uint8_t getPinZ_activeState() {
                return zPin_activeState;
            }

/*
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
                setOpenCollector( zPin, zPin_activeState );
                // pinMode( zPin, INPUT );
                // portENTER_CRITICAL( &mux );
                // syncOnZ = true;
                // portEXIT_CRITICAL( &mux );
            }

            void setPinZ_activeState( uint8_t zPin_activeState = HIGH ) {
                this->zPin_activeState = zPin_activeState;
            }

            void pinZ_openCollectorMode( uint8_t zPin ) {
                this->zPin = zPin;
                setOpenCollector( zPin, zPin_activeState );
            }

            void pinZ_openCollectorMode() {
                setOpenCollector( zPin, zPin_activeState );
                // same as pinAB_openCollectorMode()
                // if ( zPin_activeState == HIGH )
                //     pinMode( zPin, INPUT_PULLUP );
                // else {
                //     #if defined(ESP32)
                //         pinMode( zPin, INPUT_PULLDOWN );
                //     #else
                //         // pinMode( zPin, INPUT );
                //     #endif
                // }
            }
*/

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

            bool getZSyncStatus() {
                portENTER_CRITICAL( &mux );
                auto r = syncOnZ;
                portEXIT_CRITICAL( &mux );
                return r;
            }

            void setZSyncStatus( bool enable ) {
                portENTER_CRITICAL( &mux );
                syncOnZ = enable;
                portEXIT_CRITICAL( &mux );
            }

            uint64_t getMissingPulsesAbsolute() {
                portENTER_CRITICAL( &mux );
                auto r = missingPulsesAbsolute;
                portEXIT_CRITICAL( &mux );
                return r;
            }

            int64_t getMissingPulses() {
                portENTER_CRITICAL( &mux );
                auto r = missingPulses;
                portEXIT_CRITICAL( &mux );
                return r;
            }

            void clearMissingPulses() {
                portENTER_CRITICAL( &mux );
                missingPulsesAbsolute = 0;
                missingPulses = 0;
                portEXIT_CRITICAL( &mux );
            }

            bool readZPinRaw() {
                return ( digitalRead( zPin ) == HIGH );
            }

            bool readZPin() {
                return ( digitalRead( zPin ) == zPin_activeState );
            }

        protected:

            bool     syncOnZ = false;
            uint8_t  zPin;
            uint8_t  zPin_activeState;
            uint16_t encoderPPR = 100;
            int32_t  syncValue = 0;

            uint64_t missingPulsesAbsolute = 0;
            int64_t  missingPulses = 0;

            inline void syncZCore() {
                //Serial.println( "sync" );
                if ( encoderPPR == 0 ) return;

                auto adjusted = _position - syncValue;
                auto stray = adjusted % encoderPPR;
                if ( stray == 0 ) return;

                if ( stray >= 0 ) {
                    // + stray
                    if ( stray >= ( encoderPPR >> 1 ) ) {
                        // stray >= (encoderPPR/2)
                        auto diff = encoderPPR - stray;
                        missingPulsesAbsolute += diff;
                        missingPulses += diff;
                        _position += diff;
                    } else {
                        missingPulsesAbsolute += stray;
                        missingPulses -= stray;
                        _position -= stray;
                    }
                } else {
                    // - stray
                    if ( stray <= -( encoderPPR >> 1 ) ) {
                        // stray <= -(encoderPPR/2)
                        auto diff = encoderPPR + stray;
                        missingPulsesAbsolute += diff;
                        missingPulses -= diff;
                        _position -= diff;
                    } else {
                        missingPulsesAbsolute -= stray;
                        missingPulses -= stray;
                        _position -= stray;
                    }
                }
            }

        //
        // TEST
        //
        protected:

            friend void TEST_encoderTriggerSyncZ( spEncoder &e );

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

    void TEST_encoderTriggerSyncZ( spEncoder &e ) {
        e.syncZCore();
    }

    void TEST_encoderZSyncCore( int32_t currentPosition, int32_t ppr, int32_t zSync, int32_t expected ) {
        spEncoder e( 0, 0, spEncoder::openCollectorType::NPN );
        e.setZPin( 0 );
        e.setupZSync( ppr, zSync );
        // e.setupZSync( 0, ppr, zSync, spEncoder::openCollectorType::NPN );
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

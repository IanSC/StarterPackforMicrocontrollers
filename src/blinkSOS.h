#pragma once
#include <stdint.h>
#include <Arduino.h>

namespace StarterPack {

class blinkSOS {

    private:

        uint8_t  PIN;
        bool     play = false;
        uint8_t  bitPosition = 0;
        
        uint32_t lastAction;
        uint16_t nextDuration;

        // SOS Pattern  . . .   --- --- ---   . . .
        // on/off       101010  1  01  01  0  1010100
        // 1 or 3 sec   000001  1  01  01  1  0000011
        const uint32_t onOffBitsPattern    = 0B0010101010101010101;
        const uint32_t durationBitsPattern = 0B1100000110101100000;

        //  ON  DUR 
        //  --  --- --------------------
        //  0   0   off - short duration
        //  0   1   off - long  duration
        //  1   0   on  - short duration
        //  1   1   on  - long  duration

    public:
    
        #define DOT_DURATION   200
        #define DASH_DURATION  DOT_DURATION*3

        #if defined( ESP32 )
            blinkSOS( uint8_t pin = 2 ) {
                PIN = pin;
            }
        #else
            blinkSOS( uint8_t pin = 13 ) {
                PIN = pin;
            }
        #endif

        void start() {
            bitPosition = 0;
            play = true;
        }
        void stop() {
            play = false;
        }

        void update() {
            if ( !play ) return;
            if ( bitPosition == 0 ) {
                lastAction = millis();
                // durationBits = durationBitsPattern;
                // onOffBits    = onOffBitsPattern;
                bitPosition = 1;
                digitalWrite( PIN, HIGH );
                nextDuration = DOT_DURATION;
            } else {
                uint32_t now = millis();
                if ( now - lastAction > nextDuration ) {
                    if ( bitPosition >= 20 ) {
                        bitPosition = 0;
                        return;
                    }

                    uint32_t bitMask = 1 << ( bitPosition - 1 );

                    if ( ( durationBitsPattern & bitMask ) != 0 )
                        nextDuration = DASH_DURATION;
                    else
                        nextDuration = DOT_DURATION;

                    if ( ( onOffBitsPattern & bitMask ) != 0 )
                        digitalWrite( PIN, HIGH );
                    else
                        digitalWrite( PIN, LOW );

                    // durationBits = durationBits >> 1;
                    // if ( ( durationBits & 1 ) == 1 )
                    //     nextDuration = DASH_DURATION;
                    // else
                    //     nextDuration = DOT_DURATION;
                    // onOffBits = onOffBits >> 1;
                    // if ( ( onOffBits & 1 ) == 1 )
                    //     digitalWrite( PIN, HIGH );
                    // else
                    //     digitalWrite( PIN, LOW );

                    bitPosition++;
                    lastAction = now;
                }
            }
        }

};

void blinkSOSForever() {
    blinkSOS b;
    for(;;) b.update();
}

}

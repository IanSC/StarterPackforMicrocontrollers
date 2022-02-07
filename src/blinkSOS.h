#ifndef BLINK_SOS_H
#define BLINK_SOS_H

class blinkSOSclass {

    public:
    
        blinkSOSclass() {}

        #define DOT_DURATION   200
        #define DASH_DURATION  DOT_DURATION*3

        byte PIN = 13;

        void start() {
            bitPosition = 0;
            play = true;
        }
        void stop() {
            play = false;
        }

        void loop() {
            if ( !play ) return;
            if ( bitPosition == 0 ) {
                lastAction = millis();
                durationBits = durationBitsPattern;
                onOffBits    = onOffBitsPattern;
                bitPosition = 1;
                digitalWrite( PIN, HIGH );
                nextDuration = DOT_DURATION;
                //Serial.println( "START" );
            } else {
                uint32_t now = millis();
                if ( now - lastAction > nextDuration ) {
                    if ( bitPosition >= 20 ) {
                        bitPosition = 0;
                        return;
                    }

                    /*
                    Serial.print( nextDuration );
                    if ( ( onOffBits & 1 ) == 1 )
                        Serial.println( " ON" );
                    else
                        Serial.println( " OFF" );
                    */
                    
                    durationBits = durationBits >> 1;
                    if ( ( durationBits & 1 ) == 1 )
                        nextDuration = DASH_DURATION;
                    else
                        nextDuration = DOT_DURATION;
                 
                    onOffBits = onOffBits >> 1;
                    if ( ( onOffBits & 1 ) == 1 )
                        digitalWrite( PIN, HIGH );
                    else
                        digitalWrite( PIN, LOW );

                    bitPosition++;
                    lastAction = now;
                }
            }
        }

        bool     play = false;

    private:

        int8_t   bitPosition = 0;
        
        uint32_t durationBits;
        uint32_t onOffBits;
        uint32_t lastAction;
        uint16_t nextDuration;
        
        //              . . .   --- --- ---   . . .
        // on/off       101010  1  01  01  0  1010100
        // 1 or 3 sec   000001  1  01  01  1  0000011
        const uint32_t onOffBitsPattern    = 0B0010101010101010101;
        const uint32_t durationBitsPattern = 0B1100000110101100000;

};

blinkSOSclass blinkSOS = blinkSOSclass();

#endif

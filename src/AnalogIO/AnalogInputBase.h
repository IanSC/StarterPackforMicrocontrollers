#pragma once

#include <Arduino.h>
#include <spUtility.h>

#if defined(ESP32)
    // FOR ESP32 NATIVE READ
    #include <AnalogIO/AnalogInputBase_ESP32.h>
#endif

namespace StarterPack {

#if defined(ESP32)
class AnalogInputBase : protected AnalogInputBase_ESP32
#else
class AnalogInput
#endif
{

    protected:

        uint8_t PIN;

    public:

        virtual ~AnalogInputBase() {}

        AnalogInputBase( uint8_t pin ) {
            PIN = pin;
            pinMode( PIN, INPUT );
            #if defined(ESP32)
                mapGPIOtoADC(pin);
            #endif
        }

    public:

        inline int readRaw() {
            #if defined(ESP32)
                // ESP IDF style
                return readESPStyle();
            #else
                // Arduino style
                return analogRead( PIN );
            #endif
        }

    //
    // TEST ONLY
    //
    public:

        void TEST_showAverageToSerial( uint16_t unpressedValue = 0 ) {

            uint32_t total = 0;
            uint16_t cnt = 0;

            while( true ) {
                auto v = readRaw();
                if ( v == unpressedValue ) {
                    total = 0;
                    cnt = 0;
                } else {
                    total += v;
                    cnt++;
                    // Serial.print( analogRead(15) );
                    Serial.print( "average: " );
                    Serial.println( (float) total / (float) cnt );
                }
                delay( 20 );
            }
        }

};

}

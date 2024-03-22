#pragma once

#include <Arduino.h>
#include <spWDT.h>

// FOR ESP32 NATIVE READ
#include <AnalogIO/AnalogInputRaw_ESP32.h>

// #define SP_ANALOGINPUT_SKIP_ESP32_ADC

// #if defined(ESP32)
//     #define SP_ANALOGINPUT_USE_ESP32_ADC
// #endif

// #if defined(SP_ANALOGINPUT_SKIP_ESP32_ADC)
//     #undef SP_ANALOGINPUT_USE_ESP32_ADC
// #endif

// #if defined(SP_ANALOGINPUT_USE_ESP32_ADC)
//     // FOR ESP32 NATIVE READ
//     #include <AnalogIO/AnalogInputRaw_ESP32.h>
// #endif

namespace StarterPack {

#if defined(SP_ANALOGINPUT_USE_ESP32_ADC)
class AnalogInputRaw : protected AnalogInputRaw_ESP32
#else
class AnalogInputRaw
#endif
{

    public:

        // same as analogRead()
        typedef int DATA_TYPE;

    protected:

        int8_t PIN;

    public:

        AnalogInputRaw( int8_t pin ) {
            PIN = pin;
            pinMode( PIN, INPUT );
            #if defined(SP_ANALOGINPUT_USE_ESP32_ADC)
                ESP32_mapGPIOtoADC(pin);
            #endif
        }

        // virtual ~AnalogInputRaw() {}

    public:

        inline DATA_TYPE readRaw() {
            #if defined(SP_ANALOGINPUT_USE_ESP32_ADC)
                // ESP IDF style
                return ESP32_readADC();
            #else
                // Arduino style
                return analogRead( PIN );
            #endif
        }

    //
    //
    //
    public:

        void getKeyAverage( uint8_t keyCount, DATA_TYPE * values ) {
            for( int i=0 ; i<keyCount ; i++ ) {
                Serial.print( "key " ); Serial.print( i+1 ); Serial.print( " : " );

                // wait for release
                while (1) {
                    feedTheDog();
                    auto v = readRaw();
                    if ( v == 0 )
                        break;
                    delay( 20 );
                }

                values[i] = getStabilizedValue();

                Serial.println( values[i] );
            }
        }

    protected:

        DATA_TYPE getStabilizedValue() {
            uint32_t total = 0;
            uint16_t cnt = 0;
            DATA_TYPE avg = 0;
            uint8_t consistentCount = 0;
            while( true ) {

                feedTheDog();

                // wait for press
                auto v = readRaw();
                delay( 20 );
                if ( v == 0 ) continue;

                // check stability
                total += v;
                cnt++;
                DATA_TYPE prevAvg = avg;
                avg = (float) total / (float) cnt;
                if ( prevAvg != avg ) {
                    consistentCount = 0;
                    continue;
                }

                // consistently stable
                consistentCount++;
                if ( consistentCount < 50 )
                    continue;

                break;
            }
            return avg;
        }

    //
    // TEST ONLY
    //
    public:

        void TEST_showAverageToSerial( DATA_TYPE unpressedValue = 0 ) {

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

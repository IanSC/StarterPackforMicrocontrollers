#pragma once

#include <UserInterface.h>

#include <AnalogIO/AnalogInputRaw.h>

namespace StarterPack {

class AnalogInputRaw_TEST {

    typedef StarterPack::AnalogInputRaw::DATA_TYPE DATA_TYPE;

//
// CALIBRATION
//
public:

    static void TEST_calibrateKeypadSerial( StarterPack::AnalogInputRaw * input, int keyCount=16 ) {
        // read series of keys to get keyValues
        // eg. key1=adc 100, key2=adc 200
        // auto keyCount = 16;
        DATA_TYPE keyValues[keyCount+1];
        keyValues[0] = 0;
        calibrateKeypadSerialCore( input, keyCount, keyValues+1 );
        Serial.print( "int keyValues[] = { " );
        for ( int i=0 ; i<keyCount+1 ; i++ ) {
            Serial.print( keyValues[i] );
            if ( i<keyCount )
                Serial.print( ", " );
        }
        Serial.println( " };" );
    }

protected:

    static void calibrateKeypadSerialCore( StarterPack::AnalogInputRaw * input, uint8_t keyCount, DATA_TYPE * keyValuesNoZero ) {
        for( int i=0 ; i<keyCount ; i++ ) {
            Serial.print( "key " ); Serial.print( i+1 ); Serial.print( " : " );

            // wait for release
            while (1) {
                feedTheDog();
                auto v = input->readRaw();
                if ( v == 0 )
                    break;
                delay( 20 );
            }
            keyValuesNoZero[i] = input->getStabilizedValue();
            Serial.println( keyValuesNoZero[i] );
        }
    }

//
// CALIBRATE LCD
//
public:

    static void TEST_calibrateKeypadLCD( StarterPack::AnalogInputRaw * input, int keyCount, DATA_TYPE * keyValuesWithZero ) {
        // PROBLEM:  ADC readings if powered via USB and powered by VIN +5v differs
        // SOLUTION: read keypad ADC while USB is not connected, hence display via screen
        //           save to EEPROM to avoid reprogramming in case of drift
        // BETTER:   convert to 1-direction uart TX/RX instead of ADC
        namespace ui = StarterPack::UserInterface;

        keyValuesWithZero[0] = 0;
        auto keyValuesNoZero = keyValuesWithZero+1;

        ui::LCD->clear();
        for( int i=0 ; i<keyCount ; i++ ) {
            ui::LCD->printAt( 0, 0, "Press Key " );
            ui::LCD->print( i+1 );
            ui::LCD->refresh();
            // wait for release
            while (1) {
                feedTheDog();
                auto v = input->readRaw();
                if ( v == 0 )
                    break;
                delay( 20 );
            }
            keyValuesNoZero[i] = input->getStabilizedValue();
            // ui::LCD->printAt( 0, 1, keyValuesNoZero[i] );
            ui::LCD->printAt( 0, 1, "  Key " );
            ui::LCD->print( i+1 );
            ui::LCD->print( " = " );
            ui::LCD->print( keyValuesNoZero[i] );
            ui::LCD->print( "    " );
            ui::LCD->refresh();
        }
    }

    static void TEST_calibrateKeypadLCD_DisplayResult( int keyCount, DATA_TYPE * keyValuesWithZero ) {
        namespace ui = StarterPack::UserInterface;

        keyValuesWithZero++;

        ui::LCD->clear();
        uint8_t row = 0, col = 0;
        for( int i=0 ; i<keyCount ; i++ ) {
            ui::LCD->printAt( col, row, keyValuesWithZero[i] );
            col += 5; if ( col > 15 ) { col = 0; row++; }
        }
        ui::LCD->refresh();
    }

//
// TEST ONLY
//
public:

    static void TEST_showAverageToSerial( StarterPack::AnalogInputRaw * input, DATA_TYPE unpressedValue = 0 ) {

        int lastReading = 0;

        while( true ) {
            feedTheDog();
            auto v = input->readRaw();
            if ( v == unpressedValue ) {
                lastReading = 0;
            } else {
                lastReading = 0.5 * lastReading + 0.5 * v;
                Serial.print( "reading: " );
                Serial.println( lastReading );
            }
            delay( 20 );
        }

        // TOO SLOW TO DISPLAY ACTUAL VALUE
        // uint32_t total = 0;
        // uint16_t cnt = 0;

        // while( true ) {
        //     feedTheDog();
        //     auto v = readRaw();
        //     if ( v == unpressedValue ) {
        //         total = 0;
        //         cnt = 0;
        //     } else {
        //         total += v;
        //         cnt++;
        //         // Serial.print( analogRead(15) );
        //         Serial.print( "average: " );
        //         Serial.println( (float) total / (float) cnt );
        //     }
        //     // delay( 20 );
        // }
    }

};

}

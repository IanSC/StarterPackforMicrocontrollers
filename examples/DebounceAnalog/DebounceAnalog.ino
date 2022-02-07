//=======================================
//  StarterPack Debounce Analog Example
//  2022-01 isc
//=======================================
//
//  Debounce Demo
//  -------------
//  - demo to debounce any signal
//
//  Circuit
//  -------
//  - potentiometer
//  - connect center of potentiometer to A1
//  - connect the other two ends to VCC and GND
//  - use Serial Plotter

#include <StarterPack.h> // include all
#include <AnalogIO.h>    // ... or only those used in this project
#include <Debouncer.h>
using namespace StarterPack;

#if defined( ESP32 )
    int pin = 15;
#else
    int pin = A1;
#endif

AnalogIO aIO( pin, AnalogIO::SMOOTHING );

Debouncer db;

void setup() {
    Serial.begin( 115200 );
    Serial.println( "hello" );

    // affects all debouncers (global setttings)
    //Debouncer::defaultSettings.activeStatesDebounceInMs = 1000;

    // for specific debouncers only
    db.useCustomSettings();
    db.getSettings()->activeStatesDebounceInMs = 1000;    
}

void loop() {

    int potValue  = aIO.read();
    int debounced = db.debounce( potValue );
    int potMax    = aIO.getAnalogResolution();
    
    Serial.print( potValue );
    Serial.print( "\t" );
    Serial.print( debounced );
    Serial.print( "\t" );
    Serial.print( potMax );
    Serial.print( "\t" );
    Serial.print( 0 );
    Serial.println();
    delay(50);
}

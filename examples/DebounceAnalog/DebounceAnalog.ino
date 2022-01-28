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

AnalogIO aIO = AnalogIO( A1, AnalogIO::SMOOTHING );
Debouncer db = Debouncer();

void setup() {
    Serial.begin( 115200 );
    Serial.println( "hello" );

    // this only affects debouncing of anoalog buttons
    // not analog signals
    //aIO.debouncer.activeStatesDebounceInMs = 1000;

    db.activeStatesDebounceInMs = 1000;
}

void loop() {

    int potValue  = aIO.read();
    int debounced = db.debounce( potValue );
    int potMax    = aIO.analogResolution;
    
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

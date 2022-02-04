//================================
//  StarterPack AnalogIO Example
//  2022-01 isc
//================================
//
//  AnalogIO
//  --------
//  - demo of analog input
//
//  Circuit
//  -------
//  - potentiometer
//  - connect center of potentiometer to A1
//  - connect the other ends to VCC and GND
//  - use Serial Plotter

#include <StarterPack.h> // include all
#include <AnalogIO.h>    // ... or only those used in this project
using namespace StarterPack;

#if defined(ESP32)
    AnalogIO aIO( 15, AnalogIO::SMOOTHING );
#else
    AnalogIO aIO( A1, AnalogIO::SMOOTHING );
#endif

int map( int value ) {
    return map( value, 0, 1023, 0, 100 );
}

void onChange( int value ) {
    // this will fire, but sending to Serial
    // will mess up Serial plotter
    //Serial.print( "changed: " );
    //Serial.println( value );
}

void setup() {
    Serial.begin( 115200 );
    Serial.println( "hello" );

    aIO.setMappingFunction( &map );
    aIO.setOnChangeCallback( &onChange );
}

void loop() {

    int value       = aIO.read();
    int rawValue    = aIO.rawValue;
    int smoothValue = aIO.smoothValue;
    int potMax      = aIO.getAnalogResolution();
    
    Serial.print( value );
    Serial.print( "\t" );
    Serial.print( rawValue );
    Serial.print( "\t" );
    Serial.print( smoothValue );
    Serial.print( "\t" );
    Serial.print( potMax );
    Serial.print( "\t" );
    Serial.print( 100 );
    Serial.print( "\t" );
    Serial.print( 0 );
    Serial.println();
    delay(50);
}

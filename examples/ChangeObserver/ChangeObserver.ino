//======================================
//  StarterPack ChangeObserver Example
//  2022-01 isc
//======================================
//
//  - demo of change observer
//
//  Circuit
//  -------
//  - potentiometer
//  - connect center of potentiometer to A1
//  - connect the other ends to VCC and GND
//  - use Serial Plotter

#include <StarterPack.h>
using namespace StarterPack;

void Show( int a ) {
    Serial.print( a );
    Serial.print( "\t100\t0\n" );
}

ChangeObserver chg( &Show );

void setup() {
    Serial.begin(115200);
}

void loop() {
    #if defined( ESP32 )
        int value = analogRead( 15 );
    #else
        int value = analogRead( A1 );
    #endif
    value = map( value, 0, 1023, -2, 102 );
    if ( value < 0 ) value = 0;
    if ( value > 100 ) value = 100;
    chg.update( value );
}

//================================
//  StarterPack Throttle Example
//  2022-01 isc
//================================
//
//  Throttle
//  --------
//  - throttle executing a function to avoid
//    overwhelming resources

#include <StarterPack.h> // include all
#include <Throttle.h>    // ... or only those used in this project
using namespace StarterPack;

int counter = 0;

void heavyFunction( int payload ) {
    Serial.println();
    Serial.print( counter );
    counter++;
    Serial.print( ": " );
    Serial.print( payload );
    Serial.print( " / " );
    for( int i=0 ; i<=50; i++ )
        Serial.print( i );
    Serial.println();
}

void A()        { Serial.print( "#" ); }
void B(bool v)  { Serial.print( "*" ); }
void C(int v)   { Serial.print( "-" ); }
void D(float v) { Serial.print( "." ); }

Throttle tt = Throttle( &heavyFunction, 1000 );
Throttle a = Throttle( &A, 100 );
Throttle b = Throttle( &B, 200 );
Throttle c = Throttle( &C, 300 );
Throttle d = Throttle( &D, 400 );

void setup() {
    Serial.begin( 115200 );
    Serial.println( "hello" );
    Serial.println( __cplusplus );
}

void loop() {

    // RIP serial port
    // this will overwhelm the serial port
    // heavyFunction( 123 );
    
    tt.trigger( 123 );
    a.trigger();
    b.trigger(true);
    c.trigger(123);
    d.trigger(123.456);
}

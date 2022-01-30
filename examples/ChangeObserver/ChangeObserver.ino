#include <StarterPack.h>
using namespace StarterPack;

void Show( int a ) {
    Serial.print( a );
    Serial.print( "\t100\t0\n" );
}

ChangeObserver chg = ChangeObserver( &Show );

void setup() {
    Serial.begin(115200);
}

void loop() {
    int value = analogRead( A1 );
    value = map( value, 0, 1023, -2, 102 );
    if ( value < 0 ) value = 0;
    if ( value > 100 ) value = 100;
    chg.update( value );
}

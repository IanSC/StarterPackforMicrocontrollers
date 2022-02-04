//=================================
//  StarterPack DigitalIO Example
//  2022-01 isc
//=================================
//
//  Circuit
//  -------
//  - 4 buttons
//  - connect button1 to pin 2 and ground
//  - connect button2 to pin 3 and ground
//  - connect button3 to pin 4 and ground
//  - connect button4 to pin 5 and ground
//  - or just a piece of wire and touch pins 2, 3, 4 or 5

#include <StarterPack.h> // include all
#include <DigitalIO.h>   // ... or only those used in this project
using namespace StarterPack;

DigitalIO dIO1( 2, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
DigitalIO dIO2( 3, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
DigitalIO dIO3( 4, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
DigitalIO dIO4( 5, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );

void setup() {
    Serial.begin( 115200 );
    Serial.println( "hello" );
    // to play around:
    //dIO3.setRepeatDelayInMs( 1000 ); // default 400
    //dIO3.setRepeatRateInMs (  500 ); // default 250
}

int r1Count = 0;
int r2Count = 0;
int r3Count = 0;
int r4Count = 0;

void loop() {

    bool r1 = dIO1.getKeyDown();       // detect when key is pressed
    bool r2 = dIO2.getKeyUp();         // detect when key is released
    bool r3 = dIO3.getRepeatingKey();  // get repeated results while pressed
    bool r4 = dIO4.getContinuousKey(); // get continuous feedback

    if ( r1 ) r1Count++;
    if ( r2 ) r2Count++;
    if ( r3 ) r3Count++;
    if ( r4 ) r4Count++;

    Serial.print( r1Count );
    Serial.print( " / " );
    Serial.print( r2Count );
    Serial.print( " / " );
    Serial.print( r3Count );
    Serial.print( " / " );
    Serial.println( r4Count );
    delay(10);
}

//========================================
//  StarterPack Debounce Digital Example
//  2022-01 isc
//========================================
//
//  Debounce Buttons
//  ----------------
//  - debounce buttons to get clean signal
//
//  Circuit
//  -------
//  - 2 pcs of wires
//  - connect one end of wire 1 to IO PIN 2
//  - connect one end of wire 2 to Ground
//  - touch the free ends of the wires to each other
//    to simulate pressing a button

#include <StarterPack.h> // include all
#include <AnalogIO.h>    // ... or only those used in this project
#include <Debouncer.h>
using namespace StarterPack;

// we usually associate HIGH as being active/pressed
// but Arduino only has pullup, so to detect presses
// we need to connect to ground, which is LOW
// defining these to make the example clearer
#define NOT_PRESSED HIGH
#define PRESSED     LOW

#define PIN         2

Debouncer db1 = Debouncer();
Debouncer db2 = Debouncer();

bool r0PreviousState;
bool r1PreviousState;

void setup() {
    Serial.begin( 115200 );
    Serial.println( "hello" );

    pinMode( PIN, INPUT_PULLUP );

    // if not pressed, state is HIGH
    db1.inactiveState = NOT_PRESSED;
    db2.inactiveState = NOT_PRESSED;

    // initialize debouncers
    int initialState = digitalRead( PIN );
    db1.setInitialValue( initialState );
    db2.setInitialValue( initialState );

    // setup tracking variables
    r0PreviousState = initialState;
    r1PreviousState = initialState;

    // to play around:
    // db2.activeStatesDebounceInMs = 100;
    // db2.inactiveStateDebounceInMs = 100;
    // db2.confirmStateInMs = 50;
    // db2.deactivate();
    // db2.activate();
}

int r0Count = 0;
int r1Count = 0;
int r2Count = 0;

void loop() {

    int buttonStatus = digitalRead( PIN );
    
    // count the number of times "button" is pressed
    bool r0 = buttonStatus;
    if ( r0 != r0PreviousState ) {
        r0PreviousState = r0;
        if ( r0 == PRESSED )
            r0Count++;
    }

    // ... with debounce
    bool r1 = db1.debounce( buttonStatus );
    if ( r1 != r1PreviousState ) {
        r1PreviousState = r1;
        if ( r1 == PRESSED )
            r1Count++;
    }

    // same with debounce but no need to keep
    // track of previous status
    bool r2 = db2.debounce( buttonStatus );
    if ( r2 == PRESSED ) {
        // db2 will not report as being pressed
        // until it is released then pressed again
        db2.flagWaitForKeyup();
        r2Count++;
    }
    
    Serial.print( r0Count );
    Serial.print( " / " );
    Serial.print( r1Count );
    Serial.print( " / " );
    Serial.println( r2Count );
}

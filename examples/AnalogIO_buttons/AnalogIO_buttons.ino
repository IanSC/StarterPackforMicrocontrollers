//========================================
//  StarterPack AnalogIO Buttons Example
//  2022-01 isc
//========================================
//
//  Circuit
//  -------
//  - keyboard from LCD Keypad Shield v2.0 from DFRobot
//    https://wiki.dfrobot.com/LCD_Keypad_Shield_V2.0_SKU__DFR0374
//    https://dfimg.dfrobot.com/nobody/wiki/4b8cd9d63d06a834db015dadfc223749.pdf
//  - cheaper clones are available online
//    but the one from DFRobot is the best n quality and support
//  - v1.1 might be easier to use with easily accesible pinouts
//    v2.0 better when deploying inside a case
//    https://wiki.dfrobot.com/LCD_KeyPad_Shield_For_Arduino_SKU__DFR0009

#include <StarterPack.h> // include all
#include <AnalogIO.h>    // ... or only those used in this project
#include <Throttle.h>
using namespace StarterPack;

// analog resistor network keys are connected to A0
AnalogIO aIO = AnalogIO( A0 );

void setup() {
    Serial.begin( 115200 );
    Serial.println( "hello" );
    
    // particular board tested on:
    // 1022 - value when nothing is pressed
    //  834 - select key
    //  642 - left key
    //   14 - right key
    //  228 - up key
    //  430 - down key
    aIO.initButtons( 1022, 834, 642, 14, 228, 430 );
    
    // to play around:
    //aIO.setRepeatDelayInMs( 1000 ); // default 400
    //aIO.setRepeatRateInMs (  500 ); // default 250
}

int counter[6];
char name[6] = { 'X', 'S', 'L', 'R', 'U', 'D' };

void report() {
    // serial port might get overwhelmed
    for( int i=1 ; i<=5; i++ ) {
        Serial.print( " / " );
        Serial.print( name[i] );
        Serial.print( '=' );
        Serial.print( counter[i] );
    }
    Serial.println();
}

// ... so update only every 100ms
Throttle tt = Throttle( &report, 100 );

void loop() {

    byte buttonNo;

    // try each case: 1, 2, 3 or 4
    byte caseNo = 1;

    switch( caseNo ) {
    case 1:
        // detect when key is pressed
        buttonNo = aIO.getKeyDown();
        break;
    case 2:
        // detect when key is released
        buttonNo = aIO.getKeyUp();
        break;
    case 3:
        // get repeated results while pressed
        buttonNo = aIO.getRepeatingKey();
        break;
    case 4:
        // get continuous feedback
        buttonNo = aIO.getContinuousKey();
        break;
    }

    // R.I.P. counter[0]...
    counter[buttonNo]++;

    tt.trigger();
}

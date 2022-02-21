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

namespace ui = StarterPack::UserInterface;

// analog resistor network keys are connected to A0
AnalogIO aIO( A0 );

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

    // 5 buttons, so 5 keys
    // don't forget the null
    constexpr char map[] = { ui::cENTER, ui::cLEFT, ui::cRIGHT, ui::cUP, ui::cDOWN, NULL };
    aIO.assignKeymap( map );
    // aIO.assignKeymap( ui::keyMap( ui::cENTER, ui::cLEFT, ui::cRIGHT, ui::cUP, ui::cDOWN ) );

    // assign to global key handler used is used by other modules
    // such as LCDUtility, LCDMenu etc.
    ui::assignKeyHandler( aIO );
    
    // to play around:
    //aIO.setRepeatDelayInMs( 1000 ); // default 400
    //aIO.setRepeatRateInMs (  500 ); // default 250
}

void loop() {

    uint8_t key = ui::getKeyDown();

    // try each case: 1, 2, 3 or 4
    uint8_t caseNo = 1;

    switch( caseNo ) {
    case 1:
        // get continuous feedback
        key = ui::getContinuousKey();
        break;
    case 2:
        // detect when key is pressed
        key = ui::getKeyDown();
        break;
    case 3:
        // detect when key is released
        key = ui::getKeyUp();
        break;
    case 4:
        // get repeated results while pressed
        key = ui::getRepeatingKey();
        break;
    }

    switch( key ) {
    case ui::cENTER: Serial.println( "time to enter" ); break;
    case ui::cLEFT:  Serial.println( "sudden left" ); break;
    case ui::cRIGHT: Serial.println( "sudden right" ); break;
    case ui::cUP:    Serial.println( "somethings coming up" ); break;
    case ui::cDOWN:  Serial.println( "going downtown" ); break;
    }
}

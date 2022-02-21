//==================================
//  StarterPack User Input Example
//  2022-01 isc
//==================================
//
//  User Input
//  ----------
//  - helper to handle input buttons from user
//  - digital, analog or matrix keypad buttons
//    see <DigitalIO.h>, <AnalogIO.h> and <MatrixKeypad.h>
//  - UserInput is global scope, no need to create

#include <StarterPack.h> // include all
#include <UserInterface.h>   // ... or only those used in this project
using namespace StarterPack;

namespace ui = StarterPack::UserInterface;

//
// TEST CASE
//

#define KEY_TYPE   1
// 1 - digital keys
// 2 - analog keys
// 3 - matrix keypad

#if KEY_TYPE == 1

    // digital buttons, pins 2 to 6
    DigitalIO okayKey ( 2, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
    DigitalIO leftKey ( 3, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
    DigitalIO rightKey( 4, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
    DigitalIO upKey   ( 5, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
    DigitalIO downKey ( 6, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );

    DigitalIOGroup dGroup;
    
#elif KEY_TYPE == 2

    // analog buttons from LCD Keypad Shield
    // cannot get diagonals, only 1 key at a time
    AnalogIO aIO( A0 );

#elif KEY_TYPE == 3

    // 4x3 Matrix Keypad
    MatrixKeypad keypad;

#endif

//
// SETUP
//

#define KEY_UP_LEFT     'n'
#define KEY_UP_RIGHT    'e'
#define KEY_DOWN_LEFT   'a'
#define KEY_DOWN_RIGHT  't'

void setup() {
    Serial.begin( 115200 );
    Serial.println( "hello" );

    #if KEY_TYPE == 1
    
        dGroup.add( okayKey,  ui::cENTER );
        dGroup.add( leftKey,  ui::cLEFT  );
        dGroup.add( rightKey, ui::cRIGHT );
        dGroup.add( upKey,    ui::cUP    );
        dGroup.add( downKey,  ui::cDOWN  );

        dGroup.addCombinedKey( ui::cUP,   ui::cLEFT,  KEY_UP_LEFT    );
        dGroup.addCombinedKey( ui::cUP,   ui::cRIGHT, KEY_UP_RIGHT   );
        dGroup.addCombinedKey( ui::cDOWN, ui::cLEFT,  KEY_DOWN_LEFT  );
        dGroup.addCombinedKey( ui::cDOWN, ui::cRIGHT, KEY_DOWN_RIGHT );

        ui::assignKeyHandler( dGroup );
    
    #elif KEY_TYPE == 2

        // particular board tested on:
        // 1022 - value when nothing is pressed
        //  834 - select key
        //  642 - left key
        //   14 - right key
        //  228 - up key
        //  430 - down key
        // assigned as button 0, 1, 2, 3, 4, 5 respectively
        aIO.initButtons( 1022, 834, 642, 14, 228, 430 );
        //aIO.assignKeymap( ui::cENTER ui::cENTER );

        // assign button numbers to okay, left, right, up, down
        UserInterface::assignKeyHandler( aIO );

    #elif KEY_TYPE == 3
    
        keypad.assignRows( 8, 7, 6, 5 );
        keypad.assignColumns( 4, 3, 2 );
        keypad.begin();

        // assign button numbers to okay, left, right, up, down
        UserInterface::assignKeys( keypad, 12, 4, 6, 2, 8 );
        
    #endif
}

//
// LOOP
//

int counter = 0;

void loop() {
    
    uint8_t r = 0;
    
    uint8_t caseNo = 1;
    // 1 - continuous press
    // 2 - single press
    // 3 - single press when released
    // 4 - repeated, except select key

    switch( caseNo ) {
    case 1:

        // keep sending results
        r = ui::getContinuousKey();
        break;

    case 2:
    
        // must release before next key is registered
        // must hit 2 keys at exactly the same time to get diagonal
        r = ui::getKeyDown();
        break;

    case 3:
    
        // reported when keys are released
        r = ui::getKeyUp();
        break;

    case 4:
        
        // throttled result when kept pressed, except for okay key
        // to also repeat okay key use getRepeatingKey( true );
        r = ui::getRepeatingKey();
        break;

    }
    
    if ( r != 0 ) {
        Serial.print( counter++ );
        Serial.print( " - " );
        switch( r ) {
        case ui::cENTER:     Serial.println( "Select" );    break;
        case ui::cLEFT:      Serial.println( "Left" );      break;
        case ui::cRIGHT:     Serial.println( "Right" );     break;
        case ui::cUP:        Serial.println( "Up" );        break;
        case ui::cDOWN:      Serial.println( "Down" );      break;
        case KEY_UP_LEFT:    Serial.println( "UpLeft" );    break;
        case KEY_UP_RIGHT:   Serial.println( "UpRight" );   break;
        case KEY_DOWN_LEFT:  Serial.println( "DownLeft" );  break;
        case KEY_DOWN_RIGHT: Serial.println( "DownRight" ); break;
        }
    }
}

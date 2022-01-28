//==================================
//  StarterPack User Input Example
//  2022-01 isc
//==================================
//
//  User Input
//  ----------
//  - helper to handle input buttons from user
//  - digital or analog buttons
//    see <DigitalIO.h> and <AnalogIO.h>
//  - global scope, no need to create

#include <StarterPack.h> // include all
#include <UserInput.h>   // ... or only those used in this project
using namespace StarterPack;

namespace ui = StarterPack::UserInput;

//
// TEST CASE
//

#define KEY_TYPE   2
// 1 - digital keys
// 2 - analog keys

#if KEY_TYPE == 1

    // digital buttons
    DigitalIO okayKey  = DigitalIO( 2, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
    DigitalIO leftKey  = DigitalIO( 3, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
    DigitalIO rightKey = DigitalIO( 4, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
    DigitalIO upKey    = DigitalIO( 5, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
    DigitalIO downKey  = DigitalIO( 6, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
    
#elif KEY_TYPE == 2

    // analog buttons from LCD Keypad Shield
    // cannot get diagonals, only 1 key at a time
    AnalogIO aIO = AnalogIO( A0 );

#endif

//
// SETUP
//

void setup() {
    Serial.begin( 115200 );
    Serial.println( "hello" );

    #if KEY_TYPE == 1
    
        UserInput::assignKeys( &okayKey, &leftKey, &rightKey, &upKey, &downKey );
        UserInput::allowDiagonals();
    
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

        // assign button numbers to okay, left, right, up, down
        UserInput::assignKeys( aIO, 1, 2, 3, 4, 5 );

    #endif
}

//
// LOOP
//

int counter = 0;

void loop() {
    
    ui::Keys r;
    
    uint8_t caseNo = 1;
    // 1 - single press
    // 2 - continuous press
    // 3 - repeated, except select key

    switch( caseNo ) {
    case 1:
    
        // must release before next key is registered
        // must hit 2 keys at exactly the same time to get diagonal
        r = ui::getKeyDown();
        break;

    case 2:

        // keep sending results
        r = ui::getContinuousKey();
        break;

    case 3:
        
        // throttled result when kept pressed, except for okay key
        // to also repeat okay key use getRepeatingKey( true );
        r = ui::getRepeatingKey();
        break;

    }
    
    if ( r != ui::Keys::None ) {
        Serial.print( counter++ );
        Serial.print( " - " );
        switch( r ) {
        case ui::Keys::Okay:      Serial.println( "Select" );    break;
        case ui::Keys::Left:      Serial.println( "Left" );      break;
        case ui::Keys::Right:     Serial.println( "Right" );     break;
        case ui::Keys::Up:        Serial.println( "Up" );        break;
        case ui::Keys::Down:      Serial.println( "Down" );      break;
        case ui::Keys::UpLeft:    Serial.println( "UpLeft" );    break;
        case ui::Keys::UpRight:   Serial.println( "UpRight" );   break;
        case ui::Keys::DownLeft:  Serial.println( "DownLeft" );  break;
        case ui::Keys::DownRight: Serial.println( "DownRight" ); break;
        }
    }
}

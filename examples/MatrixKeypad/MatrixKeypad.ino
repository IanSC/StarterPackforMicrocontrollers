//=====================================
//  StarterPack Matrix Keypad Example
//  2022-01 isc
//=====================================
//
//  Matrix Keypad
//  -------------
//
//  hardware: 4x3 Matrix Keypad
//            rows connected to pins 5,6,7,8
//            columns to pins 2,3,4

#include <StarterPack.h>  // include all
#include <MatrixKeypad.h> // ... or only those used in this project
using namespace StarterPack;

MatrixKeypad keypad;

//
// SETUP
//
    
    void setup() {
        Serial.begin( 115200 );
        while( !Serial );

        keypad.assignRows( 8, 7, 6, 5 );
        keypad.assignColumns( 4, 3, 2 );
        keypad.begin();

        // or sideways
        //keypad.assignColumns( 5, 6, 7, 8 );
        //keypad.assignRows( 4, 3, 2 );
        //keypad.begin();
    }

//
// LOOP
//

    uint32_t prevValue = -1;
    
    void loop() {

        uint32_t value;

        uint8_t CASE = 1;

        switch( CASE ) {
        // case 0:
        //     value = keypad.readRaw();
        //     break;
        case 1:
            value = keypad.getKeyDown();
            break;
        case 2:
            value = keypad.getKeyUp();
            break;
        case 3:
            value = keypad.getRepeatingKey();
            break;
        case 4:
            value = keypad.getContinuousKey();
            break;
        }
            
        if ( prevValue != value ) {
            prevValue = value;  
            if ( value == 0 )
                Serial.println( "   up" );
            else
                Serial.println( value );
        }
        delay(10);
    }

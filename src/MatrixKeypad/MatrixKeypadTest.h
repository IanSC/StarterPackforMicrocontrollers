#pragma once

#include <UserInterface.h>
#include <MatrixKeypad.h>

namespace StarterPack {

class MatrixKeypadSupport {
    
public:

    static void MatrixKeypadMappedTest(
    UserInterfaceDebounced & keypad,
    // MatrixKeypad &keypad,
    const char *keyMap,
    const char **keyName ) {

        namespace ui = StarterPack::UserInterface;
        auto lcd = ui::LCD;

        lcd->clear();
        lcd->print( "Keypad Test" );
        lcd->printStrAtRow( 3, "Cancel 3x to exit" );
        lcd->refresh();
    
        uint8_t cancelCount = 0;

        keypad.waitUntilNothingIsPressed();
        while( true ) {

            uint8_t key = keypad.waitForAnyKeyPressed();

            if ( key == ui::kESCAPE ) {
                cancelCount++;
                if ( cancelCount >= 3 )
                    break;
            } else if ( key != 0 ) {
                cancelCount = 0;
            }

            bool found = false;
            for( int keyIndex=0 ; keyIndex<strlen(keyMap) ; keyIndex++ ) {
                if ( keyMap[keyIndex] == key ) {
                    lcd->printStrAtRow( 1, keyName[keyIndex] );
                    found = true;
                    break;
                }
            }

            if ( !found ) {

                if ( key == 0 ) {} else 
                
                if ( key == ui::kENTER )
                    lcd->printStrAtRow( 1, "Enter" );
                else if ( key == ui::kESCAPE )
                    lcd->printStrAtRow( 1, "Cancel" );
                else if ( key == ui::kQUESTION )
                    lcd->printStrAtRow( 1, "Help" );

                else if ( key == ui::kBACKSPACE )
                    lcd->printStrAtRow( 1, "Backspace" );
                else if ( key == ui::kDELETE )
                    lcd->printStrAtRow( 1, "Delete" );
                else if ( key == ui::kINSERT )
                    lcd->printStrAtRow( 1, "Insert" );

                else if ( key == ui::kUP )
                    lcd->printStrAtRow( 1, "Up" );
                else if ( key == ui::kDOWN )
                    lcd->printStrAtRow( 1, "Down" );
                else if ( key == ui::kLEFT )
                    lcd->printStrAtRow( 1, "Left" );
                else if ( key == ui::kRIGHT )
                    lcd->printStrAtRow( 1, "Right" );

                else if ( key == ui::kDECIMAL )
                    lcd->printStrAtRow( 1, "." );
                else if ( key == ui::kMINUS )
                    lcd->printStrAtRow( 1, "-" );
                else if ( key == ui::kCHG_CASE )
                    lcd->printStrAtRow( 1, "Change Case" );
                else if ( key == ui::kNUMBERS )
                    lcd->printStrAtRow( 1, "Numbers" );
                else if ( key == ui::kSYMBOLS )
                    lcd->printStrAtRow( 1, "Symbols" );
                else {
                    cancelCount = 0;
                    lcd->clearRow( 1 );
                    lcd->writeAt( 0, 1, key );
                }
            }
            lcd->refresh();
            // lcd->refreshPartial();
        }
        keypad.waitUntilNothingIsPressed();
    }

};

}

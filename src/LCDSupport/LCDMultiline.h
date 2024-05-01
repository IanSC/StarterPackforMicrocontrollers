#pragma once

#include <UserInterface.h>
#include <spWDT.h>

#include <LCDSupport/LCDUtility.h>

namespace StarterPack::spLCD {

    //  Usage:
    //      const char* msg[] = {"Apple","Banana","..."};
    //      showMultiLineMsg(msg);
    //
    //  NOT WORKING, HOW TO ???:
    //  showMultiLineMsg((const char*[]) {"Apple","Banana","...",nullptr});

    static uint8_t showMultiLineMsgN( const char **msg, int msgCount, backgroundProcessDelegate func=nullptr ) {
        namespace ui = StarterPack::UserInterface;

        if ( !ui::hasScreen() ) return 0;

        ui::LCD->clear();
        int lineNo = 0;

        ui::waitUntilNothingIsPressed();
        bool update = true;

        while ( true ) {

            feedTheDog();
            // delay( 10 );

            // display text
            if (update) {
                int tmp = lineNo;
                for( int i = 0 ; i < ui::LCD->maxRows ; i++ ) {
                    if ( tmp < msgCount )
                        ui::LCD->printStrAtRow( i, msg[tmp] );
                    else
                        ui::LCD->clearRow( i );
                    tmp++;
                }
                if ( lineNo < msgCount - ui::LCD->maxRows ) {
                    // not last line, show marker
                    ui::LCD->printAt( 18, ui::LCD->maxRows-1, " \x7E" );
                }
                update = false;
            }
            ui::LCD->refresh();
            // ui::LCD->displayAll();

            uint8_t key = ui::getRepeatingKey();
            if ( key == ui::kENTER || key == ui::kESCAPE ) {
                ui::waitUntilNothingIsPressed();
                return key;
            }
            if ( key == ui::kUP || key == ui::kLEFT ) {
                if ( lineNo > 0 ) {
                    lineNo--;
                    update = true;
                }
            } else if ( key == ui::kDOWN || key == ui::kRIGHT ) {
                if ( lineNo < msgCount - ui::LCD->maxRows ) {
                    lineNo++;
                    update = true;
                }
            // } else if ( key != 0 ) {
                // Serial.println(key);
            }

            if ( func != nullptr ) {
                if ( !func() ) return 0;
            }
        }
    }

    // https://solarianprogrammer.com/2016/11/28/cpp-passing-c-style-array-with-size-information-to-function/
    template<size_t msgCount>
    static uint8_t showMultiLineMsg( const char* (&msg)[msgCount], backgroundProcessDelegate func = nullptr ) {
        // each call with a different parameter count will generate a new version
        // must put body separately to save space
        return showMultiLineMsgN( msg, msgCount, func );
    }

}

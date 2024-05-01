#pragma once

#include <UserInterface.h>
#include <spWDT.h>

namespace StarterPack::spLCD {

    static void showCharSet() {
        namespace ui = StarterPack::UserInterface;
        if ( !ui::hasScreen() ) return;

        uint8_t charsPerRow;
        bool showLabel = true;
        if ( ui::LCD->maxColumns >= 20 ) {
            // 01234567890123456789
            // 00: 0123456789ABCDEF
            charsPerRow = 0xF;
        } else if ( ui::LCD->maxColumns >= 12 ) {
            // 012345678901
            // 00: 01234567
            charsPerRow = 0x7;
        } else if ( ui::LCD->maxColumns >= 8 ) {
            // 01234567
            // 00: 0123
            charsPerRow = 0x3;
        } else {
            // weird screen...
            showLabel = false;
            charsPerRow = ui::LCD->maxColumns;
        }

        int startingChar = 0;
        int lastDisplayed = -1;
        bool reachedEnd = false;

        while( true ) {

            feedTheDog();
            // delay( 10 );

            if ( lastDisplayed != startingChar ) {
                // Serial.printf( "%02X\n", startingChar );
                lastDisplayed = startingChar;
                int ch = startingChar;
                ui::LCD->clear();
                for( uint8_t y=0 ; y<ui::LCD->maxRows ; y++ ) {
                    if ( showLabel ) {
                        ui::LCD->printfAt( 0, y, "%02X", ch );
                        ui::LCD->printAt( 2, y, ": " );
                    } else {
                        ui::LCD->setCursor( 0, y );
                    }
                    for( uint8_t x=0 ; x<=charsPerRow ; x++ ) {
                        ui::LCD->write( ch );
                        ch++;
                        if ( ch == 0x100 ) {
                            // if ( !reachedEnd ) Serial.printf( "end" );
                            reachedEnd = true;
                            ch = 0;
                        }
                    }
                }
                ui::LCD->refresh();
                // ui::LCD->displayAll();
            }
            auto key = ui::getRepeatingKey();
            auto paging = charsPerRow + 1;
            if ( key == ui::kENTER ) {
                if ( reachedEnd ) break;
                // next page
                startingChar += ui::LCD->maxRows * paging;
                while ( startingChar > 0x100 ) startingChar -= 0x100;
            } else if ( key == ui::kRIGHT || key == ui::kDOWN ) {
                startingChar += paging;
                if ( startingChar > 0x100 ) startingChar -= 0x100;
            } else if ( key == ui::kLEFT || key == ui::kUP ) {
                startingChar -= paging;
                if ( startingChar < 0 ) startingChar += 0x100;
            } else if ( key == ui::kESCAPE ) {
                break;
            }
        }
        ui::LCD->clear();
        ui::LCD->refresh();
        // ui::LCD->displayAll();
    }

};

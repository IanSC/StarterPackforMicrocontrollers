#pragma once

#include <UserInterface.h>
#include <LCD/LCDInterface.h>
#include <LCD/LCDBuffered.h>
#include <LCDSupport/LCDEditor.h>
#include <spWDT.h>

#include <Editor/EditorScreen.h>
#include <Editor/EditorNumeric.h>

namespace StarterPack {

//
// EDITOR SCREEN LCD
//

    class EditorScreen_LCD : public EditorScreen {

    protected:

        StarterPack::LCDInterface *lcd = nullptr;
        StarterPack::LCDBuffered *lcdBuffered = nullptr;

    public:

        EditorScreen_LCD() {
            namespace ui = StarterPack::UserInterface;

            if ( !ui::hasScreen() ) return;
            lcd = ui::LCD;
            if ( lcd->isBuffered() )
                lcdBuffered = (StarterPack::LCDBuffered*) lcd;

            // if ( !ui::hasScreen() ) return ui::kESCAPE;
            // LCDInterface *lcd = ui::LCD;
            // LCDBuffered *lcdBuffered = nullptr;
            // if ( lcd->isBuffered() )
            //     lcdBuffered = (LCDBuffered*) lcd;

        }

        void displayBegin() override {}
        void displayEnd()   override {}

        void cursorOn()       override { lcd->cursorOn();       }
        void cursorOff()      override { lcd->cursorOff();      }
        void cursorBlinkOn()  override { lcd->cursorBlinkOn();  }
        void cursorBlinkOff() override { lcd->cursorBlinkOff(); }

        void displayLeftObscured( uint8_t col, uint8_t row )  override {
            lcd->writeAt( col, row, 0x7F );
        }
        void displayRightObscured( uint8_t col, uint8_t row ) override {
            lcd->writeAt( col, row, 0x7E );
        }

        void setCursor( uint8_t col, uint8_t row ) override {
            lcd->setCursor( col, row );
            if ( lcdBuffered != nullptr )
                lcdBuffered->setVirtualCursor( col, row );
        }

        void printStrN( const char *str, uint8_t N, bool clearSpace = false ) override { lcd->printStrN( str, N, clearSpace ); }
        void writeAt  ( uint8_t col, uint8_t row, uint8_t byte )              override { lcd->writeAt( col, row, byte ); }

        void refresh()        override { lcd->refresh();        }
        void refreshPartial() override { lcd->refreshPartial(); }

    };

//
// SHORTCUTS
//

    template <typename T, size_t BUFFERSIZE, bool NEG>
    bool NEW_promptForCore( uint8_t headerRow, const char *caption,
    uint8_t col, uint8_t row, uint8_t windowSize, T &value, T min, T max ) {
        namespace ui = StarterPack::UserInterface;

        if ( !ui::hasScreen() ) return false;
        LCDBuffered *bufferedLCD = nullptr;
        if ( ui::LCD->isBuffered() )
            bufferedLCD = (LCDBuffered*) ui::LCD;

        numericEditorSettings s;
        s.setWindow( col, row, windowSize );
        s.allowDecimal = false;
        s.allowNegative = NEG;
        s.bufferLength = BUFFERSIZE;

        char buffer[BUFFERSIZE];
        if ( value == 0 )
            buffer[0] = 0;
        else
            Num::mySnprintf(buffer, BUFFERSIZE, value);
            // snprintf( buffer, BUFFERSIZE, printfToken, value );
            // snprintf( buffer, BUFFERSIZE, "%d", value );

        EditorScreen_LCD eScreen;
        EditorNumeric nEdit( buffer, s, &eScreen );

        ui::LCD->printStrAtRow( headerRow, caption );
        // ui::LCD->displayAll();
        int8_t freeSpaceOnHeader = ui::LCD->maxColumns - strlen( caption ) - 1;
        if ( freeSpaceOnHeader < 0 ) freeSpaceOnHeader = 0;

        // Serial.println( value );
        // Serial.print( buffer );
        // Serial.println( "<<<" );
        // Serial.println( freeSpaceOnHeader );

        char origCursor = 0xFF;
        if ( bufferedLCD != nullptr )
            origCursor = bufferedLCD->virtualCursorChar;

        while( true ) {

            feedTheDog();
            // delay( 10 );

            uint8_t key = nEdit.prompt();
            if ( key == ui::kESCAPE ) {
                // restore original cursor
                if ( bufferedLCD != nullptr )
                    bufferedLCD->virtualCursorChar = origCursor;
                return false;
            }
            if ( key == ui::kENTER ) {
                // int64_t tmp = Num::myAtoll( buffer );
                int64_t tmp;
                Num::StrToNum( buffer, tmp );
                if ( tmp < min ) {
                    if ( bufferedLCD != nullptr )
                        bufferedLCD->virtualCursorChar = 'v';
                    if ( freeSpaceOnHeader > 0 ) {
                        ui::LCD->setCursor( strlen( caption ) + 1, headerRow );
                        ui::LCD->printStrN( "(low) ", freeSpaceOnHeader );
                    }
                } else if ( tmp > max ) {
                    if ( bufferedLCD != nullptr )
                        bufferedLCD->virtualCursorChar = '^';
                    if ( freeSpaceOnHeader > 0 ) {
                        ui::LCD->setCursor( strlen( caption ) + 1, headerRow );
                        ui::LCD->printStrN( "(over)", freeSpaceOnHeader );
                    }
                } else {
                    // restore original cursor
                    if ( bufferedLCD != nullptr )
                        bufferedLCD->virtualCursorChar = origCursor;
                    value = tmp;
                    return true;
                }
            }
        }
    }

    #define PROMPT(name,dType,LEN,NEG,MIN,MAX) \
        bool NEW_promptFor ## name( uint8_t headerRow, const char *caption, \
        uint8_t col, uint8_t row, uint8_t windowSize, dType &value, dType min=MIN, dType max=MAX ) { \
            return NEW_promptForCore<dType,LEN,NEG>( headerRow, caption, col, row, windowSize, value, min, max ); \
        }
    PROMPT( Int8,   int8_t,   4+1 , true ,  INT8_MIN, INT8_MAX   )  //        -127
    PROMPT( UInt8,  uint8_t,  3+1 , false,         0, UINT8_MAX  )  //         255
    PROMPT( Int16,  int16_t,  6+1 , true , INT16_MIN, INT16_MAX  )  //      -32768
    PROMPT( UInt16, uint16_t, 5+1 , false,         0, UINT16_MAX )  //       65535
    PROMPT( Int32,  int32_t,  11+1, true , INT32_MIN, INT32_MAX  )  // -2147483647
    PROMPT( UInt32, uint32_t, 10+1, false,         0, UINT32_MAX )  //  4294967295
    #undef PROMPT

//
// TEST
//

    void NEW_TEST_editorNumeric() {
        namespace ui = StarterPack::UserInterface;

        numericEditorSettings s;
        s.setWindow( 1, 1, 10 );
        // s.setUnit( 11, 1 );
        s.bufferLength = 5;
        s.allowNegative = true;
        s.allowDecimal = true;

        char buffer[21] = "";
        strcpy( buffer, "123" );

        // set values and check if wrong buffer editing
        buffer[s.bufferLength-1] = 100; // will be null entered chars till end
        buffer[s.bufferLength  ] = 123; // must not be overwritten

        ui::LCD->printAt(  0, 1, "[" );
        ui::LCD->printAt( 11, 1, "]" );

        EditorScreen_LCD eScreen;

        EditorNumeric nEdit( buffer, s, &eScreen );
        while( nEdit.prompt() != ui::kESCAPE );
        // while( editNumeric( buffer, s ) != ui::kESCAPE );

        Serial.println( (uint8_t) buffer[s.bufferLength-1] );
        Serial.println( (uint8_t) buffer[s.bufferLength  ] );
    }

    void NEW_TEST_shortcuts() {
        namespace ui = StarterPack::UserInterface;

        uint8_t ui08 = 200;
        NEW_promptForUInt8( 0, "hello", 0, 1, 5, ui08, 50, 100 );

        ui08 = 200;
        NEW_promptForUInt8( 0, "hello there how r", 0, 1, 5, ui08, 50, 100 );

        int8_t si08 = -100;
        // 01234567890123456789
        // hello there how are
        NEW_promptForInt8( 0, "hello there how are", 0, 1, 5, si08, -50, 100 );
    }

}

#pragma once

#include <UserInterface.h>
#include <LCD/LCDInterface.h>
#include <LCD/LCDBuffered.h>
#include <Utility/WindowedText.h>

#include <Utility/CharCycle.h>
#include <LCDSupport/LCDEditor.h>

#include <spSerial.h>

// #define DEBUG_TRACE(x)   x;
#define DEBUG_TRACE(x)   ;

namespace StarterPack {

    class alphanumericEditor {

        protected:

            bool useUpperCase = true;
            alphaEditorSettings *s = nullptr;
            char *buffer = nullptr;
            WindowedText *wText = nullptr; // give access

        public:

            bool modified = false;

            alphanumericEditor( char *buffer, alphaEditorSettings &s ) {
                this->buffer = buffer;
                this->s = &s;
                wText = new WindowedText( buffer, s.bufferLength, s.windowSize,
                    s.cursorRangeOptions );
                // wText.cursorRange = LCDEditor::WindowedText::cursorRangeOptions::WithinBuffer;
            }

            ~alphanumericEditor() {
                delete wText;
            }

            void reset() {
                wText->reset();
            }

            uint8_t prompt() {
                namespace ui = StarterPack::UserInterface;

                if ( !ui::hasScreen() ) return ui::kESCAPE;
                LCDInterface *lcd = ui::LCD;
                LCDBuffered *lcdBuffered = nullptr;
                if ( lcd->isBuffered() )
                    lcdBuffered = (LCDBuffered*) lcd;

                bool updateDisplay = true;

                ui::waitUntilNothingIsPressed();
                lcd->cursorBlinkOn();
                // lcd->cursorOn();

                CharCycle cycleAlpha(s->characterSets);
                cycleAlpha.addSymbols(s->symbolList);
                cycleAlpha.addPrefixes(s->prefixList);
                // CharCycle cAlpha(CharCycle::CharSet::UpperCase | CharCycle::CharSet::Space);
                CharCycle cycleNumber(CharCycle::CharSet::Numbers | CharCycle::CharSet::Space);
                CharCycle cycleSymbol(CharCycle::CharSet::Space);
                cycleSymbol.addSymbols( CharCycle::AllSymbols );

                while( true ) {

                    //
                    // DISPLAY
                    //
                    if ( updateDisplay ) {
                        updateDisplay = false;

                        // cursor on/off will reset blinking
                        // it will appear to be permanently displayed
                        //    if keys are held down
                        lcd->cursorOff();

                        lcd->setCursor( s->col, s->row );
                        lcd->printStrN( buffer+wText->getWindowIndex(), s->windowSize, true );
                        // lcd->printCharsN( ' ', wText->spaceToClear() );

                        if ( wText->isLeftObscured() )
                            lcd->writeAt( s->col, s->row, 0x7F );
                        if ( wText->isRightObscured() )
                            lcd->writeAt( s->col + s->windowSize-1, s->row, 0x7E );

                        if ( lcdBuffered != nullptr )
                            lcdBuffered->setVirtualCursor( s->col + wText->cursorPositionOnWindow(), s->row );
                        else
                            lcd->setCursor( s->col + wText->cursorPositionOnWindow(), s->row );
                        lcd->cursorOn();
                    }
                    // if ( lcdBuffered != nullptr )
                    //     lcdBuffered->update();
                    lcd->refreshPartial();

                    //
                    // KEY
                    //
                    uint8_t key = ui::getRepeatingKey();

                    if ( key == ui::kENTER || key == ui::kESCAPE ) {
                        if ( modified ) {
                            // remove ' ' at the end
                            wText->trimEnd();
                        }
                        lcd->cursorOff();
                        //ui::flagWaitForKeyup();
                        ui::skipRepeatingCurrentKey();
                        // ui::waitUntilNothingIsPressed();
                        return key;
                    }
                    
                    if ( key == ui::kNONE ) {

                        // ...

                    } else if ( key == ui::kLEFT && globalEditorSettings.useLeftRightArrowKeys ) {

                        if ( wText->cursorBackward() ) {
                            updateDisplay = true;

                            // enable if last character
                            bool enableJump;
                            enableJump = ( wText->getCursorPosition() == WindowedText::cursorPositionEnum::rightAfterLast );
                            if ( enableJump )
                                cycleAlpha.resetJump();
                            else
                                cycleAlpha.disableJump();
                        }

                    } else if ( key == ui::kRIGHT && globalEditorSettings.useLeftRightArrowKeys ) {

                        // might add spaces at end,
                        // but don't consider as modified
                        if ( wText->cursorForward() ) {
                            updateDisplay = true;

                            // enable if last character
                            bool enableJump = ( wText->getCursorPosition() == WindowedText::cursorPositionEnum::rightAfterLast );
                            if ( enableJump )
                                cycleAlpha.resetJump();
                            else
                                cycleAlpha.disableJump();
                        }

                    } else if ( key == ui::kUP && globalEditorSettings.cycleUpDown ) {

                        DEBUG_TRACE( SerialPrintfln( "UP-1 = %d", cycleAlpha.jMode ) )
                        if ( wText->modifyCharAtCursor( ' ' ) ) {
                            cycleAlpha.jumpUp( wText->charPtrAtCursor(), wText->getCursorPosition() );
                            if (s->allowChangeCase)
                                changeCase( useUpperCase, wText->charPtrAtCursor() );
                            modified = true;
                            updateDisplay = true;
                        }
                        DEBUG_TRACE( SerialPrintfln( "UP-2 = %d", cycleAlpha.jMode ) )

                    } else if ( key == ui::kDOWN && globalEditorSettings.cycleUpDown ) {

                        DEBUG_TRACE( SerialPrintfln( "DN-1 = %d", cycleAlpha.jMode ) )
                        if ( wText->modifyCharAtCursor( ' ' ) ) {
                        // if ( wText->canModifyCharAtCursor() ) {
                            // Serial.println("JDOWN");
                            // Serial.println(wText->charAtCursor());
                            cycleAlpha.jumpDown( wText->charPtrAtCursor(), wText->getCursorPosition() );
                            // Serial.println(wText->charAtCursor());
                            if (s->allowChangeCase)
                                changeCase( useUpperCase, wText->charPtrAtCursor() );
                            modified = true;
                            updateDisplay = true;
                        }
                        DEBUG_TRACE( SerialPrintfln( "DN-2 = %d", cycleAlpha.jMode ) )

                    } else if ( key == ui::kBACKSPACE ) {

                        // delete char before cursor
                        if ( wText->backspace() ) {
                            modified = true;
                            updateDisplay = true;
                        }

                    } else if ( key == ui::kDELETE ) {

                        // delete char at cursor
                        if ( wText->deleteAtCursor() ) {
                            modified = true;
                            updateDisplay = true;
                        }

                    } else if ( key == ui::kINSERT ) {

                        // insert char on cursor
                        if ( wText->insertAtCursor( ' ' ) ) {
                            modified = true;
                            updateDisplay = true;
                        }

                    } else if ( key == ui::kCHG_CASE ) {

                        // toggle case
                        if (s->allowChangeCase) {
                            bool afterLast = ( wText->getCursorPosition() == WindowedText::cursorPositionEnum::rightAfterLast );
                            if ( !afterLast ) {
                                char *p = wText->charPtrAtCursor();
                                if ( *p >= 'A' && *p <= 'Z' ) {
                                    useUpperCase = false;
                                    *p = tolower(*p);
                                    modified = true;
                                    updateDisplay = true;
                                } else if ( *p >= 'a' && *p <= 'z' ) {
                                    useUpperCase = true;
                                    *p = toupper(*p);
                                    modified = true;
                                    updateDisplay = true;
                                } else {
                                    useUpperCase = !useUpperCase;
                                }
                            } else {
                                useUpperCase = !useUpperCase;
                            }
                        }

                    } else if ( key == ui::kDECIMAL ) {

                        if ( wText->modifyCharAtCursor( ' ' ) ) {
                        // if ( wText->canModifyCharAtCursor() ) {
                            cycleSymbol.cycleOneUp( wText->charPtrAtCursor(), wText->getCursorPosition() );
                            modified = true;
                            updateDisplay = true;
                        }

                    } else if ( key == ui::kNUMBERS ) {

                        if ( wText->modifyCharAtCursor( ' ' ) ) {
                        // if ( wText->canModifyCharAtCursor() ) {
                            cycleNumber.cycleOneUp( wText->charPtrAtCursor(), wText->getCursorPosition() );
                            modified = true;
                            updateDisplay = true;
                        }

                    } else if ( key == ui::kQUESTION ) {

                        // pass back to user for now
                        return key;

                        /*
                        // specific to cutter app
                        lcd->cursorOff();
                        lcd->printStrAtRow( 0, "\x7F \x7E to move cursor" );
                        lcd->printStrAtRow( 1, "^ v cycle letters" );
                        lcd->printStrAtRow( 2, "  . symbols" );
                        lcd->printStrAtRow( 3, "  0 numbers" );
                        lcd->refresh();
                        // lcd->displayAll();
                        if ( ui::waitForAnyKeyPressed() == ui::kQUESTION ) {
                            lcd->printStrAtRow( 0, "[1] change case" );
                            lcd->printStrAtRow( 1, "[U] insert" );
                            lcd->printStrAtRow( 2, "BS  delete" );
                            lcd->clearRow( 3 );
                            lcd->refresh();
                            // lcd->displayAll();
                            if ( ui::waitForAnyKeyPressed() == ui::kQUESTION ) {
                                lcd->printStrAtRow( 0, "Ok accept" );
                                lcd->printStrAtRow( 1, "X  go back" );
                                lcd->clearRow( 2 );
                                lcd->refresh();
                                // lcd->displayAll();
                                ui::waitForAnyKeyPressed();
                            }
                        }
                        lcd->cursorOn();
                        updateDisplay = true;
                        */
                        
                    } else {
                        // unknown key, pass back for caller to handle
                        return key;
                    }
                }
            }

        //
        // CHARACTERS
        //
        protected:

            void changeCase( bool upperCase, char *ch ) {
                if ( upperCase )
                    *ch = toupper(*ch);
                else
                    *ch = tolower(*ch);
            }

    };

    void TEST_editorAlpha() {
        namespace ui = StarterPack::UserInterface;
        
        alphaEditorSettings s;
        s.setWindow( 1, 0, 10 );
        // s.setUnit( 11, 1 );
        s.bufferLength = 15;
        s.characterSets = CharCycle::CharSet::FullAlphabet | CharCycle::CharSet::Space;
        // s.allowNegative = true;
        // s.allowDecimal = true;

        char buffer[21] = "";
        //strcpy( buffer, "app" );

        // set values and check if wrong buffer editing
        buffer[s.bufferLength-1] = 100; // will be null entered chars till end
        buffer[s.bufferLength  ] = 123; // must not be overwritten

        ui::LCD->printAt(  0, 1, "[" );
        ui::LCD->printAt( 11, 1, "]" );

        alphanumericEditor alpha( buffer, s );
        while( alpha.prompt() != ui::kESCAPE );

        Serial.println( (uint8_t) buffer[s.bufferLength-1] );
        Serial.println( (uint8_t) buffer[s.bufferLength  ] );

        Serial.print( buffer );
        Serial.println( "}}}" );
    }

}

#undef DEBUG_TRACE

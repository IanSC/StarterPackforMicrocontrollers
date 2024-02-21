#pragma once

// #include <Utility/cSupport.h>
#include <Utility/spNum.h>
#include <spWDT.h>

#include <UserInterface.h>
// #include <LCD/LCDInterface.h>
// #include <LCD/LCDBuffered.h>
#include <Utility/WindowedText.h>

#include <LCDSupport/LCDEditor.h>

#include <Editor/EditorScreen.h>

namespace StarterPack {

//
// EDITOR
//

    class EditorNumeric {

        typedef UserInterface::KEY KEY;

        protected:

            numericEditorSettings *s = nullptr;
            char *buffer = nullptr;

            EditorScreen *eScreen = nullptr;

        public:

            bool modified = false;
            WindowedText *wText = nullptr; // give access

            // insert vs overwrite character
            bool insertText = false;

            EditorNumeric( char *buffer, numericEditorSettings &s, EditorScreen *eScreen ) {
                this->buffer = buffer;
                this->s = &s;
                this->eScreen = eScreen;
                wText = new WindowedText( buffer, s.bufferLength, s.windowSize,
                    s.cursorRangeOptions );
                // eScreen->init();
            }

            ~EditorNumeric() {
                delete wText;
            }

            void reset() {
                wText->reset();
            }

            KEY prompt() {
                namespace ui = StarterPack::UserInterface;

                // returns:
                //    kENTER  - enter pressed, accepted
                //    kESCAPE - escape pressed, cancelled
                //    <key>   - unknown key detected
                // automatically handles:
                //    kBACKSPACE, kDELETE
                //    kLEFT, kRIGHT
                //    kDECIMAL
                //    kMINUS
                //    '0' .. '9'

                // scenario:
                // - user does not need decimal entry
                // - decimal key is reused for another purpose
                //   while editing
                //   ex. goto current value, or check validity
                // - but if detected here and ignored
                //   caller will not get key press
                //
                // solution 1:
                // - map ui::kDECIMAL to dummy value
                // - call this function, eg. prompt()
                // - decimal will not get detected
                //   hence passed to caller
                // - restore
                // - too much code to reuse keys
                //
                // solution 2:
                // - pass all keys to caller if not handled
                // - if not reused, simply ignored by caller
                //
                // so, pass key to caller if:
                // - unknown key
                // - decimal key and not allowDecimal
                // - minus key and not allowNegative

                //
                // ===== NEW =====
                //
                // if ( !ui::hasScreen() ) return ui::kESCAPE;
                // LCDInterface *lcd = ui::LCD;
                // LCDBuffered *lcdBuffered = nullptr;
                // if ( lcd->isBuffered() )
                //     lcdBuffered = (LCDBuffered*) lcd;

                bool updateDisplay = true;

                ui::waitUntilNothingIsPressed();
                eScreen->cursorBlinkOn();
                // lcd->cursorBlinkOn();

                // position 0 = '-', '.', 0..9
                CharCycle cycleNegative( CharCycle::CharSet::Numbers );
                if ( s->allowNegative && s->allowDecimal )
                    cycleNegative.addPrefixes( "-." );
                else if ( s->allowNegative )
                    cycleNegative.addPrefixes( "-" );
                else if ( s->allowDecimal )
                    cycleNegative.addPrefixes( "." );
                CharCycle cycleNumberAndDecimal(CharCycle::CharSet::Numbers | CharCycle::CharSet::Space);
                if ( s->allowDecimal )
                    cycleNumberAndDecimal.addSymbols( "." );
                CharCycle cycleNumber(CharCycle::CharSet::Numbers | CharCycle::CharSet::Space);

                while( true ) {

                    feedTheDog();
                    delay( 10 );

                    //
                    // UPDATE DISPLAY
                    //
                    if ( updateDisplay ) {
                        updateDisplay = false;

                        // lcd->cursorOff();
                        // lcd->setCursor( s->col, s->row );
                        // lcd->printStrN( wText->stringToDisplay(), s->windowSize, true );

                        eScreen->displayBegin();

                        eScreen->cursorOff();
                        eScreen->setCursor( s->col, s->row );
                        eScreen->printStrN( wText->stringToDisplay(), s->windowSize, true );

                        // if (wText->isLeftObscured() )
                        //     lcd->writeAt( s->col, s->row, 0x7F );

                        if ( wText->isLeftObscured() )
                            eScreen->displayLeftObscured( s->col, s->row );
                        if ( wText->isRightObscured() )
                            eScreen->displayRightObscured( s->col + s->windowSize-1, s->row );

                        // if ( lcdBuffered != nullptr )
                        //     lcdBuffered->setVirtualCursor( s->col+wText->cursorPositionOnWindow(), s->row );
                        // else
                        //     lcd->setCursor( s->col+wText->cursorPositionOnWindow(), s->row );
                        // lcd->cursorOn();

                        eScreen->setCursor( s->col+wText->cursorPositionOnWindow(), s->row );
                        eScreen->cursorOn();

                        eScreen->displayEnd();
                    }                    
                    // lcd->refreshPartial();
                    // // if ( lcdBuffered != nullptr )
                    // //     lcdBuffered->update();
                    eScreen->refreshPartial();

                    //
                    // KEYSTROKES
                    //

                    uint8_t key = ui::getRepeatingKey();

                    if ( key == ui::kENTER || key == ui::kESCAPE ) {
                        // lcd->cursorOff();
                        eScreen->cursorOff();

                        // ui::flagWaitForKeyup();
                        ui::skipRepeatingCurrentKey();
                        // ui::waitUntilNothingIsPressed();
                        return key;
                    }
                    
                    char charToAdd = 0;

                    if ( key == ui::kNONE ) {
                        // ...
                    } else if ( key == ui::kBACKSPACE ) {
                        if ( wText->backspace() ) {
                            modified = true;
                            updateDisplay = true;
                        }
                    } else if ( key == ui::kDELETE ) {
                        if ( wText->deleteAtCursor() ) {
                            modified = true;
                            updateDisplay = true;
                        }
                    } else if ( key == ui::kLEFT && globalEditorSettings.useLeftRightArrowKeys ) {
                        if ( wText->cursorBackward() ) {
                            // modified = true;
                            updateDisplay = true;
                        } else if ( feedbackCB != nullptr ) {
                            auto r = feedbackCB( FeedbackEnum::CursorBackwardBlocked );
                            if ( r != ui::kNONE )
                                return r;
                            // if ( !feedbackCB( FeedbackEnum::CursorBackwardBlocked ) )
                            //     return ui::kESCAPE;
                        }
                    } else if ( key == ui::kRIGHT && globalEditorSettings.useLeftRightArrowKeys ) {
                        if ( wText->cursorForward() ) {
                            modified = true;
                            updateDisplay = true;
                        } else if ( feedbackCB != nullptr ) {
                            auto r = feedbackCB( FeedbackEnum::CursorForwardBlocked );
                            if ( r != ui::kNONE )
                                return r;
                            // if ( !feedbackCB( FeedbackEnum::CursorForwardBlocked ) )
                            //     return ui::kESCAPE;
                        }
                    } else if ( key == ui::kUP && globalEditorSettings.cycleUpDown ) {
                        if ( wText->modifyCharAtCursor( ' ' ) ) {
                            auto cursorPos = wText->getCursorPosition();
                            if ( cursorPos == WindowedText::cursorPositionEnum::first )
                                cycleNegative.cycleOneUp( wText->charPtrAtCursor(), cursorPos );
                            else if ( !wText->findCharacterBeforeCursor( s->decimalPoint ) )
                                cycleNumberAndDecimal.cycleOneUp( wText->charPtrAtCursor(), cursorPos );
                            else
                                cycleNumber.cycleOneUp( wText->charPtrAtCursor(), cursorPos );
                            modified = true;
                            updateDisplay = true;
                        }
                    } else if ( key == ui::kDOWN && globalEditorSettings.cycleUpDown ) {
                        if ( wText->modifyCharAtCursor( ' ' ) ) {
                            auto cursorPos = wText->getCursorPosition();
                            if ( cursorPos == WindowedText::cursorPositionEnum::first )
                                cycleNegative.cycleOneDown( wText->charPtrAtCursor(), cursorPos );
                            else if ( !wText->findCharacterBeforeCursor( s->decimalPoint ) )
                                cycleNumberAndDecimal.cycleOneDown( wText->charPtrAtCursor(), cursorPos );
                            else
                                cycleNumber.cycleOneDown( wText->charPtrAtCursor(), cursorPos );                            
                            modified = true;
                            updateDisplay = true;
                        }

                    } else if ( key == ui::kDECIMAL && s->allowDecimal ) {

                        if ( !Str::findCharacter(s->decimalPoint,buffer) ) {
                            charToAdd = s->decimalPoint; // '.';

                            // CASE (1): INSERT addCharacter() CONTENTS HERE
                            // eg. if (insertText) { if ( wText->canAddChar() ) { ... } }
                            // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                            // Flash: [====      ]  40.8% (used 12532 bytes from 30720 bytes)

                            // CASE (2): CALL addCharacter(), ADDS ALMOST 100 BYTES ???
                            // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                            // Flash: [====      ]  41.1% (used 12620 bytes from 30720 bytes)
                            // if ( addCharacter( '.' ) ) {
                            //     modified = true;
                            //     updateDisplay = true;
                            // }

                            // CASE (3): ADD ROUTINES AFTER IFs
                            // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                            // Flash: [====      ]  40.6% (used 12482 bytes from 30720 bytes)

                        }
                        ui::skipRepeatingCurrentKey();
                        // ui::flagWaitForKeyup();

                    } else if ( key == ui::kMINUS && s->allowNegative ) {

                        char *ptr = wText->charPtrAt( 0 );
                        if ( ptr != nullptr ) {
                            if ( *ptr == '-' ) {
                                if ( wText->deleteAt( 0 ) ) {
                                    modified = true;
                                    updateDisplay = true;
                                }
                            } else {
                                if (wText->canAddChar()) {
                                    if ( wText->insertAt( '-', 0 ) ) {
                                        modified = true;
                                        updateDisplay = true;
                                    }
                                }
                            }
                        }
                        ui::skipRepeatingCurrentKey();
                        // ui::flagWaitForKeyup();
                        // ui::skipRepeatingCurrentKey();
                        
                        // updateDisplay = true;
                    } else if ( key >= '0' && key <= '9' ) {
                        charToAdd = key;

                        // CASE (1): INSERT addCharacter() CONTENTS HERE
                        // eg. if (insertText) { if ( wText->canAddChar() ) { ... } }
                        // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                        // Flash: [====      ]  40.8% (used 12532 bytes from 30720 bytes)

                        // CASE (2): CALL addCharacter(), ADDS ALMOST 100 BYTES ???
                        // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                        // Flash: [====      ]  41.1% (used 12620 bytes from 30720 bytes)
                        // if ( addCharacter( '.' ) ) {
                        //     modified = true;
                        //     updateDisplay = true;
                        // }

                        // CASE (3): ADD ROUTINES AFTER IFs
                        // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                        // Flash: [====      ]  40.6% (used 12482 bytes from 30720 bytes)

                    } else if ( key == ui::kQUESTION ) {

                        // pass back to user for now
                        return key;

                        /*
                        // specific to cutter app
                        lcd->cursorOff();
                        lcd->printStrAtRow( 0, "use \x7F \x7E to highlight" );
                        lcd->printStrAtRow( 1, "  cut position" );
                        lcd->printStrAtRow( 2, "Ok - edit" );
                        lcd->printStrAtRow( 3, "X  - go back" );
                        lcd->refresh();
                        // lcd->displayAll();
                        if ( ui::waitForAnyKeyPressed() == ui::kQUESTION ) {
                            lcd->printStrAtRow( 0, ". or \x03 - insert" );
                            lcd->printStrAtRow( 1, "  cut on cursor" );
                            lcd->printStrAtRow( 2, "BS or \x04 - delete" );
                            lcd->printStrAtRow( 3, "  cut on cursor" );
                            lcd->refresh();
                            // lcd->displayAll();
                            ui::waitForAnyKeyPressed();
                        }
                        lcd->clearRow( 2 );
                        lcd->clearRow( 3 );
                        ui::waitUntilNothingIsPressed();
                        updateDisplay = true;
                        */

                    } else {
                        // unknown key, pass back for caller to handle
                        // possibly includes kDECIMAL, kMINUS if not allowed
                        return key;
                    }

                    // CASE (3-A): ADD ROUTINES AFTER IFs
                    // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                    // Flash: [====      ]  40.6% (used 12482 bytes from 30720 bytes)
                    if (charToAdd != 0) {
                        if (insertText) {
                            if ( wText->canAddChar() ) {
                                if ( wText->insertAtCursor( charToAdd ) ) {
                                    wText->cursorForward();
                                    modified = true;
                                    updateDisplay = true;
                                }
                            }
                        } else {
                            if ( wText->canModifyCharAtCursor() ) {
                                if ( wText->setCharAtCursor( charToAdd ) ) {
                                    wText->cursorForward();
                                    modified = true;
                                    updateDisplay = true;
                                }
                            }
                        }
                    }

                    if ( backgroundProcess != nullptr ) {
                        auto r = backgroundProcess();
                        if ( r != ui::kNONE )
                            return r;
                    }
                }
            }

        protected:

            bool addCharacter( char ch ) {
                if (insertText) {
                    if ( wText->canAddChar() ) {
                        if ( wText->insertAtCursor( ch ) ) {
                            wText->cursorForward();
                            return true;
                        }
                    }
                } else {
                    if ( wText->canModifyCharAtCursor() ) {
                        if ( wText->setCharAtCursor( ch ) ) {
                            wText->cursorForward();
                            return true;
                        }
                    }
                }
                return false;
            }

        //
        // BACKGROUND PROCESS/BREAKOUT
        //
        #if defined(ESP8266) || defined(ESP32)
            protected:
                // return anything but ui::kNone to breakout of loop
                typedef std::function<KEY(void)> backgroundProcessDelegateF;
                typedef KEY (*backgroundProcessDelegate)();
                backgroundProcessDelegateF backgroundProcess = nullptr;
            public:
                // static KEY backgroundProcessNoAction() {
                //     namespace ui = StarterPack::UserInterface;
                //     return ui::kNONE;
                // }
                void assignBackgroundProcess( backgroundProcessDelegateF backgroundProcess ) {
                    this->backgroundProcess = backgroundProcess;
                }
                void assignBackgroundProcess( backgroundProcessDelegate backgroundProcess ) {
                    this->backgroundProcess = [backgroundProcess]() { return backgroundProcess(); };
                }
                // void assignBackgroundProcess( uint8_t (*backgroundProcessFunc)() ) {
                //     this->backgroundProcessF = [backgroundProcessFunc]() { return backgroundProcessFunc(); };
                // }
        #else
            protected:
                // return anything but ui::kNone to breakout of loop
                typedef KEY (*backgroundProcessDelegate)();
                backgroundProcessDelegate backgroundProcess = nullptr;
            public:
                void assignBackgroundProcess( backgroundProcessDelegate backgroundProcess ) {
                    this->backgroundProcess = backgroundProcess;
                }
        #endif

        //
        // FEEDBACK TO BREAKOUT
        //
        public:

            enum class FeedbackEnum : uint8_t {
                CursorBackwardBlocked = 1, // moved cursor backward but failed
                CursorForwardBlocked  = 2  // moved cursor foward but failed
            };
            CLASS_ENUM_MANIPULATION(FeedbackEnum);

        #if defined(ESP8266) || defined(ESP32)
            protected:
                // return anything but ui::kNone to breakout of loop
                typedef std::function<KEY(FeedbackEnum)> FeedbackDelegateF;
                typedef KEY (*FeedbackDelegate)(FeedbackEnum);
                FeedbackDelegateF feedbackCB = nullptr;
            public:
                void assignFeedbackCallback( FeedbackDelegateF feedbackCB ) {
                    this->feedbackCB = feedbackCB;
                }
                void assignFeedbackCallback( FeedbackDelegate feedbackCB ) {
                    this->feedbackCB = [feedbackCB](FeedbackEnum e) { return feedbackCB(e); };
                }
        #else
            public:
                // return anything but ui::kNone to breakout of loop
                typedef KEY (*FeedbackDelegate)(FeedbackEnum);
            protected:
                FeedbackDelegate feedbackCB = nullptr;
            public:
                void assignFeedbackCallback( FeedbackDelegate feedbackCB ) {
                    this->feedbackCB = feedbackCB;
                }
        #endif

    };

}

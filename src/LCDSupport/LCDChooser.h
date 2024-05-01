#pragma once

#include <spWDT.h>

#include <LCD/LCDInterface.h>
#include <LCDSupport/LCDUtility.h>
#include <UserInterface/UserInterface_Keyboard.h>

// #include <UserInterface.h>
namespace StarterPack::UserInterface {
    // avoid circular UserInterface.h <--> LCDChooser.h
    extern LCDInterface * LCD;
    extern bool hasScreen();
    // typedef char KEY;
    // extern KEY getRepeatingKey();
    // extern KEY kENTER;
    // extern KEY kNONE;
    // extern KEY kESCAPE;
    // extern KEY kLEFT;
    // extern KEY kUP;
    // extern KEY kRIGHT;
    // extern KEY kDOWN;
    // void waitUntilNothingIsPressed();
}

namespace StarterPack::spLCD {

    class optionChooser {

        //  Usage:
        //      ... setup UserInterface keyboard and lcd
        //
        //      uint8_t r = chooseYesNoCancel( "Are you sure?" );
        //      uint8_t r = choose2( "hello", "there" );
        //
        //  Manual Looping:
        //    
        //      const char* colors[] = { "red", "green", "blue" };
        //      optionChooser chooser( colors, 0 );
        //      chooser.display( true );
        //      uint8_t input;
        //      while( true ) {
        //          input = chooser.poll();
        //          if ( input != 0 ) break;
        //          // ... do something
        //      }

        public:

            ~optionChooser() {
                freeMemory();
                // delete[] itemLen;
                // delete[] posInLayout;
            }

        //
        // DATA
        //
        protected:

            uint8_t    row;               // lcd row to use
            const char **options;         // pointer to array of string
            int8_t     optionCount;       // number of items

            uint8_t    *itemLen = nullptr;          // length of each option
            uint8_t    *posInLayout = nullptr;      // position of each option if arranged as one long string
                                          // eg. [apple][banana][carrots]
                                          //     ^      ^       ^          starting positions

            uint8_t visibleStart = 0;     // char position in layout that is dispayed on leftmost of screen

            // bool (*backgroundProcess)(void) = nullptr;
            // Throttle *throttler = nullptr;

            void freeMemory() {
                if (itemLen != nullptr)     delete[] itemLen;
                if (posInLayout != nullptr) delete[] posInLayout;
            }

        public:

            uint8_t selectedItem          = 0; // item chosen 1..N
            uint8_t selectedLeftLocation  = 0; // left position of selected item on screen
            uint8_t selectedRightLocation = 0; // right position of selected item on screen

            bool    crossover = false;         // allow crossover between 1st/last items

            void setSelectedItem( uint8_t selectedItem ) {
                if ( selectedItem >= optionCount )
                    selectedItem = optionCount;
                this->selectedItem = selectedItem;
                computePositions();
            }

        //
        // CHAIN
        //
        public:
            // chained vs unchained same memory, so let user choose style
            // eg. C.allowCrossover()->noCrossover()->setInitialSelection(1)->setOptionsRow(1);
            //     C.allowCrossover(); C.noCrossover(); C.setInitialSelection(1); C.setOptionsRow(1);
            inline optionChooser* allowCrossover() { crossover = true ; return this; }
            inline optionChooser* noCrossover()    { crossover = false; return this; }
            inline optionChooser* setInitialSelection( uint8_t initialSelection ) {
                setSelectedItem( initialSelection ); return this; }
            inline optionChooser* setOptionsRow( uint8_t row ) {
                this->row = row; return this; }

        //
        // BACKGROUND PROCESS/BREAKOUT
        //
        #if defined(ESP8266) || defined(ESP32)
            protected:
                // return false to breakout of loop
                typedef std::function<bool(void)> backgroundProcessDelegate;
                backgroundProcessDelegate backgroundProcess = nullptr;
            public:
                inline optionChooser* assignBackgroundProcess( backgroundProcessDelegate backgroundProcess ) {
                    this->backgroundProcess = backgroundProcess;
                    return this;
                }        
                inline optionChooser* assignBackgroundProcess( bool (*backgroundProcessFunc)() ) {
                    this->backgroundProcess = [backgroundProcessFunc]() { return backgroundProcessFunc(); };
                    return this;
                }
        #else
            protected:
                // return anything but ui::kNone to breakout of loop
                typedef bool (*backgroundProcessDelegate)();
                backgroundProcessDelegate backgroundProcess = nullptr;
            public:
                inline optionChooser* assignBackgroundProcess( backgroundProcessDelegate backgroundProcess ) {
                    this->backgroundProcess = backgroundProcess;
                    return this;
                }
        #endif

        /*
        //
        // THROTTLER
        //
        protected:
            Throttle_OLD *throttler = nullptr;
        public:
            inline optionChooser* assignThrottle( Throttle_OLD *throttler ) {
                this->throttler = throttler;
                return this;
            }
        */

        //
        // PAYLOAD
        //
        // public:
        //
        //     struct payloadStruct {
        //         const char *caption = nullptr;
        //         uint8_t captionRow = 0;
        //         const char **options;
        //         uint8_t optionsCount = 0;
        //         uint8_t optionsRow = 1;
        //         uint8_t initialSelection = 0;
        //         bool crossover = false;
        //         bool (*backgroundProcess)(void) = nullptr;
        //     };
        //
        //     optionChooser(payloadStruct &payload) {
        //         namespace ui = StarterPack::UserInterface;
        //         initN( payload.options, payload.optionsCount, payload.optionsRow,
        //             payload.initialSelection, payload.crossover );
        //         if ( ui::LCD != nullptr )
        //             ui::LCD->printStrAtRow( payload.captionRow, payload.caption );
        //         assignBackgroundProcess( payload.backgroundProcess );
        //     }

        public:

            optionChooser() {}

            // const char* colors[] = {"red","green","blue"};
            // optionChooser chooser( colors, 123 );
            //
            //  mix up in calling: 123 can be optCount or row
            //      (A) optionChooser( const char **options, uint8_t optCount, ...
            //      (B) template<size_t optCount>
            //          optionChooser( const char* (&options)[optCount], uint8_t row = 0, ...
            //
            //  so enable only (B)
            //  for option (A), use instead:
            //
            //      optionChooser chooser;
            //      chooser.initN( colors, 3 items, ... )

            //
            // (A) SPECIFY ITEM COUNT
            //
            // optionChooser( const char **options, uint8_t optCount,
            // uint8_t row = 0,
            // uint8_t initialSelection = 0, bool crossover = false ) {
            //     initN( options, optCount, row, initialSelection, crossover );
            // }

            //
            // (B) AUTO ITEM COUNT
            //
            template<size_t optCount>
            optionChooser( const char* (&options)[optCount],
            uint8_t row = 0,
            uint8_t initialSelection = 0, bool crossover = false ) {
                initN( options, optCount, row, initialSelection, crossover );
            }

        //
        // PROCESS OPTIONS
        //
        public:
            
            template<size_t optCount>
            inline void init( const char* (&options)[optCount],
            uint8_t row = 0,
            uint8_t initialSelection = 0, bool crossover = false ) {
                initN( options, optCount, row, initialSelection, crossover );
            }

            void initN( const char **options, uint8_t optCount,
            uint8_t row = 0,
            uint8_t initialSelection = 0, bool crossover = false ) {

                this->row = row;
                this->options = options;
                this->optionCount = optCount;
                this->crossover = crossover;

                freeMemory();

                // limit to 255 items or 255 characters total
                // if ( optionCount > UINT8_MAX )
                //     optionCount = UINT8_MAX;
                itemLen = new uint8_t[optionCount];
                posInLayout = new uint8_t[optionCount];

                int16_t pos = 0;
                for( uint8_t i = 0; i < optionCount ; i++ ) {
                    size_t len = strlen( options[i] );
                    if ( len > UINT8_MAX ) {
                        // too long, length storage is only uint8_t
                        optionCount = i;
                        break;
                    }
                    itemLen[i] = len;
                    posInLayout[i] = pos;
                    pos += itemLen[i] + 1;
                    if ( pos > UINT8_MAX ) {
                        // using uint8_t only for computations
                        optionCount = i;
                        break;
                    }
                    pos++;
                }

                setSelectedItem( initialSelection );
            }

        private:

            static constexpr uint8_t POLL_CANCELLED = -1;

        public:

            uint8_t prompt() {
                namespace ui = StarterPack::UserInterface;
                display( true );
                uint8_t input;
                while( true ) {

                    feedTheDog();
                    // delay( 10 );

                    input = poll();
                    if ( input == POLL_CANCELLED )
                        return 0;
                    if ( input != 0 )
                        return input;
                    if ( backgroundProcess != nullptr ) {
                        if ( !backgroundProcess() ) {
                            // terminated, no selection
                            // return 0, but retain currently selectedItem
                            return 0;
                        }
                    }
                    // if ( throttler != nullptr )
                    //     throttler->trigger();
                }
            }

            void display( bool showSelection = true ) {
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return;
                LCDInterface *lcd = ui::LCD;

                bool overflowRight = false;
                lcd->setCursor( 0, row );

                // print contents of layout from [visibleStart] to [visibleEnd]
                // determine which options are visible
                uint8_t visibleEnd = visibleStart + lcd->maxColumns - 1;
                uint8_t itemStart = 0, itemEnd;
                int8_t lcdSpace = lcd->maxColumns;
                // Serial.println( "*** START ***" );
                for( uint8_t i = 0 ; i < optionCount ; i++ ) {
                    if ( lcdSpace < 2 ) {
                        // no need to continue, need at least 2 characters
                        // free for an option to be visible by 1 char
                        overflowRight = true;
                        break;
                    }
                    itemEnd = itemStart + itemLen[i] + 1;
                    // SerialPrintf( "%2d/%2d - %2d/%2d - %s\n", visibleStart, visibleEnd,
                    //     itemStart, itemEnd, options[i] );
                    // print if there is overlap between:
                    //    visibleStart/visibleEnd and itemStart/itemEnd
                    if ( itemEnd < visibleStart ) {        // before start
                        // Serial.println( "   not visible - way before");
                    } else if ( itemStart > visibleEnd ) { // after end
                        // Serial.println( "   not visible - way after");
                    } else {
                        // SerialPrintf( "   lcd space = %d\n", lcdSpace );
                        // Serial.print( "   #" );
                        if ( itemStart >= visibleStart ) {
                            // Serial.print( " " );
                            lcd->write( ' ' );
                            lcdSpace--;
                        }
                        int8_t chOffset = visibleStart - itemStart - 1;
                        if ( chOffset < 0 ) chOffset = 0;
                        lcd->printStrN( options[i] + chOffset, lcdSpace );
                        // SerialPrintStrN( options[i] + chOffset, lcdSpace );
                        lcdSpace -= ( itemLen[i] - chOffset );
                        if ( lcdSpace <= 0 ) {
                            // Serial.println( "(!)" );
                            overflowRight = true;
                            break;
                        }
                        // Serial.print( " " );
                        lcd->write( ' ' );
                        lcdSpace--;
                        // Serial.println( "#" );
                    }
                    itemStart = itemEnd + 1;
                }
                if ( lcdSpace > 0 )
                    lcd->printCharsN( ' ', lcdSpace );

                // overflow markers
                if ( visibleStart != 0 )
                    lcd->writeAt( 0, row, 0x7F );
                if ( overflowRight )
                    lcd->writeAt( lcd->maxColumns-1, row, 0x7E );

                if ( showSelection )
                    this->showSelection();
                // lcd->displayAll();
                lcd->refresh();
            }

            void showSelection() { showMarkersCore( '[', ']' ); }
            void hideSelection() { showMarkersCore( ' ', ' ' ); }

            uint8_t poll() {
                namespace ui = StarterPack::UserInterface;

                // return:
                // 0    - no selection
                // 1..N - option selected

                // if ( ui::KeyHandler == nullptr ) return 0;

                uint8_t key = ui::getRepeatingKey();
                if ( key == ui::kENTER ) {
                    ui::waitUntilNothingIsPressed();
                    return selectedItem + 1;
                }
                if ( key == ui::kLEFT || key == ui::kUP )
                    moveLeft();
                else if ( key == ui::kRIGHT || key == ui::kDOWN )
                    moveRight();
                else if ( key == ui::kESCAPE )
                    return POLL_CANCELLED;
                return ui::kNONE;
            }

            bool moveLeft() {
                auto newSelection = selectedItem;
                if ( newSelection == 0 ) {
                    if ( crossover )
                        newSelection = optionCount - 1;
                } else
                    newSelection--;

                if ( selectedItem == newSelection )
                    return false;

                selectedItem = newSelection;
                computePositions();
                display( true );
                return true;

                // hideSelection();
                // auto origSelection = selectedItem;
                // if ( selectedItem == 0 ) {
                //     if ( crossover )
                //         selectedItem = optionCount - 1;
                // } else
                //     selectedItem--;
                // if ( selectedItem != origSelection ) {
                //     computePositions();
                //     display( true );
                //     return true;
                // } else {
                //     // showSelection();
                //     return false;
                // }
            }

            bool moveRight() {
                auto newSelection = selectedItem;
                if ( newSelection >= optionCount - 1 ) {
                    if ( crossover )
                        newSelection = 0;
                } else
                    newSelection++;
                
                if ( selectedItem == newSelection )
                    return false;

                selectedItem = newSelection;
                computePositions();
                display( true );
                return true;

                // hideSelection();
                // auto origSelection = selectedItem;
                // if ( selectedItem >= optionCount - 1 ) {
                //     if ( crossover )
                //         selectedItem = 0;
                // } else
                //     selectedItem++;
                // if ( selectedItem != origSelection ) {
                //     computePositions();
                //     display( true );
                //     return true;
                // } else {
                //     // showSelection();
                //     return false;
                // }
            }

        private:

            void showMarkersCore( char left, char right ) {
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return;
                LCDInterface *lcd = ui::LCD;

                uint8_t S = posInLayout[ selectedItem ];
                uint8_t E = S + itemLen[ selectedItem ] + 1;
                S -= visibleStart;
                E -= visibleStart;
                if ( /* S >= 0 && */ S < lcd->maxColumns )
                    lcd->printAt( S, row, left );
                if ( /* E >= 0 && */ E < lcd->maxColumns )
                    lcd->printAt( E, row, right );
                selectedLeftLocation  = S;
                selectedRightLocation = E;
                // lcd->displayAll();
                lcd->refresh();
            }

            void computePositions() {
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return;
                LCDInterface *lcd = ui::LCD;

                // S/E = start/end position of selection in buffer
                uint8_t S = posInLayout[ selectedItem ];
                uint8_t E = S + itemLen[ selectedItem ] + 2;

                if ( E > visibleStart + lcd->maxColumns - 1 ) {
                    // item is beyond rightmost of screen
                    // Serial.println( "End Beyond" );
                    visibleStart = E - lcd->maxColumns;
                    // if ( visibleStart < 0 )
                    //     visibleStart = 0;
                }
                if ( E == visibleStart + lcd->maxColumns ) {
                    // exactly at end of screen
                    // Serial.println( "End Exact" );
                    if ( selectedItem < optionCount - 1 ) {
                        // not last item, adjust for overflow marker
                        visibleStart++;
                    }
                }
                if ( S < visibleStart ) {
                    // item is beyond leftmost of screen
                    // Serial.println( "Start Before" );
                    visibleStart = S;
                    // for left overflow marker
                    if ( visibleStart != 0 )
                        visibleStart--;
                }
            }

        //
        // STATIC HELPERS
        //
        // protected:
        //     #if defined(ESP8266) || defined(ESP32)
        //         // return false to breakout of loop
        //         typedef std::function<bool(void)> backgroundProcessDelegate;
        //     #else
        //         // return anything but ui::kNone to breakout of loop
        //         typedef bool (*backgroundProcessDelegate)();
        //     #endif

        public:

            // NO MESSAGE
            template<size_t optCount>
            static uint8_t choose( const char* (&options)[optCount],
            uint8_t optionsRow = 0,
            uint8_t initialSelection = 0, bool crossover = false,
            backgroundProcessDelegate backgroundProcess = nullptr ) {
            // bool (*backgroundProcess)(void) = nullptr ) {
                optionChooser chooser;
                chooser.initN( options, optCount, optionsRow, initialSelection, crossover );
                chooser.assignBackgroundProcess( backgroundProcess );
                return chooser.prompt();
            }

            // WITH MESSAGE
            template<size_t optCount>
            static uint8_t choose( const char *caption, const char* (&options)[optCount],
            uint8_t captionRow = 0, uint8_t optionsRow = 1, 
            uint8_t initialSelection = 0, bool crossover = false,
            backgroundProcessDelegate backgroundProcess = nullptr ) {
            // bool (*backgroundProcess)(void) = nullptr ) {
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return 0;
                ui::LCD->printStrAtRow( captionRow, caption );
                optionChooser chooser;
                chooser.initN( options, optCount, optionsRow, initialSelection, crossover );
                // optionChooser chooser( options, optCount, optionsRow, initialSelection, crossover );
                chooser.assignBackgroundProcess( backgroundProcess );
                return chooser.prompt();
            }

            // WITH THROTTLER
            // template<size_t optCount>
            // static inline uint8_t choose( const char* (&options)[optCount],
            // uint8_t row = 0,
            // Throttle_OLD &throttler, bool crossover = false ) {
            //     optionChooser chooser( row, options, throttler, crossover );
            //     return chooser.prompt();
            // }

        //
        // QUICK CHOICE
        //
        public:

            static uint8_t choose2( const char* opt1, const char* opt2,
            uint8_t optionsRow = 0,
            uint8_t initialSelection = 0, bool crossover = false,
            backgroundProcessDelegate backgroundProcess = nullptr ) {
            // bool (*backgroundProcess)(void) = nullptr ) {
                const char *options[] = { opt1, opt2 };
                // optionChooser chooser( (const char**) options, 2, row, initialSelection, crossover );
                optionChooser chooser;
                chooser.initN( options, 2, optionsRow, initialSelection, crossover );
                chooser.assignBackgroundProcess( backgroundProcess );
                return chooser.prompt();
            }

            static uint8_t choose2( const char* caption, const char* opt1, const char* opt2,
            uint8_t captionRow = 0, uint8_t optionsRow = 1,
            uint8_t initialSelection = 0, bool crossover = false,
            // bool (*backgroundProcess)(void) = nullptr ) {
            backgroundProcessDelegate backgroundProcess = nullptr ) {
                namespace ui = StarterPack::UserInterface;
                // if ( ui::LCD == nullptr ) return 0;
                if ( !ui::hasScreen() ) return 0;
                ui::LCD->printStrAtRow( captionRow, caption );
                return choose2(opt1, opt2, optionsRow, initialSelection, crossover, backgroundProcess);
            }

            static uint8_t choose3( const char* opt1, const char* opt2, const char* opt3,
            uint8_t optionsRow = 0,
            uint8_t initialSelection = 0, bool crossover = false,
            backgroundProcessDelegate backgroundProcess = nullptr ) {
            // bool (*backgroundProcess)(void) = nullptr ) {
                const char *options[] = { opt1, opt2, opt3 };
                optionChooser chooser;
                chooser.initN( options, 3, optionsRow, initialSelection, crossover );
                // optionChooser chooser( (const char**) options, 2, row, initialSelection, crossover );
                chooser.assignBackgroundProcess( backgroundProcess );
                return chooser.prompt();
            }

            static uint8_t choose3( const char* caption, const char* opt1, const char* opt2, const char* opt3,
            uint8_t captionRow = 0, uint8_t optionsRow = 1,
            uint8_t initialSelection = 0, bool crossover = false,
            backgroundProcessDelegate backgroundProcess = nullptr ) {
            // bool (*backgroundProcess)(void) = nullptr ) {
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return 0;
                // if ( ui::LCD == nullptr ) return 0;
                ui::LCD->printStrAtRow( captionRow, caption );
                return choose3(opt1, opt2, opt3, optionsRow, initialSelection, crossover, backgroundProcess);
            }

        //
        // YES/NO/CANCEL
        //
        private:

            // YES/NO - CORE
            static uint8_t chooseYesNoCore( uint8_t captionRow, const char *caption,
            uint8_t optionsRow,
            uint8_t initialSelection, bool crossover,
            backgroundProcessDelegate backgroundProcess ) {
            // bool (*backgroundProcess)(void) ) {
                return choose2( caption, "Yes", "No",
                    captionRow, optionsRow,
                    initialSelection, crossover, backgroundProcess );
            }

            // YES/NO/CANCEL - CORE
            static uint8_t chooseYesNoCancelCore( uint8_t captionRow, const char *caption,
            uint8_t optionsRow,
            uint8_t initialSelection, bool crossover,
            backgroundProcessDelegate backgroundProcess ) {
            // bool (*backgroundProcess)(void) ) {
                return choose3( caption, "Yes", "No", "Cancel",
                    captionRow, optionsRow,
                    initialSelection, crossover, backgroundProcess );
            }

            // NO/YES - CORE
            static uint8_t chooseNoYesCore( uint8_t captionRow, const char *caption,
            uint8_t optionsRow,
            uint8_t initialSelection, bool crossover,
            backgroundProcessDelegate backgroundProcess ) {
            // bool (*backgroundProcess)(void) ) {
                return choose2( caption, "No", "Yes",
                    captionRow, optionsRow,
                    initialSelection, crossover, backgroundProcess );
            }

            // NO/YES/CANCEL - CORE
            static uint8_t chooseNoYesCancelCore( uint8_t captionRow, const char *caption,
            uint8_t optionsRow,
            uint8_t initialSelection, bool crossover,
            backgroundProcessDelegate backgroundProcess ) {
            // bool (*backgroundProcess)(void) ) {
                return choose3( caption, "No", "Yes", "Cancel",
                    captionRow, optionsRow,
                    initialSelection, crossover, backgroundProcess );
            }

        public:

            #define CHOOSER_VARIATIONS(fName,fNameCore) \
                /* PLAIN */ \
                static uint8_t fName( \
                uint8_t optionsRow=0, \
                bool crossover=false, uint8_t initialSelection=0 ) { \
                    return fNameCore( -1, nullptr, optionsRow, initialSelection, crossover, nullptr );  \
                } \
                /* PLAIN WITH MESSAGE */ \
                static uint8_t fName( \
                const char *caption, uint8_t captionRow=0, \
                uint8_t optionsRow=1, \
                bool crossover=false, uint8_t initialSelection=0 ) { \
                    return fNameCore( captionRow, caption, optionsRow, initialSelection, crossover, nullptr );  \
                } \
                /* WITH BACKGROUND PROCESS */ \
                static uint8_t fName( \
                backgroundProcessDelegate backgroundProcess, \
                uint8_t optionsRow=0, \
                bool crossover=false, uint8_t initialSelection=0 ) { \
                    return fNameCore( -1, nullptr, optionsRow, initialSelection, crossover, backgroundProcess ); \
                } \
                /* WITH MESSAGE / BACKGROUND PROCESS */ \
                static uint8_t fName( \
                backgroundProcessDelegate backgroundProcess, \
                const char *caption, \
                uint8_t captionRow=0, uint8_t optionsRow=1, \
                bool crossover=false, uint8_t initialSelection=0 ) { \
                    return fNameCore( captionRow, caption, optionsRow, initialSelection, crossover, backgroundProcess ); \
                }
                
            CHOOSER_VARIATIONS( chooseYesNo,       chooseYesNoCore       )
            CHOOSER_VARIATIONS( chooseYesNoCancel, chooseYesNoCancelCore )
            CHOOSER_VARIATIONS( chooseNoYes,       chooseNoYesCore       )
            CHOOSER_VARIATIONS( chooseNoYesCancel, chooseNoYesCancelCore )
            #undef CHOOSER_VARIATIONS

    };
    
};

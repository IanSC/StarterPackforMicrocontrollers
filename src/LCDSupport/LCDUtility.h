//  2022/01 - isc
//
//  LCD Utilities
//  -------------
//
//      chooser
//      

#pragma once

#include <UserInterface.h>
#include <LCD/LCDInterface.h>

#include <Throttle_OLD.h>
// #include <spUtility.h>

namespace StarterPack {

namespace spLCD {

//
// CHOOSER
//
// public:




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
                if ( optionCount > UINT8_MAX )
                    optionCount = UINT8_MAX;
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
                if ( S >= 0 && S < lcd->maxColumns )
                    lcd->printAt( S, row, left );
                if ( E >= 0 && E < lcd->maxColumns )
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
                    if ( visibleStart < 0 )
                        visibleStart = 0;
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

            // #define CHOOSER_VARIATIONS(fName,fNameCore) \
            //     /* PLAIN */ \
            //     static uint8_t fName( \
            //     uint8_t optionsRow=0, \
            //     bool crossover=false, uint8_t initialSelection=0 ) { \
            //         return fNameCore( -1, nullptr, optionsRow, initialSelection, crossover, nullptr );  \
            //     } \
            //     /* PLAIN WITH MESSAGE */ \
            //     static uint8_t fName( \
            //     const char *caption, uint8_t captionRow=0, \
            //     uint8_t optionsRow=1, \
            //     bool crossover=false, uint8_t initialSelection=0 ) { \
            //         return fNameCore( captionRow, caption, optionsRow, initialSelection, crossover, nullptr );  \
            //     } \
            //     /* WITH BACKGROUND PROCESS */ \
            //     static uint8_t fName( \
            //     bool (*backgroundProcess)(void), \
            //     uint8_t optionsRow=0, \
            //     bool crossover=false, uint8_t initialSelection=0 ) { \
            //         return fNameCore( -1, nullptr, optionsRow, initialSelection, crossover, backgroundProcess ); \
            //     } \
            //     /* WITH MESSAGE / BACKGROUND PROCESS */ \
            //     static uint8_t fName( \
            //     bool (*backgroundProcess)(void), \
            //     const char *caption, \
            //     uint8_t captionRow=0, uint8_t optionsRow=1, \
            //     bool crossover=false, uint8_t initialSelection=0 ) { \
            //         return fNameCore( captionRow, caption, optionsRow, initialSelection, crossover, backgroundProcess ); \
            //     }

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
    
//
// LONG MESSAGE
//
// public:

    //  Usage:
    //      const char* msg[] = {"Apple","Banana","..."};
    //      showMultiLineMsg(msg);
    //
    //  NOT WORKING, HOW TO ???:
    //  showMultiLineMsg((const char*[]) {"Apple","Banana","...",nullptr});

    #if defined(ESP8266) || defined(ESP32)
        typedef std::function<bool(void)> backgroundProcessDelegate;
    #else
        typedef bool (*backgroundProcessDelegate)();
    #endif

// protected:

    static uint8_t showMultiLineMsgN( const char **msg, int msgCount, backgroundProcessDelegate func=nullptr ) {
        namespace ui = StarterPack::UserInterface;

        if ( !ui::hasScreen() ) return 0;

        ui::LCD->clear();
        int lineNo = 0;

        ui::waitUntilNothingIsPressed();
        bool update = true;

        while ( true ) {

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

//
// CHARACTER BLINKER
//
//public:

    class charBlinker {

        //  usage:
        //      LCD_blink blinker(0,0,0xA5);
        //      void loop() {
        //          blinker.update();    
        //      }

        private:

            uint32_t blinkTime = millis();
            bool blinkMode = false;

            // LCDInterface *lcd;
            uint8_t col = 0;
            uint8_t row = 0;

        public:

            // 0xA5 - bullet Japanese version LCD, 0xCD - bullet Europe version LCD
            uint8_t symbol = '.'; // 0xA5 - bullet Japanese version LCD, 0xCD - bullet Europe version LCD
            uint16_t blinkRateMs = 1000;

            charBlinker() {}

            charBlinker( uint8_t col, uint8_t row, uint8_t symbol='.', uint16_t blinkRateMs=1000 ) {
                this->col = col;
                this->row = row;
                this->symbol = symbol;
            }

            void update() {
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return;
                uint32_t now = millis();
                if ( now - blinkTime >= blinkRateMs ) {
                    ui::LCD->writeAt( col, row, blinkMode ? symbol : ' ' );
                    blinkMode = !blinkMode;
                    blinkTime = now;
                    // ui::LCD->displayAll();
                }
            }

            void setPosition( uint8_t col, uint8_t row ) {
                // hide previous, before moving
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return;
                ui::LCD->writeAt( col, row, ' ' );
                blinkMode = true; // next is show
                // if ( !blinkMode ) {
                //     // false means dot is currently displayed, hide it
                //     ui::LCD->writeAt( col, row, ' ' );
                //     blinkMode = !blinkMode;
                //     // ui::LCD->displayAll();
                // }
                this->col = col;
                this->row = row;
            }

    };

//
// CHARACTER SETS
//
//public:

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

//
// BLINK SOS
//
/*
    class blinkSOS {

            uint8_t col; uint8_t row;
            char ch;
            
            uint8_t counter = 0;
            bool blinkOn = false;
            uint32_t lastAction;

        public:

            uint8_t blinkCount = 3;
            uint16_t blinkDuration = 1000;
    
            blinkSOS( uint8_t col, uint8_t row, char ch, uint8_t blinkCount = 3 ) {
                this->col = col; this->row = row; this->ch = ch;
                this->blinkCount = blinkCount;
            }

            void setPosition( uint8_t col, uint8_t row ) { this->col = col; this->row = row; }
            void setCharacter( char ch ) { this->ch = ch; }
            
            void start() {
                if ( counter != 0 ) return;
                counter = blinkCount;
                blinkOn = false;
                lastAction = millis();
            }
            void stop() {
                if ( !ui::hasScreen() ) return;
                lcd.setCursor( col, row );
                lcd.print( ' ' );
                counter = 0;
            }

            void loop() {
                if ( counter == 0 ) return;
                uint32_t now = millis();
                if ( now - lastAction < blinkDuration ) return;

                lcd.setCursor( col, row );
                if ( blinkOn ) {
                    lcd.write( ' ' );
                    counter--;
                } else {
                    lcd.write( ch );
                }
                blinkOn = !blinkOn;
                lastAction = now;
            }

    };
*/

};

// LCD::optionChooser LCD::staticOptionChooser;

}

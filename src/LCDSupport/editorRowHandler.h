#pragma once

#include <inttypes.h>

#include <Utility/spVector.h>

namespace StarterPack {

template <typename PAYLOAD>
class rowHandler {

    protected:
                                    // DYNAMIC
        uint8_t itemOnTop = 0;      // entry# on topmost of screen
        uint8_t focusedEntry = 0;   // focusable item on screen

                                    // FIXED
        uint8_t count = 0;          // number of items
        uint8_t lcdRows;            // number of rows of physical screen

        // keep copy to find latched entry
        // StarterPack::PropertyEditorEntry::entryCore *head;
        spVector<PAYLOAD> *head;

    public:

        bool allowCrossover = false;

        rowHandler( spVector<PAYLOAD> *se,
        uint8_t lcdRows, bool allowCrossover = false ) {
            head = se;
            this->lcdRows = lcdRows;
            this->allowCrossover = allowCrossover;
            count = se->count;
            // while( se != nullptr ) {
            //     count++;
            //     se = se->next;
            // }
            focusScanDownwards( 0 );
        }

        PAYLOAD *getEntry( uint8_t entryNo ) {
            if ( entryNo < 0 || entryNo >= count ) return nullptr;
            auto *ptr = head->getFirst();
            for( int i = 0 ; i < entryNo ; i++ )
                ptr = head->getNext();
            return ptr;
        }

        inline bool hasFocusedEntry() {
            return ( focusedEntry != 0xFF );
        }
        inline uint8_t focusedOnScreenRow() {
            // row on physical screen of focusable row
            if ( focusedEntry == 0xFF ) return 0xFF;
            return focusedEntry - itemOnTop;
        }
        inline PAYLOAD *getFocusedEntry() {
            return getEntry( focusedEntry );
        }
        inline PAYLOAD *getItemOnTopRow() {
            return getEntry( itemOnTop );
        }

        #define itemOnBottom         (itemOnTop+(lcdRows-1))
        #define incItemOnBottom      itemOnTop++
        #define decItemOnBottom      itemOnTop--
        #define setItemOnBottom(x)   itemOnTop=((x)-(lcdRows-1))

        bool scrollUpDown( bool goingUp ) {
            uint8_t origItemOnTop = itemOnTop;
            // uint8_t origCurrent = current;
            uint8_t origFocusedEntry = focusedEntry;
            scrollUpDownCore( goingUp );
            /*
            // Serial.println();
            // SerialPrintfln( "entry   : %d", latchedEntry );
            // SerialPrintfln( "top/bottom: %d - %d", itemOnTop, itemOnBottom );
            // SerialPrintfln( "min/max   : %d - %d", minFocus, maxFocus );
            // SerialPrintfln( "count     : %d", count );
            */
            return ( origItemOnTop != itemOnTop || origFocusedEntry != focusedEntry );
        }

    protected:

        void scrollUpDownCore( bool goingUp ) {
            auto origFocusedEntry = focusedEntry;
            if ( focusedEntry == 0xFF ) {
                scrollNothingFocused( goingUp );
                return;
            }
            if ( goingUp ) {
                if ( focusScanUpwards( focusedEntry-1 ) ) {
                    // has focusable item above
                    // ... check if visible on screen
                    if ( isVisibleOnScreen( focusedEntry ) )
                        return;
                    // ... not on screen, continue and scroll
                }
                if ( itemOnTop > 0 ) {
                    // scroll up and
                    //    find focusable item from above
                    itemOnTop--;
                    focusScanDownwards( itemOnTop );
                } else if ( allowCrossover ) {
                    crossoverToBottom();
                } else {
                    // nothing changed maintain previous
                    focusedEntry = origFocusedEntry;
                }
            } else {
                if ( focusScanDownwards( focusedEntry+1 ) ) {
                    // has focusable item below
                    // ... check if visible on screen
                    if ( isVisibleOnScreen( focusedEntry ) )
                        return;
                    // ... not on screen, continue and scroll
                }
                if ( itemOnBottom < count-1 ) {
                    // scroll down and
                    //    find focusable item from below
                    itemOnTop++;
                    focusScanUpwards( itemOnBottom );
                } else if ( allowCrossover ) {
                    crossoverToTop();
                } else {
                    // nothing changed maintain previous
                    focusedEntry = origFocusedEntry;
                }
            }
        }

        void scrollNothingFocused( bool goingUp ) {
            // nothing focused, scroll screen
            if ( goingUp ) {
                if ( itemOnTop > 0 ) {
                    // not topmost line, scroll up
                    // check only newly visible item
                    itemOnTop--;
                    focusOnRow( itemOnTop );
                } else if ( allowCrossover ) {
                    crossoverToBottom();
                }
            } else {
                if ( itemOnBottom < count-1 ) {
                    // not on last line, scroll down
                    // check only newly visible item
                    incItemOnBottom;
                    focusOnRow( itemOnBottom );
                } else if ( allowCrossover ) {
                    crossoverToTop();
                }
            }
        }

        bool focusOnRow( uint8_t row ) {
            auto se = getEntry( row );
            if ( se != nullptr && se->isSelectable() ) {
                focusedEntry = row;
                return true;
            }
            focusedEntry = 0xFF;
            return false;
        }

        bool focusScanUpwards( uint8_t row ) {
            for( int i = 0 ; i < lcdRows ; i++ ) {
                if ( focusOnRow( row ) )
                    return true;
                row--;
            }
            focusedEntry = 0xFF;
            return false;
        }

        bool focusScanDownwards( uint8_t row ) {
            for( int i = 0 ; i < lcdRows ; i++ ) {
                if ( focusOnRow( row ) )
                    return true;
                row++;
            }
            focusedEntry = 0xFF;
            return false;
        }

        bool isVisibleOnScreen( uint8_t item ) {
            return ( item >= itemOnTop && item <= itemOnBottom );
        }

        void crossoverToTop() {
            // focus on 1st focusable item
            itemOnTop = 0;
            focusScanDownwards( 0 );
        }

        void crossoverToBottom() {
            // focus on last focusable item
            setItemOnBottom( count-1 );
            focusScanUpwards( count-1 );
        }
        #undef itemOnBottom
        #undef incItemOnBottom
        #undef decItemOnBottom

};

}

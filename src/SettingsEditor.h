#pragma once
#include <stdint.h>
#include <limits.h>
#include <math.h>
// #include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <spUtility.h>
#include <spVector.h>

#include <LCDEditorAlpha.h>
#include <LCDEditorNumeric.h>
#include <LCDInterface.h>
// #include <keypad.h>

// using namespace StarterPack;

namespace StarterPack {

//
// ITEMS
//

class SettingsEditor;

    struct seEntry {

        enum dataType {
            custom,
            _dash,
            _bool,
            _si08,
            _ui08,
            _si16,
            _ui16,
            _si32,
            _ui32,
            _flot,
            _doob,
            _pick,
            _text
        };

        union dataUnion {
            bool       _bool;
            int8_t     _si08;
            uint8_t    _ui08;
            int16_t    _si16;
            uint16_t   _ui16;
            int32_t    _si32;
            uint32_t   _ui32;
            float      _flot;
            double     _doob;
            const char **pickOptions; // tuck here instead of in min/max
            char*      _text;
        };

        struct pickData {
            uint8_t  selected; // number picked
            uint8_t  length;   // # of picks
        };

        union rangeUnion {
            int8_t   _si08;
            uint8_t  _ui08;
            int16_t  _si16;
            uint16_t _ui16;
            int32_t  _si32;
            uint32_t _ui32;
            float    _flot;
            double   _doob;
            pickData _pick;
            uint8_t  length; // text buffer length
        };

        const char *caption;
        void       *ptr;
        dataType    type;
        dataUnion   data;
        rangeUnion  min;
        rangeUnion  max;
        bool        readonly;
        // seEntry *next = nullptr;

    //
    //
    //

        bool moveLeft() {
            if ( readonly ) return false;
            switch( type ) {
            case dataType::_bool:
                if ( data._bool ) {
                    data._bool = false;
                    return true;
                }
                break;
            case dataType::_pick:
                if ( max._pick.selected == 0 || max._pick.selected > max._pick.length ) {
                    max._pick.selected = 1;
                    return true;
                } else if ( max._pick.selected > 1 ) {
                    max._pick.selected--;
                    return true;
                }
                break;
            default: break;
            }
            return false;
        }

        bool moveRight() {
            if ( readonly ) return false;
            switch( type ) {
            case dataType::_bool:
                if ( !data._bool ) {
                    data._bool = true;
                    return true;
                }
                break;
            case dataType::_pick:
                if ( max._pick.selected == 0 || max._pick.selected > max._pick.length ) {
                    max._pick.selected = 1;
                    return true;
                } else if ( max._pick.selected < max._pick.length ) {
                    max._pick.selected++;
                    return true;
                }
                break;
            default: break;
            }
            return false;
        }

        bool enterEditMode( StarterPack::editorSettings &eSet ) {
            namespace ui = StarterPack::UserInterface;
            if ( readonly ) return false;

            switch( type ) {
            case dataType::custom:
                 break;
            case dataType::_bool:
                data._bool = !data._bool;
                return true;
            case dataType::_pick:
                // with rollover
                max._pick.selected++;
                if ( max._pick.selected < 1 || max._pick.selected > max._pick.length )
                    max._pick.selected = 1;
                return true;
            case dataType::_text: {
                    char buffer[max.length];
                    eSet.bufferLength = max.length;
                    strncpy( buffer, data._text, max.length );
                    buffer[max.length-1] = 0;
                    StarterPack::alphanumericEditor editor( buffer, eSet );
                    while( true ) {
                        uint8_t r = editor.prompt();
                        if ( r == ui::kESCAPE )
                            break;
                        if ( r == ui::kENTER ) {
                            strcpy( data._text, buffer );
                            break;
                        }
                    }
                    return true;
                }
                break;
            default: {
                    char buffer[maxLength()];
                    eSet.bufferLength = maxLength();
                    eSet.allowDecimal = isDecimalAllowed();
                    eSet.allowNegative = isNegativeAllowed();
                    ToString( buffer, maxLength() );
                    StarterPack::numericEditor editor( buffer, eSet );
                    uint8_t origWindowSize = eSet.windowSize;
                    while( true ) {
                        uint8_t r = editor.prompt();
                        if ( r == ui::kESCAPE )
                            break;
                        if ( r == ui::kENTER ) {
                            if ( isValid( buffer ) ) {
                                break;
                            } else {
                                // not valid, show warning and edit again
                                eSet.windowSize = origWindowSize-1;
                                ui::LCD->writeAt( eSet.col+origWindowSize-2, eSet.row, ']' );
                                ui::LCD->write( 0b11101111 ); // pikachu face
                            }
                        }
                    }
                    return true;
                }
                break;
            }
            return false;
        }

    //
    // ROUTINES
    //

        void ToString( char *buffer, uint8_t len ) {
            switch( type ) {
            // case _void:
            // case _dash:
            case _bool: strncpy( buffer, data._bool ?  "TRUE" : "FALSE", len ); break;
            case _si08: snprintf( buffer, len, "%d", data._si08 ); break;
            case _ui08: snprintf( buffer, len, "%u", data._ui08 ); break;
            case _si16: snprintf( buffer, len, "%d", data._si16 ); break;
            case _ui16: snprintf( buffer, len, "%u", data._ui16 ); break;
            case _si32: snprintf( buffer, len, "%d", data._si32 ); break;
            case _ui32: snprintf( buffer, len, "%u", data._ui32 ); break;
            case _flot:
            case _doob:
                if ( type == _flot )
                    snprintf( buffer, len, "%f", data._flot );
                else if ( type == _doob )
                    snprintf( buffer, len, "%f", data._doob );
                if ( isCharInString( '.', buffer ) ) {
                    char *p = buffer + strlen(buffer) - 1;
                    while( *p == '0' ) *p-- = 0;
                    if ( *p == '.' ) *p = 0;
                }
                break;
            case _pick:
                // 1-based index
                Serial.println("PICK");
                Serial.println( max._pick.selected );
                Serial.println( data.pickOptions[max._pick.selected-1] );

                if ( max._pick.selected < 1 || max._pick.selected > max._pick.length ) {
                    buffer[0] = 0;
                } else {
                    strncpy( buffer, data.pickOptions[max._pick.selected-1], len );
                    buffer[len-1] = 0; // in case option is too long
                }
                break;
            case _text:
                strncpy( buffer, data._text, len );
                buffer[len-1] = 0; // in case text is too long
                break;
            default:
                buffer[0] = 0;
            }
        }

        // bool isNumeric() {

        // }

        bool isDecimalAllowed() {
            return ( type==dataType::_flot || type==dataType::_doob );
        }

        bool isNegativeAllowed() {
            switch( type ) {
            case _si08:
            case _si16:
            case _si32:
            case _flot:
            case _doob: return true;
            default: return false;
            }
        }

        bool isSelectable() {
            return ( type!=dataType::custom && type!=dataType::_dash );
        }
        
        uint8_t maxLength() {
            switch( type ) {
            // case _void:
            // case _bool:
            case _si08: return 4+1;    //        -127
            case _ui08: return 3+1;    //         255
            case _si16: return 6+1;    //      -32768
            case _ui16: return 5+1;    //       65535
            case _si32: return 11+1;   // -2147483647
            case _ui32: return 10+1;   //  4294967295
            case _flot: return 0xFF;
            case _doob: return 0xFF;
            // case _pick:
            default: return 0;
            }
        }

        bool isValid( char *buffer ) {
            int64_t I = 0; double F = 0;
            switch( type ) {
            // case _void:
            // case _bool:
            case _si08: I = atoll( buffer ); if ( I < min._si08 || I > max._si08 ) return false; data._si08 = I; break;
            case _ui08: I = atoll( buffer ); if ( I < min._ui08 || I > max._ui08 ) return false; data._ui08 = I; break;
            case _si16: I = atoll( buffer ); if ( I < min._si16 || I > max._si16 ) return false; data._si16 = I; break;
            case _ui16: I = atoll( buffer ); if ( I < min._ui16 || I > max._ui16 ) return false; data._ui16 = I; break;
            case _si32: I = atoll( buffer ); if ( I < min._si32 || I > max._si32 ) return false; data._si32 = I; break;
            case _ui32: I = atoll( buffer ); if ( I < min._ui32 || I > max._ui32 ) return false; data._ui32 = I; break;
            case _flot: F = atof ( buffer );
                if ( F < min._flot || F > max._flot ) return false; data._flot = F; break;
            case _doob: F = atof ( buffer ); if ( F < min._doob || F > max._doob ) return false; data._doob = F; break;            
            // case _pick:
            default: return false;
            }
            return true;
        }

        void acceptChange() {
            switch( type ) {
            // case _void:
            case _bool: *((bool*)     ptr) = data._bool; break;
            case _si08: *((int8_t*)   ptr) = data._si08; break;            
            case _ui08: *((uint8_t*)  ptr) = data._ui08; break;  
            case _si16: *((int16_t*)  ptr) = data._si16; break;  
            case _ui16: *((uint16_t*) ptr) = data._ui16; break;  
            case _si32: *((int32_t*)  ptr) = data._si32; break;  
            case _ui32: *((uint32_t*) ptr) = data._ui32; break;  
            case _flot: *((float*)    ptr) = data._flot; break;  
            case _doob: *((double*)   ptr) = data._doob; break;
            case _pick: *((uint8_t*)  ptr) = max. _pick.selected; break;
            case _text: strcpy( (char*) ptr, data._text );
            default:
                break;
            }
        }

    };

//
// CORE
//

class SettingsEditor {

    public:

        ~SettingsEditor() {
            seEntry *se = head.getFirst();
            // seEntry *se = head;
            while( se != nullptr ) {
                // delete text buffers
                if ( se->type == seEntry::dataType::_text )
                    delete se->data._text;
                se = head.getNext();
                // se = se->next;
            }
            // StarterPack::delete_NextLinkedList( head );
        }

        bool allowCrossover = true;

        void text( const char *caption ) {
            seEntry *se = new seEntry();
            se->caption = caption;
            se->type = seEntry::dataType::custom;
            insert( se );
        }

        void breaker( char ch = '-' ) {
            seEntry *se = new seEntry();
            se->type = seEntry::dataType::_dash;
            se->data._ui08 = ch;
            insert( se );
        }

        #define ADD(dType,D,MIN,MAX) \
            void add( const char *caption, dType &data, bool readonly=false, dType min=MIN, dType max=MAX ) { \
                seEntry *se = new seEntry(); \
                se->caption = caption; \
                se->ptr = &data; \
                se->type = seEntry::dataType::D; \
                se->data.D = data; \
                se->min.D = min; \
                se->max.D = max; \
                se->readonly = readonly; \
                insert( se ); \
            }
        // ADD( bool,     _bool, false, true );
        ADD( int8_t,   _si08, INT8_MIN, INT8_MAX );
        ADD( uint8_t,  _ui08, 0, UINT8_MAX );
        ADD( int16_t,  _si16, INT16_MIN, INT16_MAX );
        ADD( uint16_t, _ui16, 0, UINT16_MAX );
        ADD( int32_t,  _si32, INT32_MIN, INT32_MAX );
        ADD( uint32_t, _ui32, 0, UINT32_MAX );
        ADD( float,    _flot, -__FLT_MAX__, __FLT_MAX__ );
        ADD( double,   _doob, -__DBL_MAX__, __DBL_MAX__ );
        #undef ADD

        void add( const char *caption, bool &data, bool readonly=false ) {
            seEntry *se = new seEntry();
            se->caption = caption;
            se->ptr = &data;
            se->type = seEntry::dataType::_bool;
            se->data._bool = data;
            se->readonly = readonly;
            insert( se );
        }

        template<size_t optCount>
        void addPick( const char *caption, uint8_t &data, const char* (&options)[optCount], bool readonly=false ) {
            seEntry *se = new seEntry();
            se->caption = caption;
            se->ptr = &data;
            se->type = seEntry::dataType::_pick;
            se->data.pickOptions = options;
            se->max._pick.selected = data;
            se->max._pick.length = optCount;
            se->readonly = readonly;
            insert( se );
        }

        void addString( const char *caption, char *buffer, uint8_t size, bool readonly=false ) {
            seEntry *se = new seEntry();
            se->caption = caption;
            se->ptr = buffer;
            se->type = seEntry::dataType::_text;
            se->data._text = new char[size];
            strcpy( se->data._text, buffer );
            se->max.length = size;
            se->readonly = readonly;
            insert( se );
        }

    //
    // ENTRIES
    //
    protected:

        spVector<seEntry> head;
        // seEntry *head = nullptr;

        inline void insert( seEntry *se ) {
            head.insert( se );
            // insertEnd_NextLinkedList( &head, se );
        }

        seEntry *getEntry( uint8_t entryNo ) {
            seEntry *ptr = head.getFirst();
            // seEntry *ptr = head;
            for( int i = 0 ; i < entryNo ; i++ ) {
                ptr = head.getNext();
                // ptr = ptr->next;
            }
            return ptr;
        }

    public:

        struct rowData {

            // OLD <PropertyEditor.h>'s is better

            uint8_t itemOnTop  = 0;
            uint8_t count   = 0;
            uint8_t current = 0;
            int8_t  minFocus = -1;
            uint8_t maxFocus = 0;
            uint8_t lcdRows;

            void compute(
            spVector<seEntry> *head,
            // seEntry *se,
            uint8_t lcdRows ) {

                this->lcdRows = lcdRows;

                auto se = head->getFirst();
                while( se != nullptr ) {
                    count++;
                    if ( se->isSelectable() ) {
                        if ( minFocus == -1 )
                            minFocus = count-1;
                        maxFocus = count-1;
                    }
                    se = head->getNext();
                    // se = se->next;
                }
                current = minFocus;
            }

            inline uint8_t onScreen() {
                return current - itemOnTop;
            }

            void scrollUpDown( bool goingUp, bool allowCrossover ) {

                uint8_t itemOnBottom = itemOnTop + (lcdRows-1);

                if ( goingUp ) {
                    if ( current <= minFocus ) {
                        // focused on highest already
                        if ( itemOnTop > 0 ) {
                            // scroll up some more
                            itemOnTop--;
                            if ( current < itemOnTop )
                                current--;
                        } else if ( allowCrossover ) {
                            // first line visible already
                            // crossover to bottom
                            current = maxFocus;
                            itemOnTop = (count-1) - (lcdRows-1);
                            if ( current < itemOnTop )
                                current = itemOnTop;
                        }
                    } else {
                        current--;
                        if ( current < itemOnTop ) {
                            // scroll up
                            itemOnTop = current;
                        }
                    }
                } else {
                    if ( current >= maxFocus ) {
                        // focused on max already
                        // try scrolling only
                        if ( itemOnBottom < count-1 ) {
                            // not on last line
                            // scroll down some more
                            itemOnBottom++;
                            itemOnTop = itemOnBottom - (lcdRows-1);
                            if ( current < itemOnTop )
                                current++;
                        } else if ( allowCrossover ) {
                            // last line already visible
                            // crossover to top
                            current = minFocus;
                            itemOnTop = 0;
                            itemOnBottom = itemOnTop + (lcdRows-1);
                            if ( current > itemOnBottom )
                                current = itemOnBottom;
                            itemOnTop = itemOnBottom - (lcdRows-1);
                        }
                    } else {
                        current++;
                        if ( current > itemOnBottom ) {
                            // scroll down
                            itemOnBottom = current;
                            itemOnTop = itemOnBottom - (lcdRows-1);
                        }
                    }
                }
                // Serial.println();
                // SerialPrintfln( "current   : %d", current );
                // SerialPrintfln( "top/bottom: %d - %d", itemOnTop, itemOnBottom );
                // SerialPrintfln( "min/max   : %d - %d", minFocus, maxFocus );
                // SerialPrintfln( "count     : %d", count );
            }
        };

        void prompt( uint8_t captionWidth ) {
            namespace ui = StarterPack::UserInterface;

            if ( StarterPack::UserInterface::LCD == nullptr ) return;
            LCDInterface *lcd = StarterPack::UserInterface::LCD;

            rowData row;
            row.compute( &head, lcd->maxRows );

            seEntry *se;

            char buffer[lcd->maxColumns+1];

            if ( captionWidth > lcd->maxColumns )
                captionWidth = lcd->maxColumns;
            uint8_t dataWidth = lcd->maxColumns - captionWidth - 2;
            
            StarterPack::editorSettings editorSettings;
            editorSettings.col = captionWidth+1;

            bool updateScreen = true;

            while( true ) {

                if ( updateScreen ) {
                    updateScreen = false;

                    se = head.getFirst();
                    // se = head;
                    for( int i = 0 ; i < row.itemOnTop ; i++ ) {
                        se = head.getNext();
                        // se = se->next;
                    }

                    for( int row = 0 ; row < lcd->maxRows ; row++ ) {
                        if ( se == nullptr ) {
                            lcd->clearRow( row );
                            continue;
                        }
                        lcd->setCursor( 0, row );
                        switch( se->type ) {
                        case seEntry::dataType::custom:
                            lcd->printStrN( se->caption, lcd->maxColumns, true );
                            break;
                        case seEntry::dataType::_dash:
                            lcd->printCharsN( se->data._ui08, lcd->maxColumns );
                            break;
                        default:
                            // CAPTION
                            lcd->printStrN( se->caption, captionWidth, true );
                            lcd->print( ' ' );
                            // DATA
                            se->ToString( buffer, lcd->maxColumns );
                            lcd->setCursor( captionWidth+1, row );
                            lcd->printStrN( buffer, dataWidth, true );
                            lcd->print( ' ' );
                            uint8_t len = strlen( buffer );
                            if ( len > dataWidth )
                                lcd->writeAt( lcd->maxColumns-2, row, 0x7E );                        
                        }
                        se = head.getNext();
                        // se = se->next;
                    }

                    se = getEntry( row.current );
                    if ( se->isSelectable() ) {
                        int r = row.onScreen();
                        lcd->printAt( captionWidth, r, '[' );
                        lcd->printAt( lcd->maxColumns-1, r, ']' );
                    }
                    lcd->displayAll();
                }

                uint8_t key = ui::getRepeatingKey();
                
                if ( key == ui::kESCAPE ) {
                    return;
                } else if ( key == ui::kUP ) {
                    row.scrollUpDown( true, allowCrossover );
                    updateScreen = true;
                } else if ( key == ui::kDOWN ) {
                    row.scrollUpDown( false, allowCrossover );
                    updateScreen = true;
                } else if ( key == ui::kLEFT ) {
                    se = getEntry( row.current );
                    if ( se->moveLeft() )
                        updateScreen = true;
                } else if ( key == ui::kRIGHT ) {
                    se = getEntry( row.current );
                    if ( se->moveRight() )
                        updateScreen = true;
                } else if ( key == ui::kENTER ) {
                    // edit value
                    se = getEntry( row.current );

                    StarterPack::editorSettings eSet;
                    eSet.col = captionWidth+1;
                    eSet.row = row.onScreen();
                    eSet.windowSize = dataWidth;

                    if ( se->enterEditMode( eSet ) )
                        updateScreen = true;
                }
            }
        }

        void acceptChanges() {
            seEntry *se = head.getFirst();
            // seEntry *se = head;
            while( se != nullptr ) {
                if ( !se->readonly )
                    se->acceptChange();
                se = head.getNext();
                // se = se->next;
            }
        }

};

//
// TEST
//

    void TEST_settingsEditor1() {
        SettingsEditor se;
        
        se.allowCrossover = true;
        se.allowCrossover = false;

        se.text( "Editable Options" );
        bool t = true;
        bool f = false;
        se.add( "bool 1", t );
        se.add( "bool 2", f );
        int8_t  si08 = -100;
        uint8_t ui08 = 100;
        se.breaker( '-' );
        se.add( "si08", si08 );
        se.add( "ui08", ui08 );
        int16_t  si16 = -100;
        uint16_t ui16 = 100;
        se.add( "si16", si16 );
        se.add( "ui16", ui16 );
        int32_t  si32 = -100;
        uint32_t ui32 = 100;
        se.add( "si32", si32 );
        se.add( "ui32", ui32 );
        float  flot1 = -1234567890.123456789;
        float  flot2 = 1234567890.123456789;
        double doob1 = -1234567890.123456789;
        double doob2 = -1234567890.123456789;
        se.breaker( '=' );
        se.add( "flot1", flot1 );
        se.add( "flot2", flot2 );
        se.add( "doob1", doob1 );
        se.add( "doob2", doob2 );

        uint8_t pick = 2;
        static const char *options[] = { "No", "Yes", "Cancel" };
        se.addPick( "pick", pick, options );

        char buffer[15];
        strcpy( buffer, "there" );
        se.addString( "hello", buffer, 15 );

        se.text( "end of options" );

        se.prompt( 10 );

        Serial.println( "BEFORE:" );
        Serial.println( t );
        Serial.println( si08 );
        Serial.print( buffer );
        Serial.println( "<<<" );
        se.acceptChanges();
        Serial.println( "AFTER:" );
        Serial.println( t );
        Serial.println( si08 );
        Serial.print( buffer );
        Serial.println( "<<<" );
    }

    void TEST_settingsEditor2() {
        SettingsEditor se;
        se.allowCrossover = false;
        se.allowCrossover = true;
        bool t = true;
        se.text( "*** FIRST ***" );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "Text" );
        se.add( "bool 1", t );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "Text" );
        se.add( "bool 1", t );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "*** LAST ***" );

        se.prompt( 10 );
    }

}

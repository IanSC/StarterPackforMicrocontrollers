#pragma once

#include <stdint.h>
#include <limits.h>
#include <math.h>
// #include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include <Utility/spStr.h>
#include <Utility/spVector.h>
#include <Utility/spNum.h>

#include <LCDSupport/LCDEditorAlpha.h>
#include <LCDSupport/LCDEditorNumeric.h>
#include <LCD/LCDInterface.h>

#include <LCDSupport/editorRowHandler.h>

namespace StarterPack {

//
// ITEMS
//

    class SettingsEditor;

    struct seEntry {

        enum class dataType {
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

        struct boolData {
            bool value;             // value chosen
            bool rollover;          // go to start if past end, etc.
        };

        struct pickData {
            uint8_t selected;       // number picked
            uint8_t optionCount;    // # of options
            bool    rollover;       // go to start if past end, etc.
        };

        union dataUnion {
            boolData   _bool;
            // bool       _bool;
            int8_t     _si08;
            uint8_t    _ui08;
            int16_t    _si16;
            uint16_t   _ui16;
            int32_t    _si32;
            uint32_t   _ui32;
            float      _flot;
            double     _doob;
            pickData   _pick;
            // const char **pickOptions; // tuck here instead of in min/max
            char*      _text;
        };

        union rangeUnion {
            int8_t     _si08;
            uint8_t    _ui08;
            int16_t    _si16;
            uint16_t   _ui16;
            int32_t    _si32;
            uint32_t   _ui32;
            float      _flot;
            double     _doob;
            // pickData   _pick;
            const char **pickOptions; // tuck here instead of in min/max
            uint8_t    length;        // text buffer length
            const char *text;
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
    // ACCESS MAPPING
    //

        #define BoolValue         data._bool.value
        #define BoolRollover      data._bool.rollover

        #define PickSelected      data._pick.selected
        #define PickOptionsCount  data._pick.optionCount
        #define PickRollover      data._pick.rollover
        #define PickOptions       max.pickOptions

    //
    // INTEGER
    //

        int64_t getIntegerData() {
            switch( type ) {
            case dataType::_si08: return data._si08;
            case dataType::_ui08: return data._ui08;
            case dataType::_si16: return data._si16;
            case dataType::_ui16: return data._ui16;
            case dataType::_si32: return data._si32;
            case dataType::_ui32: return data._ui32;
            default: break; // to disable warning
            }
            return 0;
        }

        int64_t getIntegerMin() {
            switch( type ) {
            case dataType::_si08: return min._si08;
            case dataType::_ui08: return min._ui08;
            case dataType::_si16: return min._si16;
            case dataType::_ui16: return min._ui16;
            case dataType::_si32: return min._si32;
            case dataType::_ui32: return min._ui32;
            default: break; // to disable warning
            }
            return 0;
        }

        int64_t getIntegerMax() {
            switch( type ) {
            case dataType::_si08: return max._si08;
            case dataType::_ui08: return max._ui08;
            case dataType::_si16: return max._si16;
            case dataType::_ui16: return max._ui16;
            case dataType::_si32: return max._si32;
            case dataType::_ui32: return max._ui32;
            default: break; // to disable warning
            }
            return 0;
        }

        uint32_t getIntegerRange() {
            auto range = abs(getIntegerMax()-getIntegerMin());
            return range;
        }

        bool canSpeedSlide() {
            switch( type ) {
            case dataType::_si08:
            case dataType::_ui08:
            case dataType::_si16:
            case dataType::_ui16:
            case dataType::_si32:
            case dataType::_ui32: return true;
            default: return false;
            }
        }

        void setIntegerData(int64_t value) {
            switch( type ) {
            case dataType::_si08: data._si08 = value; break;
            case dataType::_ui08: data._ui08 = value; break;
            case dataType::_si16: data._si16 = value; break;
            case dataType::_ui16: data._ui16 = value; break;
            case dataType::_si32: data._si32 = value; break;
            case dataType::_ui32: data._ui32 = value; break;
            default: break; // to disable warning
            }
        }

        bool setIntegerDelta(int64_t delta) {
            switch( type ) {
            case dataType::_si08: break;
            case dataType::_ui08: break;
            case dataType::_si16: break;
            case dataType::_ui16: break;
            case dataType::_si32: break;
            case dataType::_ui32: break;
            default: return false;
            }
            auto orig = getIntegerData();
            auto min = getIntegerMin();
            auto max = getIntegerMax();            
            auto value = orig + delta;
            if (value < min) {
                // Serial.println("UNDER");
                value = min;
            }
            if (value > max) {
                // Serial.println("OVER");
                value = max;
            }
            if (value != orig) {
                // Serial.println("SET");
                setIntegerData( value );
                return true;
            }
            return false;
        }

        bool moveLeft() {
            if ( readonly ) return false;
            switch( type ) {
            case dataType::_bool:
                if ( !BoolValue && !BoolRollover )
                    return false;
                BoolValue = !BoolValue;
                return true;
            case dataType::_pick: {
                    auto orig = PickSelected;
                    if ( orig < 1 ) {
                        PickSelected = 1;
                        return true;
                    }
                    auto value = orig - 1;
                    if ( value < 1 ) {
                        if ( !PickRollover ) return false;
                        value = PickOptionsCount;
                    }
                    PickSelected = value;
                    return true;
                }
                break;
            case dataType::_si08:
            case dataType::_ui08:
            case dataType::_si16:
            case dataType::_ui16:
            case dataType::_si32:
            case dataType::_ui32:
                return setIntegerDelta(-1);
            // case dataType::_flot:
            // case dataType::_doob:
            default: break;
            }
            return false;
        }

        bool moveRight() {
            if ( readonly ) return false;
            switch( type ) {
            case dataType::_bool:
                if ( BoolValue && !BoolRollover )
                    return false;
                BoolValue = !BoolValue;
                return true;
            case dataType::_pick: {
                    auto orig = PickSelected;
                    if ( orig > PickOptionsCount ) {
                        PickSelected = 1;
                        return true;
                    }
                    auto value = orig + 1;
                    if ( value > PickOptionsCount ) {
                        if ( !PickRollover ) return false;
                        value = 1;
                    }
                    PickSelected = value;
                    return true;
                }
            case dataType::_si08:
            case dataType::_ui08:
            case dataType::_si16:
            case dataType::_ui16:
            case dataType::_si32:
            case dataType::_ui32:
                return setIntegerDelta(1);
            // case dataType::_flot:
            // case dataType::_doob:
            default: break;
            }
            return false;
        }

    //
    // EDIT
    //

        bool enterEditMode(
        StarterPack::alphaEditorSettings   &textEditorSettings,
        StarterPack::numericEditorSettings &numericEditorSettings,
        StarterPack::alphaEditorSettings   &numericUpDownEditorSettings ) {

            namespace ui = StarterPack::UserInterface;
            if ( readonly ) return false;

            switch( type ) {
            case dataType::custom:
                 break;
            case dataType::_bool:
                BoolValue = !BoolValue;
                return true;
            case dataType::_pick:
                // with rollover
                PickSelected++;
                if ( PickSelected < 1 || PickSelected > PickOptionsCount )
                    PickSelected = 1;
                return true;
            case dataType::_text: {

                    char buffer[max.length];
                    textEditorSettings.bufferLength = max.length;
                    strncpy( buffer, data._text, max.length );
                    buffer[max.length-1] = 0;

                    StarterPack::alphanumericEditor editor( buffer, textEditorSettings );
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
                    // integers, floats and doubles

                    if ( ui::hasNumericKeypad ) {
                        
                        char buffer[maxLength()];
                        numericEditorSettings.bufferLength = maxLength();
                        numericEditorSettings.allowDecimal = isDecimalAllowed();
                        numericEditorSettings.allowNegative = isNegativeAllowed();
                        ToString( buffer, maxLength() );

                        StarterPack::numericEditor editor( buffer, numericEditorSettings );
                        uint8_t origWindowSize = numericEditorSettings.windowSize;
                        while( true ) {
                            uint8_t r = editor.prompt();
                            if ( r == ui::kESCAPE )
                                break;
                            if ( r == ui::kENTER ) {
                                if ( isValid( buffer ) ) {
                                    break;
                                } else {
                                    // not valid, show warning and edit again
                                    numericEditorSettings.windowSize = origWindowSize-1;
                                    ui::LCD->writeAt( numericEditorSettings.col+origWindowSize-2, numericEditorSettings.row, ']' );
                                    ui::LCD->write( 0b11101111 ); // pikachu face
                                }
                            }
                        }

                    } else {

                        char buffer[maxLength()];
                        numericUpDownEditorSettings.bufferLength = maxLength();

                        if (isNegativeAllowed())
                            numericUpDownEditorSettings.prefixList = "-";
                        else
                            numericUpDownEditorSettings.prefixList = nullptr;
                        char symbols[3]; int index=0;
                        if (isDecimalAllowed())
                            symbols[index++] = '.';
                        if (ui::kBACKSPACE == ui::kNONE && ui::kDELETE == ui::kNONE) {
                            // user cannot delete, just overwrite with space
                            symbols[index++] = ' ';
                        }
                        symbols[index] = 0;
                        if (index == 0)
                            numericUpDownEditorSettings.symbolList = nullptr;
                        else
                            numericUpDownEditorSettings.symbolList = symbols;

                        ToString( buffer, maxLength() );

                        StarterPack::alphanumericEditor editor( buffer, numericUpDownEditorSettings );
                        uint8_t origWindowSize = numericEditorSettings.windowSize;
                        while( true ) {
                            uint8_t r = editor.prompt();
                            if ( r == ui::kESCAPE )
                                break;
                            if ( r == ui::kENTER ) {
                                if ( isValid( buffer ) ) {
                                    break;
                                } else {
                                    // not valid, show warning and edit again
                                    numericEditorSettings.windowSize = origWindowSize-1;
                                    ui::LCD->writeAt( numericEditorSettings.col+origWindowSize-2, numericUpDownEditorSettings.row, ']' );
                                    ui::LCD->write( 0b11101111 ); // pikachu face
                                }
                            }
                        }
                        numericUpDownEditorSettings.symbolList = nullptr;
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

        static constexpr const char *defaultTrueValue = "YES";
        static constexpr const char *defaultFalseValue = "NO";

        void ToString( char *buffer, uint8_t len ) {
            switch( type ) {
            // case _void:
            // case _dash:
            case dataType::_bool:
                if ( BoolValue )
                    strncpy( buffer, max.text != nullptr ? max.text : defaultTrueValue, len );
                else
                    strncpy( buffer, min.text != nullptr ? min.text : defaultFalseValue, len );
                break;
            case dataType::_si08: snprintf( buffer, len, "%d",  data._si08 ); break;
            case dataType::_ui08: snprintf( buffer, len, "%u",  data._ui08 ); break;
            case dataType::_si16: snprintf( buffer, len, "%d",  data._si16 ); break;
            case dataType::_ui16: snprintf( buffer, len, "%u",  data._ui16 ); break;
            case dataType::_si32: snprintf( buffer, len, "%ld", data._si32 ); break;
            case dataType::_ui32: snprintf( buffer, len, "%lu", data._ui32 ); break;
            case dataType::_flot:
            case dataType::_doob:
                if ( type == dataType::_flot )
                    snprintf( buffer, len, "%f", (double) data._flot );
                else if ( type == dataType::_doob )
                    snprintf( buffer, len, "%f", data._doob );
                if ( Str::findCharacter( '.', buffer ) ) {
                    char *p = buffer + strlen(buffer) - 1;
                    while( *p == '0' ) *p-- = 0;
                    if ( *p == '.' ) *p = 0;
                }
                break;
            case dataType::_pick:
                // 1-based index
                // Serial.println("PICK");
                // Serial.println( max._pick.selected );
                // Serial.println( data.pickOptions[max._pick.selected-1] );

                if ( PickSelected < 1 || PickSelected > PickOptionsCount ) {
                    buffer[0] = 0;
                } else {
                    strncpy( buffer, PickOptions[PickSelected-1], len );
                    buffer[len-1] = 0; // in case option is too long
                }
                break;
            case dataType::_text:
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
            case dataType::_si08: return min._si08 < 0;
            case dataType::_si16: return min._si16 < 0;
            case dataType::_si32: return min._si32 < 0;
            case dataType::_flot: 
            case dataType::_doob: return true;
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
            case dataType::_si08: return 4+1;    //        -127
            case dataType::_ui08: return 3+1;    //         255
            case dataType::_si16: return 6+1;    //      -32768
            case dataType::_ui16: return 5+1;    //       65535
            case dataType::_si32: return 11+1;   // -2147483647
            case dataType::_ui32: return 10+1;   //  4294967295
            case dataType::_flot: return 0xFF;
            case dataType::_doob: return 0xFF;
            // case _pick:
            default: return 0;
            }
        }

        bool isValid( char *buffer ) {
            int64_t I = 0; double F = 0;
            switch( type ) {
            // case _void:
            // case _bool:
            case dataType::_si08: Num::StrToNum(buffer,I); if ( I < min._si08 || I > max._si08 ) return false; data._si08 = I; break;
            case dataType::_ui08: Num::StrToNum(buffer,I); if ( I < min._ui08 || I > max._ui08 ) return false; data._ui08 = I; break;
            case dataType::_si16: Num::StrToNum(buffer,I); if ( I < min._si16 || I > max._si16 ) return false; data._si16 = I; break;
            case dataType::_ui16: Num::StrToNum(buffer,I); if ( I < min._ui16 || I > max._ui16 ) return false; data._ui16 = I; break;
            case dataType::_si32: Num::StrToNum(buffer,I); if ( I < min._si32 || I > max._si32 ) return false; data._si32 = I; break;
            case dataType::_ui32: Num::StrToNum(buffer,I); if ( I < min._ui32 || I > max._ui32 ) return false; data._ui32 = I; break;
            // case _si08: I = atoll( buffer ); if ( I < min._si08 || I > max._si08 ) return false; data._si08 = I; break;
            // case _ui08: I = atoll( buffer ); if ( I < min._ui08 || I > max._ui08 ) return false; data._ui08 = I; break;
            // case _si16: I = atoll( buffer ); if ( I < min._si16 || I > max._si16 ) return false; data._si16 = I; break;
            // case _ui16: I = atoll( buffer ); if ( I < min._ui16 || I > max._ui16 ) return false; data._ui16 = I; break;
            // case _si32: I = atoll( buffer ); if ( I < min._si32 || I > max._si32 ) return false; data._si32 = I; break;
            // case _ui32: I = atoll( buffer ); if ( I < min._ui32 || I > max._ui32 ) return false; data._ui32 = I; break;
            case dataType::_flot:
                F = atof ( buffer );
                if ( F < min._flot || F > max._flot )
                    return false;
                data._flot = F;
                break;
            case dataType::_doob:
                F = atof ( buffer ); if ( F < min._doob || F > max._doob ) return false; data._doob = F; break;            
            // case _pick:
            default: return false;
            }
            return true;
        }

        void acceptChange() {
            switch( type ) {
            // case _void:
            case dataType::_bool: *((bool*)     ptr) = data._bool.value; break;
            case dataType::_si08: *((int8_t*)   ptr) = data._si08; break;            
            case dataType::_ui08: *((uint8_t*)  ptr) = data._ui08; break;  
            case dataType::_si16: *((int16_t*)  ptr) = data._si16; break;  
            case dataType::_ui16: *((uint16_t*) ptr) = data._ui16; break;  
            case dataType::_si32: *((int32_t*)  ptr) = data._si32; break;  
            case dataType::_ui32: *((uint32_t*) ptr) = data._ui32; break;  
            case dataType::_flot: *((float*)    ptr) = data._flot; break;  
            case dataType::_doob: *((double*)   ptr) = data._doob; break;
            case dataType::_pick: *((uint8_t*)  ptr) = data._pick.selected; break;
            case dataType::_text: strcpy( (char*) ptr, data._text );
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

        void addBoolean( const char *caption, bool &data,
        bool rollover=false, bool readonly=false ) {
            seEntry *se = new seEntry();
            se->caption = caption;
            se->ptr = &data;
            se->type = seEntry::dataType::_bool;
            se->data._bool.value = data;
            se->data._bool.rollover = rollover;
            // se->min.text = nullptr;
            // se->max.text = nullptr;
            se->readonly = readonly;
            insert( se );
        }

        void addBoolean( const char *caption, bool &data, const char *trueValue, const char *falseValue,
        bool rollover=false, bool readonly=false ) {
            seEntry *se = new seEntry();
            se->caption = caption;
            se->ptr = &data;
            se->type = seEntry::dataType::_bool;
            se->data._bool.value = data;
            se->data._bool.rollover = rollover;
            se->min.text = falseValue;
            se->max.text = trueValue;
            se->readonly = readonly;
            insert( se );
        }

        template<size_t optCount>
        void addPick( const char *caption, uint8_t &defaultSelection, const char* (&options)[optCount],
        bool rollover = false, bool readonly=false ) {
            seEntry *se = new seEntry();
            se->caption = caption;
            se->ptr = &defaultSelection;
            se->type = seEntry::dataType::_pick;

            se->data._pick.selected = defaultSelection;
            se->data._pick.optionCount = optCount;
            se->data._pick.rollover = rollover;
            se->max.pickOptions = options;

            // se->data.pickOptions = options;
            // se->max._pick.selected = defaultSelection;
            // // se->max._pick.length = optCount;
            // se->max._pick.optionCount = optCount;
            // se->max._pick.rollover = rollover;

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
            for( int i = 0 ; i < entryNo ; i++ )
                ptr = head.getNext();
            return ptr;
        }

    //
    // SETTINGS
    //
    public:

        bool readOnly = false;

        // save memory by instantiating only what's needed
        // eg. which numeric editor
        // but confusing for users:
        // - seg fault
        // - how to force switching
        // - so never mind
        // StarterPack::alphaEditorSettings   *textEditorSettings;
        // StarterPack::numericEditorSettings *numericEditorSettings;
        // StarterPack::alphaEditorSettings   *numericUpDownEditorSettings;

        StarterPack::alphaEditorSettings   textEditorSettings;
        StarterPack::numericEditorSettings numericEditorSettings;
        StarterPack::alphaEditorSettings   numericUpDownEditorSettings;

        SettingsEditor() {
            namespace ui = StarterPack::UserInterface;
            numericUpDownEditorSettings.characterSets = CharCycle::CharSet::Numbers; 
            numericUpDownEditorSettings.allowChangeCase = false;
        }

    /*
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
*/

    //
    // PROMPT
    //
    public:

        void prompt( uint8_t captionWidth ) {
            namespace ui = StarterPack::UserInterface;

            if ( !ui::hasScreen() ) return;
            LCDInterface *lcd = ui::LCD;

            rowHandler<StarterPack::seEntry> rowHandler( &head, lcd->maxRows, allowCrossover );

            // rowData row;
            // row.compute( &head, lcd->maxRows );

            seEntry *se;

            char buffer[lcd->maxColumns+1];

            if ( captionWidth > lcd->maxColumns )
                captionWidth = lcd->maxColumns;
            uint8_t dataWidth = lcd->maxColumns - captionWidth - 2;
            
            bool updateScreen = true;

            // track actual key pressed, before debounce, repeat, etc.
            uint8_t prevActualKey = -1;

            ui::waitUntilNothingIsPressed();

            while( true ) {

                if ( updateScreen ) {
                    updateScreen = false;

                    lcd->cursorOff();
                    lcd->cursorBlinkOff();

                    se = rowHandler.getItemOnTopRow();
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
                    }

                    if ( rowHandler.hasFocusedEntry() ) {
                        se = rowHandler.getFocusedEntry();
                        if ( se->isSelectable() ) {
                            int r = rowHandler.focusedOnScreenRow();
                            lcd->printAt( captionWidth, r, '[' );
                            lcd->printAt( lcd->maxColumns-1, r, ']' );
                        }
                    }
                    lcd->refresh();
                    // lcd->displayAll();
                }

                uint8_t key = ui::getRepeatingKey();
                
                // check actual key pressed
                // cannot use continuous as it changes: left, none, left, none, ...
                // if none is handled, other situations won't work: left, release, left, release, ... timer won't reset
                uint8_t actualKey = ui::getContinuousKey();
                if ( actualKey != prevActualKey ) {
                    // previously left or right key, but now something else
                    if ( prevActualKey==ui::kLEFT || prevActualKey==ui::kRIGHT )
                        sliderSpeedReset();
                    prevActualKey = actualKey;
                }

                if ( key == ui::kESCAPE ) {
                    return;
                } else if ( key == ui::kUP ) {

                    // row.scrollUpDown( true, allowCrossover );
                    // updateScreen = true;

                    if ( rowHandler.scrollUpDown( true ) )
                        updateScreen = true;

                } else if ( key == ui::kDOWN ) {

                    // row.scrollUpDown( false, allowCrossover );
                    // updateScreen = true;

                    if ( rowHandler.scrollUpDown( false ) )
                        updateScreen = true;

                } else if ( key == ui::kLEFT ) {

                    se = actionableEntry( rowHandler );
                    if ( se == nullptr ) continue;

                    // sliderSpeedStart();
                    auto delta = sliderValue(se);
                    if (delta <= 1) {
                        if ( se->moveLeft() )
                            updateScreen = true;
                    } else {
                        if ( se->setIntegerDelta(-delta) )
                            updateScreen = true;
                    }

                    // se = getEntry( row.current );

                } else if ( key == ui::kRIGHT ) {

                    se = actionableEntry( rowHandler );
                    if ( se == nullptr ) continue;
                    // se = getEntry( row.current );

                    // sliderSpeedStart();
                    auto delta = sliderValue(se);
                    if (delta <= 1) {
                        if ( se->moveRight() )
                            updateScreen = true;
                    } else {
                        if ( se->setIntegerDelta(delta) )
                            updateScreen = true;
                    }

                } else if ( key == ui::kENTER ) {

                    // viewing only, exit
                    if ( readOnly ) return;

                    se = actionableEntry( rowHandler );
                    if ( se == nullptr ) continue;

                    textEditorSettings         .setWindow( captionWidth+1, rowHandler.focusedOnScreenRow(), dataWidth );
                    numericEditorSettings      .setWindow( captionWidth+1, rowHandler.focusedOnScreenRow(), dataWidth );
                    numericUpDownEditorSettings.setWindow( captionWidth+1, rowHandler.focusedOnScreenRow(), dataWidth );

                    se->enterEditMode( textEditorSettings, numericEditorSettings, numericUpDownEditorSettings );
                    updateScreen = true;
                }
            }
        }

        void acceptChanges() {
            seEntry *se = head.getFirst();
            while( se != nullptr ) {
                if ( !se->readonly )
                    se->acceptChange();
                se = head.getNext();
            }
        }

    private:

        seEntry *actionableEntry( rowHandler<StarterPack::seEntry> &rowHandler ) {
            if ( readOnly ) return nullptr;
            if ( !rowHandler.hasFocusedEntry() ) return nullptr;
            auto se = rowHandler.getFocusedEntry();
            if ( !se->isSelectable() || se->readonly ) return nullptr;
            return se;
        }

    //
    // SLIDER SPEED
    //
    public:
        
        uint16_t sliderSpeedUpDelayInMs = 1500; // delay before speed up
        uint16_t sliderSpeedUpRateInMs = 800;   // time before next step

        uint8_t sliderSpeedUpInitialDivisor = 80; // initial divisor
        uint8_t sliderSpeedUpFinalDivisor = 4;    // final divisor

        uint8_t sliderSpeedDivisorSteps = 8; // number of steps from initial to final divisor

        // use divisor from initialDiv to finalDiv linearly
        //    ticks = timeElapsed / speedUpRate
        //    divisorAdj = ( initial - final ) / steps
        //    divisor = initial - divisorAdj * ticks

    protected:
        bool sliderActivated = false;
        unsigned long sliderStartTime;

        void sliderSpeedReset() {
            sliderActivated = false;
            // Serial.println("STOP");
        }

        int32_t sliderValue(seEntry *se) {
            if (!se->canSpeedSlide()) return 0;

            if (!sliderActivated) {
                // start
                sliderStartTime = millis();
                sliderActivated = true;
                // Serial.println("SLIDE");
                return 1;
            }

            auto elapsed = millis() - sliderStartTime;

            if (elapsed < sliderSpeedUpDelayInMs)
                return 1;

            auto range = se->getIntegerRange();

            float ticks = ((float) (elapsed - sliderSpeedUpDelayInMs)) / sliderSpeedUpRateInMs;

            float divisor;
            if (ticks > sliderSpeedDivisorSteps)
                divisor = sliderSpeedUpFinalDivisor;
            else {
                divisor = sliderSpeedUpInitialDivisor 
                    - (sliderSpeedUpInitialDivisor-sliderSpeedUpFinalDivisor) * ticks / sliderSpeedDivisorSteps;
                if ( divisor < 1 ) divisor = 1;
                // Serial.print(ticks);
                // Serial.print(" / ");
                // Serial.print(divisor);
                // Serial.print(" / ");
            }
            // Serial.print(range / divisor);
            // Serial.println();
            return range / divisor;

            // ticks / divisor / delta
            // 0.19 / 78.22 / 3.26
            // 0.50 / 75.25 / 3.39
            // 0.81 / 72.28 / 3.53
            // 1.12 / 69.31 / 3.68
            // 1.44 / 66.34 / 3.84
            // 1.75 / 63.38 / 4.02
            // 2.06 / 60.41 / 4.22
            // 2.38 / 57.44 / 4.44
            // 2.69 / 54.47 / 4.68
            // 3.00 / 51.50 / 4.95
            // 3.31 / 48.53 / 5.25
            // 3.63 / 45.56 / 5.60
            // 3.94 / 42.59 / 5.99
            // 4.25 / 39.63 / 6.44
            // 4.56 / 36.66 / 6.96
            // 4.88 / 33.69 / 7.57
            // 5.19 / 30.72 / 8.30
            // 5.50 / 27.75 / 9.19
            // 5.81 / 24.78 / 10.29
            // 6.13 / 21.81 / 11.69
            // 6.44 / 18.84 / 13.53
            // 6.75 / 15.88 / 16.06
            // 7.06 / 12.91 / 19.76
            // 7.38 / 9.94 / 25.66
            // 7.69 / 6.97 / 36.59
            // 8.00 / 4.00 / 63.75
            // 63.75
            // ...
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
        se.addBoolean( "bool 1", t );
        se.addBoolean( "bool 2", f );
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
        se.addBoolean( "bool 1", t );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "Text" );
        se.addBoolean( "bool 1", t );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "Text" );
        se.text( "*** LAST ***" );

        se.prompt( 10 );
    }

}

#pragma once

// #include <stdint.h>
// #include <limits.h>
// #include <math.h>
// // #include <string.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <float.h>
#include <string.h>

#include <Utility/spStr.h>
// #include <Utility/spVector.h>
#include <Utility/spNum.h>

// #include <LCDSupport/LCDEditorAlpha.h>
// #include <LCDSupport/LCDEditorNumeric.h>
// #include <LCD/LCDInterface.h>

// #include <LCDSupport/editorRowHandler.h>

namespace StarterPack {

    struct SettingsEditorEntry {

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
            void       * pickData; // max = displayText, min = returnValue
            // const char **pickOptions; // tuck here instead of in min/max
            uint8_t    length;        // text buffer length
            const char *text;
        };

        union deltaUnion {
            int8_t     _si08;
            uint8_t    _ui08;
            int16_t    _si16;
            uint16_t   _ui16;
            int32_t    _si32;
            uint32_t   _ui32;
            float      _flot;
            double     _doob;
        };

        const char *caption;
        void       *ptr;
        dataType    type;
        dataUnion   data;
        rangeUnion  min;
        rangeUnion  max;
        // deltaUnion  increment;
        // deltaUnion  decrement;
        bool        readonly = false;
        // seEntry *next = nullptr;

        inline void init( dataType type ) {
            this->type = type;
        }

    //
    // MIN / MAX
    //

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

        void setIntegerMin(int64_t value) {
            switch( type ) {
            case dataType::_si08: min._si08 = value; break;
            case dataType::_ui08: min._ui08 = value; break;
            case dataType::_si16: min._si16 = value; break;
            case dataType::_ui16: min._ui16 = value; break;
            case dataType::_si32: min._si32 = value; break;
            case dataType::_ui32: min._ui32 = value; break;
            default: break; // to disable warning
            }
        }

        void setIntegerMax(int64_t value) {
            switch( type ) {
            case dataType::_si08: max._si08 = value; break;
            case dataType::_ui08: max._ui08 = value; break;
            case dataType::_si16: max._si16 = value; break;
            case dataType::_ui16: max._ui16 = value; break;
            case dataType::_si32: max._si32 = value; break;
            case dataType::_ui32: max._ui32 = value; break;
            default: break; // to disable warning
            }
        }

    //
    // INCREMENT / DECREMENT
    //

        // int64_t getIntegerIncrement() {
        //     switch( type ) {
        //     case dataType::_si08: return increment._si08;
        //     case dataType::_ui08: return increment._ui08;
        //     case dataType::_si16: return increment._si16;
        //     case dataType::_ui16: return increment._ui16;
        //     case dataType::_si32: return increment._si32;
        //     case dataType::_ui32: return increment._ui32;
        //     default: break; // to disable warning
        //     }
        //     return 0;
        // }

        // int64_t getIntegerDecrement() {
        //     switch( type ) {
        //     case dataType::_si08: return decrement._si08;
        //     case dataType::_ui08: return decrement._ui08;
        //     case dataType::_si16: return decrement._si16;
        //     case dataType::_ui16: return decrement._ui16;
        //     case dataType::_si32: return decrement._si32;
        //     case dataType::_ui32: return decrement._ui32;
        //     default: break; // to disable warning
        //     }
        //     return 0;
        // }

        // void setIntegerIncrement(int64_t value) {
        //     switch( type ) {
        //     case dataType::_si08: increment._si08 = value; break;
        //     case dataType::_ui08: increment._ui08 = value; break;
        //     case dataType::_si16: increment._si16 = value; break;
        //     case dataType::_ui16: increment._ui16 = value; break;
        //     case dataType::_si32: increment._si32 = value; break;
        //     case dataType::_ui32: increment._ui32 = value; break;
        //     default: break; // to disable warning
        //     }
        // }

        // void setIntegerDecrement(int64_t value) {
        //     switch( type ) {
        //     case dataType::_si08: decrement._si08 = value; break;
        //     case dataType::_ui08: decrement._ui08 = value; break;
        //     case dataType::_si16: decrement._si16 = value; break;
        //     case dataType::_ui16: decrement._ui16 = value; break;
        //     case dataType::_si32: decrement._si32 = value; break;
        //     case dataType::_ui32: decrement._ui32 = value; break;
        //     default: break; // to disable warning
        //     }
        // }

    //
    // ACCESS MAPPING
    //

        #define BoolValue         data._bool.value
        #define BoolRollover      data._bool.rollover

        #define PickSelected      data._pick.selected
        #define PickOptionsCount  data._pick.optionCount
        #define PickRollover      data._pick.rollover
        // #define PickOptions       max.pickOptions
        #define PickDisplays      ((const char **) max.pickData)
        #define PickValues        ((const uint8_t *) min.pickData)

    //
    // PICK
    //

        inline void setPickDiplays( const char ** options ) {
            // PickDisplays = (void*) options;
            max.pickData = (void*) options;
        }

        inline void setPickValues( const uint8_t * options ) {
            // PickValues = (void*) options;
            min.pickData = (void*) options;
        }

        inline const char ** getPickDisplay() {
            return PickDisplays;
        }

        inline const uint8_t * getPickValues() {
            return PickValues;
        }

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

        void setIntegerData( int64_t value ) {
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

        int64_t adjustIntegerRange( int64_t value ) {
            auto min = getIntegerMin();
            auto max = getIntegerMax();
            if ( value < min ) value = min;
            if ( value > max ) value = max;
            return value;
        }

        bool setIntegerDataWithChecking( int64_t value ) {
            // return: true  - within range
            //         false - value set was adjusted
            auto adjusted = adjustIntegerRange( value );
            setIntegerData( adjusted );
            return ( adjusted == value );
        }

        bool setIntegerDelta( int64_t delta ) {
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
            auto value = adjustIntegerRange( orig + delta );
            if ( value != orig ) {
                // Serial.println("SET");
                setIntegerData( value );
                return true;
            }
            // Serial.println("NO CHANGE");
            return false;
        }

    //
    // FLOAT / DOUBLE
    //

        float adjustFloatRange( float value ) {
            if ( value < min._flot ) value = min._flot;
            if ( value > max._flot ) value = max._flot;
            return value;
        }

        double adjustDoubleRange( double value ) {
            if ( value < min._doob ) value = min._doob;
            if ( value > max._doob ) value = max._doob;
            return value;
        }

    //
    // ACTIONS
    //

        bool toggle() {
            if ( readonly ) return false;
            switch( type ) {
            case dataType::_bool:
                BoolValue = !BoolValue;
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
            case dataType::_flot: {
                auto v = adjustFloatRange( data._flot-1 );
                if ( data._flot == v ) return false;
                data._flot = v;
                return true;
                }
            case dataType::_doob: {
                auto v = adjustDoubleRange( data._doob-1 );
                if ( data._doob == v ) return false;
                data._doob = v;
                return true;
                }
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
            case dataType::_flot: {
                auto v = adjustFloatRange( data._flot+1 );
                if ( data._flot == v ) return false;
                data._flot = v;
                return true;
                }
            case dataType::_doob: {
                auto v = adjustDoubleRange( data._doob+1 );
                if ( data._doob == v ) return false;
                data._doob = v;
                return true;
                }
            default: break;
            }
            return false;
        }

    //
    // EDIT
    //

/*
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
*/

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
                    strncpy( buffer, PickDisplays[PickSelected-1], len );
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

    //
    //
    //

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
            case dataType::_flot: return min._flot < 0;
            case dataType::_doob: return min._doob < 0;
            // case dataType::_flot:
            // case dataType::_doob: return true;
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
            if ( ptr == nullptr ) return;
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
            case dataType::_pick:
            
                if ( PickSelected < 1 || PickSelected > PickOptionsCount ) {
                    // invalid selection
                } else if ( PickValues == nullptr )
                    *((uint8_t*) ptr) = data._pick.selected;
                else
                    *((uint8_t*) ptr) = PickValues[PickSelected-1];
                break;

            case dataType::_text: strcpy( (char*) ptr, data._text );
            default:
                break;
            }
        }

        void revertChanges() {
            switch( type ) {
            // case _void:
            case dataType::_bool: data._bool.value    = *((bool*)     ptr); break;
            case dataType::_si08: data._si08          = *((int8_t*)   ptr); break;            
            case dataType::_ui08: data._ui08          = *((uint8_t*)  ptr); break;  
            case dataType::_si16: data._si16          = *((int16_t*)  ptr); break;  
            case dataType::_ui16: data._ui16          = *((uint16_t*) ptr); break;  
            case dataType::_si32: data._si32          = *((int32_t*)  ptr); break;  
            case dataType::_ui32: data._ui32          = *((uint32_t*) ptr); break;  
            case dataType::_flot: data._flot          = *((float*)    ptr); break;  
            case dataType::_doob: data._doob          = *((double*)   ptr); break;
            case dataType::_pick:

                if ( PickValues == nullptr ) {
                    // no value lookup, use index
                    data._pick.selected = *((uint8_t*) ptr);
                } else {
                    // match current data to value options
                    for( int i=0 ; i<PickOptionsCount ; i++ ) {
                        if ( *((uint8_t*) ptr) == PickValues[i] ) {
                            PickSelected = i + 1;
                            break;
                        }
                    }
                }
                break;

            case dataType::_text: strcpy( data._text, (char*) ptr );
            default:
                break;
            }
        }

    };

}

#pragma once
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include <Utility/spStr.h>
#include <Utility/spVector.h>
// #include <Utility/cSupport.h>
#include <Utility/spNum.h>

#include <UserInterface.h>
#include <LCDSupport/LCDEditorAlpha.h>
#include <LCDSupport/LCDEditorNumeric.h>

namespace StarterPack {

    class PropertyEditor;

    // class entryCore;
    // template<class T>
    // extern void delete_NextLinkedList( T *head );

    namespace PropertyEditorEntry {
        
        enum incrementLevel {
            small,
            medium,
            large
        };

        class entryCore {

            // make "next" public for now
            // need to give access to: void StarterPack::delete_NextLinkedList(T*);
            // but does not work:
            //    template<class T>
            //    friend void delete_NextLinkedList( T *head );
            //    friend void StarterPack::delete_NextLinkedList( entryCore* );
            public:

                // entryCore  *next      = nullptr;

                // let user handle freeing memory
                // bool       skipDelete = false;
            
            protected:

                void       *ptr     = nullptr; // pointer to actual data

            public:

                const char *caption = nullptr;
                bool       readonly = false;

                enum editResult {
                    accepted, cancelled, invalid
                };

                virtual ~entryCore() {}
                virtual bool isSingleLiner() { return false; }
                virtual bool isSelectable()  { return true;  }
                virtual bool moveLeft()      { return false; }
                virtual bool moveRight()     { return false; }
                virtual bool parseUserEntry( char *buffer ) { return true; }

                virtual uint8_t editBufferSize() { return 0; }
                virtual editResult enterEditMode( char *editBuffer, StarterPack::windowPosition &wPos ) { return accepted; }
                virtual void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {}
                virtual void toEditString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {}
                virtual void acceptChange() {}

                virtual bool decrement( incrementLevel level ) { return moveLeft(); }
                virtual bool increment( incrementLevel level ) { return moveRight(); }

            public:

                enum class specialCase {
                    cNone,
                    cAccept,
                    cCancel
                };
                specialCase sCase = specialCase::cNone;

        };

        template <typename T>
        class entryWithData : public entryCore { public:
            T data;
            entryWithData( const char *caption, T &data ) {
                this->caption = caption;
                ptr = &data;
                data = data;
            }
        };

        //
        // NON-FOCUSABLE
        //

        class breaker : public entryCore { public:
            char ch;
            uint8_t length;
            breaker( char ch, uint8_t length ) { this->ch=ch; this->length=length; readonly=true; }

            bool isSingleLiner() override { return true;  }
            bool isSelectable()  override { return false; }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                if (bufferSize > length+1)
                    bufferSize = length+1;
                // bufferSize = std::min( bufferSize, (uint8_t) (length+1) );
                memset( buffer, ch, bufferSize-1 );
                buffer[bufferSize-1] = 0;
            }
        };

        class message : public entryCore { public:
            message( const char *str ) { caption=str; readonly=true; }
            bool isSingleLiner() override { return true;  }
            bool isSelectable()  override { return false; }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                strncpy( buffer, caption, bufferSize );
                buffer[bufferSize-1] = 0;
            }
        };

        //
        // SPECIAL
        //
        class acceptButton : public entryCore { public:
            // acceptButton() { caption="ACCEPT"; readonly=true; }
            // acceptButton() : acceptButton( "ACCEPT" ) {}
            acceptButton( const char *str ) { caption=str; readonly=false; sCase=specialCase::cAccept; }
            // bool isSingleLiner() { return true; }
            bool isSelectable() override { return true; }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                strncpy( buffer, caption, bufferSize );
                buffer[bufferSize-1] = 0;
            }
        };

        class cancelButton : public entryCore { public:
            // cancelButton() { caption="CANCEL"; readonly=true; }
            // cancelButton() : cancelButton( "CANCEL" ) {}
            cancelButton( const char *str ) { caption=str; readonly=false; sCase=specialCase::cCancel; }
            // bool isSingleLiner() { return true; }
            bool isSelectable() override { return true; }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                strncpy( buffer, caption, bufferSize );
                buffer[bufferSize-1] = 0;
            }
        };

        //
        // BOOL
        //

        class _bool : public entryCore { public:
            bool data;
            const char *trueValue = nullptr;
            const char *falseValue = nullptr;
            _bool( const char *caption, bool &data, bool readonly=false ) {
                this->caption=caption; this->ptr=&data; this->data=data; this->readonly=readonly; }
            _bool( const char *caption, bool &data, const char *trueValue, const char *falseValue, bool readonly=false ) {
                this->caption=caption; this->ptr=&data; this->data=data; this->trueValue=trueValue; this->falseValue=falseValue; this->readonly=readonly; }
            bool moveLeft()  override { if (  data ) { data = false; return true; } else return false; }
            bool moveRight() override { if ( !data ) { data = true;  return true; } else return false; }
            editResult enterEditMode( char *editBuffer, StarterPack::windowPosition &wPos ) override {
                if ( data ) data=false; else data=true; return accepted; }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                if ( trueValue==nullptr && falseValue==nullptr )
                    strncpy( buffer, data ? "TRUE" : "FALSE", bufferSize );
                else if ( data && trueValue != nullptr )
                    strncpy( buffer, trueValue, bufferSize );
                else if ( !data && falseValue != nullptr )
                    strncpy( buffer, falseValue, bufferSize );
                else
                    buffer[0]=0;
                buffer[bufferSize-1] = 0;
            }
            bool parseUserEntry( char *buffer ) override { return true; }
            void acceptChange() override { *((bool*)ptr) = data; }
        };

        //
        // INTEGERS
        //
        // RAM:   [========= ]  86.3% (used 1768 bytes from 2048 bytes)
        // Flash: [==========]  102.4% (used 31464 bytes from 30720 bytes)
        class entryRangedInt : public entryCore { public:
            // typedef int64_t DATA_TYPE;
            // RAM:   [======    ]  57.7% (used 1182 bytes from 2048 bytes)
            // Flash: [=======   ]  70.9% (used 21786 bytes from 30720 bytes)
            // typedef uint64_t DATA_TYPE;
            // RAM:   [======    ]  57.6% (used 1180 bytes from 2048 bytes)
            // Flash: [=======   ]  69.5% (used 21350 bytes from 30720 bytes)
            // typedef uint32_t DATA_TYPE;
            // RAM:   [======    ]  57.3% (used 1174 bytes from 2048 bytes)
            // Flash: [======    ]  63.9% (used 19618 bytes from 30720 bytes)
            // typedef int32_t DATA_TYPE;
            // RAM:   [======    ]  57.3% (used 1174 bytes from 2048 bytes)
            // Flash: [======    ]  63.9% (used 19638 bytes from 30720 bytes)
            typedef int64_t DATA_TYPE;
            DATA_TYPE data;
            DATA_TYPE min;
            DATA_TYPE max;
            uint8_t bufferSize;
            entryRangedInt( const char *caption, void *dataPtr, DATA_TYPE dataValue, DATA_TYPE min, DATA_TYPE max, 
            uint8_t bufferSize,
            bool readonly ) {
                this->caption = caption; ptr = dataPtr; this->readonly = readonly;
                this->data = dataValue; this->min = min; this->max = max;
                this->bufferSize = bufferSize;
            }
            bool moveLeft()  override { if ( data>min ) { data--; return true; } else return false; }
            bool moveRight() override { if ( data<max ) { data++; return true; } else return false; }
            uint8_t editBufferSize() override { return bufferSize; }
            editResult enterEditMode( char *editBuffer, StarterPack::windowPosition &wPos ) override {
                namespace ui = StarterPack::UserInterface;
                StarterPack::numericEditorSettings nSet;
                nSet.setPosition(wPos.col, wPos.row, wPos.windowSize);
                nSet.allowDecimal = false;
                nSet.allowNegative = min<0;
                nSet.bufferLength = bufferSize;
                StarterPack::numericEditor editor( editBuffer, nSet );
                while( true ) {
                    uint8_t r = editor.prompt();
                    if ( r == ui::kESCAPE )
                        return cancelled;
                    if ( r == ui::kENTER ) {
                        if ( parseUserEntry( editBuffer ) )
                            return accepted;
                        else
                            return invalid;
                    }
                }
            }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                Num::mySnprintf( buffer, bufferSize, data );
                // snprintf( buffer, bufferSize, "%d", data );
            }
            void toEditString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                if ( data == 0 )
                    buffer[0] = 0;
                else
                    Num::mySnprintf( buffer, bufferSize, data );
                    // Num::mySnprintf( buffer, bufferSize, "%d", data );
            }
            bool parseUserEntry( char *buffer ) override {
                DATA_TYPE I;
                Num::StrToNum( buffer, I );
                if ( I < min || I > max ) return false;
                data = I;
                return true;
            }
        };

        #define CLS(dType,D,LEN,NEG) \
            class D : public entryRangedInt { public: \
                D( const char *caption, dType &data, dType min, dType max, bool readonly ) \
                    : entryRangedInt(caption,&data,data,min,max,LEN,readonly) {} \
                void acceptChange() override { *((dType*) ptr) = data; } \
            };
        CLS( int8_t,   _si08, 4+1 , true  )  //        -127
        CLS( uint8_t,  _ui08, 3+1 , false )  //         255
        CLS( int16_t,  _si16, 6+1 , true  )  //      -32768
        CLS( uint16_t, _ui16, 5+1 , false )  //       65535
        CLS( int32_t,  _si32, 11+1, true  )  // -2147483647
        CLS( uint32_t, _ui32, 10+1, false )  //  4294967295
        #undef CLS

        // ====== ]  86.4% (used 1770 bytes from 2048 bytes)
        // Flash: [==========]  107.9% (used 33148 bytes from 30720 bytes)
        //
        // INTEGERS
        //
        // template <typename T, size_t BUFFERSIZE, bool NEG>
        // class entryRangedInt : public entryCore { public:
        //     T data;
        //     T min;
        //     T max;
        //     entryRangedInt( const char *caption, T &data, T min, T max, bool readonly ) {
        //         this->caption = caption; ptr = &data; this->readonly = readonly;
        //         this->data = data; this->min = min; this->max = max;
        //     }
        //     bool moveLeft()  override { if ( data>min ) { data--; return true; } else return false; }
        //     bool moveRight() override { if ( data<max ) { data++; return true; } else return false; }
        //     uint8_t editBufferSize() override { return BUFFERSIZE; }
        //     editResult enterEditMode( char *editBuffer, StarterPack::windowPosition &wPos ) override {
        //         namespace ui = StarterPack::UserInterface;
        //         StarterPack::numericEditorSettings nSet;
        //         nSet.setPosition(wPos.col, wPos.row, wPos.windowSize);
        //         // nSet.col = wPos.col; nSet.row = wPos.row; nSet.windowSize = wPos.windowSize;
        //         nSet.allowDecimal = false;
        //         nSet.allowNegative = NEG && min<0;
        //         nSet.bufferLength = BUFFERSIZE;
        //         StarterPack::numericEditor editor( editBuffer, nSet );
        //         while( true ) {
        //             uint8_t r = editor.prompt();
        //             if ( r == ui::kESCAPE )
        //                 return cancelled;
        //             if ( r == ui::kENTER ) {
        //                 if ( parseUserEntry( editBuffer ) )
        //                     return accepted;
        //                 else
        //                     return invalid;
        //             }
        //         }
        //     }
        //     void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
        //         Num::mySnprintf( buffer, bufferSize, data );
        //         // snprintf( buffer, bufferSize, "%d", data );
        //     }
        //     void toEditString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
        //         if ( data == 0 )
        //             buffer[0] = 0;
        //         else
        //             Num::mySnprintf( buffer, bufferSize, data );
        //             // Num::mySnprintf( buffer, bufferSize, "%d", data );
        //     }
        //     bool parseUserEntry( char *buffer ) override {
        //         // int64_t I = Num::myAtoll( buffer );
        //         int64_t I;
        //         Num::StrToNum( buffer, I );
        //         if ( I < min || I > max ) return false;
        //         data = I;
        //         return true;
        //     }
        // };

        // #define CLS(dType,D,LEN,NEG) \
        //     class D : public entryRangedInt<dType,LEN,NEG> { public: \
        //         D( const char *caption, dType &data, dType min, dType max, bool readonly ) \
        //             : entryRangedInt(caption,data,min,max,readonly) {} \
        //         void acceptChange() override { *((dType*) ptr) = data; } \
        //     };
        // CLS( int8_t,   _si08, 4+1 , true  )  //        -127
        // CLS( uint8_t,  _ui08, 3+1 , false )  //         255
        // CLS( int16_t,  _si16, 6+1 , true  )  //      -32768
        // CLS( uint16_t, _ui16, 5+1 , false )  //       65535
        // CLS( int32_t,  _si32, 11+1, true  )  // -2147483647
        // CLS( uint32_t, _ui32, 10+1, false )  //  4294967295
        // #undef CLS


        // RAM:   [========= ]  86.3% (used 1768 bytes from 2048 bytes)
        // Flash: [==========]  102.4% (used 31448 bytes from 30720 bytes)
        //
        // FLOATING POINT
        //

        // template <typename T, size_t BUFFERSIZE, bool NEG>
        class entryRangedDouble : public entryCore { public:
            double data;
            double min;
            double max;
            uint8_t bufferSize;
            entryRangedDouble( const char *caption, void *data, double dataValue, double min, double max, 
            uint8_t bufferSize,
            bool readonly ) {
                this->caption = caption; ptr = data; this->readonly = readonly;
                this->data = dataValue; this->min = min; this->max = max;
                this->bufferSize = bufferSize;
            }
            uint8_t editBufferSize() override { return bufferSize; }
            editResult enterEditMode( char *editBuffer, StarterPack::windowPosition &wPos ) override {
                namespace ui = StarterPack::UserInterface;
                StarterPack::numericEditorSettings nSet;
                nSet.setPosition(wPos.col, wPos.row, wPos.windowSize);
                // eSet.col = wPos.col; eSet.row = wPos.row; eSet.windowSize = wPos.windowSize;
                nSet.allowDecimal = true;
                nSet.allowNegative = min<0;
                nSet.bufferLength = bufferSize;
                StarterPack::numericEditor editor( editBuffer, nSet );
                while( true ) {
                    uint8_t r = editor.prompt();
                    if ( r == ui::kESCAPE )
                        return cancelled;
                    if ( r == ui::kENTER ) {
                        if ( parseUserEntry( editBuffer ) )
                            return accepted;
                        else
                            return invalid;
                    }
                }
            }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                Num::mySnprintf( buffer, bufferSize, data );
                // snprintf( buffer, bufferSize, "%f", data );
                if ( Str::findCharacter( '.', buffer ) ) {
                // if ( isCharInString( '.', buffer ) ) {
                    // remove trailing '0' and '.'
                    char *p = buffer + strlen(buffer) - 1;
                    while( *p == '0' ) *p-- = 0;
                    if ( *p == '.' ) *p = 0;
                }                
            }
            void toEditString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                if ( data == 0 )
                    buffer[0] = 0;
                else
                    toString( buffer, bufferSize, wPos );
            }            
            bool parseUserEntry( char *buffer ) override {
                double F = atof( buffer );
                if ( F < min || F > max ) return false;
                data = F;
                return true;
            }
        };

        #define CLS(dType,D,LEN,NEG) \
            class D : public entryRangedDouble { public: \
                D( const char *caption, dType &data, dType min, dType max, bool readonly) \
                    : entryRangedDouble(caption,&data,data,min,max,LEN,readonly) {} \
                void acceptChange() override { *((dType*) ptr) = data; } \
            };
        CLS( float,  _flot, 20, true )
        CLS( double, _doob, 20, true )
        #undef CLS

        //
        // PICK
        //

        class pick : public entryCore { public:
            uint8_t      selected; // 1-based
            const char **options; 
            uint8_t      optionCount;

            pick( const char *caption, uint8_t &selected, const char **options, uint8_t optionCount, bool readonly=false ) {
                this->caption = caption;
                this->ptr = &selected;
                this->selected = selected;
                this->options = options;
                this->optionCount = optionCount;
                this->readonly = readonly;
            }
            bool moveLeft() override {
                if ( selected == 0 || selected > optionCount ) {
                    selected = 1;
                    return true;
                } else if ( selected > 1 ) {
                    selected--;
                    return true;
                }
                return false;
            }
            bool moveRight() override {
                if ( selected == 0 || selected > optionCount ) {
                    selected = 1;
                    return true;
                } else if ( selected < optionCount ) {
                    selected++;
                    return true;
                }
                return false;
            }
            editResult enterEditMode( char *editBuffer, StarterPack::windowPosition &wPos ) override {
                // with rollover
                selected++;
                if ( selected < 1 || selected > optionCount )
                    selected = 1;
                return accepted;
            }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                if ( selected < 1 || selected > optionCount ) {
                    buffer[0] = 0;
                } else {
                    // 1-based index
                    strncpy( buffer, options[selected-1], bufferSize );
                    buffer[bufferSize-1] = 0; // in case option is too long
                }
            }
            bool parseUserEntry( char *buffer ) override { return true; }
            void acceptChange() override { *((uint8_t*)ptr) = selected; }
        };

        //
        // STRING
        //
        class _string : public entryCore { public:
            char    *buffer;
            uint8_t  length;

            _string( const char *caption, char *str, uint8_t maxLength, bool readonly=false ) {
                this->caption = caption;
                this->ptr = str;
                this->length = maxLength;
                this->readonly = readonly;
                buffer = new char[length];
                strncpy( buffer, str, maxLength );
                buffer[maxLength-1] = 0;
            }
            ~_string() {
                delete buffer;
            }
            uint8_t editBufferSize() override { return length; }
            editResult enterEditMode( char *editBuffer, StarterPack::windowPosition &wPos ) override {
                namespace ui = StarterPack::UserInterface;

                StarterPack::alphaEditorSettings eSet;
                eSet.col = wPos.col; eSet.row = wPos.row; eSet.windowSize = wPos.windowSize;
                eSet.bufferLength = length;

                StarterPack::alphanumericEditor editor( editBuffer, eSet );
                while( true ) {
                    uint8_t r = editor.prompt();
                    if ( r == ui::kESCAPE )
                        return cancelled;
                    if ( r == ui::kENTER ) {
                        if ( parseUserEntry( editBuffer ) )
                            return accepted;
                        else
                            return invalid;
                    }
                }
            }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                strncpy( buffer, this->buffer, bufferSize );
                buffer[bufferSize-1] = 0; // in case text is too long
            }
            void toEditString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                strncpy( buffer, this->buffer, bufferSize );
                buffer[bufferSize-1] = 0;
            }
            bool parseUserEntry( char *buffer ) override {
                strncpy( this->buffer, buffer, length );
                this->buffer[length-1] = 0;
                return true;
            }
            void acceptChange() override {
                strncpy( (char*) ptr, buffer, length );
            }
        };

        // RAM:   [==========]  97.6% (used 1998 bytes from 2048 bytes)
        // Flash: [==========]  108.7% (used 33406 bytes from 30720 bytes)
        //
        // SLIDER
        //

        // template <typename T>
        class sliderInt : public entryCore { public:
            int64_t data;
            int64_t min;
            int64_t max;
            sliderInt( const char *caption, void *dataPtr, int64_t dataValue, int64_t min, int64_t max,
            bool readonly ) {
                this->caption = caption; ptr = dataPtr; this->readonly = readonly;
                this->data = dataValue; this->min = min; this->max = max;
            }
            bool moveLeft()  override { if ( data>min ) { data--; return true; } else return false; }
            bool moveRight() override { if ( data<max ) { data++; return true; } else return false; }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
                Num::mySnprintf( buffer, bufferSize, data );
                // snprintf( buffer, bufferSize, "%d", data );
            }
            int64_t incLow  = 1;
            int64_t incMed  = 10;
            int64_t incHigh = 100;
            bool decrement( incrementLevel level ) override {
                // switch( level ) {
                // case incrementLevel::small:  if ( data>min+incLow  ) { data-=incLow; Serial.println("s"); return true; } break;
                // case incrementLevel::medium: if ( data>min+incMed  ) { data-=incMed; Serial.println("m"); return true; } break;
                // case incrementLevel::large:  if ( data>min+incHigh ) { data-=incHigh; Serial.println("h"); return true; } break;
                // }
                int64_t delta;
                switch( level ) {
                case incrementLevel::small:  delta = incLow;  break;
                case incrementLevel::medium: delta = incMed;  break;
                case incrementLevel::large:  delta = incHigh; break;
                }
                if ( data > min + delta  ) {
                    data -= delta;
                    // Serial.printf( "DEC: %d - %d\n", data, delta );
                    return true;
                }  else if ( data > min ) {
                    data = min;
                    return true;
                }
                return false;
            }
            bool increment( incrementLevel level ) override {
                // switch( level ) {
                // case incrementLevel::small:  if ( data+incLow <max ) { data+=incLow; Serial.println("S"); return true; } break;
                // case incrementLevel::medium: if ( data+incMed <max ) { data+=incMed; Serial.println("M"); return true; } break;
                // case incrementLevel::large:  if ( data+incHigh<max ) { data+=incHigh; Serial.println("H"); return true; } break;
                // }
                int64_t delta;
                switch( level ) {
                case incrementLevel::small:  delta = incLow;  break;
                case incrementLevel::medium: delta = incMed;  break;
                case incrementLevel::large:  delta = incHigh; break;
                }
                if ( data + delta < max ) {
                    data += delta;
                    // Serial.printf( "INC: %d + %d\n", data, delta );
                    return true;
                } else if ( data < max ) {
                    data = max;
                    return true;
                }
                return false;
            }
        };
        #define CLS(dType,D) \
            class D : public sliderInt { public: \
                D( const char *caption, dType &data, dType min, dType max, bool readonly ) \
                    : sliderInt(caption,&data,data,min,max,readonly) {} \
                void acceptChange() override { *((dType*) ptr) = data; } \
            };
        CLS( int8_t,   _si08_slider )
        CLS( uint8_t,  _ui08_slider )
        CLS( int16_t,  _si16_slider )
        CLS( uint16_t, _ui16_slider )
        CLS( int32_t,  _si32_slider )
        CLS( uint32_t, _ui32_slider )
        #undef CLS


        // RAM:   98.0% (used 2008 bytes from 2048 bytes)
        // Flash: [==========]  110.4% (used 33910 bytes from 30720 bytes)
        // //
        // // SLIDER
        // //

        // template <typename T>
        // class sliderInt : public entryCore { public:
        //     T data;
        //     T min;
        //     T max;
        //     sliderInt( const char *caption, T &data, T min, T max, bool readonly ) {
        //         this->caption = caption; ptr = &data; this->readonly = readonly;
        //         this->data = data; this->min = min; this->max = max;
        //     }
        //     bool moveLeft()  override { if ( data>min ) { data--; return true; } else return false; }
        //     bool moveRight() override { if ( data<max ) { data++; return true; } else return false; }
        //     void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) override {
        //         Num::mySnprintf( buffer, bufferSize, data );
        //         // snprintf( buffer, bufferSize, "%d", data );
        //     }
        //     T incLow  = 1;
        //     T incMed  = 10;
        //     T incHigh = 100;
        //     bool decrement( incrementLevel level ) override {
        //         // switch( level ) {
        //         // case incrementLevel::small:  if ( data>min+incLow  ) { data-=incLow; Serial.println("s"); return true; } break;
        //         // case incrementLevel::medium: if ( data>min+incMed  ) { data-=incMed; Serial.println("m"); return true; } break;
        //         // case incrementLevel::large:  if ( data>min+incHigh ) { data-=incHigh; Serial.println("h"); return true; } break;
        //         // }
        //         T delta;
        //         switch( level ) {
        //         case incrementLevel::small:  delta = incLow;  break;
        //         case incrementLevel::medium: delta = incMed;  break;
        //         case incrementLevel::large:  delta = incHigh; break;
        //         }
        //         if ( data > min + delta  ) {
        //             data -= delta;
        //             // Serial.printf( "DEC: %d - %d\n", data, delta );
        //             return true;
        //         }  else if ( data > min ) {
        //             data = min;
        //             return true;
        //         }
        //         return false;
        //     }
        //     bool increment( incrementLevel level ) override {
        //         // switch( level ) {
        //         // case incrementLevel::small:  if ( data+incLow <max ) { data+=incLow; Serial.println("S"); return true; } break;
        //         // case incrementLevel::medium: if ( data+incMed <max ) { data+=incMed; Serial.println("M"); return true; } break;
        //         // case incrementLevel::large:  if ( data+incHigh<max ) { data+=incHigh; Serial.println("H"); return true; } break;
        //         // }
        //         T delta;
        //         switch( level ) {
        //         case incrementLevel::small:  delta = incLow;  break;
        //         case incrementLevel::medium: delta = incMed;  break;
        //         case incrementLevel::large:  delta = incHigh; break;
        //         }
        //         if ( data + delta < max ) {
        //             data += delta;
        //             // Serial.printf( "INC: %d + %d\n", data, delta );
        //             return true;
        //         } else if ( data < max ) {
        //             data = max;
        //             return true;
        //         }
        //         return false;
        //     }
        // };
        // #define CLS(dType,D) \
        //     class D : public sliderInt<dType> { public: \
        //         D( const char *caption, dType &data, dType min, dType max, bool readonly ) \
        //             : sliderInt(caption,data,min,max,readonly) {} \
        //         void acceptChange() override { *((dType*) ptr) = data; } \
        //     };
        // CLS( int8_t,   _si08_slider )
        // CLS( uint8_t,  _ui08_slider )
        // CLS( int16_t,  _si16_slider )
        // CLS( uint16_t, _ui16_slider )
        // CLS( int32_t,  _si32_slider )
        // CLS( uint32_t, _ui32_slider )
        // #undef CLS

        //
        // ROW HANDLER
        //

        class rowHandler {

            protected:
                                            // DYNAMIC
                uint8_t itemOnTop = 0;      // entry# on topmost of screen
                uint8_t focusedEntry = 0;   // focusable item on screen

                                            // FIXED
                uint8_t count = 0;          // number of items
                uint8_t lcdRows;            // number of rows of physical screen

                // keep copy to find latched entry
                // PropertyEditorEntry::entryCore *head;
                spVector<PropertyEditorEntry::entryCore> *head;

            public:

                bool allowCrossover = false;

                rowHandler( spVector<PropertyEditorEntry::entryCore> *se,
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

                PropertyEditorEntry::entryCore *getEntry( uint8_t entryNo ) {
                    if ( entryNo < 0 || entryNo >= count ) return nullptr;
                    auto *ptr = head->getFirst();
                    // auto *ptr = head;
                    for( int i = 0 ; i < entryNo ; i++ ) {
                        ptr = head->getNext();
                        // ptr = ptr->next;
                    }
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
                inline PropertyEditorEntry::entryCore *getFocusedEntry() {
                    return getEntry( focusedEntry );
                }
                inline PropertyEditorEntry::entryCore *getItemOnTopRow() {
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

//
// CORE
//

    typedef StarterPack::PropertyEditorEntry::entryCore entryCore;

    class PropertyEditor {

        public:

            ~PropertyEditor() {
                // in <spMacros.h>
                // StarterPack::delete_NextLinkedListWithSkip( head );
            }

            bool allowCrossover = false;
            char errorCharacter = 0b11101111; // pikachu face

        //
        // ADD
        //
        public:
        
            //
            //
            //

            void add( PropertyEditorEntry::entryCore *property ) {
                // entry will be deleted
                insert( property );
            }

            // void add( PropertyEditorEntry::entryCore *property ) {
            //     // let user handle freeing memory
            //     property->skipDelete = true;
            //     insert( property );
            // }

            // void addAndDelete( PropertyEditorEntry::entryCore *property ) {
            //     insert( property );
            // }

            PropertyEditorEntry::breaker *breaker( char ch = '-' ) {
                namespace ui = StarterPack::UserInterface;
                auto *se = new PropertyEditorEntry::breaker( ch, ui::LCD->maxColumns );
                insert( se );
                return se;
            }

            PropertyEditorEntry::message *message( const char *str ) {
                auto *se = new PropertyEditorEntry::message(str);
                insert( se );
                return se;
            }

            //
            // SPECIAL
            //
            // void acceptButton() {
            //     namespace ui = StarterPack::UserInterface;
            //     auto *se = new StarterPack::PropertyEditorEntry::acceptButton();
            //     insert( se );
            // }
            PropertyEditorEntry::acceptButton *acceptButton( const char *str = "\x7F ACCEPT" ) {
                namespace ui = StarterPack::UserInterface;
                auto *se = new PropertyEditorEntry::acceptButton( str );
                insert( se );
                return se;
            }
            // void cancelButton() {
            //     namespace ui = StarterPack::UserInterface;
            //     auto *se = new PropertyEditorEntry::cancelButton();
            //     insert( se );
            // }
            PropertyEditorEntry::cancelButton *cancelButton( const char *str = "\x7F CANCEL" ) {
                namespace ui = StarterPack::UserInterface;
                auto *se = new PropertyEditorEntry::cancelButton( str );
                insert( se );
                return se;
            }
            PropertyEditorEntry::acceptButton *exitButton( const char *str = "\x7F EXIT" ) {
                namespace ui = StarterPack::UserInterface;
                auto *se = new PropertyEditorEntry::acceptButton( str );
                insert( se );
                return se;
            }

            //
            // BOOL
            //

            PropertyEditorEntry::_bool *add( const char *caption, bool &data, bool readonly=false ) {
                auto *se = new PropertyEditorEntry::_bool(caption,data,readonly);
                insert( se );
                return se;
            }

            PropertyEditorEntry::_bool *add( const char *caption, bool &data, const char *trueValue, const char *falseValue, bool readonly=false ) {
                auto *se = new PropertyEditorEntry::_bool(caption,data,trueValue,falseValue,readonly);
                insert( se );
                return se;
            }

            //
            // INTEGER/FLOAT
            //
            #define ADD(dType,D,MIN,MAX) \
                PropertyEditorEntry::D *add( const char *caption, dType &data, bool readonly=false ) { \
                    auto *se = new PropertyEditorEntry::D(caption,data,MIN,MAX,readonly); \
                    insert( se ); \
                    return se; \
                } \
                PropertyEditorEntry::D *add( const char *caption, dType &data, dType min, dType max, bool readonly=false ) { \
                    auto *se = new PropertyEditorEntry::D(caption,data,min,max,readonly); \
                    insert( se ); \
                    return se; \
                }
            ADD( int8_t,   _si08,  INT8_MIN, INT8_MAX   )
            ADD( uint8_t,  _ui08,         0, UINT8_MAX  )
            ADD( int16_t,  _si16, INT16_MIN, INT16_MAX  )
            ADD( uint16_t, _ui16,         0, UINT16_MAX )
            ADD( int32_t,  _si32, INT32_MIN, INT32_MAX  )
            ADD( uint32_t, _ui32,         0, UINT32_MAX )
            ADD( float,    _flot, -__FLT_MAX__, __FLT_MAX__ );
            ADD( double,   _doob, -__DBL_MAX__, __DBL_MAX__ );
            #undef ADD

            //
            // PICK
            //
            template<size_t optCount>
            PropertyEditorEntry::pick *addPicker( const char *caption, uint8_t &selected, const char* (&options)[optCount], bool readonly=false ) {
                auto *se = new PropertyEditorEntry::pick(
                    caption,selected,options,optCount,readonly);
                insert( se );
                return se;
            }

            PropertyEditorEntry::_string *addString( const char *caption, char *buffer, uint8_t size, bool readonly=false ) {
                auto *se = new PropertyEditorEntry::_string(caption,buffer,size,readonly);
                insert( se );
                return se;
            }

            //
            // SLIDERS
            //
            #define ADD(dType,D,MIN,MAX) \
                PropertyEditorEntry::D ## _slider *addSlider( const char *caption, dType &data, bool readonly=false ) { \
                    auto *se = new PropertyEditorEntry::D ## _slider(caption,data,MIN,MAX,readonly); \
                    insert( se ); \
                    return se; \
                } \
                PropertyEditorEntry::D ## _slider *addSlider( const char *caption, dType &data, dType min, dType max, bool readonly=false ) { \
                    auto *se = new PropertyEditorEntry::D ## _slider(caption,data,min,max,readonly); \
                    insert( se ); \
                    return se; \
                }
            ADD( int8_t,   _si08,  INT8_MIN, INT8_MAX   )
            ADD( uint8_t,  _ui08,         0, UINT8_MAX  )
            ADD( int16_t,  _si16, INT16_MIN, INT16_MAX  )
            ADD( uint16_t, _ui16,         0, UINT16_MAX )
            ADD( int32_t,  _si32, INT32_MIN, INT32_MAX  )
            ADD( uint32_t, _ui32,         0, UINT32_MAX )
            #undef ADD

        //
        // ENTRIES
        //
        protected:

            // PropertyEditorEntry::entryCore *head = nullptr;
            spVector<PropertyEditorEntry::entryCore> head;

            inline void insert( PropertyEditorEntry::entryCore *se ) {
                head.insert( se );
                // insertEnd_NextLinkedList( &head, se );
            }

    //
    // BACKGROUND PROCESS/BREAKOUT
    //
    #if defined(ESP8266) || defined(ESP32)
        protected:
            // return anything but ui::kNone to breakout of loop
            typedef std::function<uint8_t(void)> backgroundProcessDelegate;
            backgroundProcessDelegate backgroundProcess = nullptr;
        public:
            void assignBackgroundProcess( uint8_t (*backgroundProcessFunc)() ) {
                this->backgroundProcess = [backgroundProcessFunc]() { return backgroundProcessFunc(); };
            }
    #else
        protected:
            // return anything but ui::kNone to breakout of loop
            typedef bool (*backgroundProcessDelegate)();
            backgroundProcessDelegate backgroundProcess = nullptr;
    #endif

    public:
        static uint8_t backgroundProcessNoAction() {
            namespace ui = StarterPack::UserInterface;
            return ui::kNONE;
        }
        void assignBackgroundProcess( backgroundProcessDelegate backgroundProcess ) {
            this->backgroundProcess = backgroundProcess;
        }

    public:

        bool hasChanges = false;

        uint8_t show( uint8_t captionWidth ) {
            keypad = kpadStyle::kStandard;
            readOnly = true;
            return promptCore( captionWidth );
        }

        void prompt( uint8_t captionWidth ) {
            keypad = kpadStyle::kStandard;
            readOnly = false;
            promptCore( captionWidth );
        }

        void promptSliderOnly( uint8_t captionWidth ) {
            keypad = kpadStyle::kSlidersOnly;
            readOnly = false;
            promptCore( captionWidth );
        }

        struct markerStruct {
            char Left = '[';
            char Right = ']';
            char EditingLeft  = 0b10100010;
            char EditingRight = 0b10100011;
        };
        static markerStruct markers;

    //
    // SLIDER SPEED
    //
    public:
        uint16_t longPressMs = 4000;
        uint16_t mediumPressMs = 2000;
    protected:
        unsigned long pressedTime;
        PropertyEditorEntry::incrementLevel calcIncrementLevel() {
            auto pressedDuration = millis() - pressedTime;
// Serial.println( pressedDuration );
            if ( pressedDuration >= longPressMs   ) return PropertyEditorEntry::incrementLevel::large;
            if ( pressedDuration >= mediumPressMs ) return PropertyEditorEntry::incrementLevel::medium;
            return PropertyEditorEntry::incrementLevel::small;
        }

    protected:

        bool readOnly = false;

        enum kpadStyle {

            kStandard,
            // full keypad
            // - up, down, left, right (can be part of 0..9)
            // - enter
            // - escape
            // - backspace
            // - for alphanumeric: (optional) delete, insert, changeCase, number, symbol
            // - for numeric     : 0..9
            //                   : (optional) delete, decimal, negative

            kSlidersOnly
            // 3 keys only
            // - common up/left
            // - common down/right
            // - enter
            // to use: [up]/[down]    select item
            //         [enter]        begin editing mode
            //         [left]/[right] change value
            //         [enter]        end editing mode
            // note: add exitButton() to be able to exit
        };
        kpadStyle keypad = kpadStyle::kStandard;

        uint8_t promptCore( uint8_t captionWidth ) {
            namespace ui = StarterPack::UserInterface;

            if ( !ui::hasScreen() ) return 0;
            LCDInterface *lcd = ui::LCD;
            LCDBuffered *lcdBuffered = nullptr;
            if ( lcd->isBuffered() )
                lcdBuffered = (LCDBuffered*) lcd;

            PropertyEditorEntry::rowHandler rowHandler( &head, lcd->maxRows, allowCrossover );

            char buffer[lcd->maxColumns+1];

            if ( captionWidth > lcd->maxColumns )
                captionWidth = lcd->maxColumns;
            int8_t dataWidth = lcd->maxColumns - captionWidth - 2;
            if ( dataWidth < 0 ) dataWidth = 0;
            
            // display window for whole row
            StarterPack::windowPosition rowWindow;
            rowWindow.col = 0;
            rowWindow.windowSize = lcd->maxColumns;

            // display window for data only, put row later
            StarterPack::windowPosition dataWindow;
            dataWindow.col = captionWidth+1;
            dataWindow.windowSize = dataWidth;

            bool updateScreen = true;

            // ui::kENTER          - enter/leave edit mode
            // ui::kUP / ui::kDOWN - move up down if not edit mode
            // ui::kUP / ui::kDOWN - left/right if edit mode
            bool kpadSliderOnlyEditing = false;

            bool leftPressed = false;
            bool rightPressed = false;
            uint8_t prevActualKey = 1;

            ui::waitUntilNothingIsPressed();

            while( true ) {

                //
                // DISPLAY
                //
                if ( updateScreen ) {
                    updateScreen = false;

                    auto se = rowHandler.getItemOnTopRow();

                    for( int r = 0 ; r < lcd->maxRows ; r++ ) {
                        if ( se == nullptr ) {
                            lcd->clearRow( r );
                            continue;
                        }
                        lcd->setCursor( 0, r );
                        switch( se->sCase ) {
                        case PropertyEditorEntry::entryCore::specialCase::cAccept:
                        case PropertyEditorEntry::entryCore::specialCase::cCancel:
                            lcd->write( ' ' );
                            se->toString( buffer, lcd->maxColumns-2+1, rowWindow );
                            lcd->printStrN( buffer, lcd->maxColumns-2, true );
                            lcd->write( ' ' );
                            break;
                        case PropertyEditorEntry::entryCore::specialCase::cNone:
                            if ( se->isSingleLiner() ) {
                                rowWindow.row = r;
                                se->toString( buffer, lcd->maxColumns+1, rowWindow );
                                lcd->printStrN( buffer, lcd->maxColumns, true );
                            } else {
                                // CAPTION
                                lcd->printStrN( se->caption, captionWidth, true );
                                lcd->print( ' ' );
                                // DATA
                                dataWindow.row = r;
                                se->toString( buffer, lcd->maxColumns, dataWindow );
                                lcd->setCursor( captionWidth+1, r );
                                lcd->printStrN( buffer, dataWidth, true );
                                lcd->print( ' ' );
                                uint8_t len = strlen( buffer );
                                if ( len > dataWidth )
                                    lcd->writeAt( lcd->maxColumns-2, r, 0x7E );
                            }
                            break;
                        }
                        se = head.getNext();
                        // se = se->next;
                    }

                    // se = getEntry( row.current );
                    if ( rowHandler.hasFocusedEntry() ) {
                        se = rowHandler.getFocusedEntry();
                        if ( se->isSelectable() ) {
                            int r = rowHandler.focusedOnScreenRow();
                            switch( se->sCase ) {
                            case PropertyEditorEntry::entryCore::specialCase::cAccept:
                            case PropertyEditorEntry::entryCore::specialCase::cCancel:
                                lcd->printAt( 0, r, markers.Left ); // '[' );
                                lcd->printAt( lcd->maxColumns-1, r, markers.Right ); // ']' );
                                break;
                            case PropertyEditorEntry::entryCore::specialCase::cNone:
                                if ( keypad == kpadStyle::kSlidersOnly && kpadSliderOnlyEditing ) {
                                    lcd->printAt( captionWidth, r, markers.EditingLeft ); // 0b10100010 ); // '*' );
                                    lcd->printAt( lcd->maxColumns-1, r, markers.EditingRight ); // 0b10100011 ); // '*' );
                                } else {
                                    lcd->printAt( captionWidth, r, markers.Left ); // '[' );
                                    lcd->printAt( lcd->maxColumns-1, r, markers.Right ); //']' );
                                }
                                break;
                            }
                        }
                    }
                }
                if ( lcdBuffered != nullptr )
                    lcdBuffered->update();

                //
                // KEY
                //
                uint8_t key = ui::getRepeatingKey();

                // check actual key pressed
                // cannot use continuous as it changes: left, none, left, none, ...
                // if none is handled, other situations won't work: left, release, left, release, ... timer won't reset
                uint8_t actualKey = ui::getContinuousKey();
                if ( actualKey != prevActualKey ) {
                    if ( actualKey != ui::kLEFT ) {
                        // Serial.printf( "LEFT OFF = %d\n", key );
                        leftPressed  = false;
                    }
                    if ( actualKey != ui::kRIGHT ) {
                        // Serial.printf( "RIGHT OFF = %d\n", key );
                        rightPressed = false;
                    }
                    prevActualKey = actualKey;
                }
                
                if ( key == ui::kESCAPE ) {
                    ui::waitUntilNothingIsPressed();
                    return ui::kESCAPE;
                } else if ( key == ui::kUP ) {
                    if ( keypad == kpadStyle::kSlidersOnly && kpadSliderOnlyEditing ) {
                        // LEFT instead of UP
                        goto CASE_LEFT;
                        // updateScreen = doLeft( rowHandler );
                        // continue;
                    }
                    if ( rowHandler.scrollUpDown( true ) )
                        updateScreen = true;
                } else if ( key == ui::kDOWN ) {
                    if ( keypad == kpadStyle::kSlidersOnly && kpadSliderOnlyEditing ) {
                        // RIGHT instead of DOWN
                        goto CASE_RIGHT;
                        // updateScreen = doRight( rowHandler );
                        // continue;
                    }
                    if ( rowHandler.scrollUpDown( false ) )
                        updateScreen = true;
                } else if ( key == ui::kLEFT ) { CASE_LEFT:
// Serial.printf( "LEFT=%d\n", leftPressed );
                    if ( leftPressed ) {
                        // still pressed
                        updateScreen = doDecrement( rowHandler, calcIncrementLevel() );
                    } else {
                        pressedTime = millis();
                        updateScreen = doDecrement( rowHandler, PropertyEditorEntry::incrementLevel::small );
                    }
                    leftPressed = true;
                    // --- ORIG ---
                    // updateScreen = doLeft( rowHandler );
                } else if ( key == ui::kRIGHT ) { CASE_RIGHT:
// Serial.printf( "RIGHT=%d\n", rightPressed );
                    if ( rightPressed ) {
                        // still pressed
                        updateScreen = doIncrement( rowHandler, calcIncrementLevel() );
                    } else {
                        pressedTime = millis();
                        updateScreen = doIncrement( rowHandler, PropertyEditorEntry::incrementLevel::small );
                    }
                    rightPressed = true;
                    // --- ORIG ---
                    // updateScreen = doRight( rowHandler );
                } else if ( key == ui::kENTER ) {
                    // edit/accept value

                    if ( readOnly ) return ui::kENTER;

                    auto se = actionableEntry( rowHandler );
                    if ( se == nullptr ) continue;

                    switch( se->sCase ) {
                    case PropertyEditorEntry::entryCore::specialCase::cAccept:
                        ui::waitUntilNothingIsPressed();
                        return ui::kENTER;
                    case PropertyEditorEntry::entryCore::specialCase::cCancel:
                        ui::waitUntilNothingIsPressed();
                        return ui::kESCAPE;
                    case PropertyEditorEntry::entryCore::specialCase::cNone:
                        if ( keypad == kpadStyle::kSlidersOnly ) {
                            kpadSliderOnlyEditing = !kpadSliderOnlyEditing;
                            updateScreen = true;
                            continue;
                        }
                        break;
                    }

                    char *editBuffer = nullptr;
                    dataWindow.row = rowHandler.focusedOnScreenRow();

                    uint8_t bSize = se->editBufferSize();        
                    if ( bSize != 0 ) {
                        editBuffer = new char[bSize];
                        se->toEditString( editBuffer, bSize, dataWindow );
                    }

                    while( true ) {
                        auto r = se->enterEditMode( editBuffer, dataWindow );
                        if ( r == PropertyEditorEntry::entryCore::editResult::accepted ) {
                            hasChanges = true;
                            break;
                        }
                        if ( r == PropertyEditorEntry::entryCore::editResult::cancelled )
                            break;
                        if ( r == PropertyEditorEntry::entryCore::editResult::invalid ) {
                            // invalid, show warning and edit again
                            dataWindow.windowSize = dataWidth-1;
                            ui::LCD->writeAt( dataWindow.col+dataWidth-1, dataWindow.row, ']' );
                            ui::LCD->write( errorCharacter );
                        }
                    }
                    // restore
                    dataWindow.windowSize = dataWidth;
                    if ( editBuffer != nullptr ) {
                        delete editBuffer;
                        editBuffer = nullptr;
                    }
                    updateScreen = true;
                }
                if ( backgroundProcess != nullptr ) {
                    auto r = backgroundProcess();
                    if ( r != ui::kNONE )
                        return r;
                }
            }
        }

        bool doLeft( PropertyEditorEntry::rowHandler rowHandler ) {
            auto se = actionableEntry( rowHandler );
            if ( se == nullptr ) return false;
            if ( se->moveLeft() ) {
                hasChanges = true;
                return true;
            }
            return false;
        }

        bool doRight( PropertyEditorEntry::rowHandler rowHandler ) {
            auto se = actionableEntry( rowHandler );
            if ( se == nullptr ) return false;
            if ( se->moveRight() ) {
                hasChanges = true;
                return true;
            }
            return false;
        }

        bool doDecrement( PropertyEditorEntry::rowHandler rowHandler, PropertyEditorEntry::incrementLevel level  ) {
            auto se = actionableEntry( rowHandler );
            if ( se == nullptr ) return false;
            if ( se->decrement( level ) ) {
                hasChanges = true;
                return true;
            }
            return false;
        }

        bool doIncrement( PropertyEditorEntry::rowHandler rowHandler, PropertyEditorEntry::incrementLevel level ) {
            auto se = actionableEntry( rowHandler );
            if ( se == nullptr ) return false;
            if ( se->increment( level ) ) {
                hasChanges = true;
                return true;
            }
            return false;
        }

        PropertyEditorEntry::entryCore *actionableEntry( PropertyEditorEntry::rowHandler &rowHandler ) {
            if ( readOnly ) return nullptr;
            if ( !rowHandler.hasFocusedEntry() ) return nullptr;
            auto se = rowHandler.getFocusedEntry();
            if ( !se->isSelectable() || se->readonly ) return nullptr;
            return se;
        }

    public:

        void acceptChanges() {
            if ( readOnly ) return;
            auto *se = head.getFirst();
            // auto *se = head;
            while( se != nullptr ) {
                if ( !se->readonly )
                    se->acceptChange();
                se = head.getNext();
                // se = se->next;
            }
        }

};

PropertyEditor::markerStruct PropertyEditor::markers;

}

//
// TEST
//

    #include <Arduino.h>

namespace StarterPack {

    void TEST_propertyEditorBasic() {
        PropertyEditor se;
        
        // se.allowCrossover = true;
        se.allowCrossover = false;

        // ATOLL
        float a = atof( "hello" );
        Serial.println( a );
        // long i = Num::myAtoll( "hello 123" );
        long i;
        Num::StrToNum( "hello 123", i );
        Serial.println( i );
        // i = Num::myAtoll( "456 123" );
        Num::StrToNum( "456 123", i );
        Serial.println( i );

        se.message( "Editable Options" );
        bool t = true;
        bool f = false;
        se.add( "bool 1", t );
        se.add( "bool 2", f );
        se.breaker( '-' );
        int8_t  si08 = -100;
        uint8_t ui08 = 100;
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
        se.addPicker( "pick", pick, options );

        char buffer[15];
        strcpy( buffer, "there" );
        se.addString( "hello", buffer, 15 );

        se.message( "end of options" );

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

    void TEST_propertyEditorLatching() {
        PropertyEditor se;

        // se.allowCrossover = true;
        se.allowCrossover = false;

        bool t = true;
        se.message( "*** FIRST ***" );
        se.message( "Text" );
        se.message( "Text" );
        se.message( "Text" );
        se.message( "Text" );
        se.add( "bool 1", t );
        se.message( "Text" );
        se.message( "Text" );
        se.message( "Text" );
        se.message( "Text" );
        se.message( "Text" );
        se.add( "bool 1", t );
        se.message( "Text" );
        se.message( "Text" );
        se.message( "Text" );
        se.message( "Text" );
        se.message( "*** LAST ***" );

        se.prompt( 10 );
    }

    void TEST_propertyEditorSliders() {
        namespace ui = StarterPack::UserInterface;
        PropertyEditor se;
        
        // se.allowCrossover = true;
        se.allowCrossover = false;

        ui::setRepeatDelayAndRateInMs( 200, 10 );

        se.message( "Editable Options" );
        bool t = true;
        bool f = false;
        se.add( "bool 1", t );
        se.add( "bool 2", f );
        se.breaker( '-' );
        int8_t  si08 = -100;
        uint8_t ui08 = 100;
        se.addSlider( "si08", si08 );
        se.addSlider( "ui08", ui08 );
        int16_t  si16 = -100;
        uint16_t ui16 = 100;
        se.addSlider( "si16", si16 );
        se.addSlider( "ui16", ui16 );
        int32_t  si32 = -100;
        uint32_t ui32 = 100;
        se.addSlider( "si32", si32 );
        se.addSlider( "ui32", ui32 );

        uint8_t pick = 2;
        static const char *options[] = { "No", "Yes", "Cancel" };
        se.addPicker( "pick", pick, options );

        char buffer[15];
        strcpy( buffer, "there" );
        se.addString( "hello", buffer, 15 );

        se.message( "end of options" );

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

}

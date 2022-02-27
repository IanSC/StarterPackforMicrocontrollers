#pragma once
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <spUtility.h>
#include <UserInterface.h>
// #include <LCDEditor.h>
#include <LCDEditorAlpha.h>

using namespace StarterPack;

namespace StarterPack {

    class PropertyEditor;

    // class entryCore;
    // template<class T>
    // extern void delete_NextLinkedList( T *head );

    namespace PropertyEditorEntry {
        
        class entryCore {

            // make "next" public for now
            // need to give access to: void StarterPack::delete_NextLinkedList(T*);
            // but does not work:
            //    template<class T>
            //    friend void delete_NextLinkedList( T *head );
            //    friend void StarterPack::delete_NextLinkedList( entryCore* );
            public:

                entryCore  *next      = nullptr;
                bool       skipDelete = false;
            
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

            bool isSingleLiner() { return true;  }
            bool isSelectable()  { return false; }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {
                bufferSize = std::min( bufferSize, (uint8_t) (length+1) );
                memset( buffer, ch, bufferSize-1 );
                buffer[bufferSize-1] = 0;
            }
        };

        class message : public entryCore { public:
            message( const char *str ) { caption=str; readonly=true; }
            bool isSingleLiner() { return true;  }
            bool isSelectable()  { return false; }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {
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
            const char *falseValue = nullptr;;
            _bool( const char *caption, bool &data, bool readonly=false ) {
                this->caption=caption; this->ptr=&data; this->data=data; this->readonly=readonly; }
            _bool( const char *caption, bool &data, const char *trueValue, const char *falseValue, bool readonly=false ) {
                this->caption=caption; this->ptr=&data; this->data=data; this->trueValue=trueValue; this->falseValue=falseValue; this->readonly=readonly; }
            bool moveLeft()  { if (  data ) { data = false; return true; } else return false; }
            bool moveRight() { if ( !data ) { data = true;  return true; } else return false; }
            editResult enterEditMode( char *editBuffer, StarterPack::windowPosition &wPos ) {
                if ( data ) data=false; else data=true; return accepted; }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {
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
            bool parseUserEntry( char *buffer ) { return true; }
            void acceptChange() { *((bool*)ptr) = data; }
        };

        //
        // INTEGERS
        //

        template <typename T, size_t BUFFERSIZE, bool NEG>
        class entryRangedInt : public entryCore { public:
            T data;
            T min;
            T max;
            entryRangedInt( const char *caption, T &data, T min, T max, bool readonly ) {
                this->caption = caption; ptr = &data; this->readonly = readonly;
                this->data = data; this->min = min; this->max = max;
            }
            bool moveLeft()  { if ( data>min ) { data--; return true; } else return false; }
            bool moveRight() { if ( data<max ) { data++; return true; } else return false; }
            uint8_t editBufferSize() { return BUFFERSIZE; }
            editResult enterEditMode( char *editBuffer, StarterPack::windowPosition &wPos ) {
                namespace ui = StarterPack::UserInterface;
                StarterPack::editorSettings eSet;
                eSet.col = wPos.col; eSet.row = wPos.row; eSet.windowSize = wPos.windowSize;
                eSet.allowDecimal = false;
                eSet.allowNegative = NEG && min<0;
                eSet.bufferLength = BUFFERSIZE;
                StarterPack::numericEditor editor( editBuffer, eSet );
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
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {
                snprintf( buffer, bufferSize, "%d", data );
            }
            void toEditString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {
                if ( data == 0 )
                    buffer[0] = 0;
                else
                    snprintf( buffer, bufferSize, "%d", data );
            }
            bool parseUserEntry( char *buffer ) {
                int64_t I = atoll( buffer );
                if ( I < min || I > max ) return false;
                data = I;
                return true;
            }
        };

        #define CLS(dType,D,LEN,NEG) \
            class D : public entryRangedInt<dType,LEN,NEG> { public: \
                D( const char *caption, dType &data, dType min, dType max, bool readonly ) \
                    : entryRangedInt(caption,data,min,max,readonly) {} \
                void acceptChange() { *((dType*) ptr) = data; } \
            };
        CLS( int8_t,   _si08, 4+1 , true  )  //        -127
        CLS( uint8_t,  _ui08, 3+1 , false )  //         255
        CLS( int16_t,  _si16, 6+1 , true  )  //      -32768
        CLS( uint16_t, _ui16, 5+1 , false )  //       65535
        CLS( int32_t,  _si32, 11+1, true  )  // -2147483647
        CLS( uint32_t, _ui32, 10+1, false )  //  4294967295
        #undef CLS

        //
        // FLOATING POINT
        //

        template <typename T, size_t BUFFERSIZE, bool NEG>
        class entryRangedFloat : public entryCore { public:
            T data;
            T min;
            T max;
            entryRangedFloat( const char *caption, T &data, T min, T max, bool readonly ) {
                this->caption = caption; ptr = &data; this->readonly = readonly;
                this->data = data; this->min = min; this->max = max;
            }
            uint8_t editBufferSize() { return BUFFERSIZE; }
            editResult enterEditMode( char *editBuffer, StarterPack::windowPosition &wPos ) {
                namespace ui = StarterPack::UserInterface;
                StarterPack::editorSettings eSet;
                eSet.col = wPos.col; eSet.row = wPos.row; eSet.windowSize = wPos.windowSize;
                eSet.allowDecimal = true;
                eSet.allowNegative = min<0;
                eSet.bufferLength = BUFFERSIZE;
                StarterPack::numericEditor editor( editBuffer, eSet );
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
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {
                snprintf( buffer, bufferSize, "%f", data );
                if ( isCharInString( '.', buffer ) ) {
                    // remove trailing '0' and '.'
                    char *p = buffer + strlen(buffer) - 1;
                    while( *p == '0' ) *p-- = 0;
                    if ( *p == '.' ) *p = 0;
                }                
            }
            void toEditString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {
                if ( data == 0 )
                    buffer[0] = 0;
                else
                    toString( buffer, bufferSize, wPos );
            }            
            bool parseUserEntry( char *buffer ) {
                double F = atof( buffer );
                if ( F < min || F > max ) return false;
                data = F;
                return true;
            }
        };

        #define CLS(dType,D,LEN,NEG) \
            class D : public entryRangedFloat<dType,LEN,NEG> { public: \
                D( const char *caption, dType &data, dType min, dType max, bool readonly) \
                    : entryRangedFloat(caption,data,min,max,readonly) {} \
                void acceptChange() { *((dType*) ptr) = data; } \
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
            bool moveLeft()  {
                if ( selected == 0 || selected > optionCount ) {
                    selected = 1;
                    return true;
                } else if ( selected > 1 ) {
                    selected--;
                    return true;
                }
                return false;
            }
            bool moveRight() {
                if ( selected == 0 || selected > optionCount ) {
                    selected = 1;
                    return true;
                } else if ( selected < optionCount ) {
                    selected++;
                    return true;
                }
                return false;
            }
            editResult enterEditMode( char *editBuffer, StarterPack::windowPosition &wPos ) {
                // with rollover
                selected++;
                if ( selected < 1 || selected > optionCount )
                    selected = 1;
                return accepted;
            }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {
                if ( selected < 1 || selected > optionCount ) {
                    buffer[0] = 0;
                } else {
                    // 1-based index
                    strncpy( buffer, options[selected-1], bufferSize );
                    buffer[bufferSize-1] = 0; // in case option is too long
                }
            }
            bool parseUserEntry( char *buffer ) { return true; }
            void acceptChange() { *((uint8_t*)ptr) = selected; }
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
            uint8_t editBufferSize() { return length; }
            editResult enterEditMode( char *editBuffer, StarterPack::windowPosition &wPos ) {
                namespace ui = StarterPack::UserInterface;

                StarterPack::editorSettings eSet;
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
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {
                strncpy( buffer, this->buffer, bufferSize );
                buffer[bufferSize-1] = 0; // in case text is too long
            }
            void toEditString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {
                strncpy( buffer, this->buffer, bufferSize );
                buffer[bufferSize-1] = 0;
            }
            bool parseUserEntry( char *buffer ) {
                strncpy( this->buffer, buffer, length );
                this->buffer[length-1] = 0;
                return true;
            }
            void acceptChange() {
                strncpy( (char*) ptr, buffer, length );
            }
        };

        //
        // SLIDER
        //

        template <typename T>
        class sliderInt : public entryCore { public:
            T data;
            T min;
            T max;
            sliderInt( const char *caption, T &data, T min, T max, bool readonly ) {
                this->caption = caption; ptr = &data; this->readonly = readonly;
                this->data = data; this->min = min; this->max = max;
            }
            bool moveLeft()  { if ( data>min ) { data--; return true; } else return false; }
            bool moveRight() { if ( data<max ) { data++; return true; } else return false; }
            void toString( char *buffer, uint8_t bufferSize, StarterPack::windowPosition &wPos ) {
                snprintf( buffer, bufferSize, "%d", data );
            }
        };
        #define CLS(dType,D) \
            class D : public sliderInt<dType> { public: \
                D( const char *caption, dType &data, dType min, dType max, bool readonly ) \
                    : sliderInt(caption,data,min,max,readonly) {} \
                void acceptChange() { *((dType*) ptr) = data; } \
            };
        CLS( int8_t,   _si08_slider )
        CLS( uint8_t,  _ui08_slider )
        CLS( int16_t,  _si16_slider )
        CLS( uint16_t, _ui16_slider )
        CLS( int32_t,  _si32_slider )
        CLS( uint32_t, _ui32_slider )
        #undef CLS

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
                StarterPack::PropertyEditorEntry::entryCore *head;

            public:

                bool allowCrossover = false;

                rowHandler( StarterPack::PropertyEditorEntry::entryCore *se, uint8_t lcdRows, bool allowCrossover = false ) {
                    head = se;
                    this->lcdRows = lcdRows;
                    this->allowCrossover = allowCrossover;
                    while( se != nullptr ) {
                        count++;
                        se = se->next;
                    }
                    focusScanDownwards( 0 );
                }

                StarterPack::PropertyEditorEntry::entryCore *getEntry( uint8_t entryNo ) {
                    if ( entryNo < 0 || entryNo >= count ) return nullptr;
                    auto *ptr = head;
                    for( int i = 0 ; i < entryNo ; i++ )
                        ptr = ptr->next;
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
                inline StarterPack::PropertyEditorEntry::entryCore *getFocusedEntry() {
                    return getEntry( focusedEntry );
                }
                inline StarterPack::PropertyEditorEntry::entryCore *getItemOnTopRow() {
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

    class PropertyEditor {

        public:

            ~PropertyEditor() {
                // in <spMacros.h>
                StarterPack::delete_NextLinkedListWithSkip( head );
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

            void add( StarterPack::PropertyEditorEntry::entryCore *property ) {
                // let user handle freeing from memory
                property->skipDelete = true;
                insert( property );
            }

            void addAndDelete( StarterPack::PropertyEditorEntry::entryCore *property ) {
                insert( property );
            }

            void breaker( char ch = '-' ) {
                namespace ui = StarterPack::UserInterface;
                auto *se = new StarterPack::PropertyEditorEntry::breaker( ch, ui::LCD->maxColumns );
                insert( se );
            }

            void message( const char *str ) {
                auto *se = new StarterPack::PropertyEditorEntry::message(str);
                insert( se );
            }

            //
            // BOOL
            //

            void add( const char *caption, bool &data, bool readonly=false ) {
                auto *se = new StarterPack::PropertyEditorEntry::_bool(caption,data,readonly);
                insert( se );
            }

            void add( const char *caption, bool &data, const char *trueValue, const char *falseValue, bool readonly=false ) {
                auto *se = new StarterPack::PropertyEditorEntry::_bool(caption,data,trueValue,falseValue,readonly);
                insert( se );
            }

            //
            // INTEGER/FLOAT
            //
            #define ADD(dType,D,MIN,MAX) \
                void add( const char *caption, dType &data, bool readonly=false ) { \
                    auto *se = new StarterPack::PropertyEditorEntry::D(caption,data,MIN,MAX,readonly); \
                    insert( se ); \
                } \
                void add( const char *caption, dType &data, dType min, dType max, bool readonly=false ) { \
                    auto *se = new StarterPack::PropertyEditorEntry::D(caption,data,min,max,readonly); \
                    insert( se ); \
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
            void addPicker( const char *caption, uint8_t &selected, const char* (&options)[optCount], bool readonly=false ) {
                auto *se = new StarterPack::PropertyEditorEntry::pick(
                    caption,selected,options,optCount,readonly);
                insert( se );
            }

            void addString( const char *caption, char *buffer, uint8_t size, bool readonly=false ) {
                auto *se = new StarterPack::PropertyEditorEntry::_string(caption,buffer,size,readonly);
                insert( se );
            }

            //
            // SLIDERS
            //
            #define ADD(dType,D,MIN,MAX) \
                void addSlider( const char *caption, dType &data, bool readonly=false ) { \
                    auto *se = new StarterPack::PropertyEditorEntry::D ## _slider(caption,data,MIN,MAX,readonly); \
                    insert( se ); \
                } \
                void addSlider( const char *caption, dType &data, dType min, dType max, bool readonly=false ) { \
                    auto *se = new StarterPack::PropertyEditorEntry::D ## _slider(caption,data,min,max,readonly); \
                    insert( se ); \
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

            StarterPack::PropertyEditorEntry::entryCore *head = nullptr;

            inline void insert( StarterPack::PropertyEditorEntry::entryCore *se ) {
                insertEnd_NextLinkedList( &head, se );
                // if ( head == nullptr ) {
                //     head = se;
                // } else {
                //     auto *ptr = head;
                //     while ( ptr->next != nullptr )
                //         ptr = ptr->next;
                //     ptr->next = se;
                // }
            }

    public:

        bool hasChanges = false;

        uint8_t show( uint8_t captionWidth ) {
            readOnly = true;
            return promptCore( captionWidth );
        }

        void prompt( uint8_t captionWidth ) {
            readOnly = false;
            promptCore( captionWidth );
        }

    protected:

        bool readOnly = false;

        uint8_t promptCore( uint8_t captionWidth ) {
            namespace ui = StarterPack::UserInterface;

            if ( !ui::hasScreen() ) return 0;
            LCDInterface *lcd = ui::LCD;
            LCDBuffered *lcdBuffered = nullptr;
            if ( lcd->isBuffered() )
                lcdBuffered = (LCDBuffered*) lcd;

            StarterPack::PropertyEditorEntry::rowHandler rowHandler( head, lcd->maxRows, allowCrossover );

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
                        se = se->next;
                    }

                    // se = getEntry( row.current );
                    if ( rowHandler.hasFocusedEntry() ) {
                        se = rowHandler.getFocusedEntry();
                        if ( se->isSelectable() ) {
                            int r = rowHandler.focusedOnScreenRow();
                            lcd->printAt( captionWidth, r, '[' );
                            lcd->printAt( lcd->maxColumns-1, r, ']' );
                        }
                    }
                }
                if ( lcdBuffered != nullptr )
                    lcdBuffered->update();

                //
                // KEY
                //
                uint8_t key = ui::getRepeatingKey();
                
                if ( key == ui::kESCAPE ) {
                    ui::waitUntilNothingIsPressed();
                    return ui::kESCAPE;
                } else if ( key == ui::kUP ) {
                    if ( rowHandler.scrollUpDown( true ) )
                        updateScreen = true;
                } else if ( key == ui::kDOWN ) {
                    if ( rowHandler.scrollUpDown( false ) )
                        updateScreen = true;
                } else if ( key == ui::kLEFT ) {
                    auto se = actionableEntry( rowHandler );
                    if ( se == nullptr ) continue;
                    if ( se->moveLeft() ) {
                        hasChanges = true;
                        updateScreen = true;
                    }
                } else if ( key == ui::kRIGHT ) {
                    auto se = actionableEntry( rowHandler );
                    if ( se == nullptr ) continue;
                    if ( se->moveRight() ) {
                        hasChanges = true;
                        updateScreen = true;
                    }
                } else if ( key == ui::kENTER ) {
                    // edit value
                    if ( readOnly ) return ui::kENTER;
                    auto se = actionableEntry( rowHandler );
                    if ( se == nullptr ) continue;

                    char *editBuffer = nullptr;
                    dataWindow.row = rowHandler.focusedOnScreenRow();

                    uint8_t bSize = se->editBufferSize();        
                    if ( bSize != 0 ) {
                        editBuffer = new char[bSize];
                        se->toEditString( editBuffer, bSize, dataWindow );
                    }

                    while( true ) {
                        auto r = se->enterEditMode( editBuffer, dataWindow );
                        if ( r == StarterPack::PropertyEditorEntry::entryCore::editResult::accepted ) {
                            hasChanges = true;
                            break;
                        }
                        if ( r == StarterPack::PropertyEditorEntry::entryCore::editResult::cancelled )
                            break;
                        if ( r == StarterPack::PropertyEditorEntry::entryCore::editResult::invalid ) {
                            // invalid, show warning and edit again
                            dataWindow.windowSize = dataWidth-1;
                            ui::LCD->writeAt( dataWindow.col+dataWidth-1, dataWindow.row, ']' );
                            ui::LCD->write( errorCharacter );
                        }
                    }
                    // restore
                    dataWindow.windowSize = dataWidth;
                    if ( editBuffer != nullptr )
                        delete editBuffer;
                    updateScreen = true;
                }
            }
        }

        StarterPack::PropertyEditorEntry::entryCore *actionableEntry( StarterPack::PropertyEditorEntry::rowHandler &rowHandler ) {
            if ( readOnly ) return nullptr;
            if ( !rowHandler.hasFocusedEntry() ) return nullptr;
            auto se = rowHandler.getFocusedEntry();
            if ( !se->isSelectable() || se->readonly ) return nullptr;
            return se;
        }

    public:

        void acceptChanges() {
            if ( readOnly ) return;
            auto *se = head;
            while( se != nullptr ) {
                if ( !se->readonly )
                    se->acceptChange();
                se = se->next;
            }
        }

};

//
// TEST
//

    #include <Arduino.h>

    void TEST_propertyEditorBasic() {
        PropertyEditor se;
        
        // se.allowCrossover = true;
        se.allowCrossover = false;

        float a = atof( "hello" );
        Serial.println( a );
        long i = atoll( "hello 123" );
        Serial.println( i );
        i = atoll( "456 123" );
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

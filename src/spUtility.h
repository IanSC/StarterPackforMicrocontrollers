#pragma once
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

namespace StarterPack {

//
// MAP CONVERTERS
//

    // template <typename TType>
    // TType spMapCore( TType value, TType inputMin, TType inputMax, TType outputMin, TType outputMax ) {
    //     // from WMath.cpp - but fixed
    //     const TType run = inputMax - inputMin;
    //     if( divisor == 0 ) {
    //         log_e( "spMap(): invalid input range, min == max" );
    //         return -1; //AVR returns -1, SAM returns 0
    //     }
    //     const TType rise = outputMax - outputMin;
    //     const TType delta = value - inputMin;
    //     return ( delta * rise ) / run + outputMin;
    // }

    template<typename T, typename U>
    void spMapLimit(T& value, U min, U max) {
        if (min <= max) {
            if (value<min) value=min;
            if (value>max) value=max;
        } else {
            if (value<max) value=max;
            if (value>min) value=min;
        }
    }

    int spMap( int value, int inputMin, int inputMax, int outputMin, int outputMax ) {
        int run = inputMax - inputMin;
        if ( run == 0 ) return -1;
        auto r = ( (long)(value - inputMin) * (long)(outputMax - outputMin) ) / run + outputMin;
        spMapLimit(r, outputMin, outputMax);
        return r;
    }

    long spMapLong( long value, long inputMin, long inputMax, long outputMin, long outputMax ) {
        int run = inputMax - inputMin;
        if ( run == 0 ) return -1;
        auto r = ( (long long)(value - inputMin) * (long long)(outputMax - outputMin) ) / run + outputMin;
        spMapLimit(r, outputMin, outputMax);
        return r;
    }

    inline float spMapFloat( int value, int inputMin, int inputMax, int outputMin, int outputMax ) {
        float run = inputMax - inputMin;
        if ( run == 0 ) return -1;
        auto r = (float) ( (value - inputMin) * (outputMax - outputMin) ) / run + outputMin;
        spMapLimit(r, outputMin, outputMax);
        return r;
    }

    inline float spMapFloat( float value, float inputMin, float inputMax, float outputMin, float outputMax ) {
        float run = inputMax - inputMin;
        if ( run == 0 ) return -1;
        auto r = ( (value - inputMin) * (outputMax - outputMin) ) / run + outputMin;
        spMapLimit(r, outputMin, outputMax);
        return r;
    }

    // template <typename TType>
    // TType spLimit( TType value, TType min, TType max ) {
    //     if ( value < min ) return min;
    //     if ( value > max ) return max;
    //     return value;
    // }

//
// SERIAL
//

    void SerialPrintCharsN( char ch, uint8_t count ) {
        while( count-- > 0 )
            Serial.print( ch );
    }

    void SerialPrintf( const char *format, ... ) {
        const uint8_t maxChars = 100;
        char buffer[maxChars+1];
        va_list args;
        va_start( args, format );
        vsnprintf(buffer, maxChars, format, args);
        va_end( args );
        Serial.print( buffer );
    }

    void SerialPrintfln( const char *format, ... ) {
        // how to pass to SerialPrintf() without template
        // since it does not work in Arduino
        const uint8_t maxChars = 100;
        char buffer[maxChars+1];
        va_list args;
        va_start( args, format );
        vsnprintf(buffer, maxChars, format, args);
        va_end( args );
        Serial.println( buffer );
    }

    void SerialPrintStrN( const char *str, uint8_t N ) {
        // print first N characters of string
        while( N > 0 && *str != 0 ) {
            Serial.print( *str++ );
            N--;
        }
    }

//
// STRING
//

    bool isCharInString( uint8_t key, const char *list ) {
        if ( list == nullptr ) return false;
        return ( strchr( list, key ) != nullptr );
    }

    void sortCharsInString(char *str) {
        auto n = strlen(str);
        char temp;
        for (size_t i = 0; i < n-1; i++) {
            for (size_t j = i+1; j < n; j++) {
                if (str[i] > str[j]) {
                    temp = str[i];
                    str[i] = str[j];
                    str[j] = temp;
                }
            }
        }
    }

    bool areCharsInStringSimple(const char *str1, const char *str2) {
        // repetition not handled
        // will match "ABB" and "BAA"
        auto L1 = strlen(str1);
        auto L2 = strlen(str2);
        if (L1 != L2) return false;
        for (size_t i=0; i<L1; i++) {
            if (!isCharInString(str1[i],str2))
                return false;
        }
        return true;
    }

    bool areCharsInString(const char *str1, const char *str2) {
        // slower but repetition handled
        // will NOT match "ABB" and "BAA"
        auto L1 = strlen(str1);
        auto L2 = strlen(str2);
        if (L1 != L2) return false;

        char buffer[L2+1];
        strcpy(buffer,str2);

        // for each char in str1
        for (size_t i1=0; i1<L1; i1++) {
            // ... find in buffer and flag it
            auto found = false;
            for (size_t i2=0; i2<L1; i2++) {
                if (buffer[i2] == str1[i1]) {
                    // match
                    buffer[i2] = 0;
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
        return true;
    }

//
// BITS
//

    template <typename TType>
    uint8_t countBits( TType input ) {
        // count number of "1" bits
        uint8_t bits = 0;
        while( input != 0 ) {
            if ( (input & 1) == 1 )
                bits++;
            input >>= 1;
        }
        return bits;
    }

    uint8_t first1BitOccurence( uint32_t input ) {
        if ( input == 0 ) return 0;
        #if defined(__GNUC__)
            // https://stackoverflow.com/a/31393298
            // https://stackoverflow.com/questions/28166565/detect-gcc-as-opposed-to-msvc-clang-with-macro
            // https://www.geeksforgeeks.org/builtin-functions-gcc-compiler/
            return __builtin_ctzl( input ) + 1;
        #else
            uint8_t r = 1;
            while( ( input & 1 ) == 0 ) {
                input >>= 1;
                r++;
            }
            return r;
        #endif
    }

//
// NEXT-LINKED LIST
//

    /*
    
    USE: spVector<>

    // insert new item as head or at end of list
    // A* head = nullptr;
    // class A {
    //    ...
    //    A* next;
    // }
    template<typename T>
    void insertEnd_NextLinkedList( T **head, T *item ) {
        if ( *head == nullptr )
            *head = item;
        else {
            T *p = *head;
            while ( p->next != nullptr ) p = p->next;
            p->next = item;
        }
    }

    // delete "next-linked" list
    // class A {
    //    ...
    //    A* next;
    // }
    template<class T>
    void delete_NextLinkedList( T *head ) {
        if ( head == nullptr ) return;
        while( head->next != nullptr ) {
            // slow but avoids recursion
            T *p1 = head;
            T *p2 = p1->next;
            while ( p2->next != nullptr ) {
                p1 = p2;
                p2 = p2->next;
            }
            delete p2;
            p1->next = nullptr;
        }
        delete head;
    }

    // delete "next-linked" list
    // class A {
    //    ...
    //    bool skipDelete;
    //    A* next;
    // }
    template<class T>
    void delete_NextLinkedListWithSkip( T *head ) {
        if ( head == nullptr ) return;
        while( head->next != nullptr ) {
            // slow but avoids recursion
            T *p1 = head;
            T *p2 = p1->next;
            while ( p2->next != nullptr ) {
                p1 = p2;
                p2 = p2->next;
            }
            if ( !p2->skipDelete )
                delete p2;
            p1->next = nullptr;
        }
        delete head;
    }
    */

}

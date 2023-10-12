#pragma once

#include <ctype.h>
// #include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include <Arduino.h>

namespace StarterPack {
    
class Num {

    //
    //
    //
    public:

        static void TEST_StrToNum() {
            int8_t  Int8;  uint8_t  UInt8;
            int16_t Int16; uint16_t UInt16;
            int32_t Int32; uint32_t UInt32;
            int64_t Int64; uint64_t UInt64;

            size_t bufferSize = 100;
            char buffer[bufferSize];

            // Int8
            mySnprintf(buffer,bufferSize,(int8_t)INT8_MIN);
            StrToNum(buffer,Int8);
            if (Int8 != INT8_MIN) Serial.print("(ERR) ");
            Serial.print("Int8 = ");
            Serial.print(Int8);
            Serial.print(" = ");
            Serial.println(buffer);

            mySnprintf(buffer,bufferSize,(int8_t)INT8_MAX);
            StrToNum(buffer,Int8);
            if (Int8 != INT8_MAX) Serial.print("(ERR) ");
            Serial.print("Int8 = ");
            Serial.print(Int8);
            Serial.print(" = ");
            Serial.println(buffer);

            // UInt8
            mySnprintf(buffer,bufferSize,(uint8_t)UINT8_MAX);
            StrToNum(buffer,UInt8);
            if (UInt8 != UINT8_MAX) Serial.print("(ERR) ");
            Serial.print("UInt8 = ");
            Serial.print(UInt8);
            Serial.print(" = ");
            Serial.println(buffer);
            
            // Int16
            mySnprintf(buffer,bufferSize,(int16_t)INT16_MIN);
            StrToNum(buffer,Int16);
            if (Int16 != INT16_MIN) Serial.print("(ERR) ");
            Serial.print("Int16 = ");
            Serial.print(Int16);
            Serial.print(" = ");
            Serial.println(buffer);
            
            mySnprintf(buffer,bufferSize,(int16_t)INT16_MAX);
            StrToNum(buffer,Int16);
            if (Int16 != INT16_MAX) Serial.print("(ERR) ");
            Serial.print("Int16 = ");
            Serial.print(Int16);
            Serial.print(" = ");
            Serial.println(buffer);
            
            // UInt16
            mySnprintf(buffer,bufferSize,(uint16_t)UINT16_MAX);
            StrToNum(buffer,UInt16);
            if (UInt16 != UINT16_MAX) Serial.print("(ERR) ");
            Serial.print("UInt16 = ");
            Serial.print(UInt16);
            Serial.print(" = ");
            Serial.println(buffer);
            
            // Int32
            mySnprintf(buffer,bufferSize,(int32_t)INT32_MIN);
            StrToNum(buffer,Int32);
            if (Int32 != INT32_MIN) Serial.print("(ERR) ");
            Serial.print("Int32 = ");
            Serial.print(Int32);
            Serial.print(" = ");
            Serial.println(buffer);

            mySnprintf(buffer,bufferSize,(int32_t)INT32_MAX);
            StrToNum(buffer,Int32);
            if (Int32 != INT32_MAX) Serial.print("(ERR) ");
            Serial.print("Int32 = ");
            Serial.print(Int32);
            Serial.print(" = ");
            Serial.println(buffer);

            // UInt32
            mySnprintf(buffer,bufferSize,(uint32_t)UINT32_MAX);
            StrToNum(buffer,UInt32);
            if (UInt32 != UINT32_MAX) Serial.print("(ERR) ");
            Serial.print("UInt32 = ");
            Serial.print(UInt32);
            Serial.print(" = ");
            Serial.println(buffer);            

            // Int64
            mySnprintf(buffer,bufferSize,(int64_t)INT64_MIN);
            StrToNum(buffer,Int64);
            if (Int64 != INT64_MIN) Serial.print("(ERR) ");
            Serial.print("Int64 = ");
            Serial.println(buffer);

            mySnprintf(buffer,bufferSize,(int64_t)INT64_MAX);
            StrToNum(buffer,Int64);
            if (Int64 != INT64_MAX) Serial.print("(ERR) ");
            Serial.print("Int64 = ");
            Serial.println(buffer);

            // UInt64
            mySnprintf(buffer,bufferSize,(uint64_t)UINT64_MAX);
            StrToNum(buffer,UInt64);
            if (UInt64 != UINT64_MAX) Serial.print("(ERR) ");
            Serial.print("UInt64 = ");
            Serial.println(buffer);

            // check if 0 in between
            Int64 = 1;
            for(int i=0; i<25; i++) {
                mySnprintf(buffer,bufferSize,Int64);
                int64_t tInt64;
                StrToNum(buffer,tInt64);
                if (Int64 != tInt64) Serial.print("(ERR) ");
                Serial.print("Int64 = ");
                Serial.println(buffer);
                Int64 = Int64*10;
            }

            Int64 = -1;
            for(int i=0; i<25; i++) {
                mySnprintf(buffer,bufferSize,Int64);
                int64_t tInt64;
                StrToNum(buffer,tInt64);
                if (Int64 != tInt64) Serial.print("(ERR) ");
                Serial.print("Int64 = ");
                Serial.println(buffer);
                Int64 = Int64*10;
            }

            UInt64 = 1;
            for(int i=0; i<25; i++) {
                mySnprintf(buffer,bufferSize,UInt64);
                uint64_t tUInt64;
                StrToNum(buffer,tUInt64);
                if (UInt64 != tUInt64) Serial.print("(ERR) ");
                Serial.print("UInt64 = ");
                Serial.println(buffer);
                UInt64 = UInt64*10;
            }

            Serial.println("TEST DONE");
        }

    //
    // SNPRINTF
    //
    // prevent snprintf() from complaining about "%d"
    // handle 64 bits
    //
    // type      bufferSize             min                   max
    // int8_t    4+1                   -128                   127
    // uint8_t   3+1                      0                   255
    // int16_t   6+1                 -32768                 32767
    // uint16_t  5+1                      0                 65535
    // int32_t   11+1           -2147483648            2147483647
    // uint32_t  10+1                     0            4294967295
    // int64_t   20+1  -9223372036854775808   9223372036854775807
    // uint64_t  20+1                     0  18446744073709551615
    public:
        inline static int mySnprintf(char *buffer, size_t bufferSize, int8_t value) {
            return snprintf( buffer, bufferSize, "%d", value );
        }
        inline static int mySnprintf(char *buffer, size_t bufferSize, uint8_t value) {
            return snprintf( buffer, bufferSize, "%u", value );
        }
        inline static int mySnprintf(char *buffer, size_t bufferSize, int16_t value) {
            return snprintf( buffer, bufferSize, "%d", value );
        }
        inline static int mySnprintf(char *buffer, size_t bufferSize, uint16_t value) {
            return snprintf( buffer, bufferSize, "%u", value );
        }
        inline static int mySnprintf(char *buffer, size_t bufferSize, int32_t value) {
            return snprintf( buffer, bufferSize, "%ld", value );
        }
        inline static int mySnprintf(char *buffer, size_t bufferSize, uint32_t value) {
            return snprintf( buffer, bufferSize, "%lu", value );
        }
        inline static int mySnprintf(char *buffer, size_t bufferSize, int64_t value) {
            // NOT WORKING: snprintf( buffer, bufferSize, "%lld", value );
            // int32  32-bit  -2,147,483,648              2,147,483,647
            // int64  64-bit  -9,223,372,036,854,775,808  9,223,372,036,854,775,807
            if (value < 0) {
                strncpy(buffer, "-", bufferSize);
                return mySnprintf(buffer+1,bufferSize-1,(uint64_t)-value);
            }
            return mySnprintf(buffer,bufferSize,(uint64_t)value);
        }
        inline static int mySnprintf(char *buffer, size_t bufferSize, uint64_t value) {
            // NOT WORKING: snprintf( buffer, bufferSize, "%llu", value );
            // UInt32   4294967295
            // UInt64   18446744073709551615
            //          10000000000
            //                    1000000
            uint32_t UInt32;
            UInt32 = value / 10000000000;
            bool hasValue = false;
            if (UInt32 != 0) {
                snprintf(buffer,bufferSize,"%lu",UInt32);
                hasValue = true;
                value = value % 10000000000;
            } // else not above 10000000000

            UInt32 = value / 100000;
            if (hasValue) {
                auto p = buffer + strlen(buffer);
                auto ps = bufferSize - strlen(buffer);
                snprintf(p,ps,"%05lu",UInt32);
            } else {
                if (UInt32 != 0) {
                    snprintf(buffer,bufferSize,"%lu",UInt32);
                    hasValue = true;
                }
            }

            UInt32 = value % 100000;
            if (hasValue) {
                auto p = buffer + strlen(buffer);
                auto ps = bufferSize - strlen(buffer);
                return snprintf(p,ps,"%05lu",UInt32);
            } else {
                return snprintf(buffer,bufferSize,"%lu",UInt32);
            }
        }
        inline static int mySnprintf(char *buffer, size_t bufferSize, float value) {
            return snprintf( buffer, bufferSize, "%f", (double) value );
        }
        inline static int mySnprintf(char *buffer, size_t bufferSize, double value) {
            return snprintf( buffer, bufferSize, "%f", value );
        }
        static const void mySnprintf_FloatingPointFormat( char *fmt, uint8_t decimalPlaces ) {
            // ex. %.2f --> 2 decimal places
            strcpy( fmt, "%." );
            sprintf( fmt+2, "%d", decimalPlaces );
            strcat( fmt, "f" );
            // Serial.print("fmt=");
            // Serial.println(fmt);
        } 
        inline static int mySnprintf(char *buffer, size_t bufferSize, float value, uint8_t decimalPlaces) {
            // %.??f --> max 5 characters only
            if ( decimalPlaces > 99 ) decimalPlaces = 99;
            char fmt[6];
            mySnprintf_FloatingPointFormat( fmt, decimalPlaces );
            return snprintf( buffer, bufferSize, fmt, (double) value );
        }
        inline static int mySnprintf(char *buffer, size_t bufferSize, double value, uint8_t decimalPlaces) {
            // %.??f --> max 5 characters only
            if ( decimalPlaces > 99 ) decimalPlaces = 99;
            char fmt[6];
            mySnprintf_FloatingPointFormat( fmt, decimalPlaces );
            return snprintf( buffer, bufferSize, fmt, value );
        }

    //
    // ATOLL - missing in Arduino
    //
    inline static void StrToNum( const char *buffer, int8_t &result ) {
        result = atoi( buffer );
    }
    inline static void StrToNum( const char *buffer, uint8_t &result ) {
        result = atoi( buffer );
    }
    inline static void StrToNum( const char *buffer, int16_t &result ) {
        result = atoi( buffer );
    }
    inline static void StrToNum( const char *buffer, uint16_t &result ) {
        result = atoi( buffer );
    }
    inline static void StrToNum( const char *buffer, int32_t &result ) {
        result = atol( buffer );
    }
    inline static void StrToNum( const char *buffer, uint32_t &result ) {
        result = atol( buffer );
    }

    #if defined(ESP8266) || defined(ESP32)    

        public:
            // inline static int64_t myAtoll( const char *buffer ) {
            //     return atoll( buffer );
            // }
            inline static void StrToNum( const char *buffer, int64_t &result ) {
                result = atoll( buffer );
            }
            inline static void StrToNum( const char *buffer, uint64_t &result ) {
                result = strtoull( buffer, NULL, 10 );
            }

            inline static void StrToNum( const char *buffer, long &result ) {
                result = atol( buffer );
            }
            inline static void StrToNum( const char *buffer, unsigned long &result ) {
                result = strtoul( buffer, NULL, 10 );
            }

    #else

        public:
            // inline int64_t static myAtoll( const char *buffer ) {
            //     return strtoll( buffer, NULL, 10 );
            // }
            inline static void StrToNum( const char *buffer, int64_t &result ) {
                result = strtoll( buffer, NULL, 10 );
            }
            inline static void StrToNum( const char *buffer, uint64_t &result ) {
                result = strtoull( buffer, NULL, 10 );
            }

        private:

            __extension__
            typedef unsigned long long ullong_type;

            __extension__
            typedef long long llong_type;

            #ifndef ULLONG_MAX
            #define ULLONG_MAX (~(ullong_type)0) /* 0xFFFFFFFFFFFFFFFF */
            #endif

            #ifndef LLONG_MAX
            #define LLONG_MAX ((llong_type)(ULLONG_MAX >> 1)) /* 0x7FFFFFFFFFFFFFFF */
            #endif

            #ifndef LLONG_MIN
            #define LLONG_MIN (~LLONG_MAX) /* 0x8000000000000000 */
            #endif

            static llong_type strtoll(const char *nptr, char **endptr, register int base) {

                // https://github.com/gcc-mirror/gcc/blob/master/libiberty/strtoll.c
                // Copyright (c) 2014 The Regents of the University of California.

                register const char *s = nptr;
                register ullong_type acc;
                register int c;
                register ullong_type cutoff;
                register int neg = 0, any, cutlim;

                /*
                * Skip white space and pick up leading +/- sign if any.
                * If base is 0, allow 0x for hex and 0 for octal, else
                * assume decimal; if base is already 16, allow 0x.
                */
                do {
                    c = *s++;
                } while (isspace(c));
                if (c == '-') {
                    neg = 1;
                    c = *s++;
                } else if (c == '+')
                    c = *s++;
                if ((base == 0 || base == 16) &&
                    c == '0' && (*s == 'x' || *s == 'X')) {
                    c = s[1];
                    s += 2;
                    base = 16;
                }
                if (base == 0)
                    base = c == '0' ? 8 : 10;

                /*
                * Compute the cutoff value between legal numbers and illegal
                * numbers.  That is the largest legal value, divided by the
                * base.  An input number that is greater than this value, if
                * followed by a legal input character, is too big.  One that
                * is equal to this value may be valid or not; the limit
                * between valid and invalid numbers is then based on the last
                * digit.  For instance, if the range for longs is
                * [-2147483648..2147483647] and the input base is 10,
                * cutoff will be set to 214748364 and cutlim to either
                * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
                * a value > 214748364, or equal but the next digit is > 7 (or 8),
                * the number is too big, and we will return a range error.
                *
                * Set any if any `digits' consumed; make it negative to indicate
                * overflow.
                */
                cutoff = neg ? -(ullong_type)LLONG_MIN : LLONG_MAX;
                cutlim = cutoff % (ullong_type)base;
                cutoff /= (ullong_type)base;
                for (acc = 0, any = 0;; c = *s++) {
                    if (isdigit(c))
                        c -= '0';
                    else if (isalpha(c))
                        c -= isupper(c) ? 'A' - 10 : 'a' - 10;
                    else
                        break;
                    if (c >= base)
                        break;
                    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
                        any = -1;
                    else {
                        any = 1;
                        acc *= base;
                        acc += c;
                    }
                }
                if (any < 0) {
                    acc = neg ? LLONG_MIN : LLONG_MAX;
                    // errno = ERANGE;
                } else if (neg)
                    acc = -acc;
                if (endptr != 0)
                    *endptr = (char *) (any ? s - 1 : nptr);
                return (acc);
            }

            static ullong_type strtoull(const char *nptr, char **endptr, register int base)
            {

                // https://github.com/gcc-mirror/gcc/blob/master/libiberty/strtoull.c
                // Copyright (c) 2014 Regents of the University of California.

                register const char *s = nptr;
                register ullong_type acc;
                register int c;
                register ullong_type cutoff;
                register int neg = 0, any, cutlim;

                /*
                * See strtol for comments as to the logic used.
                */
                do {
                    c = *s++;
                } while (isspace(c));
                if (c == '-') {
                    neg = 1;
                    c = *s++;
                } else if (c == '+')
                    c = *s++;
                if ((base == 0 || base == 16) &&
                    c == '0' && (*s == 'x' || *s == 'X')) {
                    c = s[1];
                    s += 2;
                    base = 16;
                }
                if (base == 0)
                    base = c == '0' ? 8 : 10;
                cutoff = (ullong_type)ULLONG_MAX / (ullong_type)base;
                cutlim = (ullong_type)ULLONG_MAX % (ullong_type)base;
                for (acc = 0, any = 0;; c = *s++) {
                    if (isdigit(c))
                        c -= '0';
                    else if (isalpha(c))
                        c -= isupper(c) ? 'A' - 10 : 'a' - 10;
                    else
                        break;
                    if (c >= base)
                        break;
                    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
                        any = -1;
                    else {
                        any = 1;
                        acc *= base;
                        acc += c;
                    }
                }
                if (any < 0) {
                    acc = ULLONG_MAX;
                    // errno = ERANGE;
                } else if (neg)
                    acc = -acc;
                if (endptr != 0)
                    *endptr = (char *) (any ? s - 1 : nptr);
                return (acc);
            }

    #endif

};

}

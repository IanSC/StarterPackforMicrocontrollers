#pragma once

// #include <Arduino.h>
// #include <stdint.h>
// #include <stdarg.h>
#include <string.h>

namespace StarterPack { namespace Str {

    bool findCharacter( uint8_t key, const char *list ) {
    // bool isCharInString( uint8_t key, const char *list ) {
        if ( list == nullptr ) return false;
        return ( strchr( list, key ) != nullptr );
    }

    void sortContents(char *str) {
    // void sortCharsInString(char *str) {
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

    bool matchUnorderedSimple(const char *str1, const char *str2) {
    // bool areCharsInStringSimple(const char *str1, const char *str2) {
        // repetition not handled
        // will match "ABB" and "BAA"
        auto L1 = strlen(str1);
        auto L2 = strlen(str2);
        if (L1 != L2) return false;
        for (size_t i=0; i<L1; i++) {
            if (!findCharacter(str1[i],str2))
                return false;
        }
        return true;
    }

    // bool areCharsInString(const char *str1, const char *str2) {
    bool matchUnordered(const char *str1, const char *str2) {
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

}}

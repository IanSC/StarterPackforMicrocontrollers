#pragma once

// https://stackoverflow.com/a/15889501
#define ENUM_MANIPULATION_CORE(enumType,friend) \
    inline friend enumType operator | ( enumType a, enumType b ) { \
        return static_cast<enumType>( static_cast<int>(a) | static_cast<int>(b) ); } \
    inline friend enumType operator & ( enumType a, enumType b ) { \
        return static_cast<enumType>( static_cast<int>(a) & static_cast<int>(b) ); } \
    inline friend enumType& operator |= ( enumType& a, enumType b ) { \
        a = a | b; return a; } \
    inline friend enumType& operator &= ( enumType& a, enumType b ) { \
        a = a & b; return a; }

// use this inside class, with benefits
// class {
//     enum eType { hello, there };
//     ENUM_MANIPULATION(eType,friend) 
// }
#define CLASS_ENUM_MANIPULATION(enumType) \
    ENUM_MANIPULATION_CORE(enumType,friend)

// use this outside class
//     enum eType { hello, there };
//     ENUM_MANIPULATION(eType)
#define ENUM_MANIPULATION(enumType) \
    ENUM_MANIPULATION_CORE(enumType,)

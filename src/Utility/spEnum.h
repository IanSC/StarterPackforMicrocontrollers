#pragma once

// #include <type_traits> // std::underlying_type

//
// ENUM OPERATION
//

    // enum option { A,B,C };
    // .. option::A | option::B

    // https://stackoverflow.com/a/15889501
    /*
    #define ENUM_MANIPULATION_CORE(enumType,friend) \
        inline friend enumType operator | ( enumType a, enumType b ) { \
            return static_cast<enumType>( static_cast<std::underlying_type<enumType>::type>(a) | static_cast<std::underlying_type<enumType>::type>(b) ); } \
        inline friend enumType operator & ( enumType a, enumType b ) { \
            return static_cast<enumType>( static_cast<std::underlying_type<enumType>::type>(a) & static_cast<std::underlying_type<enumType>::type>(b) ); } \
        inline friend enumType& operator |= ( enumType& a, enumType b ) { \
            a = a | b; return a; } \
        inline friend enumType& operator &= ( enumType& a, enumType b ) { \
            a = a & b; return a; }
    */
    #define ENUM_MANIPULATION_CORE(enumType,friend) \
        inline friend enumType operator | ( enumType a, enumType b ) { \
            return static_cast<enumType>( static_cast<int>(a) | static_cast<int>(b) ); } \
        inline friend enumType operator & ( enumType a, enumType b ) { \
            return static_cast<enumType>( static_cast<int>(a) & static_cast<int>(b) ); } \
        inline friend enumType& operator |= ( enumType& a, enumType b ) { \
            a = a | b; return a; } \
        inline friend enumType& operator &= ( enumType& a, enumType b ) { \
            a = a & b; return a; } \
        inline friend enumType operator ~ ( enumType a ) { \
            return static_cast<enumType>( ~static_cast<int>(a) ); } \
        inline friend bool operator == ( enumType a, int b ) { \
            return ( static_cast<int>(a) == b ); } \
        inline friend bool operator != ( enumType a, int b ) { \
            return ( static_cast<int>(a) != b ); }

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

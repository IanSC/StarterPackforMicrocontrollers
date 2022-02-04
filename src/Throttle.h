//  2022/01 - isc
//
//  Throttle
//  --------
//  - throttle executing a function
//    to avoid overwhelming resources
//
//  To Use
//
//      Throttle( &function, throttleTimeInMs )
//
//          supported functions:
//              void FUNC()
//              bool FUNC()
//
//      update()      run function every throttleTimeInMs only
//      updateNow()   run function immediately
//
//  Static Version
//
//      - helper for wrapping callbacks without creating instances
//      - can only be used one at a time
//        ex. not allowed:
//            execute( Throttle::Me( &process1, 100 ), Throttle::Me( &process2, 100 ) );
//
//      void process() {
//          // ...
//      }
//      void execute( voidVoidFunction func ) {
//          while( true ) {
//             // ... do stuff, break
//             if ( func != nullptr ) func();    
//          }
//      }
//      void loop() {
//          execute( &process );                      // process() gets called every iteration
//          execute( Throttle::Me( &process, 100 ) ); // process() gets called every 100ms
//      }
//
//  Example
//
//      int counter = 0;
//      void process() {
//          lcd.setCursor( 0, 0 );
//          lcd.print( counter++ );
//          int v = analogRead(A0);
//          lcd.setCursor( 0, 1 );
//          lcd.print( v );
//      }
//
//      Throttle tt = Throttle( &process, 1000 );
//
//      void setup() {
//          tt.updateNow();
//      }
//      void loop() {
//          // process() will get called every 1000 ms only
//          tt.update();
//      }

#pragma once
#include <Arduino.h>
#include <stdint.h>

namespace StarterPack {

class Throttle {

    // applicable, can't get it to work ???
    // https://stackoverflow.com/questions/9625526/check-at-compile-time-if-template-argument-is-void
    
    // typedef void  (*voidVoidFunction)  (void );  // void  function( void  )
    // typedef void  (*voidBoolFunction)  (bool );  // void  function( bool  )
    // typedef void  (*voidIntFunction)   (int  );  // void  function( int   )
    // typedef void  (*voidFloatFunction) (float);  // void  function( float ) 

    typedef void (*_void_void_F) (void    );
    typedef void (*_void_bool_F) (bool    );
    typedef void (*_void_si08_F) (int8_t  );
    typedef void (*_void_ui08_F) (uint8_t );
    typedef void (*_void_si16_F) (int16_t );
    typedef void (*_void_ui16_F) (uint16_t);
    typedef void (*_void_si32_F) (int32_t );
    typedef void (*_void_ui32_F) (uint32_t);
    typedef void (*_void_flot_F) (float   );
    typedef void (*_void_doob_F) (double  );

    typedef bool (*_bool_void_F) (void    );
    typedef bool (*_bool_bool_F) (bool    );
    typedef bool (*_bool_si08_F) (int8_t  );
    typedef bool (*_bool_ui08_F) (uint8_t );
    typedef bool (*_bool_si16_F) (int16_t );
    typedef bool (*_bool_ui16_F) (uint16_t);
    typedef bool (*_bool_si32_F) (int32_t );
    typedef bool (*_bool_ui32_F) (uint32_t);
    typedef bool (*_bool_flot_F) (float   );
    typedef bool (*_bool_doob_F) (double  );

    typedef int8_t (*_si08_void_F) (void    );
    typedef int8_t (*_si08_bool_F) (bool    );
    typedef int8_t (*_si08_si08_F) (int8_t  );
    typedef int8_t (*_si08_ui08_F) (uint8_t );
    typedef int8_t (*_si08_si16_F) (int16_t );
    typedef int8_t (*_si08_ui16_F) (uint16_t);
    typedef int8_t (*_si08_si32_F) (int32_t );
    typedef int8_t (*_si08_ui32_F) (uint32_t);
    typedef int8_t (*_si08_flot_F) (float   );
    typedef int8_t (*_si08_doob_F) (double  );

    typedef uint8_t (*_ui08_void_F) (void    );
    typedef uint8_t (*_ui08_bool_F) (bool    );
    typedef uint8_t (*_ui08_si08_F) (int8_t  );
    typedef uint8_t (*_ui08_ui08_F) (uint8_t );
    typedef uint8_t (*_ui08_si16_F) (int16_t );
    typedef uint8_t (*_ui08_ui16_F) (uint16_t);
    typedef uint8_t (*_ui08_si32_F) (int32_t );
    typedef uint8_t (*_ui08_ui32_F) (uint32_t);
    typedef uint8_t (*_ui08_flot_F) (float   );
    typedef uint8_t (*_ui08_doob_F) (double  );

    typedef int16_t (*_si16_void_F) (void    );
    typedef int16_t (*_si16_bool_F) (bool    );
    typedef int16_t (*_si16_si08_F) (int8_t  );
    typedef int16_t (*_si16_ui08_F) (uint8_t );
    typedef int16_t (*_si16_si16_F) (int16_t );
    typedef int16_t (*_si16_ui16_F) (uint16_t);
    typedef int16_t (*_si16_si32_F) (int32_t );
    typedef int16_t (*_si16_ui32_F) (uint32_t);
    typedef int16_t (*_si16_flot_F) (float   );
    typedef int16_t (*_si16_doob_F) (double  );

    typedef uint16_t (*_ui16_void_F) (void    );
    typedef uint16_t (*_ui16_bool_F) (bool    );
    typedef uint16_t (*_ui16_si08_F) (int8_t  );
    typedef uint16_t (*_ui16_ui08_F) (uint8_t );
    typedef uint16_t (*_ui16_si16_F) (int16_t );
    typedef uint16_t (*_ui16_ui16_F) (uint16_t);
    typedef uint16_t (*_ui16_si32_F) (int32_t );
    typedef uint16_t (*_ui16_ui32_F) (uint32_t);
    typedef uint16_t (*_ui16_flot_F) (float   );
    typedef uint16_t (*_ui16_doob_F) (double  );

    typedef int32_t (*_si32_void_F) (void    );
    typedef int32_t (*_si32_bool_F) (bool    );
    typedef int32_t (*_si32_si08_F) (int8_t  );
    typedef int32_t (*_si32_ui08_F) (uint8_t );
    typedef int32_t (*_si32_si16_F) (int16_t );
    typedef int32_t (*_si32_ui16_F) (uint16_t);
    typedef int32_t (*_si32_si32_F) (int32_t );
    typedef int32_t (*_si32_ui32_F) (uint32_t);
    typedef int32_t (*_si32_flot_F) (float   );
    typedef int32_t (*_si32_doob_F) (double  );

    typedef uint32_t (*_ui32_void_F) (void    );
    typedef uint32_t (*_ui32_bool_F) (bool    );
    typedef uint32_t (*_ui32_si08_F) (int8_t  );
    typedef uint32_t (*_ui32_ui08_F) (uint8_t );
    typedef uint32_t (*_ui32_si16_F) (int16_t );
    typedef uint32_t (*_ui32_ui16_F) (uint16_t);
    typedef uint32_t (*_ui32_si32_F) (int32_t );
    typedef uint32_t (*_ui32_ui32_F) (uint32_t);
    typedef uint32_t (*_ui32_flot_F) (float   );
    typedef uint32_t (*_ui32_doob_F) (double  );

    typedef float (*_flot_void_F) (void    );
    typedef float (*_flot_bool_F) (bool    );
    typedef float (*_flot_si08_F) (int8_t  );
    typedef float (*_flot_ui08_F) (uint8_t );
    typedef float (*_flot_si16_F) (int16_t );
    typedef float (*_flot_ui16_F) (uint16_t);
    typedef float (*_flot_si32_F) (int32_t );
    typedef float (*_flot_ui32_F) (uint32_t);
    typedef float (*_flot_flot_F) (float   );
    typedef float (*_flot_doob_F) (double  );

    typedef double (*_doob_void_F) (void    );
    typedef double (*_doob_bool_F) (bool    );
    typedef double (*_doob_si08_F) (int8_t  );
    typedef double (*_doob_ui08_F) (uint8_t );
    typedef double (*_doob_si16_F) (int16_t );
    typedef double (*_doob_ui16_F) (uint16_t);
    typedef double (*_doob_si32_F) (int32_t );
    typedef double (*_doob_ui32_F) (uint32_t);
    typedef double (*_doob_flot_F) (float   );
    typedef double (*_doob_doob_F) (double  );

    // typedef bool  (*boolVoidFunction)  (void );  // bool  function( void  )
    // typedef bool  (*boolBoolFunction)  (bool );  // bool  function( bool  )
    // typedef bool  (*boolIntFunction)   (int  );  // bool  function( int   )
    // typedef bool  (*boolFloatFunction) (float);  // bool  function( float )

    // typedef int   (*intVoidFunction)   (void );  // int   function( void  )
    // typedef int   (*intBoolFunction)   (bool );  // int   function( bool  )
    // typedef int   (*intIntFunction)    (int  );  // int   function( int   )
    // typedef int   (*intFloatFunction)  (float);  // int   function( float )

    // typedef float (*floatVoidFunction) (void );  // float function( void  )
    // typedef float (*floatBoolFunction) (bool );  // float function( bool  )
    // typedef float (*floatIntFunction)  (int  );  // float function( int   )
    // typedef float (*floatFloatFunction)(float);  // float function( float )

    inline Throttle( uint16_t throttleInMs = 100 ) {
        this->throttleInMs = throttleInMs;
        lastUpdateTime = millis() - throttleInMs;
    }

    enum paramType {
        _void,
        _bool,
        _si08,
        _ui08,
        _si16,
        _ui16,
        _si32,
        _ui32,
        _flot,
        _doob // double, get it?
    };

    paramType inputType;
    paramType outputType;

    // enum functionType {
    //     voidVoid  ,
    //     voidBool  ,
    //     voidInt8  ,
    //     voidUInt8 ,
    //     voidInt16 ,
    //     voidUInt16,
    //     voidInt32 ,
    //     voidUInt32,
    //     voidFloat ,
    //     voidDouble,


    //     /*voidVoid ,*/ boolVoid , intVoid , floatVoid ,
    //     /*voidBool ,*/ boolBool , intBool , floatBool ,
    //     /*voidInt  ,*/ boolInt  , intInt  , floatInt  ,
    //     /*voidFloat,*/ boolFloat, intFloat, floatFloat
    // };

    void *ptr = nullptr;
    // functionType fType;
    uint32_t lastUpdateTime;

    // union UnionType {
    //     bool     vBool;
    //     int8_t   vInt8;
    //     uint8_t  vUInt8;
    //     int16_t  vInt16;
    //     uint16_t vUInt16;
    //     int32_t  vInt32;
    //     uint32_t vUInt32;
    //     float    vFloat;
    //     double   vDouble;
    // };
    // UnionType inputValue;
    // UnionType outputValue;

public:

    uint16_t throttleInMs = 100;

    // #define CONS(fPrototype,fKey) \
    //     Throttle( fPrototype f, uint16_t throttleInMs = 100 ) : Throttle(throttleInMs) { \
    //         this->ptr = (void*) f; \
    //         fType = functionType::fKey; }

    // // CONS( voidVoidFunction,   voidVoid   )
    // // CONS( voidBoolFunction,   voidBool   )
    // // CONS( voidIntFunction,    voidInt    )
    // // CONS( voidFloatFunction,  voidFloat  )

    // CONS( voidVoidFunction  , voidVoid   )
    // CONS( voidBoolFunction  , voidBool   )
    // CONS( voidInt8Function  , voidInt8   )
    // CONS( voidUInt8Function , voidUInt8  )
    // CONS( voidInt16Function , voidInt16  )
    // CONS( voidUInt16Function, voidUInt16 )
    // CONS( voidInt32Function , voidInt32  )
    // CONS( voidUInt32Function, voidUInt32 )
    // CONS( voidFloatFunction , voidFloat  )
    // CONS( voidDoubleFunction, voidDouble )

    // CONS( boolVoidFunction,   boolVoid   )
    // CONS( boolBoolFunction,   boolBool   )
    // CONS( boolIntFunction,    boolInt    )
    // CONS( boolFloatFunction,  boolFloat  )

    // CONS( intVoidFunction,    intVoid    )
    // CONS( intBoolFunction,    intBool    )
    // CONS( intIntFunction,     intInt     )
    // CONS( intFloatFunction,   intFloat   )

    // CONS( floatVoidFunction,  floatVoid  )
    // CONS( floatBoolFunction,  floatBool  )
    // CONS( floatIntFunction,   floatInt   )
    // CONS( floatFloatFunction, floatFloat )

    // #undef CONS

    #define CONS( out, in ) \
        Throttle( out ## in ## _F f, uint16_t throttleInMs = 100 ) : Throttle( throttleInMs ) { \
            this->ptr = (void*) f; \
            inputType = paramType::in; \
            outputType = paramType::out; \
        }

    CONS( _void, _void ) CONS( _bool, _void )
    CONS( _void, _bool ) CONS( _bool, _bool )
    CONS( _void, _si08 ) CONS( _bool, _si08 )
    CONS( _void, _ui08 ) CONS( _bool, _ui08 )
    CONS( _void, _si16 ) CONS( _bool, _si16 )
    CONS( _void, _ui16 ) CONS( _bool, _ui16 )
    CONS( _void, _si32 ) CONS( _bool, _si32 )
    CONS( _void, _ui32 ) CONS( _bool, _ui32 )
    CONS( _void, _flot ) CONS( _bool, _flot )
    CONS( _void, _doob ) CONS( _bool, _doob )

    CONS( _si08, _void ) CONS( _ui08, _void )
    CONS( _si08, _bool ) CONS( _ui08, _bool )
    CONS( _si08, _si08 ) CONS( _ui08, _si08 )
    CONS( _si08, _ui08 ) CONS( _ui08, _ui08 )
    CONS( _si08, _si16 ) CONS( _ui08, _si16 )
    CONS( _si08, _ui16 ) CONS( _ui08, _ui16 )
    CONS( _si08, _si32 ) CONS( _ui08, _si32 )
    CONS( _si08, _ui32 ) CONS( _ui08, _ui32 )
    CONS( _si08, _flot ) CONS( _ui08, _flot )
    CONS( _si08, _doob ) CONS( _ui08, _doob )

    CONS( _si16, _void ) CONS( _ui16, _void )
    CONS( _si16, _bool ) CONS( _ui16, _bool )
    CONS( _si16, _si08 ) CONS( _ui16, _si08 )
    CONS( _si16, _ui08 ) CONS( _ui16, _ui08 )
    CONS( _si16, _si16 ) CONS( _ui16, _si16 )
    CONS( _si16, _ui16 ) CONS( _ui16, _ui16 )
    CONS( _si16, _si32 ) CONS( _ui16, _si32 )
    CONS( _si16, _ui32 ) CONS( _ui16, _ui32 )
    CONS( _si16, _flot ) CONS( _ui16, _flot )
    CONS( _si16, _doob ) CONS( _ui16, _doob )

    CONS( _si32, _void ) CONS( _ui32, _void )
    CONS( _si32, _bool ) CONS( _ui32, _bool )
    CONS( _si32, _si08 ) CONS( _ui32, _si08 )
    CONS( _si32, _ui08 ) CONS( _ui32, _ui08 )
    CONS( _si32, _si16 ) CONS( _ui32, _si16 )
    CONS( _si32, _ui16 ) CONS( _ui32, _ui16 )
    CONS( _si32, _si32 ) CONS( _ui32, _si32 )
    CONS( _si32, _ui32 ) CONS( _ui32, _ui32 )
    CONS( _si32, _flot ) CONS( _ui32, _flot )
    CONS( _si32, _doob ) CONS( _ui32, _doob )

    CONS( _flot, _void ) CONS( _doob, _void )
    CONS( _flot, _bool ) CONS( _doob, _bool )
    CONS( _flot, _si08 ) CONS( _doob, _si08 )
    CONS( _flot, _ui08 ) CONS( _doob, _ui08 )
    CONS( _flot, _si16 ) CONS( _doob, _si16 )
    CONS( _flot, _ui16 ) CONS( _doob, _ui16 )
    CONS( _flot, _si32 ) CONS( _doob, _si32 )
    CONS( _flot, _ui32 ) CONS( _doob, _ui32 )
    CONS( _flot, _flot ) CONS( _doob, _flot )
    CONS( _flot, _doob ) CONS( _doob, _doob )

    #undef CONS

    //
    // VOID --> VOID
    //

    void trigger() {
        uint32_t now = millis();
        if ( now - lastUpdateTime < throttleInMs ) return;
        lastUpdateTime = now;
        if ( ptr != nullptr && inputType == _void ) {
            // possible to cast as void output
            // then ignore result ???
            switch( outputType ) {
            case _void: ((_void_void_F) ptr)(); break;
            case _bool: ((_bool_void_F) ptr)(); break;
            case _si08: ((_si08_void_F) ptr)(); break;
            case _ui08: ((_ui08_void_F) ptr)(); break;
            case _si16: ((_si16_void_F) ptr)(); break;
            case _ui16: ((_ui16_void_F) ptr)(); break;
            case _si32: ((_si32_void_F) ptr)(); break;
            case _ui32: ((_ui32_void_F) ptr)(); break;
            case _flot: ((_flot_void_F) ptr)(); break;
            case _doob: ((_doob_void_F) ptr)(); break;
            default: break;
            }
        }
    }

    // void trigger() {
    //     uint32_t now = millis();
    //     if ( now - lastUpdateTime >= throttleInMs )
    //         triggerNow();
    // }

    // void triggerNow() {
    //     lastUpdateTime = millis();
    //     if ( ptr != nullptr ) {
    //         if ( inputType == _void ) {
    //             // possible to cast as void output
    //             // then ignore result ???
    //             switch( outputType ) {
    //             case _void: ((_void_void_F) ptr)(); break;
    //             case _bool: ((_bool_void_F) ptr)(); break;
    //             case _si08: ((_si08_void_F) ptr)(); break;
    //             case _ui08: ((_ui08_void_F) ptr)(); break;
    //             case _si16: ((_si16_void_F) ptr)(); break;
    //             case _ui16: ((_ui16_void_F) ptr)(); break;
    //             case _si32: ((_si32_void_F) ptr)(); break;
    //             case _ui32: ((_ui32_void_F) ptr)(); break;
    //             case _flot: ((_flot_void_F) ptr)(); break;
    //             case _doob: ((_doob_void_F) ptr)(); break;
    //             default: break;
    //             }
    //         }
    //     }
    // }

    //
    // INPUT --> VOID / IGNORE
    //

    // template<typename IN>
    // void trigger( IN input ) {
    //     uint32_t now = millis();
    //     if ( now - lastUpdateTime >= throttleInMs )
    //         triggerNow( input );
    // }

    #define M( in, dType ) \
        void trigger( dType input ) { \
            uint32_t now = millis(); \
            if ( now - lastUpdateTime < throttleInMs ) return; \
            lastUpdateTime = millis(); \
            if ( ptr == nullptr || inputType != in ) return; \
            switch( outputType ) { \
            case _void: ((_void ## in ## _F) ptr)(input); break; \
            case _bool: ((_bool ## in ## _F) ptr)(input); break; \
            case _si08: ((_si08 ## in ## _F) ptr)(input); break; \
            case _ui08: ((_ui08 ## in ## _F) ptr)(input); break; \
            case _si16: ((_si16 ## in ## _F) ptr)(input); break; \
            case _ui16: ((_ui16 ## in ## _F) ptr)(input); break; \
            case _si32: ((_si32 ## in ## _F) ptr)(input); break; \
            case _ui32: ((_ui32 ## in ## _F) ptr)(input); break; \
            case _flot: ((_flot ## in ## _F) ptr)(input); break; \
            case _doob: ((_doob ## in ## _F) ptr)(input); break; \
            } \
        }
    M( _bool, bool     )
    M( _si08, int8_t   )
    M( _ui08, uint8_t  )
    M( _si16, int16_t  )
    M( _ui16, uint16_t )
    M( _si32, int32_t  )
    M( _ui32, uint32_t )
    M( _flot, float    )
    M( _doob, double   )
    #undef M

    /*
    #define M( in ) \
        switch( outputType ) { \
        case _void: ((_void ## in ## _F) ptr)(input); break; \
        case _bool: ((_bool ## in ## _F) ptr)(input); break; \
        case _si08: ((_si08 ## in ## _F) ptr)(input); break; \
        case _ui08: ((_ui08 ## in ## _F) ptr)(input); break; \
        case _si16: ((_si16 ## in ## _F) ptr)(input); break; \
        case _ui16: ((_ui16 ## in ## _F) ptr)(input); break; \
        case _si32: ((_si32 ## in ## _F) ptr)(input); break; \
        case _ui32: ((_ui32 ## in ## _F) ptr)(input); break; \
        case _flot: ((_flot ## in ## _F) ptr)(input); break; \
        case _doob: ((_doob ## in ## _F) ptr)(input); break; \
        } break;

    template<typename IN>
    void triggerNow( IN input ) {
        lastUpdateTime = millis();
        if ( ptr != nullptr ) {
            switch( inputType ) {
            case _bool: M(_bool)
            case _si08: M(_si08)
            case _ui08: M(_ui08)
            case _si16: M(_si16)
            case _ui16: M(_ui16)
            case _si32: M(_si32)
            case _ui32: M(_ui32)
            case _flot: M(_flot)
            case _doob: M(_doob)
            }
        }
    }
    #undef M
*/

    //
    // VOID --> OUTPUT
    //

    #define M( out, dType ) \
        bool triggerCheck( dType &output ) { \
            uint32_t now = millis(); \
            if ( now - lastUpdateTime >= throttleInMs ) { \
                lastUpdateTime = millis(); \
                if ( ptr != nullptr && outputType == out && inputType == _void ) { \
                    output = ((out ## _void_F) ptr)(); \
                    return true; \
                } \
            } \
            return false; \
        }
    M( _bool, bool     )
    M( _si08, int8_t   )
    M( _ui08, uint8_t  )
    M( _si16, int16_t  )
    M( _ui16, uint16_t )
    M( _si32, int32_t  )
    M( _ui32, uint32_t )
    M( _flot, float    )
    M( _doob, double   )
    #undef M

    //
    // INPUT --> OUTPUT
    //

    #define M( out, odType, in, idType ) \
        bool triggerCheck( odType &output, idType input ) { \
            uint32_t now = millis(); \
            if ( now - lastUpdateTime >= throttleInMs ) { \
                lastUpdateTime = millis(); \
                if ( ptr != nullptr && outputType == out && inputType == in ) { \
                    output = ((out ## in ## _F) ptr)( input ); \
                    return true; \
                } \
            } \
            return false; \
        }

    M( _bool, bool, _bool, bool     )
    M( _bool, bool, _si08, int8_t   )
    M( _bool, bool, _ui08, uint8_t  )
    M( _bool, bool, _si16, int16_t  )
    M( _bool, bool, _ui16, uint16_t )
    M( _bool, bool, _si32, int32_t  )
    M( _bool, bool, _ui32, uint32_t )
    M( _bool, bool, _flot, float    )
    M( _bool, bool, _doob, double   )

    M( _si08, int8_t, _bool, bool     ) M( _ui08, uint8_t, _bool, bool     )
    M( _si08, int8_t, _si08, int8_t   ) M( _ui08, uint8_t, _si08, int8_t   )
    M( _si08, int8_t, _ui08, uint8_t  ) M( _ui08, uint8_t, _ui08, uint8_t  )
    M( _si08, int8_t, _si16, int16_t  ) M( _ui08, uint8_t, _si16, int16_t  )
    M( _si08, int8_t, _ui16, uint16_t ) M( _ui08, uint8_t, _ui16, uint16_t )
    M( _si08, int8_t, _si32, int32_t  ) M( _ui08, uint8_t, _si32, int32_t  )
    M( _si08, int8_t, _ui32, uint32_t ) M( _ui08, uint8_t, _ui32, uint32_t )
    M( _si08, int8_t, _flot, float    ) M( _ui08, uint8_t, _flot, float    )
    M( _si08, int8_t, _doob, double   ) M( _ui08, uint8_t, _doob, double   )

    M( _si16, int16_t, _bool, bool     ) M( _ui16, uint16_t, _bool, bool     )
    M( _si16, int16_t, _si08, int8_t   ) M( _ui16, uint16_t, _si08, int8_t   )
    M( _si16, int16_t, _ui08, uint8_t  ) M( _ui16, uint16_t, _ui08, uint8_t  )
    M( _si16, int16_t, _si16, int16_t  ) M( _ui16, uint16_t, _si16, int16_t  )
    M( _si16, int16_t, _ui16, uint16_t ) M( _ui16, uint16_t, _ui16, uint16_t )
    M( _si16, int16_t, _si32, int32_t  ) M( _ui16, uint16_t, _si32, int32_t  )
    M( _si16, int16_t, _ui32, uint32_t ) M( _ui16, uint16_t, _ui32, uint32_t )
    M( _si16, int16_t, _flot, float    ) M( _ui16, uint16_t, _flot, float    )
    M( _si16, int16_t, _doob, double   ) M( _ui16, uint16_t, _doob, double   )

    M( _si32, int32_t, _bool, bool     ) M( _ui32, uint32_t, _bool, bool     )
    M( _si32, int32_t, _si08, int8_t   ) M( _ui32, uint32_t, _si08, int8_t   )
    M( _si32, int32_t, _ui08, uint8_t  ) M( _ui32, uint32_t, _ui08, uint8_t  )
    M( _si32, int32_t, _si16, int16_t  ) M( _ui32, uint32_t, _si16, int16_t  )
    M( _si32, int32_t, _ui16, uint16_t ) M( _ui32, uint32_t, _ui16, uint16_t )
    M( _si32, int32_t, _si32, int32_t  ) M( _ui32, uint32_t, _si32, int32_t  )
    M( _si32, int32_t, _ui32, uint32_t ) M( _ui32, uint32_t, _ui32, uint32_t )
    M( _si32, int32_t, _flot, float    ) M( _ui32, uint32_t, _flot, float    )
    M( _si32, int32_t, _doob, double   ) M( _ui32, uint32_t, _doob, double   )

    M( _flot, float, _bool, bool     ) M( _doob, double, _bool, bool     )
    M( _flot, float, _si08, int8_t   ) M( _doob, double, _si08, int8_t   )
    M( _flot, float, _ui08, uint8_t  ) M( _doob, double, _ui08, uint8_t  )
    M( _flot, float, _si16, int16_t  ) M( _doob, double, _si16, int16_t  )
    M( _flot, float, _ui16, uint16_t ) M( _doob, double, _ui16, uint16_t )
    M( _flot, float, _si32, int32_t  ) M( _doob, double, _si32, int32_t  )
    M( _flot, float, _ui32, uint32_t ) M( _doob, double, _ui32, uint32_t )
    M( _flot, float, _flot, float    ) M( _doob, double, _flot, float    )
    M( _flot, float, _doob, double   ) M( _doob, double, _doob, double   )

    #undef M

/*


    void triggerNow() {
        lastUpdateTime = millis();
        if ( ptr != nullptr ) {
            switch( fType ) {
            case voidVoid:  ((voidVoidFunction)  ptr)(); break;
            case boolVoid:  ((boolVoidFunction)  ptr)(); break;
            case intVoid:   ((intVoidFunction)   ptr)(); break;
            case floatVoid: ((floatVoidFunction) ptr)(); break;
            default: break;
            }
        }
    }

    template<typename IN>
    void trigger( IN value ) {
        uint32_t now = millis();
        if ( now - lastUpdateTime >= throttleInMs )
            triggerNow( value );
    }
    
    template<typename IN>
    void triggerNow( IN value ) {
        lastUpdateTime = millis();
        if ( ptr != nullptr ) {
            switch( fType ) {
            // case voidVoid:   ((voidVoidFunction)   ptr)(); break;
            // case voidBool:   ((voidBoolFunction)   ptr)( value ); break;
            // case voidInt:    ((voidIntFunction)    ptr)( value ); break;
            // case voidFloat:  ((voidFloatFunction)  ptr)( value ); break;


            // case voidVoid  :  ((voidVoidFunction  ) ptr)( value ); break;
            case voidBool  :  ((voidBoolFunction  ) ptr)( value ); break;
            case voidInt8  :  ((voidInt8Function  ) ptr)( value ); break;
            case voidUInt8 :  ((voidUInt8Function ) ptr)( value ); break;
            case voidInt16 :  ((voidInt16Function ) ptr)( value ); break;
            case voidUInt16:  ((voidUInt16Function) ptr)( value ); break;
            case voidInt32 :  ((voidInt32Function ) ptr)( value ); break;
            case voidUInt32:  ((voidUInt32Function) ptr)( value ); break;
            case voidFloat :  ((voidFloatFunction ) ptr)( value ); break;
            case voidDouble:  ((voidDoubleFunction) ptr)( value ); break;









            // case boolVoid:   ((boolVoidFunction)   ptr)(); break;
            case boolBool:   ((boolBoolFunction)   ptr)( value ); break;
            case boolInt:    ((boolIntFunction)    ptr)( value ); break;
            case boolFloat:  ((boolFloatFunction)  ptr)( value ); break;
            // case intVoid:    ((intVoidFunction)    ptr)(); break;
            case intBool:    ((intBoolFunction)    ptr)( value ); break;
            case intInt:     ((intIntFunction)     ptr)( value ); break;
            case intFloat:   ((intFloatFunction)   ptr)( value ); break;
            // case floatVoid:  ((floatVoidFunction)  ptr)(); break;
            case floatBool:  ((floatBoolFunction)  ptr)( value ); break;
            case floatInt:   ((floatIntFunction)   ptr)( value ); break;
            case floatFloat: ((floatFloatFunction) ptr)( value ); break;
            default: break;
            }
        }
    }

    //
    // BOOL
    //

    bool triggerBool() {
        uint32_t now = millis();
        if ( now - lastUpdateTime >= throttleInMs )
            return triggerBoolNow();
        return false;
    }

    bool triggerBoolNow() {
        lastUpdateTime = millis();
        if ( ptr != nullptr ) {
            switch( fType ) {
            case boolVoid:  return ((boolVoidFunction)  ptr)();
            case intVoid:   return ((intVoidFunction)   ptr)();
            case floatVoid: return ((floatVoidFunction) ptr)();
            default: break;
            }
        }
        return false;
    }

    template<typename IN>
    bool triggerBool( IN value ) {
        uint32_t now = millis();
        if ( now - lastUpdateTime >= throttleInMs )
            return triggerBoolNow( value );
        return false;
    }

    template<typename IN>
    bool triggerBoolNow( IN value ) {
        lastUpdateTime = millis();
        if ( ptr != nullptr ) {
            switch( fType ) {
            // case voidVoid:   ((voidVoidFunction)   ptr)();
            // case voidBool:   ((voidBoolFunction)   ptr)( value );
            // case voidInt:    ((voidIntFunction)    ptr)( value );
            // case voidFloat:  ((voidFloatFunction)  ptr)( value );
            // case boolVoid:   return ((boolVoidFunction)   ptr)();
            case boolBool:   return ((boolBoolFunction)   ptr)( value );
            case boolInt:    return ((boolIntFunction)    ptr)( value );
            case boolFloat:  return ((boolFloatFunction)  ptr)( value );
            // case intVoid:    return ((intVoidFunction)    ptr)();
            case intBool:    return ((intBoolFunction)    ptr)( value );
            case intInt:     return ((intIntFunction)     ptr)( value );
            case intFloat:   return ((intFloatFunction)   ptr)( value );
            // case floatVoid:  return ((floatVoidFunction)  ptr)();
            case floatBool:  return ((floatBoolFunction)  ptr)( value );
            case floatInt:   return ((floatIntFunction)   ptr)( value );
            case floatFloat: return ((floatFloatFunction) ptr)( value );
            default: break;
            }
        }
        return false;
    }

    //
    // INTEGER
    //

    int triggerInt() {
        uint32_t now = millis();
        if ( now - lastUpdateTime >= throttleInMs )
            return triggerIntNow();
        return 0;
    }

    int triggerIntNow() {
        lastUpdateTime = millis();
        if ( ptr != nullptr ) {
            switch( fType ) {
            case boolVoid:  return ((boolVoidFunction)  ptr)();
            case intVoid:   return ((intVoidFunction)   ptr)();
            case floatVoid: return ((floatVoidFunction) ptr)();
            default: break;
            }
        }
        return 0;
    }

    template<typename IN>
    int triggerInt( IN value ) {
        uint32_t now = millis();
        if ( now - lastUpdateTime >= throttleInMs )
            return triggerIntNow( value );
        return 0;
    }

    template<typename IN>
    int triggerIntNow( IN value ) {
        lastUpdateTime = millis();
        if ( ptr != nullptr ) {
            switch( fType ) {
            // case voidVoid:   ((voidVoidFunction)   ptr)();
            // case voidBool:   ((voidBoolFunction)   ptr)( value );
            // case voidInt:    ((voidIntFunction)    ptr)( value );
            // case voidFloat:  ((voidFloatFunction)  ptr)( value );
            // case boolVoid:   return ((boolVoidFunction)   ptr)();
            case boolBool:   return ((boolBoolFunction)   ptr)( value );
            case boolInt:    return ((boolIntFunction)    ptr)( value );
            case boolFloat:  return ((boolFloatFunction)  ptr)( value );
            // case intVoid:    return ((intVoidFunction)    ptr)();
            case intBool:    return ((intBoolFunction)    ptr)( value );
            case intInt:     return ((intIntFunction)     ptr)( value );
            case intFloat:   return ((intFloatFunction)   ptr)( value );
            // case floatVoid:  return ((floatVoidFunction)  ptr)();
            case floatBool:  return ((floatBoolFunction)  ptr)( value );
            case floatInt:   return ((floatIntFunction)   ptr)( value );
            case floatFloat: return ((floatFloatFunction) ptr)( value );
            default: break;
            }
        }
        return 0;
    }

    //
    // FLOAT
    //

    float triggerFloat() {
        uint32_t now = millis();
        if ( now - lastUpdateTime >= throttleInMs )
            return triggerFloatNow();
        return 0;
    }

    float triggerFloatNow() {
        lastUpdateTime = millis();
        if ( ptr != nullptr ) {
            switch( fType ) {
            case boolVoid:  return ((boolVoidFunction)  ptr)();
            case intVoid:   return ((intVoidFunction)   ptr)();
            case floatVoid: return ((floatVoidFunction) ptr)();
            default: break;
            }
        }
        return 0;
    }

    template<typename IN>
    float triggerFloat( IN value ) {
        uint32_t now = millis();
        if ( now - lastUpdateTime >= throttleInMs )
            return triggerFloatNow( value );
        return 0;
    }

    template<typename IN>
    float triggerFloatNow( IN value ) {
        lastUpdateTime = millis();
        if ( ptr != nullptr ) {
            switch( fType ) {
            // case voidVoid:   ((voidVoidFunction)   ptr)();
            // case voidBool:   ((voidBoolFunction)   ptr)( value );
            // case voidInt:    ((voidIntFunction)    ptr)( value );
            // case voidFloat:  ((voidFloatFunction)  ptr)( value );
            // case boolVoid:   return ((boolVoidFunction)   ptr)();
            case boolBool:   return ((boolBoolFunction)   ptr)( value );
            case boolInt:    return ((boolIntFunction)    ptr)( value );
            case boolFloat:  return ((boolFloatFunction)  ptr)( value );
            // case intVoid:    return ((intVoidFunction)    ptr)();
            case intBool:    return ((intBoolFunction)    ptr)( value );
            case intInt:     return ((intIntFunction)     ptr)( value );
            case intFloat:   return ((intFloatFunction)   ptr)( value );
            // case floatVoid:  return ((floatVoidFunction)  ptr)();
            case floatBool:  return ((floatBoolFunction)  ptr)( value );
            case floatInt:   return ((floatIntFunction)   ptr)( value );
            case floatFloat: return ((floatFloatFunction) ptr)( value );
            default: break;
            }
        }
        return 0;
    }
*/

//
// STATIC
//

private:

    static Throttle *TT;

public:

    static _void_void_F Me( _void_void_F f, uint16_t throttleInMs = 100 ) {
        if ( TT != nullptr ) delete TT;
        TT = new Throttle( f, throttleInMs );
        return &staticVoidRunner;
    }

    static _bool_void_F Me( _bool_void_F f, uint16_t throttleInMs = 100 ) {
        if ( TT != nullptr ) delete TT;
        TT = new Throttle( f, throttleInMs );
        return &staticRunnerBool;
    }

private:

    static void staticVoidRunner() {
        if ( TT != nullptr )
            TT->trigger();
    }

    static bool staticRunnerBool() {
        if ( TT != nullptr ) {
            bool r;
            return TT->triggerCheck( r );
        }
        return false;
    }

public:

    static void release() {
        if ( TT != nullptr ) delete TT;        
    }
    
};

Throttle* Throttle::TT = nullptr;

}

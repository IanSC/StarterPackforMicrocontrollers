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

    // applicable ???
    // https://stackoverflow.com/questions/9625526/check-at-compile-time-if-template-argument-is-void
    
    typedef void  (*voidVoidFunction)  (void );  // void  function( void  )
    typedef void  (*voidBoolFunction)  (bool );  // void  function( bool  )
    typedef void  (*voidIntFunction)   (int  );  // void  function( int   )
    typedef void  (*voidFloatFunction) (float);  // void  function( float ) 

    typedef bool  (*boolVoidFunction)  (void );  // bool  function( void  )
    typedef bool  (*boolBoolFunction)  (bool );  // bool  function( bool  )
    typedef bool  (*boolIntFunction)   (int  );  // bool  function( int   )
    typedef bool  (*boolFloatFunction) (float);  // bool  function( float )

    typedef int   (*intVoidFunction)   (void );  // int   function( void  )
    typedef int   (*intBoolFunction)   (bool );  // int   function( bool  )
    typedef int   (*intIntFunction)    (int  );  // int   function( int   )
    typedef int   (*intFloatFunction)  (float);  // int   function( float )

    typedef float (*floatVoidFunction) (void );  // float function( void  )
    typedef float (*floatBoolFunction) (bool );  // float function( bool  )
    typedef float (*floatIntFunction)  (int  );  // float function( int   )
    typedef float (*floatFloatFunction)(float);  // float function( float )

    Throttle( uint16_t throttleInMs = 100 ) {
        this->throttleInMs = throttleInMs;
        lastUpdateTime = millis() - throttleInMs;
    }

    enum functionType {
        voidVoid , boolVoid , intVoid , floatVoid ,
        voidBool , boolBool , intBool , floatBool ,
        voidInt  , boolInt  , intInt  , floatInt  ,
        voidFloat, boolFloat, intFloat, floatFloat
    };

    void *ptr = nullptr;
    functionType fType;

public:

    uint16_t throttleInMs = 100;
    uint32_t lastUpdateTime;

    #define CONS(fPrototype,fKey) \
        Throttle( fPrototype f, uint16_t throttleInMs = 100 ):Throttle(throttleInMs) { \
            this->ptr = (void*) f; \
            fType = functionType::fKey; }

    CONS( voidVoidFunction,   voidVoid   )
    CONS( voidBoolFunction,   voidBool   )
    CONS( voidIntFunction,    voidInt    )
    CONS( voidFloatFunction,  voidFloat  )

    CONS( boolVoidFunction,   boolVoid   )
    CONS( boolBoolFunction,   boolBool   )
    CONS( boolIntFunction,    boolInt    )
    CONS( boolFloatFunction,  boolFloat  )

    CONS( intVoidFunction,    intVoid    )
    CONS( intBoolFunction,    intBool    )
    CONS( intIntFunction,     intInt     )
    CONS( intFloatFunction,   intFloat   )

    CONS( floatVoidFunction,  floatVoid  )
    CONS( floatBoolFunction,  floatBool  )
    CONS( floatIntFunction,   floatInt   )
    CONS( floatFloatFunction, floatFloat )

    //
    // VOID
    //

    void trigger() {
        uint32_t now = millis();
        if ( now - lastUpdateTime >= throttleInMs )
            triggerNow();
    }

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
            case voidBool:   ((voidBoolFunction)   ptr)( value ); break;
            case voidInt:    ((voidIntFunction)    ptr)( value ); break;
            case voidFloat:  ((voidFloatFunction)  ptr)( value ); break;
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

//
// STATIC
//

private:

    static Throttle *TT;

public:

    static voidVoidFunction Me( voidVoidFunction voidFunction, uint16_t throttleInMs = 100 ) {
        if ( TT != nullptr ) delete TT;
        TT = new Throttle( voidFunction, throttleInMs );
        return &staticVoidRunner;
    }

    static boolVoidFunction Me( boolVoidFunction boolFunction, uint16_t throttleInMs = 100 ) {
        if ( TT != nullptr ) delete TT;
        TT = new Throttle( boolFunction, throttleInMs );
        return &staticRunnerBool;
    }

    static void staticVoidRunner() {
        if ( TT != nullptr )
            TT->trigger();
    }

    static bool staticRunnerBool() {
        if ( TT != nullptr )
            return TT->triggerBool();
        return false;
    }    

    static void staticRelease() {
        if ( TT != nullptr ) delete TT;        
    }
    
};

Throttle* Throttle::TT = nullptr;

}

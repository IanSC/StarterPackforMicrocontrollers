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

class ChangeObserver {

    // applicable ???
    // https://stackoverflow.com/questions/9625526/check-at-compile-time-if-template-argument-is-void
    
private:
    typedef void (*voidBoolFunction  ) (bool    ); // void function( bool     )
    typedef void (*voidInt8Function  ) (int8_t  ); // void function( int8_t   )
    typedef void (*voidUInt8Function ) (uint8_t ); // void function( uint8_t  )
    typedef void (*voidInt16Function ) (int16_t ); // void function( int16_t  )
    typedef void (*voidUInt16Function) (uint16_t); // void function( uint16_t )
    typedef void (*voidInt32Function ) (int32_t ); // void function( int32_t  )
    typedef void (*voidUInt32Function) (uint32_t); // void function( uint32_t )
    typedef void (*voidFloatFunction ) (float   ); // void function( float    )
    typedef void (*voidDoubleFunction) (double  ); // void function( double   )

public:
    typedef bool (*byteObserver  ) (byte    );     // bool function( byte     )
    typedef bool (*intObserver   ) (int     );     // bool function( int      )
    typedef bool (*longObserver  ) (long    );     // bool function( long     )

    typedef bool (*boolObserver  ) (bool    );     // bool function( bool     )
    typedef bool (*int8Observer  ) (int8_t  );     // void function( int8_t   )
    typedef bool (*uint8Observer ) (uint8_t );     // void function( uint8_t  )
    typedef bool (*int16Observer ) (int16_t );     // void function( int16_t  )
    typedef bool (*uint16Observer) (uint16_t);     // void function( uint16_t )
    typedef bool (*int32Observer ) (int32_t );     // void function( int32_t  )
    typedef bool (*uint32Observer) (uint32_t);     // void function( uint32_t )
    typedef bool (*floatObserver ) (float   );     // void function( float    )
    typedef bool (*doubleObserver) (double  );     // void function( double   )

private:

    enum functionIDs {
        voidBool  ,
        voidInt8  ,
        voidUInt8 ,
        voidInt16 ,
        voidUInt16,
        voidInt32 ,
        voidUInt32,
        voidFloat ,
        voidDouble
    };

    void *ptr = nullptr;
    functionIDs fID;

    union UnionType {
        bool     vBool;
        int8_t   vInt8;
        uint8_t  vUInt8;
        int16_t  vInt16;
        uint16_t vUInt16;
        int32_t  vInt32;
        uint32_t vUInt32;
        float    vFloat;
        double   vDouble;
    };
    UnionType oldValue;

public:

    #define CONS(fSig,fSigID,dataType,unionSlot) \
    ChangeObserver( fSig f, dataType initialValue = -1 ) {\
        this->ptr = (void*) f;\
        fID = functionIDs::fSigID;\
        oldValue.unionSlot = initialValue;\
    }

    CONS( voidBoolFunction  , voidBool  , bool    , vBool   )
    CONS( voidInt8Function  , voidInt8  , int8_t  , vInt8   )
    CONS( voidUInt8Function , voidUInt8 , uint8_t , vUInt8  )
    CONS( voidInt16Function , voidInt16 , int16_t , vInt16  )
    CONS( voidUInt16Function, voidUInt16, uint16_t, vUInt16 )
    CONS( voidInt32Function , voidInt32 , int32_t , vInt32  )
    CONS( voidUInt32Function, voidUInt32, uint32_t, vUInt32 )
    CONS( voidFloatFunction , voidFloat , float   , vFloat  )
    CONS( voidDoubleFunction, voidDouble, double  , vDouble )

    #undef CONS

    //
    // UPDATE
    //

    #define UPDATE(dataType,unionSlot,fSig,fSigID)\
    bool update( dataType value ) {\
        if ( ptr == nullptr && fID != functionIDs::fSigID )\
            return false;\
        if ( oldValue.unionSlot != value ) {\
            oldValue.unionSlot = value;\
            ((fSig) ptr)( value );\
            return true;\
        }\
        return false;\
    }

    UPDATE( bool    , vBool  , voidBoolFunction  , voidBool   )
    UPDATE( int8_t  , vInt8  , voidInt8Function  , voidInt8   )
    UPDATE( uint8_t , vUInt8 , voidUInt8Function , voidUInt8  )
    UPDATE( int16_t , vInt16 , voidInt16Function , voidInt16  )
    UPDATE( uint16_t, vUInt16, voidUInt16Function, voidUInt16 )
    UPDATE( int32_t , vInt32 , voidInt32Function , voidInt32  )
    UPDATE( uint32_t, vUInt32, voidUInt32Function, voidUInt32 )
    UPDATE( float   , vFloat , voidFloatFunction , voidFloat  )
    UPDATE( double  , vDouble, voidDoubleFunction, voidDouble )

    #undef UPDATE

//
// STATIC
//
public:

    #define OBS(outSig,inSig,dataType,runnerName)\
    public: static outSig Me( inSig f, dataType initialValue = -1 ) {\
        if ( CHG != nullptr ) delete CHG;\
        CHG = new ChangeObserver( f, initialValue );\
        return &runnerName;\
    }\
    private: static bool runnerName( dataType value ) {\
        if ( CHG != nullptr )\
            return CHG->update( value );\
        return false;\
    }

    OBS( boolObserver  , voidBoolFunction  , bool    , runVoidBool   )
    OBS( int8Observer  , voidInt8Function  , int8_t  , runVoidInt8   )
    OBS( uint8Observer , voidUInt8Function , uint8_t , runVoidUInt8  )
    OBS( int16Observer , voidInt16Function , int16_t , runVoidInt16  )
    OBS( uint16Observer, voidUInt16Function, uint16_t, runVoidUInt16 )
    OBS( int32Observer , voidInt32Function , int32_t , runVoidInt32  )
    OBS( uint32Observer, voidUInt32Function, uint32_t, runVoidUInt32 )
    OBS( floatObserver , voidFloatFunction , float   , runVoidFloat  )
    OBS( doubleObserver, voidDoubleFunction, double  , runVoidDouble )

    #undef OBS

    static void release() {
        if ( CHG != nullptr ) delete CHG;
    }

private:

    // option 1
    // - create object, but waste space
    // option 2
    // - create/delete object when needed
    // - save space but waste runtime
    // use #2, seldom used anyways
    static ChangeObserver *CHG;
    
};

ChangeObserver* ChangeObserver::CHG = nullptr;

}

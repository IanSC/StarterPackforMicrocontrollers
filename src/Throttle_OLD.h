//  2022/01 - isc
//
//  Throttle
//  --------
//  throttles executing a function
//  to avoid overwhelming resources
//
//  To Use
//
//      Throttle( &function, throttleTimeInMs )
//          supported functions: * function( * )
//
//      update()      run function every throttleTimeInMs only
//      updateNow()   run function immediately
//
//      Example:
//          int counter = 0;
//          void process() {
//              lcd.setCursor( 0, 0 );
//              lcd.print( counter++ );
//              int v = analogRead(A0);
//              lcd.setCursor( 0, 1 );
//              lcd.print( v );
//          }
//          Throttle tt( &process, 1000 );
//          void setup() {
//              tt.updateNow();
//          }
//          void loop() {
//              // process() gets called every 1000 ms only
//              tt.update();
//          }
//
//  Static Version
//
//      - wrapper for functions to be passed to other long running functions
//        without creating instances
//      - can only be used one at a time
//        ex. not allowed:
//            execute( Throttle::Me( &process1, 100 ), Throttle::Me( &process2, 100 ) );
//
//      Example:
//          void showResults( int value ) {
//              Serial.println( value );
//          }    
//          void process( voidIntFunction func ) {
//              while( true ) {
//                  int pot = analogRead( A0 );
//                  // ... do stuff, break
//                  if ( func != nullptr ) func( pot );
//              }
//          }
//          void loop() {
//              // BEFORE: showResults() gets called every iteration
//              process( &showResults );
//              // AFTER: showResults() gets called every 100ms
//              process( Throttle::Me( &showResults ) );
//          }

#pragma once
#include <Arduino.h>
#include <stdint.h>
#if defined(ESP8266) || defined(ESP32)
    #include <functional>
#endif

namespace StarterPack {

class Throttle_OLD {

    // applicable, can't get it to work ???
    // https://stackoverflow.com/questions/9625526/check-at-compile-time-if-template-argument-is-void
    
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

    /*
    //
    // FUNCTIONAL
    //

    typedef std::function<void(void    )> _void_void_FF;
    typedef std::function<void(bool    )> _void_bool_FF;
    typedef std::function<void(int8_t  )> _void_si08_FF;
    typedef std::function<void(uint8_t )> _void_ui08_FF;
    typedef std::function<void(int16_t )> _void_si16_FF;
    typedef std::function<void(uint16_t)> _void_ui16_FF;
    typedef std::function<void(int32_t )> _void_si32_FF;
    typedef std::function<void(uint32_t)> _void_ui32_FF;
    typedef std::function<void(float   )> _void_flot_FF;
    typedef std::function<void(double  )> _void_doob_FF;

    typedef std::function<bool(void    )> _bool_void_FF;
    typedef std::function<bool(bool    )> _bool_bool_FF;
    typedef std::function<bool(int8_t  )> _bool_si08_FF;
    typedef std::function<bool(uint8_t )> _bool_ui08_FF;
    typedef std::function<bool(int16_t )> _bool_si16_FF;
    typedef std::function<bool(uint16_t)> _bool_ui16_FF;
    typedef std::function<bool(int32_t )> _bool_si32_FF;
    typedef std::function<bool(uint32_t)> _bool_ui32_FF;
    typedef std::function<bool(float   )> _bool_flot_FF;
    typedef std::function<bool(double  )> _bool_doob_FF;

    typedef std::function<int8_t(void    )> _si08_void_FF;
    typedef std::function<int8_t(bool    )> _si08_bool_FF;
    typedef std::function<int8_t(int8_t  )> _si08_si08_FF;
    typedef std::function<int8_t(uint8_t )> _si08_ui08_FF;
    typedef std::function<int8_t(int16_t )> _si08_si16_FF;
    typedef std::function<int8_t(uint16_t)> _si08_ui16_FF;
    typedef std::function<int8_t(int32_t )> _si08_si32_FF;
    typedef std::function<int8_t(uint32_t)> _si08_ui32_FF;
    typedef std::function<int8_t(float   )> _si08_flot_FF;
    typedef std::function<int8_t(double  )> _si08_doob_FF;

    typedef std::function<uint8_t(void    )> _ui08_void_FF;
    typedef std::function<uint8_t(bool    )> _ui08_bool_FF;
    typedef std::function<uint8_t(int8_t  )> _ui08_si08_FF;
    typedef std::function<uint8_t(uint8_t )> _ui08_ui08_FF;
    typedef std::function<uint8_t(int16_t )> _ui08_si16_FF;
    typedef std::function<uint8_t(uint16_t)> _ui08_ui16_FF;
    typedef std::function<uint8_t(int32_t )> _ui08_si32_FF;
    typedef std::function<uint8_t(uint32_t)> _ui08_ui32_FF;
    typedef std::function<uint8_t(float   )> _ui08_flot_FF;
    typedef std::function<uint8_t(double  )> _ui08_doob_FF;

    typedef std::function<int16_t(void    )> _si16_void_FF;
    typedef std::function<int16_t(bool    )> _si16_bool_FF;
    typedef std::function<int16_t(int8_t  )> _si16_si08_FF;
    typedef std::function<int16_t(uint8_t )> _si16_ui08_FF;
    typedef std::function<int16_t(int16_t )> _si16_si16_FF;
    typedef std::function<int16_t(uint16_t)> _si16_ui16_FF;
    typedef std::function<int16_t(int32_t )> _si16_si32_FF;
    typedef std::function<int16_t(uint32_t)> _si16_ui32_FF;
    typedef std::function<int16_t(float   )> _si16_flot_FF;
    typedef std::function<int16_t(double  )> _si16_doob_FF;

    typedef std::function<uint16_t(void    )> _ui16_void_FF;
    typedef std::function<uint16_t(bool    )> _ui16_bool_FF;
    typedef std::function<uint16_t(int8_t  )> _ui16_si08_FF;
    typedef std::function<uint16_t(uint8_t )> _ui16_ui08_FF;
    typedef std::function<uint16_t(int16_t )> _ui16_si16_FF;
    typedef std::function<uint16_t(uint16_t)> _ui16_ui16_FF;
    typedef std::function<uint16_t(int32_t )> _ui16_si32_FF;
    typedef std::function<uint16_t(uint32_t)> _ui16_ui32_FF;
    typedef std::function<uint16_t(float   )> _ui16_flot_FF;
    typedef std::function<uint16_t(double  )> _ui16_doob_FF;

    typedef std::function<int32_t(void    )> _si32_void_FF;
    typedef std::function<int32_t(bool    )> _si32_bool_FF;
    typedef std::function<int32_t(int8_t  )> _si32_si08_FF;
    typedef std::function<int32_t(uint8_t )> _si32_ui08_FF;
    typedef std::function<int32_t(int16_t )> _si32_si16_FF;
    typedef std::function<int32_t(uint16_t)> _si32_ui16_FF;
    typedef std::function<int32_t(int32_t )> _si32_si32_FF;
    typedef std::function<int32_t(uint32_t)> _si32_ui32_FF;
    typedef std::function<int32_t(float   )> _si32_flot_FF;
    typedef std::function<int32_t(double  )> _si32_doob_FF;

    typedef std::function<uint32_t(void    )> _ui32_void_FF;
    typedef std::function<uint32_t(bool    )> _ui32_bool_FF;
    typedef std::function<uint32_t(int8_t  )> _ui32_si08_FF;
    typedef std::function<uint32_t(uint8_t )> _ui32_ui08_FF;
    typedef std::function<uint32_t(int16_t )> _ui32_si16_FF;
    typedef std::function<uint32_t(uint16_t)> _ui32_ui16_FF;
    typedef std::function<uint32_t(int32_t )> _ui32_si32_FF;
    typedef std::function<uint32_t(uint32_t)> _ui32_ui32_FF;
    typedef std::function<uint32_t(float   )> _ui32_flot_FF;
    typedef std::function<uint32_t(double  )> _ui32_doob_FF;

    typedef std::function<float(void    )> _flot_void_FF;
    typedef std::function<float(bool    )> _flot_bool_FF;
    typedef std::function<float(int8_t  )> _flot_si08_FF;
    typedef std::function<float(uint8_t )> _flot_ui08_FF;
    typedef std::function<float(int16_t )> _flot_si16_FF;
    typedef std::function<float(uint16_t)> _flot_ui16_FF;
    typedef std::function<float(int32_t )> _flot_si32_FF;
    typedef std::function<float(uint32_t)> _flot_ui32_FF;
    typedef std::function<float(float   )> _flot_flot_FF;
    typedef std::function<float(double  )> _flot_doob_FF;

    typedef std::function<double(void    )> _doob_void_FF;
    typedef std::function<double(bool    )> _doob_bool_FF;
    typedef std::function<double(int8_t  )> _doob_si08_FF;
    typedef std::function<double(uint8_t )> _doob_ui08_FF;
    typedef std::function<double(int16_t )> _doob_si16_FF;
    typedef std::function<double(uint16_t)> _doob_ui16_FF;
    typedef std::function<double(int32_t )> _doob_si32_FF;
    typedef std::function<double(uint32_t)> _doob_ui32_FF;
    typedef std::function<double(float   )> _doob_flot_FF;
    typedef std::function<double(double  )> _doob_doob_FF;
    */

    inline Throttle_OLD( uint16_t throttleInMs = 100 ) {
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

    void *ptr = nullptr;
    uint32_t lastUpdateTime;

public:

    uint16_t throttleInMs = 100;

    #define CONS( out, in ) \
        Throttle_OLD( out ## in ## _F f, uint16_t throttleInMs = 100 ) : Throttle_OLD( throttleInMs ) { \
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

    /*
    //
    // FUNCTIONAL
    //

    void(*ptrF)();

    Throttle( std::function<void(void)> f, uint16_t throttleInMs = 100 ) : Throttle( throttleInMs ) {
        this->ptrF = (void(*ptrF)()) f;
        inputType = paramType::_void;
        outputType = paramType::_void;
    }

    #define CONS( out, in ) \
        Throttle( out ## in ## _FF f, uint16_t throttleInMs = 100 ) : Throttle( throttleInMs ) { \
            this->ptrF = f; \
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
    */

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

    //
    // INPUT --> VOID / IGNORE
    //

    #define M( in, dType ) \
        void trigger( dType input ) { \
            uint32_t now = millis(); \
            if ( now - lastUpdateTime < throttleInMs ) return; \
            lastUpdateTime = now; \
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

    //
    // VOID --> OUTPUT
    //

    #define M( out, dType ) \
        bool triggerCheck( dType &output ) { \
            uint32_t now = millis(); \
            if ( now - lastUpdateTime >= throttleInMs ) { \
                lastUpdateTime = now; \
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
                lastUpdateTime = now; \
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

//
// STATIC
//

private:

    static Throttle_OLD *TT; // = nullptr;

public:

    static _void_void_F Me( _void_void_F f, uint16_t throttleInMs = 100 ) {
        if ( TT != nullptr ) delete TT;
        TT = new Throttle_OLD( f, throttleInMs );
        return &staticVoidRunner;
    }

    static _bool_void_F Me( _bool_void_F f, uint16_t throttleInMs = 100 ) {
        if ( TT != nullptr ) delete TT;
        TT = new Throttle_OLD( f, throttleInMs );
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
        if ( TT != nullptr ) { delete TT; TT = nullptr; }
    }
    
};

Throttle_OLD* Throttle_OLD::TT = nullptr;

}

//  2022/01 - isc
//
//  Analog Input
//  ------------
//  - uses <ResponsiveAnalogRead.h> for output smoothing
//  - native ESP32 analog read
//  - supports button mode (resistor network)
//    eg. LCD Keypad Shield
//
//  Usage
//
//      AnalogIO( pin, options = NONE, mappingFunction = nullptr, onChangeHandler = nullptr )
//
//          options = NONE                just set as input
//                    NO_SMOOTHING        no output smoothing
//                    SMOOTHING           smooth output (using ResponsiveAnalogRead)
//                    NO_PULLUP           no pullup
//                    WITH_PULLUP         with pullup to +VCC
//                    WITH_PULLDOWN       with pulldown to GND (ESP32 only)
//          mappingFunction               function to map raw analog to intended range
//                                        template: int map( int value )
//          onChangeHandler               function to call when value changes
//                                        template: void OnChange( int )
//
//      void init()                       initialize smoothing and initial value
//                                        only needed if using smoothing or onChange callback
//                                            smoothing - send to smoothing function current value several times
//                                            onChange callback - set initial state
//                                        assign mapping function first (if required) before calling 
//                                            init() to get correct onChange value
//      setMappingFunction( &function )   specify function to convert raw adc value to usable range
//                                        applies to read() and onChange handlers
//                                        signature: int map( int )
//
//      bool useSmoothing                 whether smoothing is activated
//      setAnalogResolution( int )        set resolution for smoothing function
//      int getAnalogResolution()         get resolution of smoothing function
//      RAR                               access to embedded ResponsiveAnalogRead variable
//
//      int  read()                       analog value, apply optional smoothing and/or mapping
//      int  readRaw()                    raw analog value, same as analogRead()
//
//      int  rawValue                     last reading from ADC, analog and buttons mode
//      int  smoothValue                  last applied smoothing, analog only
//      int  value                        last processed value, analog and buttons mode
//
//   Callback
//
//      setOnChangeCallback( &handler )   specify callback if value has changed
//                                        applies to analog and button modes
//                                        signature: void onChange( int )
//
//  Button Mode - initialize as resistor network buttons
//
//      initButtons( zeroValue, value1, value2, ..., value10 )
//          zeroValue     analog read value if nothing is pressed
//          value1        value for button 1
//          ... value10   value for succeeding buttons
//                        max 10 buttons
//          
//      initButtonsN( entryCount, zeroValue, value1, ..., valueN )
//          use this if more than 10 buttons
//          entryCount   number of entries (zeroValue should be included in count)
//          
//          ex. aIO.initButtons (    1022, 834, 642, 14, 228, 430 ); // up to 10 buttons, including zeroValue, 11 entries
//              aIO.initButtonsN( 6, 1022, 834, 642, 14, 228, 430 ); // 1st param is number of total entries
//
//      uint8_t  getButtonCount()                 query number of buttons assigned
//      int      getButtonValue( buttonNo )       query value for specified button
//      setInactiveButton( buttonNo )             adc value slot that is considered not pressed
//                                                when initButtons() or initButtonsN() was called
//                                                default to slot 0 (corresponding to zeroValue)
//
//      int8_t getContinuousKey()                 send button being pressed on each call
//      int8_t getKeyDown()                       process key so it only register once per click
//      int8_t getKeyUp()                         process key so it registers upon release
//      int8_t getRepeatingKey()                  send multiple clicks if held down
//      setRepeatDelayInMs( uint16_t )            time before sending 1st repeated key
//      setRepeatRateInMs( uint16_t )             time before sending subsequent keys
//
//      Debouncer debouncer                       access to internal debouncer, see <Debouncer.h>
//
//      setActiveDebounceTimeInMs( uint16_t )     set debounce time for active state, default 50
//      setInactiveDebounceTimeInMs( uint16_t )   set debounce time for inactive state, default 50
//      setMinimumDebounceTimeInMs( uint16_t)     set minimum time of debouncing before allowed to be cancelled, default 50
//      setConfirmStateTimeInMs( uint16_t )       confirm state change is consistent for specified time
//
//      flagWaitForKeyup()                        detect keyup before sending keydown again, see <Debouncer.h>
//      cancelDebouncing()                        stop debouncing if beyond certain period (MinimumDebounceTime)
//                                                eg. signal already processed, so stop debouncing if for next call
//
//      To determine values, run this sketch and press each button, record the values.
//          AnalogIO aIO = AnalogIO( A0 );
//          void setup() { Serial.begin( 9600 ); }
//          void loop() {
//              Serial.println( aIO.read() );
//              delay( 50 );
//          }
//      To use, run this sketch with result from above:
//          AnalogIO aIO = AnalogIO( A0 );
//          void setup() {
//              Serial.begin( 9600 );
//              // LCD Keypad Shield
//              aIO.initButtons( 1022, 834, 642, 14, 228, 430 );
//              // 1022   value if nothing is pressed, must be 1st entry   0 - result from readButton()
//              // 834    select button                                    1
//              // 642    left button                                      2
//              // 14     right button                                     3
//              // 228    up button                                        4
//              // 430    down button                                      5
//          }
//          void loop() {
//              Serial.println( aIO.readButton() );
//              delay( 50 );
//          }

#pragma once
#include <Debouncer.h>
#include <limits.h>

    #if defined(ESP32)
        // FOR ESP32 NATIVE READ
        #include <driver/adc.h>
    #endif

    // SMOOTHING FILTER - CHOOSE ONLY 1
    #define USE_RESPONSIVE_AR
    //#define USE_KALMAN_FILTER

    #if defined(USE_RESPONSIVE_AR)
        // https://github.com/dxinteractive/ResponsiveAnalogRead
        #include <ResponsiveAnalogRead.h>
    #endif
    #if defined(USE_KALMAN_FILTER)
        // https://github.com/denyssene/SimpleKalmanFilter
        #include <SimpleKalmanFilter.h>
    #endif

namespace StarterPack {

typedef int  (*intIntFunction) (int); // int function( int )
typedef void (*voidIntFunction)(int); // void function( int )

class AnalogIO {

//
// INIT
//

    private:

        uint8_t PIN;

    public:

        ~AnalogIO() {
            if ( buttonList != nullptr ) delete buttonList;
        }

        int rawValue    = 0;
        int smoothValue = 0;
        int value       = -1;

        enum optionsEnum {
            NONE          = 0,
            NO_SMOOTHING  = 0,
            SMOOTHING     = 1,
            NO_PULLUP     = 0,
            WITH_PULLUP   = 2,
            WITH_PULLDOWN = 4
        };

        inline friend optionsEnum operator | ( optionsEnum a, optionsEnum b ) {
            return static_cast<optionsEnum>( static_cast<int>(a) | static_cast<int>(b) );
        }
        inline friend optionsEnum& operator |= ( optionsEnum& a, optionsEnum b ) {
            a = a | b; return a;
        }

        AnalogIO( uint8_t pin, optionsEnum options = optionsEnum::NONE, intIntFunction mapFunction = nullptr, voidIntFunction onChange = nullptr ) {

            PIN = pin;
            useSmoothing = ( ( options & optionsEnum::SMOOTHING ) == optionsEnum::SMOOTHING );
            if ( ( options & optionsEnum::WITH_PULLUP ) == optionsEnum::WITH_PULLUP )
                pinMode( PIN, INPUT_PULLUP );
            else if ( ( options & optionsEnum::WITH_PULLDOWN ) == optionsEnum::WITH_PULLDOWN )
                #if defined( ESP32 )
                    pinMode( PIN, INPUT_PULLDOWN );
                #else
                    pinMode( PIN, INPUT );
                #endif
            else
                pinMode( PIN, INPUT );
            pinMode( PIN, INPUT_PULLUP );
            this->mapFunction = mapFunction;
            this->onChange = onChange;

            #if defined(ESP32)
                mapGPIOtoADC();
            #endif

            if ( useSmoothing )
                setupSmoothing();
        }
        
        void init() {
            if ( useSmoothing )
                initSmoothing();
            read();
        }

        inline void setMappingFunction( intIntFunction mapFunction ) {
            this->mapFunction = mapFunction;
        }

        inline void setOnChangeCallback( voidIntFunction onChange ) {
            this->onChange = onChange;
        }

    private:

        #if defined(ESP32)
            int analogResolution = 4096;
        #else
            int analogResolution = 1024;
        #endif

    public:

        inline void setAnalogResolution( int resolution ) {
            analogResolution = resolution;
            #if defined(USE_RESPONSIVE_AR)
                RAR.setAnalogResolution( resolution );
            #endif
        }

        inline int getAnalogResolution() {
            return analogResolution;
        }

//
// READ ESP NATIVE METHOD
//
    
    // FOR ESP32 DevKitC
    //
    //  Style           Code                               Time on ESP32 DevKitC
    //  ESP32 native    v = adc1_get_raw( ADC_CHANNEL1 )   55 ms
    //  Arduino         v = analogRead( PIN )              80 ms
    //
    //  if speed is close in the future updates, this section can be removed
    //
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html

    private:

        #if defined(ESP32)

            uint8_t ADC_PORT = 0;
            adc1_channel_t ADC_CHANNEL1;
            adc2_channel_t ADC_CHANNEL2;

            inline void mapGPIOtoADC() {
                // don't know how to map GPIO to ADC channel
                // ~/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/driver/driver/adc.h
                switch( PIN ) {
                case 36: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_0; break;
                case 37: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_1; break;
                case 38: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_2; break;
                case 39: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_3; break;
                case 32: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_4; break;
                case 33: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_5; break;
                case 34: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_6; break;
                case 35: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_7; break;
                case  4: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_0; break;
                case  0: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_1; break;
                case  2: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_2; break;
                case 15: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_3; break;
                case 13: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_4; break;
                case 12: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_5; break;
                case 14: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_6; break;
                case 27: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_7; break;
                case 25: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_8; break;
                case 26: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_9; break;
                }
                if ( ADC_PORT == 1 ) {
                    adc1_config_width( ADC_WIDTH_BIT_12 );
                    adc1_config_channel_atten( ADC_CHANNEL1, ADC_ATTEN_DB_11 );
                } else if ( ADC_PORT == 2 )
                    adc2_config_channel_atten( ADC_CHANNEL2, ADC_ATTEN_DB_11 );
            }

            inline int readESPStlye() {
                if ( ADC_PORT == 1 )
                    return adc1_get_raw( ADC_CHANNEL1 );
                else if ( ADC_PORT == 2 ) {
                    // just ignore error and pass whatever the function returned
                    int rawValue;
                    //esp_err_t err = 
                    adc2_get_raw( ADC_CHANNEL2, ADC_WIDTH_BIT_12, &rawValue );
                    // ESP_OK                if success
                    // ESP_ERR_TIMEOUT       ADC2 is being used by other controller and the request timed out
                    // ESP_ERR_INVALID_STATE The controller status is invalid. Please try again
                    return rawValue;
                }
                return 0;
            }

        #endif

//
// SMOOTHING
//
    public:

        // SimpleKalmanFilter:
        // - need to tweak settings
        // - not as responsive, takes longer to get to actual value
        //   maybe not appropriate for potentiometers or wrong settings
        // - no need to set range
        // - time: 25 ~ 30 ms (on ESP32 DevKitC)
        //
        // ResponsiveAnalogRead:
        // - runs out of the box
        // - need to set range
        // - time: 25 ~ 30 ms (on ESP32 DevKitC)

        bool useSmoothing = false;

        #ifdef USE_KALMAN_FILTER
            // const float e_mea = 2; const float e_est = 2; const float q = 0.01;
            const float e_mea = 20; const float e_est = 5; const float q = 0.1;
            // const float e_mea = 20; const float e_est = 5;  const float q = 0.1;
            SimpleKalmanFilter SKF = SimpleKalmanFilter( e_mea, e_est, q );
        #endif

        #ifdef USE_RESPONSIVE_AR
            ResponsiveAnalogRead RAR = ResponsiveAnalogRead( PIN, false );
        #endif

    private:

        inline void setupSmoothing() {
            #if defined(USE_RESPONSIVE_AR)
                //#if defined(ESP32)
                    RAR.setAnalogResolution( analogResolution );
                //#endif
            #endif
            #if defined(USE_KALMAN_FILTER)
            #endif
        }

        inline int initSmoothing() {
            const int FEED_COUNT = 10;
            #if defined(USE_RESPONSIVE_AR)
                for ( int i = 0 ; i < FEED_COUNT ; i++ ) {
                    readRaw();
                    RAR.update( rawValue );
                    // RAR.update( readRaw() );
                }
                smoothValue = RAR.getValue();
                // return smoothValue;
                // return RAR.getValue();
            #endif
            #if defined(USE_KALMAN_FILTER)
                for ( int i = 0 ; i < FEED_COUNT-1 ; i++ ) {
                    readRaw();
                    SKF.updateEstimate( rawValue );
                    // SKF.updateEstimate( readRaw() );
                }
                readRaw();
                smoothValue = SKF.updateEstimate( rawValue );
                // return SKF.updateEstimate( readRaw() );
            #endif
            return smoothValue;
        }

        inline int applySmoothing( int rawValue ) {
            #if defined(USE_RESPONSIVE_AR)
                RAR.update( rawValue );
                return RAR.getValue();
            #endif
            #if defined(USE_KALMAN_FILTER)
                return SKF.updateEstimate( rawValue );
            #endif
        }

//
// ANALOG
//
    private:

        intIntFunction mapFunction = nullptr;
        voidIntFunction onChange = nullptr;
        int lastValue = -1;

        inline void handleOnChanged() {
            if ( value != lastValue ) {
                lastValue = value;
                if ( onChange != NULL )
                    onChange( value );
            }
        }

    public:

        void readRaw() {
            #if defined(ESP32)
                // ESP IDF style
                rawValue = readESPStlye();
            #else
                // Arduino style
                rawValue = analogRead( PIN );
            #endif
        }

        int read() {
            //int rawValue = 
            readRaw();

            if ( useSmoothing )
                smoothValue = applySmoothing( rawValue );
            else
                smoothValue = rawValue;

            if ( mapFunction != nullptr )
                value = mapFunction( smoothValue );
            else
                value = smoothValue;

            handleOnChanged();
            // if ( value != lastValue ) {
            //     lastValue = value;
            //     if ( onChange != NULL )
            //         onChange( value );
            // }
            return value;
        }
        
//
// AS BUTTONS
//

    private:
    
        class buttonRange { public:
            int from;
            int to;
            int value;
        };
        buttonRange *buttonList = nullptr;
        uint8_t buttonCount;

    //
    // INIT
    //
    public:

        inline uint8_t getButtonCount() {
            return buttonCount;
        }

        int getButtonValue( uint8_t buttonNo ) {
            if ( buttonNo < 0 || buttonNo >= buttonCount )
                return -1;
            return buttonList[buttonNo].value;
        }

        // user convenience, don't know how to count number of args without so much extra code
        // https://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments/2124385#2124385
        // 1 to 10 buttons
        // zero - value if not pressed
        inline void initButtons( int zero, int v1 ) { initButtonsN( 2, zero, v1 ); }
        inline void initButtons( int zero, int v1, int v2 ) { initButtonsN( 3, zero, v1, v2 ); }
        inline void initButtons( int zero, int v1, int v2, int v3 ) { initButtonsN( 4, zero, v1, v2, v3 ); }
        inline void initButtons( int zero, int v1, int v2, int v3, int v4 ) { initButtonsN( 5, zero, v1, v2, v3, v4 ); }
        inline void initButtons( int zero, int v1, int v2, int v3, int v4, int v5 ) { initButtonsN( 6, zero, v1, v2, v3, v4, v5 ); }
        inline void initButtons( int zero, int v1, int v2, int v3, int v4, int v5, int v6 ) { initButtonsN( 7, zero, v1, v2, v3, v4, v5, v6 ); }
        inline void initButtons( int zero, int v1, int v2, int v3, int v4, int v5, int v6, int v7 ) { initButtonsN( 8, zero, v1, v2, v3, v4, v5, v6, v7 ); }
        inline void initButtons( int zero, int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8 ) { initButtonsN( 9, zero, v1, v2, v3, v4, v5, v6, v7, v8 ); }
        inline void initButtons( int zero, int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9 ) { initButtonsN( 10, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9 ); }
        inline void initButtons( int zero, int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, int v10 ) { initButtonsN( 11, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10 ); }

        // specify number of entries on 1st argument
        // including value if nothing is pressed
        void initButtonsN( int argCount, ... ) {
            // 1st arg is number of buttons

            this->buttonCount = argCount;
            if ( buttonList != nullptr )
                delete buttonList;
            buttonList = new buttonRange[buttonCount];
            int sorted[buttonCount];

            va_list valist;
            va_start( valist, argCount );
            for ( int i = 0 ; i < buttonCount; i++ ) {
                buttonList[i].value = va_arg( valist, int );
                sorted[i] = buttonList[i].value;
            }
            va_end( valist );

            // sort list
            // https://stackoverflow.com/questions/33067907/simple-program-to-sort-numbers-in-c
            for( int i = 0 ; i < buttonCount-1 ; i++ ) {
                for( int k = 0 ; k < buttonCount-1-i ; k++ ) {
                    if( sorted[k] > sorted[k+1] ) {
                        int temp = sorted[k];
                        sorted[k] = sorted[k+1];
                        sorted[k+1] = temp;
                    }
                }
            }

            // for ( int i = 0 ; i < buttonCount; i++ ) {
            //     Serial.print( sorted[i] );
            //     Serial.print( ", " );
            // }
            // Serial.println();

            for ( int i = 0 ; i < buttonCount; i++ ) {
                for ( int j = 0 ; j < buttonCount; j++ ) {
                    if ( buttonList[j].value == sorted[i] ) {
                        // divide distance between target values by 3
                        // 1/3 for previous value, 1/3 dead range, 1/3 for next value
                        // ex. values     range
                        //       0        -inf to 33
                        //     100        66 to 133
                        //     200        166 to +inf
                        if ( i == 0 )
                            buttonList[j].from = INT_MIN;
                        else
                            buttonList[j].from = buttonList[j].value - ( sorted[i] - sorted[i-1] ) / 3;
                        if ( i < buttonCount-1 )
                            buttonList[j].to = buttonList[j].value + ( sorted[i+1] - sorted[i] )/3;
                        else
                            buttonList[j].to = INT_MAX;
                    }
                }
            }
            // DEBUG
            // for ( int i = 0 ; i < buttonCount; i++ )
            //     Serial.printf( "%d = %d - %d\n", buttonList[i].value, buttonList[i].from, buttonList[i].to );

            // do initial read, otherwise debouncer will give wrong value 1st time
            debouncer.setInitialValue( readButtonCore() );

            // by default was set to false (assume = 0) already
            //buttonDebouncer.inactiveState = inactiveButton;
        }

    //
    // READ
    //
    // public:

    //     int8_t readButton() {
    //         value = debouncer.debounce( readButtonCore() );
    //         if ( value != lastValue ) {
    //             lastValue = value;
    //             if ( onChange != NULL )
    //                 onChange( value );
    //         }
    //         return value;
    //     }

    private:

        int8_t readButtonCore() {
            readRaw();
            for ( int i = 0 ; i < buttonCount; i++ ) {
                if ( buttonList[i].from <= rawValue && rawValue <= buttonList[i].to ) {
                    return debouncer.debounce( i );
                }
            }
            // Serial.println( "GOT INVALID VALUE" );
            // do not debounce invalid button
            return debouncer.inactiveState;
        }

    public:

        int8_t getContinuousKey() {
            value = readButtonCore();
            value = debouncer.getContinuousKey( value );
            handleOnChanged();
            return value;
        }

        int8_t getKeyDown() {
            value = readButtonCore();
            value = debouncer.getKeyDown( value );
            handleOnChanged();
            return value;
        }

        int8_t getKeyUp() {
            value = readButtonCore();
            value = debouncer.getKeyUp( value );
            handleOnChanged();
            return value;
        }

        int8_t getRepeatingKey() {
            // if user keeps button held down send signal intermitently
            // eg. selecting an option, by sending right key every few seconds
            //     as opposed to keep going right as in gamepad
            value = readButtonCore();
            value = debouncer.getRepeatingKey( value );
            handleOnChanged();
            return value;
        }

        // direct access to debouncer being used
        Debouncer debouncer;

        // inline void setActiveDebounceTimeInMs  ( uint16_t time ) { debouncer.activeStatesDebounceInMs  = time; }
        // inline void setInactiveDebounceTimeInMs( uint16_t time ) { debouncer.inactiveStateDebounceInMs = time; }
        // inline void setMinimumDebounceTimeInMs ( uint16_t time ) { debouncer.minimumDebounceTimeInMs   = time; }
        // inline void setConfirmStateTimeInMs    ( uint16_t time ) { debouncer.confirmStateTimeInMs      = time; }
        inline void flagWaitForKeyup()                    { debouncer.flagWaitForKeyup();       }
        inline void cancelDebouncing()                    { debouncer.cancelDebouncing();       }
        // inline void setRepeatDelayInMs( uint16_t delay )  { debouncer.repeatDelayInMs = delay;  }
        // inline void setRepeatRateInMs( uint16_t rate )    { debouncer.repeatRateInMs = rate;    }

        inline void setInactiveButton( uint8_t buttonNo ) { debouncer.inactiveState = buttonNo; }

};

}

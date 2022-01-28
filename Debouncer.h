//  2022-01 isc
//
//  Debouncer
//  - basic: use to debounce a digital button so when pressed
//    high/low chatter will be eliminated and only a single press
//    will be detected
//  - advance: retain any int value for a specified duration
//  - can be used for analog style buttons (resistor network)
//    ex. LCD Keypad Shield
//
//  Functions/Fields
//
//      int debounce( int state )             debounce a signal
//      int inactiveState                     state that is considered inactive or "keyup"
//      uint16_t activeStatesDebounceInMs     time to debounce/maintain active states
//                                            active states are values that are not equal to inactive state
//                                            eg. got high, maintain as high value for this long
//      uint16_t inactiveStateDebounceInMs    time to debounce/maintain inactive state
//                                            releasing buttons also introduces chatter
//                                            eg. got low maintain as low for this long
//      uint16_t minimumDebounceTimeInMs      minimum time of debouncing before allowed to be stopped
//                                            eg. long debounce time can be stopped once processed, after this period
//      uint16_t confirmStateTimeInMs         a state will only be considered if after this time 
//                                            it is still maintained
//                                            eg. got high, after 10 ms and still high, accept the state change
//      setInitialValue( state )              set the initial value during startup
//      deactivate()                          deactivate debouncing
//      activate()                            reactivate debouncing
//      flagWaitForKeyup()                    do not send any active state until inactive state is seen again
//                                            eg. user keeps a button pressed
//                                                after debounce time
//      assignGetStateHandler( function )     assign a function to read the state
//      int debounce()                        use function from assignGetStateHandler()
//
//      int actualState                       last value used without applying debounce
//      int effectiveState                    last debounced value
//
//  Example: basic
//
//      Debouncer db = Debouncer();
//      bool result = db.debounce( digitalRead( 8 ) );
//
//  Example: assign state reader
//
//      int readPin() {
//          return digitalRead( A0 );
//      }
//      db.assignGetStateHandler( &readPin );
//      bool result = db.debonce();
//
//  Example: analog buttons
//
//      db.inactiveState = mapAnalogToButton( ANALOG_VALUE_IF_NOTHING_IS_PRESSED );
//      int input = mapAnalogToButton( analogRead( A0 ) );
//      int result = db.debounce( input );
//      // mapAnalogToButton() = converts analog value to button number
//
//  Example: increment counter by only 1 on each press even if user keeps it down
//
//      bool result = db.debounce( digitalRead( 8 ) );
//      if ( result ) {
//          counter++;    
//          db.flagWaitForKeyup();
//      }
//      

#pragma once
#include <Arduino.h>
#include <stdint.h>

namespace StarterPack {

class Debouncer {

    public:

        int actualState = false;                    // actual physical state of the button
        int effectiveState = false;                 // state to consider taken into account debouncing, confirmation
        int inactiveState = false;                  // value that is considered the "keyUp"
        uint16_t activeStatesDebounceInMs = 50;     // debouncing keyDown time        
        uint16_t inactiveStateDebounceInMs = 50;    // debouncing keyUp time
        uint16_t minimumDebounceTimeInMs = 50;      // minimum debounce before allowed to be cancelled 
        uint16_t confirmStateTimeInMs = 0;          // time delay to confirm a state

        void setInitialValue( int state ) {
            actualState = state;
            effectiveState = state;
            mode = mIdle;
        }

        void deactivate() {
            active = false;
            effectiveState = actualState;
            mode = mIdle;
        }

        void activate() {
            active = true;
            mode = mIdle;
        }

        // example:
        //                                             keyDown    keyUp
        // device                      inactiveState   debounce   debounce   confirm   notes
        // -------------------------   -------------   --------   --------   -------   -----
        // push button (active high)   false           50         50         0
        // push button (active low)    true            50         50         0
        // analog keypad               [depends]       50         50         0         1
        // analog rotary switch        [depends]       50         50         50        2
        
        // 1. resistor network style keys wired to a single analog pin
        //    similar to dfrobot lcd keypad shield
        // 2. rotary switch wired with resistor network same as #1
        //    when switching there is small gap between contacts that is unconnected
        //    switching will be: A --> unconnected --> B
        //    to eliminate the momentary unconnected, put a confirm time value
        //
        // note: analog value must be mapped to button number before feeding to debouncer

    //
    //
    //

    private:

        bool active = true;

    public:

        int debounce( int state ) {
            actualState = state; // keep copy for user
            if ( !active ) {
                effectiveState = state;
                return effectiveState;
            }
            return debounceCore( state );
        }

        //  debouncer db1;
        //  debouncer db2;
        //  int readDigitalPin() {
        //      return digitalRead( 10 );
        //  }
        //  int readAnalogPin() {
        //      return potToButtonMap( analogRead( A0 ) );
        //  }
        //  void setup() {
        //      db1.assignGetStateHandler( &readDigitalPin );
        //      db2.assignGetStateHandler( &readAnalogPin );
        //  }
        //  void loop() {
        //      bool input = db1.debounce();
        //      int result = db2.debounce();
        //      ...
        //  }

        int debounce() {
            if ( stateGetter == nullptr )
                return inactiveState;
            actualState = stateGetter();
            if ( !active ) {
                effectiveState = actualState;
                return effectiveState;
            }
            return debounceCore( actualState );
        }
        void assignGetStateHandler( int (*stateGetter)(void) ) {
            this->stateGetter = stateGetter;
        }

    private:
    
        int (*stateGetter)(void) = nullptr;

    //
    // CORE
    //

    private:

        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;

        enum modes {
            mIdle,
            mConfirm,
            mDebouncing
        };
        modes mode = mIdle;

        int debounceCore( int currentState ) {

            switch( mode ) {
            case mIdle: JUMP_IDLE:
                if ( currentState == effectiveState ) {
                    DEBUG_TRACE( Serial.print( "#" ) );
                } else {
                    if ( confirmStateTimeInMs == 0 ) {
                        // no need to confirm
                        DEBUG_TRACE( Serial.printf( "accepted: %d\n", currentState ) );
                        updateState( currentState );
                    } else {
                        // confirm first
                        DEBUG_TRACE( Serial.printf( "to confirm: %d\n", currentState ) );
                        toConfirmState = currentState;
                        confirmStartTime = millis();
                        mode = mConfirm;
                    }
                }
                break;
            case mConfirm:
                if ( millis() - confirmStartTime >= confirmStateTimeInMs ) {
                    if ( toConfirmState == currentState ) {
                        // still same, confirmed
                        DEBUG_TRACE( Serial.printf( "confirmed: %d\n", currentState ) );
                        updateState( currentState );
                    } else {
                        // different !!! confirm again or ignore ??? confirm...
                        // if fed high frequency signal will go on forever confirming
                        // and return the last confirmed state, which is technically 
                        // still correct since new state cannot be confirmed
                        DEBUG_TRACE( Serial.printf( "reconfirm: %d\n", currentState ) );
                        toConfirmState = currentState;
                        confirmStartTime = millis();
                        // for now return same as before
                    }
                } else {
                    DEBUG_TRACE( Serial.printf( "not confirmed: %d\n", currentState ) );
                }
                break;
            case mDebouncing:
                if ( isDebouncing() ) {
                    DEBUG_TRACE( Serial.printf( "debounce: %d\n", currentState ) );
                    break;
                }
                goto JUMP_IDLE;
            }

            if ( waitForKeyupFlag ) {
                // note: consider only inactive states after debouncing, otherwise false inactive
                //       hence check only after all operations
                // eg. input states:  0001010101111111 
                //     debounce of 1:    ========        do not consider 0's here
                //     check for 0:              ======= begin checking for 0's here
                //     ignore 0's while still debouncing 1's
                if ( effectiveState == inactiveState ) {
                    DEBUG_TRACE( Serial.printf( "done waiting for keyup: %d\n", currentState ) );
                    waitForKeyupFlag = false;
                } else {
                    DEBUG_TRACE( Serial.print( "w" ) );
                }
                return inactiveState;
            }
            return effectiveState;
        }

    //
    // CONFIRM
    //

    private:

        uint32_t confirmStartTime;
        int toConfirmState;

    //
    // KEY UP
    //

    public:

        inline void flagWaitForKeyup() {
            // keep reporting inactive, until real inactive has been detected
            // eg. do not send true that a button if pressed again
            //     until it is released and pressed again

            // whatever the mode is, cannot return active
            // so effectively waiting for inactive
            waitForKeyupFlag = true;
        }

        inline void cancelDebouncing() {
            // signal already processed, stop current debouncing
            // use last real state for next operations
            if ( mode == mDebouncing ) {
                uint32_t now = millis();
                uint32_t ellapsed = now - debouncingStartTime;
                if ( ellapsed < minimumDebounceTimeInMs ) {
                    // still debouncing, adjust time to stop debouncing once minimum is reached
                    //                          |<-- min -->|
                    //  ------------------------|--------|----------------------|
                    //                          start   now |                   end
                    //                          |           |
                    // -----|-------------------------------|
                    //      start                           end
                    //      adjust start so end will
                    //      be same as min time
                    uint32_t shortage = minimumDebounceTimeInMs - ellapsed;
                    if ( effectiveState == inactiveState ) {
                        // debouncing inactive state
                        debouncingStartTime = now + shortage - inactiveStateDebounceInMs;
                    } else {
                        // debouncing active state
                        debouncingStartTime = now + shortage - activeStatesDebounceInMs;
                    }
                } else {
                    // already reached minimum, cancel
                    effectiveState = actualState;
                    mode = mIdle;
                }
            }
        }

    private:

        bool waitForKeyupFlag = false;

    //
    // DEBOUNCE
    //

    private:

        uint32_t debouncingStartTime;
        
        inline bool isDebouncing() {
            uint32_t now = millis();
            // debounce time of inactive and active are possibly different
            if ( effectiveState == inactiveState ) {
                if ( now - debouncingStartTime < inactiveStateDebounceInMs ) {
                    // still debouncing, maintain effectiveState
                    return true;
                }
            } else {
                if ( now - debouncingStartTime < activeStatesDebounceInMs ) {
                    // still debouncing, maintain effectiveState
                    return true;
                }
            }
            return false;
        }

        void updateState( int currentState ) {
            if ( currentState != effectiveState ) {
                // change state, start debounce
                mode = mDebouncing;
                debouncingStartTime = millis();
                effectiveState = currentState;
            } else {
                // same as before
                mode = mIdle;
            }
        }

    //
    // REPEATING
    //
    private:

        enum repeatModeStates {
            rIdle,
            rSentFirstKey, // waiting for for [repeatDelay] to go into Repeating
            rRepeating     // send every [repeatRate]
        };

        repeatModeStates repeatMode = rIdle;
        int repeatValue = 0;
        uint32_t lastActionTime;

    public:

        uint16_t repeatDelayInMs = 400;
        uint16_t repeatRateInMs  = 250;

        int getRepeatingCore( int current ) {
            actualState = current;
            if ( current != inactiveState ) {
                uint32_t now = millis();
                switch( repeatMode ) {
                case rIdle:
                    repeatMode = rSentFirstKey;
                    repeatValue = current;
                    lastActionTime = now;
                    return current;
                case rSentFirstKey: {
                        if ( current != repeatValue ) {
                            // pressed another key
                            break;
                        }
                        if ( now - lastActionTime >= repeatDelayInMs ) {
                            repeatMode = rRepeating;
                            lastActionTime = now;
                            return current;
                        } else {
                            return inactiveState;
                        }
                    }
                    break;
                case rRepeating: {
                        if ( current != repeatValue ) {
                            // pressed another key
                            break;
                        }                    
                        if ( now - lastActionTime >= repeatRateInMs ) {
                            lastActionTime = now;
                            return current;
                        } else {
                            return inactiveState;
                        }
                    }
                    break;
                }
            }
            repeatMode = rIdle;
            return actualState;
        }

    //
    // BUTTONS
    //

        int lastValue;

        int getKeyDown( int value ) {
            if ( value != lastValue ) {
                lastValue = value;
                if ( value != inactiveState )
                    return value;
            }
            return inactiveState;
        }

        int getKeyUp( int value ) {
            if ( value != lastValue ) {
                int releasedButton;
                if ( value == inactiveState ) {
                    // key released, keep what was being pressed before
                    releasedButton = lastValue;
                } else {
                    // key pressed, return nothing
                    releasedButton = inactiveState;
                }
                lastValue = value;
                return releasedButton;
            }
            return false;
        }

        int getContinuous( int value ) {
            lastValue = value;
            return value;
        }

        int getRepeating( int value ) {
            // if user keeps button held down send signal intermitently
            // eg. selecting an option, by sending right key every few seconds
            //     as opposed to keep going right as in gamepad
            value = getRepeatingCore( value );
            lastValue = value;
            return value;
        }

};

}

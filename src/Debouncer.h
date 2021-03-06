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
//
//      int actualState                       last value used without applying debounce
//      int effectiveState                    last debounced value
//
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
//
//      setInitialValue( state )              set the initial value during startup
//      deactivate()                          deactivate debouncing
//      activate()                            reactivate debouncing
//
//      flagWaitForKeyup()                    do not send any active state until inactive state is seen again
//                                            eg. user keeps a button pressed
//                                                after debounce time
//      cancelDebouncing()                    stop debouncing if beyond certain period (MinimumDebounceTime)
//                                            eg. signal already processed, so stop debouncing if for next call
//
//      updateLastValue()                     just internal last value
//      getKeyDown()                          process key so it only register once per click
//      getKeyUp()                            process key so it registers upon release
//      getRepeating()                        send multiple clicks if held down
//      uint16_t repeatDelayInMs              time before sending 1st repeated key
//      uint16_t repeatRateInMs               time before sending subsequent keys
//
//      REMOVED:
//      assignGetStateHandler( function )     assign a function to read the state
//      int debounce()                        use function from assignGetStateHandler()
//
//
//  Example: basic
//
//      Debouncer db = Debouncer();
//      bool result = db.debounce( digitalRead( 8 ) );
//
//  REMOVED!!! Example: assign state reader
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
#include <spUtility.h>

namespace StarterPack {

class Debouncer {

    public:

        struct Settings {
            uint16_t activeStatesDebounceInMs = 50;     // debouncing keyDown time
            uint16_t inactiveStateDebounceInMs = 50;    // debouncing keyUp time
            uint16_t minimumDebounceTimeInMs = 50;      // minimum debounce time before allowed to be cancelled 
            uint16_t confirmActiveStateTimeInMs = 0;    // time delay to confirm keyDown time
            uint16_t confirmInactiveStateTimeInMs = 0;  // time delay to confirm keyUp time
            uint16_t repeatDelayInMs = 400;
            uint16_t repeatRateInMs  = 250;
        };

        static Settings defaultSettings;            // global single instance setting to save space

    private:

        Settings *settings = &defaultSettings;
        bool mustDeleteSettings = false;

    public:

        ~Debouncer() {
            if ( mustDeleteSettings ) delete settings;
        }

        //int actualState = false;                    // actual physical state of the button
        int debouncedState = false;                 // state to consider taken into account debouncing and confirmation logic

        int inactiveState = false;                  // value that is considered the "keyUp"
        // uint16_t activeStatesDebounceInMs = 50;     // debouncing keyDown time
        // uint16_t inactiveStateDebounceInMs = 50;    // debouncing keyUp time
        // uint16_t minimumDebounceTimeInMs = 50;      // minimum debounce time before allowed to be cancelled 
        // uint16_t confirmStateTimeInMs = 0;          // time delay to confirm a state

        void useCustomSettings() {
            if ( mustDeleteSettings ) return; // already customized
            this->settings = new Settings;
            mustDeleteSettings = true;
        }
        
        inline Settings* getSettings() {
            return settings;
        }

        void assignSettings( Settings &settings ) {
            if ( mustDeleteSettings ) delete this->settings;
            mustDeleteSettings = false;
            this->settings = &settings;
        }

        void setInitialValue( int state ) {
            //actualState = state;
            debouncedState = state;
            lastValue = state;
            mode = mIdle;
            repeatMode = rIdle;
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

    // private:

    //     bool active = true;

    // public:

    //     void deactivate() {
    //         active = false;
    //         effectiveState = actualState;
    //         mode = mIdle;
    //         repeatMode = rIdle;
    //     }

    //     void activate() {
    //         active = true;
    //         mode = mIdle;
    //         repeatMode = rIdle;
    //     }

        // int debounce( int state ) {
        //     //actualState = state; // keep copy for user
        //     // if ( !active ) {
        //     //     effectiveState = state;
        //     //     return effectiveState;
        //     // }
        //     return debounceCore( state );
        // }

    /*
    public:
        int debounce() {
            // auto get value:
            // int readValue() { return digitalRead(10); }
            // db.assignGetStateHandler( &readValue );
            // db.debounce(); // <-- will call readValue() automatically
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
    */

    //
    // CORE
    //

    private:

        //  pressed
        //  confirm     start       end
        //  debounce                start     ~ end
        //  repeat                  start     ~ 1st ~ next ~next
        //  +-----------+-----------+------------>
        //
        //  can combine storage for: confirmStart, debounceStart
        //  can combine storage for: confirmKey, repeatKey
        //  cannot combine storage for: confirmKey, repeatKey
        //     while repeating, new key is pressed
        //  must separate debounce and repeat since can overlap
        
        enum modes {
            mIdle = 0,
            mConfirm,
            mDebouncing
        };
        modes mode = mIdle;

        // will confirm before debounce
        union 
        // struct
        startingTimesUnion {
            uint32_t confirmStartTime;
            uint32_t debouncingStartTime;
        };
        startingTimesUnion startingTimes;

        // union
        // // struct 
        // keyToMonitorUnion {
        //     int keyToConfirm = -1;
        //     int keyBeingRepeated;
        // };
        // keyToMonitorUnion keyToMonitor;

        int keyToConfirm = -1;
        int keyBeingRepeated = -1;

        // uint32_t confirmStartTime;
        //int toConfirmState;

    public:

        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;

        int debounce( int currentState ) {
            
            uint32_t now;

            switch( mode ) {
            case modes::mIdle: JUMP_IDLE:
                if ( currentState == debouncedState ) {
                    // DEBUG_TRACE( Serial.print( "#" ) );
                } else {
                    //DEBUG_TRACE( Serial.println( "repeat cancelled" ) );
                    //repeatMode = rIdle;
                    // confirm if:
                    // - if "keyUp"   and has keyUp   confirmation time
                    // - if "keyDown" and has keyDown confirmation time
                    //
                    // ... worst case 3 comparisons
                    //    if ( ( currentState == inactiveState && settings->confirmInactiveStateTimeInMs == 0 )
                    //      || ( currentState != inactiveState && settings->confirmActiveStateTimeInMs == 0 ) ) {

                    //     // no confirmation required
                    //     DEBUG_TRACE( if ( currentState != inactiveState ) SerialPrintf( "accepted: %d\n", currentState ) );
                    //     updateState( currentState );
                    // } else {
                    //     // confirm first
                    //     DEBUG_TRACE( if ( currentState != inactiveState ) SerialPrintf( "to confirm: %d\n", currentState ) );
                    //     keyToMonitor.keyToConfirm = currentState;
                    //     startingTimes.confirmStartTime = millis();
                    //     mode = modes::mConfirm;
                    // }
                    if ( currentState == inactiveState ) {
                        // keyUp
                        if ( settings->confirmInactiveStateTimeInMs == 0 ) {
                            // no need to confirm
                            // DEBUG_TRACE( SerialPrintf( "accepted: %d\n", currentState ) );
                            updateState( currentState );
                        } else {
                            // confirm first
                            // DEBUG_TRACE( SerialPrintf( "to confirm: %d\n", currentState ) );
                            JUMP_TO_CONFIRM:
                            keyToConfirm = currentState;
                            startingTimes.confirmStartTime = millis();
                            mode = mConfirm;
                            goto JUMP_CONFIRM;
                        }
                    } else {
                        // keyDown
                        if ( settings->confirmActiveStateTimeInMs == 0 ) {
                            // no need to confirm
                            DEBUG_TRACE( SerialPrintf( "accepted: %d\n", currentState ) );
                            updateState( currentState );
                        } else {
                            // confirm first
                            DEBUG_TRACE( SerialPrintf( "to confirm: %d\n", currentState ) );
                            goto JUMP_TO_CONFIRM;
                            // keyToMonitor.keyToConfirm = currentState;
                            // startingTimes.confirmStartTime = millis();
                            // mode = mConfirm;
                        }
                    }
                    // if ( settings->confirmStateTimeInMs == 0 ) {
                    //     // no need to confirm
                    //     DEBUG_TRACE( SerialPrintf( "accepted: %d\n", currentState ) );
                    //     updateState( currentState );
                    // } else {
                    //     // confirm first
                    //     DEBUG_TRACE( if ( currentState != 0 ) )
                    //     DEBUG_TRACE(    SerialPrintf( "to confirm: %d\n", currentState ) );
                    //     keyToMonitor.keyToConfirm = currentState;
                    //     startingTimes.confirmStartTime = millis();
                    //     mode = mConfirm;
                    // }
                }
                break;
            case modes::mConfirm: JUMP_CONFIRM:
                // confirm:
                // - inactive state
                //   if keyUp, check time
                //   if now a keyDown, new confirmation
                // - active state
                //   if same key, check time
                //   if keyUp, abort confirmation
                //   if different key, new confirmation
                now = millis();
                if ( keyToConfirm == inactiveState ) {                        
                    // confirming keyUp
                    if ( currentState == inactiveState ) {
                        // ... still keyUp
                        if ( now - startingTimes.confirmStartTime >= settings->confirmInactiveStateTimeInMs ) {
                            // confirmed
                            // DEBUG_TRACE( SerialPrintf( "confirmed: %d\n", currentState ) );
                            updateState( currentState );
                        } else {
                            // still confirming, return previous debouncedState for now
                            // DEBUG_TRACE( SerialPrintf( "still confirming: %d\n", currentState ) );
                        }
                    } else {
                        // ... a key was pressed, reset
                        DEBUG_TRACE( SerialPrintf( "new key while confirming 1: %d\n", currentState ) );
                        mode = modes::mIdle;
                        //debouncedState = inactiveState;
                        goto JUMP_IDLE;
                        // keyToMonitor.keyToConfirm = currentState;
                        // startingTimes.confirmStartTime = now;
                    }
                } else {
                    // confirming keyDown
                    if ( keyToConfirm == currentState ) {
                        // ... still same key
                        if ( now - startingTimes.confirmStartTime >= settings->confirmActiveStateTimeInMs ) {
                            // confirmed
                            DEBUG_TRACE( SerialPrintf( "confirmed: %d\n", currentState ) );
                            updateState( currentState );
                        } else {
                            // still confirming, return previous debouncedState for now
                            DEBUG_TRACE( SerialPrintf( "still confirming: %d\n", currentState ) );
                        }                        
                    } else if ( currentState == inactiveState ) {
                        // ... released (or jittle during press), meaning not confirmed
                        // abort confirmation
                        //debouncedState = currentState;
                        DEBUG_TRACE( SerialPrintf( "not confirmed: %d\n", keyToConfirm ) );
                        mode = modes::mIdle;
                        goto JUMP_IDLE;
                    } else {
                        // ... another key pressed (or analog button not yet stable)
                        // confirm again or ignore ??? confirm...
                        //    if fed high frequency changing signal will go on forever confirming
                        //    and keep returning the last confirmed state, which is technically 
                        //    still correct since new state cannot be confirmed
                        DEBUG_TRACE( SerialPrintf( "new key while confirming 2: %d\n", currentState ) );
                        //debouncedState = inactiveState;
                        mode = modes::mIdle;
                        goto JUMP_IDLE;
                        // DEBUG_TRACE( SerialPrintf( "new to confirm: %d\n", currentState ) );
                        // keyToMonitor.keyToConfirm = currentState;
                        // startingTimes.confirmStartTime = now;
                    }
                }
                // if ( keyToMonitor.keyToConfirm == currentState ) {
                //     // still same
                //     if ( now - startingTimes.confirmStartTime >= settings->confirmStateTimeInMs ) {
                //         // confirmed
                //         DEBUG_TRACE( SerialPrintf( "confirmed: %d\n", currentState ) );
                //         updateState( currentState );
                //     } else {
                //         // still confirming
                //         // will return debouncedState
                //         DEBUG_TRACE( SerialPrintf( "not yet confirmed: %d\n", currentState ) );
                //     }
                // } else {
                //     if ( currentState == inactiveState ) {
                //         // released or jittle during press ...
                //         debouncedState = currentState;
                //         goto JUMP_IDLE;
                //     } else {
                //         // pressed another key or analog button not yet stable

                //         // different !!! confirm again or ignore ??? confirm...
                //         // if fed high frequency signal will go on forever confirming
                //         // and return the last confirmed state, which is technically 
                //         // still correct since new state cannot be confirmed

                //         DEBUG_TRACE( SerialPrintf( "new to confirm: %d\n", currentState ) );
                //         keyToMonitor.keyToConfirm = currentState;
                //         startingTimes.confirmStartTime = now;
                //         // for now return same as before
                //     }
                // }
                /*
                if ( millis() - startingTimes.confirmStartTime >= settings->confirmStateTimeInMs ) {
                    if ( keyToMonitor.keyToConfirm == currentState ) {
                        // still same, confirmed
                        DEBUG_TRACE( SerialPrintf( "confirmed: %d\n", currentState ) );
                        updateState( currentState );
                    } else {
                        // different !!! confirm again or ignore ??? confirm...
                        // if fed high frequency signal will go on forever confirming
                        // and return the last confirmed state, which is technically 
                        // still correct since new state cannot be confirmed
                        DEBUG_TRACE( SerialPrintf( "reconfirm: %d\n", currentState ) );
                        keyToMonitor.keyToConfirm = currentState;
                        startingTimes.confirmStartTime = millis();
                        // for now return same as before
                    }
                } else {
                    DEBUG_TRACE( SerialPrintf( "not yet confirmed: %d\n", currentState ) );
                }
                */
                break;
            case modes::mDebouncing:
                if ( isDebouncing() ) {
                    // DEBUG_TRACE( if ( currentState != 0 ) SerialPrintf( "debounce: %d\n", currentState ) );
                } else {
                    mode = modes::mIdle;
                    goto JUMP_IDLE;
                }
                break;
            }

            if ( waitForKeyupFlag ) {
                // note: consider only inactive states after debouncing, otherwise false inactive
                //       hence check only after all operations
                // eg. input states:  0001010101111111 
                //     debounce of 1:    ========        do not consider 0's here
                //     check for 0:              ======= begin checking for 0's here
                //     ignore 0's while still debouncing 1's
                if ( debouncedState == inactiveState ) {
                    DEBUG_TRACE( SerialPrintf( "done waiting for keyup: %d\n", currentState ) );
                    waitForKeyupFlag = false;
                } else {
                    DEBUG_TRACE( Serial.print( "w" ) );
                }
                return inactiveState;
            }
            return debouncedState;
        }

        #undef DEBUG_TRACE

    //
    // KEY UP
    //
    private:

        bool waitForKeyupFlag = false;

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
                uint32_t ellapsed = now - startingTimes.debouncingStartTime;
                if ( ellapsed < settings->minimumDebounceTimeInMs ) {
                    // still debouncing, adjust time to stop debouncing once minimum is reached
                    //                          |<-- min -->|
                    //  ------------------------|--------|----------------------|
                    //                          start   now |                   end
                    //                          |           |
                    // -----|-------------------------------|
                    //      start                           end
                    //      adjust start so end will
                    //      be same as min time
                    uint32_t shortage = settings->minimumDebounceTimeInMs - ellapsed;
                    if ( debouncedState == inactiveState ) {
                        // debouncing inactive state
                        startingTimes.debouncingStartTime = now + shortage - settings->inactiveStateDebounceInMs;
                    } else {
                        // debouncing active state
                        startingTimes.debouncingStartTime = now + shortage - settings->activeStatesDebounceInMs;
                    }
                } else {
                    // already reached minimum, stop
                    //effectiveState = actualState;
                    mode = mIdle;
                }
            }
        }

    //
    // DEBOUNCE
    //
    public:

        // uint32_t debouncingStartTime;
        
        inline bool isDebouncing() {
            uint32_t now = millis();
            // debounce time of inactive and active are possibly different
            if ( debouncedState == inactiveState ) {
                if ( now - startingTimes.debouncingStartTime < settings->inactiveStateDebounceInMs ) {
                    // still debouncing, maintain effectiveState
                    return true;
                }
            } else {
                if ( now - startingTimes.debouncingStartTime < settings->activeStatesDebounceInMs ) {
                    // still debouncing, maintain effectiveState
                    return true;
                }
            }
            return false;
        }

    private:

        void updateState( int currentState ) {
            if ( currentState != debouncedState ) {
                // change state, start debounce
                mode = mDebouncing;
                startingTimes.debouncingStartTime = millis();
                debouncedState = currentState;
            } else {
                // same as before
                mode = mIdle;
            }
        }

    //
    // BUTTONS
    //
    private:

        // value send to user, not actual state
        // cannot combine with debouncedState
        // eg. while pressed debounceState = HIGH
        //     but repeating key is LOW..LOW, HIGH, LOW..LOW, HIGH, ...
        int lastValue = -1;

    public:

        // void updateLastValue( int value ) {
        //     lastValue = value;
        // }
        int getContinuousKey( int value ) {
            lastValue = value;
            return value;
        }

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

    //
    // REPEATING
    //
    private:

        enum repeatModeStates {
            rIdle,
            rSentFirstKey, // waiting for [repeatDelay] to go into Repeating
            rRepeating     // send every [repeatRate]
        };
        repeatModeStates repeatMode = rIdle;

        // int repeatValue = 0;
        uint32_t lastRepeatActionTime;

    public:

        // uint16_t repeatDelayInMs = 400;
        // uint16_t repeatRateInMs  = 250;

        int getRepeatingKey( int value ) {
            // if user keeps button held down send signal intermitently
            // eg. selecting an option, by sending right key every few seconds
            //     as opposed to keep going right as in gamepad
            value = getRepeatingCore( value );
            lastValue = value;
            return value;
        }

    private:

        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;

        int getRepeatingCore( int current ) {
            //actualState = current;
            if ( current != inactiveState ) {
                uint32_t now = millis();
                switch( repeatMode ) {
                case rIdle:
                    _IDLE:
                    repeatMode = rSentFirstKey;
                    keyBeingRepeated = current;
                    lastRepeatActionTime = now;
                    DEBUG_TRACE( SerialPrintfln( "1st: %d", current ) );
                    return current;
                case rSentFirstKey: {
                        if ( current != keyBeingRepeated ) {
                            // pressed another key
                            DEBUG_TRACE( Serial.println( "2nd: diff" ) );
                            goto _IDLE;
                            break;
                        }
                        if ( now - lastRepeatActionTime >= settings->repeatDelayInMs ) {
                            repeatMode = rRepeating;
                            lastRepeatActionTime = now;
                            DEBUG_TRACE( SerialPrintfln( "2nd: %d", current ) );
                            return current;
                        } else {
                            return inactiveState;
                        }
                    }
                    break;
                case rRepeating: {
                        if ( current != keyBeingRepeated ) {
                            // pressed another key
                            DEBUG_TRACE( Serial.println( "Nth: diff" ) );
                            goto _IDLE;
                            break;
                        }                    
                        if ( now - lastRepeatActionTime >= settings->repeatRateInMs ) {
                            lastRepeatActionTime = now;
                            DEBUG_TRACE( SerialPrintfln( "Nth: %d", current ) );
                            return current;
                        } else {
                            return inactiveState;
                        }
                    }
                    break;
                }
            }
            repeatMode = rIdle;
            return current;
            // return actualState;
        }

        #undef DEBUG_TRACE

};

Debouncer::Settings Debouncer::defaultSettings;

}

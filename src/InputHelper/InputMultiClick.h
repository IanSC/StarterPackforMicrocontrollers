// detect multiple clicks
// eg. double-click, triple-click
//
//   INPUT                               OUTPUT
//   sendRepeatedKeys   User-Action      multiClickCount   isLongPressed   isRepeated
//   ----------------   -------------    ---------------   -------------   ----------
//   - ANY -            1 click          1                 FALSE           FALSE
//   - ANY -            2 clicks         2                 FALSE           FALSE
//   FALSE              1 click, hold    1                 TRUE            FALSE
//   FALSE              2 click, hold    2                 TRUE            FALSE
//   TRUE               1 click, hold    1                 TRUE            FALSE  ... 1st sent is not flagged as repeated
//                                       1                 TRUE            TRUE   ... continues while pressed
//   TRUE               2 click, hold    2                 TRUE            FALSE
//                                       2                 TRUE            TRUE   ... continues while pressed
//
//   slower response time the simple repeat
//   maxClickIntervalInMs+debounce has to pass, before key is sent
//   whereas, repeat will send immediately after debounce
//      so user will feel the lag if single-clicking

#pragma once

#include <Arduino.h>
#include <inttypes.h>

#include <InputHelper/InputFilterInterface.h>
#include <InputHelper/InputMultiClickMapper.h>

namespace StarterPack {

template<typename DATA_TYPE>
class InputMultiClick : public InputFilterInterface<DATA_TYPE>,
                        public InputMultiClickMapper<DATA_TYPE> {

    private:

        // typedef T DATA_TYPE;
        static constexpr DATA_TYPE INACTIVE_KEY = 0;

    //
    // FILTER BASE
    //
    public:
        inline DATA_TYPE actionApplyFilter( DATA_TYPE value ) override {
            return actionGetMultiClickKey(value);
        }

    //
    // SETTINGS
    //
    public:

        // maxClickIntervalInMs       maximum time between clicks allowed
        // multiClickSendRepeated     keep sending keystrokes if held down
        //                            repeat delay same as maxClickIntervalInMs
        // multiClickRepeatRateInMs   succeeding keys
        #if defined(SP_INPUTMULTICLICK_MAX_CLICK_INTERVAL_MS)
            uint16_t multiClickMaxIntervalInMs = SP_INPUTMULTICLICK_MAX_CLICK_INTERVAL_MS;
        #else
            uint16_t multiClickMaxIntervalInMs = 400;
        #endif
        #if defined(SP_INPUTMULTICLICK_REPEAT_IF_KEPT_PRESSED)
            uint16_t multiClickSendRepeatedKeys = SP_INPUTMULTICLICK_REPEAT_IF_KEPT_PRESSED;
        #else
            bool multiClickSendRepeatedKeys = false;
        #endif
        #if defined(SP_INPUTMULTICLICK_REPEAT_RATE_MS)
            uint16_t multiClickRepeatRateInMs = SP_INPUTMULTICLICK_REPEAT_RATE_MS;
        #else
            uint16_t multiClickRepeatRateInMs = 250;
        #endif

        inline void setMultiClickSettingsInMs(uint16_t maxInterval, uint16_t repeatRate=0) {
            multiClickMaxIntervalInMs = maxInterval;
            multiClickSendRepeatedKeys = (repeatRate!=0);
            multiClickRepeatRateInMs = repeatRate;
        }

        void copyMultiClickSettings(InputMultiClick &instance) {
            multiClickMaxIntervalInMs  = instance.multiClickMaxIntervalInMs;
            multiClickSendRepeatedKeys = instance.multiClickSendRepeatedKeys;
            multiClickRepeatRateInMs   = instance.multiClickRepeatRateInMs;
        }

    //
    // ACTION
    //
    private:

        enum class mClickModeEnum : uint8_t {
            Idle,                   // waiting
            CountingWaitForRelease, // processing, waiting for key release
            CountingWaitForPress,   // processing, waiting for another press
            WaitForRelease,         // waiting for release of last processed key
            SendRepeatedKey         // keep sending keys if kept pressed
        };
        mClickModeEnum mClickMode = mClickModeEnum::Idle;

        unsigned long mClickLastKeyDownTime;
        DATA_TYPE mClickTargetKey = -1;
        uint8_t mClickCount = 0;

    public:

        // if INACTIVE_KEY, all of these are invalid
        // not specifically reset to save time

        uint8_t multiClick_count = 0;           // number of clicks detected
        bool multiClick_isLongPressed = false;  // if button was kept pressed
        bool multiClick_isRepeated = false;     // if kept pressed, flag that key is from auto-repeat logic

        inline DATA_TYPE actionGetMultiClickKey( DATA_TYPE current ) {
            // use settings if repeating or not
            auto key = actionGetMultiClickCore(current,multiClickSendRepeatedKeys);
            return InputMultiClickMapper<DATA_TYPE>::actionMultiClickMapKey(
                key, multiClick_count, multiClick_isLongPressed, multiClick_isRepeated );
        }
        inline DATA_TYPE actionGetMultiClickRepeated( DATA_TYPE current ) {
            // force repeated mode, disregard setting
            auto key = actionGetMultiClickCore(current,true);
            return InputMultiClickMapper<DATA_TYPE>::actionMultiClickMapKey(
                key, multiClick_count, multiClick_isLongPressed, multiClick_isRepeated );
        }
        inline DATA_TYPE actionGetMultiClickNonRepeated( DATA_TYPE current ) {
            // force non-repeated mode, disregard setting
            auto key = actionGetMultiClickCore(current,false);
            return InputMultiClickMapper<DATA_TYPE>::actionMultiClickMapKey(
                key, multiClick_count, multiClick_isLongPressed, multiClick_isRepeated );
        }

    protected:

        // #define DEBUG_TRACE(x)   x;
        #define DEBUG_TRACE(x)   ;

        DATA_TYPE actionGetMultiClickCore( DATA_TYPE current, bool repeatedMode ) {

            bool hasTimedOut;

            switch( mClickMode ) {

            case mClickModeEnum::Idle:
            _CASE_IDLE:

                if (current == INACTIVE_KEY)
                    return INACTIVE_KEY;

                mClickMode = mClickModeEnum::CountingWaitForRelease;
                mClickTargetKey = current;
                mClickLastKeyDownTime = millis();
                mClickCount = 1;

                DEBUG_TRACE( Serial.print( "[#]" ) );
                return INACTIVE_KEY;

            case mClickModeEnum::CountingWaitForRelease:

                if (current!=INACTIVE_KEY && current!=mClickTargetKey) {
                    // not released and not key being processed ==> new key pressed
                    // send to user, and process new key
                    goto SEND_AND_PROCESS_CURRENT_KEY_PRESSED;
                }

                hasTimedOut = (millis() - mClickLastKeyDownTime) > multiClickMaxIntervalInMs;

                if (hasTimedOut) {
                    DEBUG_TRACE( Serial.print( "[TO-1]" ) );

                    //
                    // TOO LATE
                    //
                    if (current == mClickTargetKey) {
                        // still pressed, must wait for its release
                        // if (multiClickSendRepeatedKeys) {
                        if (repeatedMode) {
                             mClickMode = mClickModeEnum::SendRepeatedKey;
                             mClickLastKeyDownTime = millis();
                        } else
                             mClickMode = mClickModeEnum::WaitForRelease;
                        multiClick_isLongPressed = true;
                    } else { // current == INACTIVE_KEY
                        // released, wait for next key
                        mClickMode = mClickModeEnum::Idle;
                        multiClick_isLongPressed = false;
                    }
                    // send to user
                    multiClick_count = mClickCount;
                    multiClick_isRepeated = false;
                    return mClickTargetKey;

                } else {

                    //
                    // ENOUGH TIME
                    //
                    if ( current == INACTIVE_KEY ) {
                        // released, wait for next press
                        mClickMode = mClickModeEnum::CountingWaitForPress;
                    } else {
                        // current == mClickTargetKey
                        // same key still being held down, continue waiting for release
                    }
                    return INACTIVE_KEY;

                }
                // ... this won't be reached, already jumped in all cases
                
            case mClickModeEnum::CountingWaitForPress:

                if (current!=INACTIVE_KEY && current!=mClickTargetKey) {
                    // not released and not key being processed ==> new key being pressed
                    // send to user, and process new key
                    goto SEND_AND_PROCESS_CURRENT_KEY_PRESSED;
                }

                hasTimedOut = (millis() - mClickLastKeyDownTime) > multiClickMaxIntervalInMs;

                if (hasTimedOut) {
                    DEBUG_TRACE( Serial.print( "[TO-2]" ) );

                    //
                    // TOO LATE
                    //
                    if (current == mClickTargetKey) {
                        // pressed same key
                        // since too late, send current one
                        // but process again as part of the next bunch
                        goto SEND_AND_PROCESS_CURRENT_KEY_PRESSED;
                    } else { // current == INACTIVE_KEY
                        // still not pressed, sail away
                        mClickMode = mClickModeEnum::Idle;
                        multiClick_count = mClickCount;
                        multiClick_isLongPressed = false;
                        multiClick_isRepeated = false;
                        return mClickTargetKey;
                    }

                } else {

                    //
                    // ENOUGH TIME
                    //
                    if (current == mClickTargetKey) {
                        // pressed same key, increment and wait for release
                        mClickMode = mClickModeEnum::CountingWaitForRelease;
                        mClickLastKeyDownTime = millis();
                        mClickCount++;
                    } else {
                        // current == INACTIVE_KEY
                        // nothing pressed yet, continue waiting
                    }
                    return INACTIVE_KEY;
                    
                }
                // ... this won't be reached, already jumped in all cases

            case mClickModeEnum::WaitForRelease:

                // timeout occurred while waiting for key to be released
                // must wait for actual release otherwise will send repeated keys
                // ex. pressed [A],[A],[A], but last key not released
                //     send [A]x3
                //     must wait for release of [A]
                //     do not keep processing [A]

                if (current == mClickTargetKey) {
                    // still pressed, continue waiting
                    return INACTIVE_KEY;
                }

                // if nothing is pressed, _CASE_IDLE will do nothing                
                // if another key is pressed, process it immediately to count it's 1st press
                DEBUG_TRACE( Serial.print( "[>]" ) );
                mClickMode = mClickModeEnum::Idle;
                goto _CASE_IDLE;

            case mClickModeEnum::SendRepeatedKey:

                if (current == mClickTargetKey) {
                    uint32_t now = millis();
                    if ( now - mClickLastKeyDownTime >= multiClickRepeatRateInMs ) {
                        mClickLastKeyDownTime = now;
                        // isLongPressed = true;
                        multiClick_isRepeated = true;
                        return current;
                    } else {
                        // still waiting for next repeat cycle
                        return INACTIVE_KEY;
                    }
                }

                // if nothing is pressed, _CASE_IDLE will do nothing                
                // if another key is pressed, process it immediately to count it's 1st press
                DEBUG_TRACE( Serial.print( "[R]" ) );
                mClickMode = mClickModeEnum::Idle;
                goto _CASE_IDLE;
            }

            // tuck routine here ...
        SEND_AND_PROCESS_CURRENT_KEY_PRESSED:
            // pressed a different key, send current key being processed
            // ex. pressed [A],[A],[B] in quick succession
            //     send [A]x2
            //     but must hook [B], with count 1
            //     otherwise might get lost, eg. next call [B] is already released
            auto keyBeingProcessed = mClickTargetKey;
            multiClick_count = mClickCount;

            // hook new key
            mClickMode = mClickModeEnum::CountingWaitForRelease;
            mClickTargetKey = current;
            mClickLastKeyDownTime = millis();
            mClickCount = 1;
            multiClick_isLongPressed = false;
            multiClick_isRepeated = false;
            
            return keyBeingProcessed;

        }

        #undef DEBUG_TRACE
};

}

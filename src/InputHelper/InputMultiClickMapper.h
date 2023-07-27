#pragma once

#include <Utility/spVector.h>
#include <Utility/spMacros.h>

namespace StarterPack {

template<typename DATA_TYPE>
class InputMultiClickMapper {

    //
    // SETTINGS
    //
    protected:

        enum class boolEnum : uint8_t {
             bAny, bTrue, bFalse
        };

        struct multiClickMap {
            DATA_TYPE inputKey;
            uint8_t   count;
            boolEnum  isLongPressed = boolEnum::bAny;
            boolEnum  isRepeated = boolEnum::bAny;
            DATA_TYPE outputKey;
        };
        StarterPack::spVector<multiClickMap> multiClickMapList;

        // enum class Init : uint8_t {
        //     None = 0
        //     // CountMatch = 0,
        //     // CountIgnore = 1,
        //     LongPressedAny = 0,
        //     LongPressedYes = 1,
        //     LongPressedNo = 2,
        //     RepeatedAny = 0,
        //     RepeatedYes = 4,
        //     RepeatedNo = 8
        // };

        // CLASS_ENUM_MANIPULATION(Init);

    public:

        void clearMultiClickMap() {
            multiClickMapList.deletePayload = true;
            multiClickMapList.clear();
        }

        void addMultiClickMap( DATA_TYPE inputKey, uint8_t count, DATA_TYPE outputKey ) {
            // set count=0 to ignore count match
            auto e = new multiClickMap();
            multiClickMapList.insert(e);
            e->inputKey = inputKey;
            e->count = count;
            e->outputKey = outputKey;
        }

        // void addMap( DATA_TYPE inputKey, uint8_t count, DATA_TYPE outputKey, Init conditions ) {
        //     // set count=0 to ignore count match
        //     auto e = new multiClickMap();
        //     multiClickMapList.insert(e);
        //     e->inputKey = inputKey;
        //     e->count = count;
        //     e->outputKey = outputKey;

        //     if ( ( conditions & Init::LongPressedYes ) == Init::LongPressedYes )
        //         e->isLongPressed = boolEnum::bTrue;
        //     else if ( ( conditions & Init::LongPressedNo ) == Init::LongPressedNo )
        //         e->isLongPressed = boolEnum::bFalse;
        //     if ( ( conditions & Init::RepeatedYes ) == Init::RepeatedYes )
        //         e->isRepeated = boolEnum::bTrue;
        //     else if ( ( conditions & Init::RepeatedNo ) == Init::RepeatedNo )
        //         e->isRepeated = boolEnum::bFalse;
        // }

        void addMultiClickMap( DATA_TYPE inputKey, uint8_t count, DATA_TYPE outputKey, bool longPressedStatus, bool repeatedStatus ) {
            auto e = new multiClickMap();
            multiClickMapList.insert(e);
            e->inputKey = inputKey;
            e->count = count;
            e->outputKey = outputKey;
            if (longPressedStatus)
                e->isLongPressed = boolEnum::bTrue;
            else
                e->isLongPressed = boolEnum::bFalse;
            if (repeatedStatus)
                e->isRepeated = boolEnum::bTrue;
            else
                e->isRepeated = boolEnum::bFalse;
        }

        //
        // SINGLE CONDITION
        //

        void addMultiClickMap_LongPressed( DATA_TYPE inputKey, uint8_t count, DATA_TYPE outputKey, bool longPressedStatus ) {
            auto e = new multiClickMap();
            multiClickMapList.insert(e);
            e->inputKey = inputKey;
            e->count = count;
            e->outputKey = outputKey;
            if (longPressedStatus)
                e->isLongPressed = boolEnum::bTrue;
            else
                e->isLongPressed = boolEnum::bFalse;
        }

        void addMultiClickMap_Repeated( DATA_TYPE inputKey, uint8_t count, DATA_TYPE outputKey, bool repeatedStatus ) {
            auto e = new multiClickMap();
            multiClickMapList.insert(e);
            e->inputKey = inputKey;
            e->count = count;
            e->outputKey = outputKey;
            if (repeatedStatus)
                e->isRepeated = boolEnum::bTrue;
            else
                e->isRepeated = boolEnum::bFalse;
        }

    //
    // ACTION
    //
    public:

        bool multiClickMapClearInfoIfMatched = true;

        DATA_TYPE actionMultiClickMapKey( DATA_TYPE inputKey, uint8_t &count, bool &isLongPressed, bool &isRepeated ) {
            auto e = multiClickMapList.getFirst();
            while( e != nullptr ) {
                if ( e->inputKey != inputKey ) goto NEXT;
                if ( e->count != 0 && e->count != count ) goto NEXT;
                if ( e->isLongPressed != boolEnum::bAny ) {
                    if ( e->isLongPressed == boolEnum::bTrue ) {
                        if ( !isLongPressed ) goto NEXT;
                    } else { // e->isLongPressed == boolEnum::bFalse
                        if ( isLongPressed ) goto NEXT;
                    }
                }
                if ( e->isRepeated != boolEnum::bAny ) {
                    if ( e->isRepeated == boolEnum::bTrue ) {
                        if ( !isRepeated ) goto NEXT;
                    } else { // e->isRepeated == boolEnum::bFalse
                        if ( isRepeated ) goto NEXT;
                    }
                }
                // matched !
                if (multiClickMapClearInfoIfMatched) {
                    count = 1;
                    isLongPressed = false;
                    isRepeated = false;
                }
                return e->outputKey;
            NEXT:
                e = multiClickMapList.getNext();
            }
            return inputKey;
        }

};

}

#pragma once

#include <stdint.h>

#include <Utility/spStr.h>
#include <InputHelper/InputFilterInterface.h>

namespace StarterPack {

template<typename DATA_TYPE>
class InputKeyFilterCore { // : public InputFilterInterface<DATA_TYPE> {

    private:

        static constexpr DATA_TYPE INACTIVE_KEY = 0;

    // //
    // // FILTER BASE
    // //
    // public:
    //     inline DATA_TYPE actionApplyFilter( DATA_TYPE value ) override {
    //         return actionFilterKey(value);
    //     }

    //
    // SETTINGS
    //
    private:

        const char *allowedKeys = nullptr;
        const char *disallowedKeys = nullptr;

    public:

        inline void setAllowedKeys( const char *list ) {
            allowedKeys = list;
        }

        inline void setDisallowedKeys( const char *list ) {
            disallowedKeys = list;
        }

    //
    // ACTION
    //
    public:

        bool isKeyAllowed( DATA_TYPE key ) {
            if ( allowedKeys != nullptr ) {
                if ( Str::findCharacter( key, allowedKeys ) )
                    return true;
                return false;
            }
            if ( disallowedKeys != nullptr ) {
                if ( Str::findCharacter( key, disallowedKeys ) )
                    return false;
                return true;
            }
            return true;
        }

        DATA_TYPE actionFilterKey( DATA_TYPE key ) {
            if ( isKeyAllowed( key ) )
                return key;
            return INACTIVE_KEY;
        }

};

template<typename DATA_TYPE>
class InputKeyFilter : public InputKeyFilterCore<DATA_TYPE>,
                       public InputFilterInterface<DATA_TYPE> {

    //
    // FILTER BASE
    //
    public:
        inline DATA_TYPE actionApplyFilter( DATA_TYPE value ) override {
            return actionFilterKey(value);
        }

};

}

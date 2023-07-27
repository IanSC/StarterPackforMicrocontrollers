#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace StarterPack {

template<typename DATA_TYPE>
class InputFilterInterface {

    // private:

    //     typedef T DATA_TYPE;

    public:
        virtual ~InputFilterInterface() {}

    //
    // VIRTUAL
    //
    public:

        virtual DATA_TYPE actionApplyFilter( DATA_TYPE value ) = 0;
        
};

}

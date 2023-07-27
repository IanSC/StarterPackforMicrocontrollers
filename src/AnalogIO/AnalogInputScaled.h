#pragma once

#include <AnalogIO/AnalogInputRaw.h>
#include <InputHelper/InputLinearScale.h>

#include <Utility/spRangeMap.h>

namespace StarterPack {

class AnalogInputScaled : public AnalogInputRaw, public InputLinearScale<int> {

    public:

        AnalogInputScaled( uint8_t pin ) : AnalogInputRaw( pin ) {}

    //
    //
    //
    public:

        int readScaled() {
            auto value = readRaw();
            return InputLinearScale::actionApplyLineaScale(value);
        }

        float readScaledFloat() {
            auto value = readRaw();
            return InputLinearScale::actionApplyLineaScaleFloat(value);
        }

};

}

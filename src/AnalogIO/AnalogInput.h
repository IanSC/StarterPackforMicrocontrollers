#pragma once

#include <AnalogIO/AnalogInputBase.h>
#include <Utility/spRangeMap.h>

namespace StarterPack {

class AnalogInput : public AnalogInputBase {

    public:

        AnalogInput( uint8_t pin ) : AnalogInputBase( pin ) {}

    //
    // SCALE
    //
    protected:

        int srcLow, srcHigh;
        int dstLow, dstHigh;

    public:

        inline void setScale(int srcLow, int srcHigh, int dstLow, int dstHigh) {
            this->srcLow = srcLow; this->srcHigh = srcHigh;
            this->dstLow = dstLow; this->dstHigh = dstHigh;
        }

        inline void setSourceScale(int srcLow, int srcHigh) {
            this->srcLow = srcLow; this->srcHigh = srcHigh;
        }

        inline void setDestinationScale(int dstLow, int dstHigh) {
            this->dstLow = dstLow; this->dstHigh = dstHigh;
        }

        int readScaled() {
            auto value = readRaw();
            return StarterPack::RangeMap::IntInt(value,srcLow,srcHigh,dstLow,dstHigh);
        }

        float readScaledFloat() {
            auto value = readRaw();
            return StarterPack::RangeMap::IntFloat(value,srcLow,srcHigh,dstLow,dstHigh);
        }

};

}

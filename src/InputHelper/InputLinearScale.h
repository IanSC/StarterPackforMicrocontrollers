#pragma once

#include <Utility/spRangeMap.h>
#include <InputHelper/InputFilterInterface.h>

namespace StarterPack {

template<typename DATA_TYPE>
class InputLinearScale : public InputFilterInterface<DATA_TYPE> {

    private:
    
        // typedef T KEY;
        typedef int COEFF_TYPE; // coefficient typename

    //
    // FILTER BASE
    //
    public:
        inline DATA_TYPE actionApplyFilter( DATA_TYPE value ) override {
            return actionApplyLineaScale(value);
        }

    public:

        InputLinearScale() {}
        InputLinearScale(COEFF_TYPE srcLow, COEFF_TYPE srcHigh, COEFF_TYPE dstLow, COEFF_TYPE dstHigh) {
            this->srcLow = srcLow; this->srcHigh = srcHigh;
            this->dstLow = dstLow; this->dstHigh = dstHigh;
        }

    //
    // SETTINGS
    //
    private:

        COEFF_TYPE srcLow, srcHigh;
        COEFF_TYPE dstLow, dstHigh;

    public:

        inline void setScale(COEFF_TYPE srcLow, COEFF_TYPE srcHigh, COEFF_TYPE dstLow, COEFF_TYPE dstHigh) {
            this->srcLow = srcLow; this->srcHigh = srcHigh;
            this->dstLow = dstLow; this->dstHigh = dstHigh;
        }

        inline void setSourceScale(COEFF_TYPE srcLow, COEFF_TYPE srcHigh) {
            this->srcLow = srcLow; this->srcHigh = srcHigh;
        }

        inline void setDestinationScale(COEFF_TYPE dstLow, COEFF_TYPE dstHigh) {
            this->dstLow = dstLow; this->dstHigh = dstHigh;
        }

    //
    // ACTION
    //
    public:

        inline DATA_TYPE actionApplyLineaScale( DATA_TYPE value ) {
            // assume COEFF_TYPE = int
            return StarterPack::RangeMap::IntInt(value,srcLow,srcHigh,dstLow,dstHigh);
        }

        inline float actionApplyLineaScaleFloat( DATA_TYPE value ) {
            // assume COEFF_TYPE = int
            return StarterPack::RangeMap::IntFloat(value,srcLow,srcHigh,dstLow,dstHigh);
        }

};

}

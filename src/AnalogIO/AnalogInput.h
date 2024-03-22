#pragma once

#include <Utility/spVector.h>

#include <AnalogIO/AnalogInputRaw.h>
#include <InputHelper/InputFilterList.h>

namespace StarterPack {

class AnalogInput : public AnalogInputRaw, public InputFilterList<AnalogInputRaw::DATA_TYPE,int32_t> {

    public:

        typedef AnalogInputRaw::DATA_TYPE DATA_TYPE;

    public:

        AnalogInput( int8_t pin ) : AnalogInputRaw( pin ) {}

    //
    // FILTERS
    //
    public:

        DATA_TYPE readFiltered() {
            auto raw = readRaw();
            return InputFilterList::actionApplyFilter(raw);
        }

};

}

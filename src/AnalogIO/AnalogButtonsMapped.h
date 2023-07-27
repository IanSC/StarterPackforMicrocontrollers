#pragma once

#include <AnalogIO/AnalogInput.h>
#include <InputHelper/InputSlotter.h>
#include <InputHelper/InputKeyMapper.h>

namespace StarterPack {

class AnalogButtonsMapped : public AnalogInput,
                            public InputSlotter<AnalogInput::DATA_TYPE,uint8_t>, public InputKeyMapper<uint8_t,uint8_t> {

    public:

        typedef uint8_t KEY;
        static constexpr KEY INACTIVE_KEY = 0;

    public:

        AnalogButtonsMapped( uint8_t pin ) : AnalogInput( pin ) {}

        virtual KEY read() {
            auto value = readFiltered();
            value = InputSlotter::actionFindSlot(value);
            value = InputKeyMapper::actionMapKey(value);
            return value;
        }

};

}

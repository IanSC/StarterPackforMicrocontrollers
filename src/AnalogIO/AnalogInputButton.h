#pragma once

#include <AnalogIO/AnalogInput.h>
#include <InputHelper/InputSlotter.h>
#include <InputHelper/InputMapper.h>

namespace StarterPack {

class AnalogInputButton : public AnalogInput,
                          public InputSlotter, public InputMapper {

    public:

        AnalogInputButton( uint8_t pin ) : AnalogInput( pin ) {}

        uint8_t readSlot() {
            auto value = readRaw();
            value = InputSlotter::actionFindSlot(value);
            value = InputMapper::actionMapKey(value);

            return value;
        }

};

}

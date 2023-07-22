#pragma once

#include <AnalogIO/AnalogInput.h>
#include <InputHelper/InputSlotter.h>
#include <InputHelper/InputMapper.h>

namespace StarterPack {

class AnalogInputButton : public AnalogInput,
                          public InputSlotter, public InputMapper {

    protected:

        typedef uint8_t KEY;
        static constexpr KEY INACTIVE_KEY = 0;

    public:

        AnalogInputButton( uint8_t pin ) : AnalogInput( pin ) {}

        virtual KEY readSlot() {
            auto value = readRaw();
            value = InputSlotter::actionFindSlot(value);
            value = InputMapper::actionMapKey(value);
            return value;
        }

};

}

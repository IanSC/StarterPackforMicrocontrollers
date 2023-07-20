#pragma once

#include <DigitalIO/DigitalInputGroupBase.h>
#include <UserInterface/UserInterfaceRepeated.h>

namespace StarterPack {

class DigitalInputGroupRepeated : public DigitalInputGroupBase,
                                  public UserInterfaceRepeated {

    protected:

        typedef UserInterfaceRepeated::KEY KEY;
        static constexpr KEY INACTIVE_KEY = UserInterfaceRepeated::INACTIVE_KEY;

    //
    // VIRTUAL OVERRIDES
    //
    public:

        inline KEY getNonDebouncedKey() override {
            return DigitalInputGroupBase::getNonDebouncedKey();
        }

};

}

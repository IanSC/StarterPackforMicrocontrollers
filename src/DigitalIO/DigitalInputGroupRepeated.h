#pragma once

#include <DigitalIO/DigitalInputGroupDebounced.h>
#include <DigitalIO/DigitalInputGroupBase.h>
#include <UserInterface/UserInterfaceRepeated.h>

namespace StarterPack {

class DigitalInputGroupRepeated : public DigitalInputGroupDebounced, //Base,
                                  public UserInterfaceRepeated {

    protected:

        typedef UserInterfaceRepeated::KEY KEY;
        static constexpr KEY INACTIVE_KEY = UserInterfaceRepeated::INACTIVE_KEY;

    //
    // VIRTUAL OVERRIDES
    //
    public:

        // inline KEY getDebouncedKey() override {
        //     return DigitalInputGroupDebounced::getDebouncedKey();
        // }

        inline KEY getNonDebouncedKey() override {
            return DigitalInputGroupBase::getNonDebouncedKey();
        }

};

}

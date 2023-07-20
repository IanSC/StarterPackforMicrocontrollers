#pragma once

#include <DigitalIO/DigitalInputGroupBase.h>
#include <UserInterface/UserInterfaceKeyUpDown.h>

namespace StarterPack {

class DigitalInputGroupDebounced : public DigitalInputGroupBase,
                                   public UserInterfaceKeyUpDown {

    // possible to use each functionality by composition instead of inheritance
    // but will need extra pointer
    // eg. create object of InputCombiner, InputDebouncerBase, ...
    //     will need extra pointer for each

    protected:

        typedef UserInterfaceKeyUpDown::KEY KEY;
        static constexpr UserInterfaceKeyUpDown::KEY INACTIVE_KEY = UserInterfaceKeyUpDown::INACTIVE_KEY;

    public:

        virtual ~DigitalInputGroupDebounced() {}

    //
    // INPUTS
    //
    protected:

        struct digitalInputMap {
            DigitalInput *dIO = nullptr;
            char key;
        };
        StarterPack::spVector<digitalInputMap> digitalInputMapList;

    public:

        void addInput( DigitalInput &dIO, KEY key ) {
            auto e = new digitalInputMap();
            e->dIO = &dIO;
            e->key = key;
            digitalInputMapList.insert(e);
        }

    //
    // VIRTUAL OVERRIDES
    //
    public:

        inline KEY getNonDebouncedKey() override {
            return DigitalInputGroupBase::getNonDebouncedKey();
        }

};

}

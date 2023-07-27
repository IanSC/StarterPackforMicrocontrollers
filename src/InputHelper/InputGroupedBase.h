// Group the input of several digital or analog inputs.
//
// Before: if (button1.read()) do1();
//         if (button2.read()) do2();
// After:  InputGroupDebounced ioGrp;
//         ioGrp.addInput(button1,'1');
//         ioGrp.addInput(button2,'2');
//         switch(ioGrp.getKeyDown()) {
//         case '1': do1();
//         case '2': do2();
//         }
// Then:   change to keypad instead
//         AnalogInputButtonMapped aInput;
//         aInput.assignKeymap("123");
//         ... configure analog input
//         ioGrp.addInput(aInput);
//         switch(ioGrp.getKeyDown()) {
//         ... same

#pragma once

#include <Utility/spVector.h>

#include <DigitalIO/DigitalInput.h>
#include <AnalogIO/AnalogButtonsMapped.h>

#include <InputHelper/InputCombiner.h>

namespace StarterPack {

// class InputGroupedBase : public InputCombiner<uint8_t> {
class InputGroupedBase : public InputCombiner<char> {

    protected:

        // typedef uint8_t KEY;
        typedef char KEY;
        static constexpr KEY INACTIVE_KEY = 0;

    public:

        virtual ~InputGroupedBase() {}

    //
    // DIGITAL INPUTS
    //
    protected:

        struct digitalInputMap {
            DigitalInput *dIO = nullptr;
            KEY key;
        };
        StarterPack::spVector<digitalInputMap> digitalInputMapList;

    public:

        void clearDigitalInput() {
            digitalInputMapList.deletePayload = true;
            digitalInputMapList.clear();
        }

        void addInput( DigitalInput &dIO, KEY key ) {
            auto e = new digitalInputMap();
            e->dIO = &dIO;
            e->key = key;
            digitalInputMapList.insert(e);
        }
        void addInput( DigitalInput *dIO, KEY key ) {
            auto e = new digitalInputMap();
            e->dIO = dIO;
            e->key = key;
            digitalInputMapList.insert(e);
        }

    //
    // ANALOG BUTTONS
    //
    protected:

        struct analogInput {
            AnalogButtonsMapped *aIO = nullptr;
        };
        StarterPack::spVector<analogInput> analogInputList;

    public:

        void clearAnalogInput() {
            analogInputList.deletePayload = true;
            analogInputList.clear();
        }

        void addInput( AnalogButtonsMapped &aIO ) {
            auto e = new analogInput();
            e->aIO = &aIO;
            analogInputList.insert(e);
        }
        void addInput( AnalogButtonsMapped *aIO ) {
            auto e = new analogInput();
            e->aIO = aIO;
            analogInputList.insert(e);
        }

    //
    // READ KEYS
    //
    protected:

        static const uint8_t MAX_SIMULTANEOUS_KEYS = 5;
        char keysPressed[MAX_SIMULTANEOUS_KEYS+1] = "";

    public:

        char *readKeys() {
            // record each key pressed into global [keysPressed]
            uint8_t index = 0;

            //
            // DIGITAL
            //
            auto *dInput = digitalInputMapList.getFirst();
            while ( dInput != nullptr ) {
                if ( dInput->dIO->readLogicalRaw() ) {
                    keysPressed[index] = dInput->key;
                    index++;
                    if ( index >= MAX_SIMULTANEOUS_KEYS )
                        break;
                }
                dInput = digitalInputMapList.getNext();
            }

            //
            // ANALOG
            //
            auto *aInput = analogInputList.getFirst();
            while ( aInput != nullptr ) {
                auto key = aInput->aIO->read();
                if (key != AnalogButtonsMapped::INACTIVE_KEY) {
                    keysPressed[index] = key;
                    index++;
                    if ( index >= MAX_SIMULTANEOUS_KEYS )
                        break;
                }
                aInput = analogInputList.getNext();
            }

            keysPressed[index] = 0;

            // if (strlen(keysPressed)>0) {
            //     Serial.print("keys=");
            //     Serial.println(keysPressed);
            // }

            return keysPressed;
        }

    //
    // READ DEVICE
    //
    public:

        KEY getNonDebouncedKey() {

            // combine multiple keys into single key

            auto keysPressed = readKeys();
            if ( keysPressed == nullptr ) return INACTIVE_KEY;

            switch( strlen( keysPressed ) ) {
            case 0:
                return INACTIVE_KEY;
            case 1:
                return keysPressed[0];
            default:
                // combine keys into single key
                // ex. "UL" --> '1'
                return InputCombiner::actionCombineKeys(keysPressed);
            }
        }

};

}

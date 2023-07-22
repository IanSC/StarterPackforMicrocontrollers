#pragma once

#include <Utility/spVector.h>

#include <DigitalIO/DigitalInput.h>
#include <InputHelper/InputCombiner.h>

namespace StarterPack {

class DigitalInputGroupBase : public InputCombiner {

    // possible to use each functionality by composition instead of inheritance
    // but will need extra pointer
    // eg. create object of InputCombiner, InputDebouncerBase, ...
    //     will need extra pointer for each

    protected:

        typedef uint8_t KEY;
        static constexpr KEY INACTIVE_KEY = 0;

    public:

        virtual ~DigitalInputGroupBase() {}

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
    // READ KEYS
    //
    protected:

        static const uint8_t MAX_SIMULTANEOUS_KEYS = 5;
        char keysPressed[MAX_SIMULTANEOUS_KEYS+1] = "";

    public:

        char *readKeys() {
            // record each key pressed into global [keysPressed]
            uint8_t index = 0;
            digitalInputMap *p = digitalInputMapList.getFirst();
            while ( p != nullptr ) {
                if ( p->dIO->readLogicalRaw() ) {
                    keysPressed[index] = p->key;
                    index++;
                    if ( index >= MAX_SIMULTANEOUS_KEYS )
                        break;
                }
                p = digitalInputMapList.getNext();
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

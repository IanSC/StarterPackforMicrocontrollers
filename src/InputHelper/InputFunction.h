#pragma once

namespace StarterPack {

class InputFunction {

        typedef int (*intIntFunction) (int);

    //
    // SETTINGS
    //
    protected:

        intIntFunction mapFunction = nullptr;

    public:

        inline void setMapFunction( intIntFunction func ) {
            this->mapFunction = func;
        }

    //
    // ACTION
    //
    public:

        int actionApplyMapFunction( int input ) {
            if ( mapFunction != nullptr )
                return mapFunction( input );
            return input;
        }

};

}

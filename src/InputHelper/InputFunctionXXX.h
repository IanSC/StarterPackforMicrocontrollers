#pragma once

namespace StarterPack {

template<typename T>
class InputFunction {

    private:

        typedef T DATA_TYPE;

    //
    // SETTINGS
    //
    private:

        typedef int (*intIntFunction) (int);
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

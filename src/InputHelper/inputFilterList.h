#pragma once

#include <Utility/spVector.h>

#include <InputHelper/InputFilterInterface.h>

#include <InputHelper/InputLinearScale.h>
#include <InputHelper/InputRunningAvg.h>

#include <InputHelper/InputDebouncer.h>
#include <InputHelper/InputRepeater.h>
#include <InputHelper/InputMultiClick.h>

#include <InputHelper/InputSlotter.h>
#include <InputHelper/InputKeyMapper.h>

namespace StarterPack {

template<typename DATA_TYPE, typename SUM_DATA_TYPE>
class InputFilterList : public InputFilterInterface<DATA_TYPE> {

    // private:

    //     typedef T DATA_TYPE;

    public:

        ~InputFilterList() {
            // CANNOT DELETE FILTERS
            // might be globals
            auto *filter = filterList.getFirst();
            while ( filter != nullptr )
                delete filter;
        }

    //
    // LIST
    //
    protected:

        spVector<InputFilterInterface<DATA_TYPE>> filterList;

    public:

        void clearFilters() {
            filterList.deletePayload = true;
            filterList.clear();
        }

    protected:

        // DO NOT ALLOW ADDING MANUALLY
        // - sure ownership
        // - can delete without worrying if created on stack
        // - user might add to multiple list
        // void addFilter( InputFilterInterface<DATA_TYPE> &filter ) {
        //     filterList.insert(&filter);
        // }
        void addFilter( InputFilterInterface<DATA_TYPE> *filter ) {
            filterList.insert(filter);
        }

    //
    // ACTION
    //
    public:

        DATA_TYPE actionApplyFilter( DATA_TYPE value ) override {
            auto *filter = filterList.getFirst();
            while ( filter != nullptr ) {
                value = filter->actionApplyFilter(value);   
                filter = filterList.getNext();
            }
            return value;
        }

    //
    // FILTERS
    //
    public:

        //
        // RUNNING AVERAGE
        //
        InputRunningAvg<DATA_TYPE,SUM_DATA_TYPE> *addRunningAvg() {
            auto f = new InputRunningAvg<DATA_TYPE,SUM_DATA_TYPE>();
            addFilter(f);
            return f;
        }
        InputRunningAvg<DATA_TYPE,SUM_DATA_TYPE> *addRunningAvg(uint8_t storageSlots, DATA_TYPE fluctuationRange=0) {
            auto f = new InputRunningAvg<DATA_TYPE,SUM_DATA_TYPE>(storageSlots, fluctuationRange);
            addFilter(f);
            return f;
        }

        //
        // LINEAR SCALE
        //
        InputLinearScale<DATA_TYPE> *addLinearScale() {
            auto f = new InputLinearScale<DATA_TYPE>();
            addFilter(f);
            return f;
        }
        InputLinearScale<DATA_TYPE> *addLinearScale(int srcLow, int srcHigh, int dstLow, int dstHigh) {
            auto f = new InputLinearScale<DATA_TYPE>(srcLow,srcHigh,dstLow,dstHigh);
            addFilter(f);
            return f;
        }

    //
    // FILTERS EXTRA
    //
    // disable for now:
    // - 99.99% its should NOT be used for AnalogInput
    //   unless making a custom class
    // - AnalogButton has these built in
    // - if added by mistake, Slotter and KeyMap can easily be detected
    // - Debounce, Repeat, MultiClick might work but appear wonky
    protected:

        //
        // SLOTTER
        //
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter() {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 2, zero, v1 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 3, zero, v1, v2 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 4, zero, v1, v2, v3 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 5, zero, v1, v2, v3, v4 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 6, zero, v1, v2, v3, v4, v5 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 7, zero, v1, v2, v3, v4, v5, v6 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 8, zero, v1, v2, v3, v4, v5, v6, v7 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 9, zero, v1, v2, v3, v4, v5, v6, v7, v8 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, DATA_TYPE v9, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 10, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, DATA_TYPE v9, DATA_TYPE v10, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 11, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, DATA_TYPE v9, DATA_TYPE v10, DATA_TYPE v11, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 12, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, DATA_TYPE v9, DATA_TYPE v10, DATA_TYPE v11, DATA_TYPE v12, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 13, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, DATA_TYPE v9, DATA_TYPE v10, DATA_TYPE v11, DATA_TYPE v12, DATA_TYPE v13, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 14, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, DATA_TYPE v9, DATA_TYPE v10, DATA_TYPE v11, DATA_TYPE v12, DATA_TYPE v13, DATA_TYPE v14, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 15, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, DATA_TYPE v9, DATA_TYPE v10, DATA_TYPE v11, DATA_TYPE v12, DATA_TYPE v13, DATA_TYPE v14, DATA_TYPE v15, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 16, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, DATA_TYPE v9, DATA_TYPE v10, DATA_TYPE v11, DATA_TYPE v12, DATA_TYPE v13, DATA_TYPE v14, DATA_TYPE v15, DATA_TYPE v16, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 17, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, DATA_TYPE v9, DATA_TYPE v10, DATA_TYPE v11, DATA_TYPE v12, DATA_TYPE v13, DATA_TYPE v14, DATA_TYPE v15, DATA_TYPE v16, DATA_TYPE v17, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 18, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, DATA_TYPE v9, DATA_TYPE v10, DATA_TYPE v11, DATA_TYPE v12, DATA_TYPE v13, DATA_TYPE v14, DATA_TYPE v15, DATA_TYPE v16, DATA_TYPE v17, DATA_TYPE v18, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 19, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, DATA_TYPE v9, DATA_TYPE v10, DATA_TYPE v11, DATA_TYPE v12, DATA_TYPE v13, DATA_TYPE v14, DATA_TYPE v15, DATA_TYPE v16, DATA_TYPE v17, DATA_TYPE v18, DATA_TYPE v19, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 20, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19 );
            return f;
        }
        InputSlotter<DATA_TYPE,DATA_TYPE> *addSlotter( DATA_TYPE zero, DATA_TYPE v1, DATA_TYPE v2, DATA_TYPE v3, DATA_TYPE v4, DATA_TYPE v5, DATA_TYPE v6, DATA_TYPE v7, DATA_TYPE v8, DATA_TYPE v9, DATA_TYPE v10, DATA_TYPE v11, DATA_TYPE v12, DATA_TYPE v13, DATA_TYPE v14, DATA_TYPE v15, DATA_TYPE v16, DATA_TYPE v17, DATA_TYPE v18, DATA_TYPE v19, DATA_TYPE v20, bool useGuard=true ) {
            auto f = new InputSlotter<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->initSlotsN( useGuard, 21, zero, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20 );
            return f;
        }

        //
        // KEY MAPPER
        //
        InputKeyMapper<DATA_TYPE,DATA_TYPE> *addKeyMapper() {
            auto f = new InputKeyMapper<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            return f;
        }
        InputKeyMapper<DATA_TYPE,DATA_TYPE> *addKeyMapper(const char *keyMap) {
            auto f = new InputKeyMapper<DATA_TYPE,DATA_TYPE>();
            addFilter(f);
            f->addKeyMap(keyMap);
            return f;
        }

        //
        // DEBOUNCER
        //
        InputDebouncer<DATA_TYPE> *addDebouncer() {
            auto f = new InputDebouncer<DATA_TYPE>();
            addFilter(f);
            return f;
        }
        InputDebouncer<DATA_TYPE> *addDebouncer(
        uint16_t stabilizedTimePressedInMs,
        uint16_t stabilizedTimeReleasedInMs,
        uint16_t debounceDelayPressedInMs,
        uint16_t debounceDelayReleasedInMs) {
            auto f = new InputDebouncer<DATA_TYPE>();
            addFilter(f);
            f->debounceDelayPressedInMs   = debounceDelayPressedInMs;
            f->debounceDelayReleasedInMs  = debounceDelayReleasedInMs;
            f->stabilizedTimePressedInMs  = stabilizedTimePressedInMs;
            f->stabilizedTimeReleasedInMs = stabilizedTimeReleasedInMs;
            return f;
        }
        InputDebouncer<DATA_TYPE> *addDebouncer(
        uint16_t stabilizedTimePressedInMs,
        uint16_t stabilizedTimeReleasedInMs) {
            auto f = new InputDebouncer<DATA_TYPE>();
            addFilter(f);
            f->stabilizedTimePressedInMs  = stabilizedTimePressedInMs;
            f->stabilizedTimeReleasedInMs = stabilizedTimeReleasedInMs;
            return f;
        }

        //
        // REPEATER
        //
        InputRepeater<DATA_TYPE> *addRepeater() {
            auto f = new InputRepeater<DATA_TYPE>();
            addFilter(f);
            return f;
        }
        InputRepeater<DATA_TYPE> *addRepeater(uint16_t repeatDelayInMs, uint16_t repeatRateInMs) {
            auto f = new InputRepeater<DATA_TYPE>();
            addFilter(f);
            f->repeatDelayInMs = repeatDelayInMs;
            f->repeatRateInMs = repeatRateInMs;
            return f;
        }

        //
        // MULTI-CLICK
        //
        InputMultiClick<DATA_TYPE> *addMultiClick() {
            auto f = new InputMultiClick<DATA_TYPE>();
            addFilter(f);
            return f;
        }
        InputMultiClick<DATA_TYPE> *addMultiClick(uint16_t maxClickIntervalInMs) {
            // will disable repeat
            auto f = new InputMultiClick<DATA_TYPE>();
            addFilter(f);
            f->maxClickIntervalInMs       = maxClickIntervalInMs;
            f->multiClickSendRepeatedKeys = false;
            return f;
        }
        InputMultiClick<DATA_TYPE> *addMultiClick(uint16_t maxClickIntervalInMs, uint16_t multiClickRepeatRateInMs) {
            // will enable repeat
            auto f = new InputMultiClick<DATA_TYPE>();
            addFilter(f);
            f->maxClickIntervalInMs       = maxClickIntervalInMs;
            f->multiClickSendRepeatedKeys = true;
            f->multiClickRepeatRateInMs   = multiClickRepeatRateInMs;
            return f;
        }

};

}

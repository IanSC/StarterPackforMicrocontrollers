AnalogRaw
    raw ADC read

AnalogInputScaled : AnalogRaw + InputLinearScale
    raw ADC read
    scaled, eg. 0..1024 --> 0..100

AnalogInput : AnalogInputRaw + InputFilterList
    raw ADC
    user selectable filters

AnalogButtonsMapped : AnalogInput + InputSlotter + InputKeyMapper
    raw ADC
    slot to values, eg. 0..100 --> 1, 101..200 --> 2
    map keys, eg. 1 --> 'A', 2 --> 'B'

AnalogButtonDB : AnalogButtonsMapped + UserInterfaceDebounced
    add debounce
AnalogButtonRP : AnalogButtonsMapped + UserInterfaceRepeated
    add repeater
AnalogButtonsMC : AnalogButtonsMapped + UserInterfaceMultiClick
    detect multi-click

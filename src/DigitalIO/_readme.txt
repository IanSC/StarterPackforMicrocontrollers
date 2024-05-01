DigitalInputRaw
    raw HIGH/LOW read

DigitalInput : DigitalInputRaw
    logic for on/off, eg. isOn(), checks if HIGH or LOW based on settings

DigitalButtonDB : DigitalInput + UserInterfaceDebounced


DigitalButtonDB : DigitalInput + UserInterfaceDebounced
    add debounce
DigitalButtonRP : DigitalInput + UserInterfaceRepeated
    add repeater
DigitalButtonMC : DigitalInput + UserInterfaceMultiClick
    detect multi-click

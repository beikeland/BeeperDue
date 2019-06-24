# BeeperDue
PanelDue surogate that only beeps.

Intended to be used with a ESP8266 board like Wemos D1 mini, with the buzzer shield. Wifi manager accepts GPIO pin numbers for setup, the default Wemos D5 is GPIO14, so 14 is the default value to use.

The PanelDue serial port on the Duet will need to be configured with `M575` and firmware versions prior to 2.03.1 cannot have 12864 display configured with `M918`  at the same time.
```
Connect
Due 5v - Wemos 5v
Due Tx - Wemos Rx
Due Gnd - Wemos Gnd
```

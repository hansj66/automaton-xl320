# PAX Counter

This device scans for visible bluetooth and wifi devices and then transmits the device count over NBIoT to a backend server.

Tested on a ESP-WROOM-32D + [EE-NBIoT-01](https://github.com/ExploratoryEngineering/ee-nbiot-01) rat's nest ;)

## Wiring (ESP):
* VCC   3.3V
* EN    to VCC via 10k pull-up
* IO0   to GND via boot switch (enable programming mode by grounding IO0 before power-on)
* GND
* IO16    to EE-NBIoT-01 RXD
* IO17    to EE-NBIoT-01 TXD
* RXD0    a) to FTDI TX and b) to GND via 10k pull-down
* TXD0    to FTDI RX

1) Use common ground for ESP-WROOM-32D, EE-NBIoT-01 and FTDI.
2) Leave FTDI VCC pin unconnected


![Prototype](https://github.com/ExploratoryEngineering/pax-counter/raw/master/images/prototype.jpg)

## Suggested initial PID settings

P: 274
I: 1657
D: 3.3
Setpoint: 90.12


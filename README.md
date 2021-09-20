# automaton-xl320
Dynamixel controller for AX-12A/W, XL-320 with IMU and BT

## Note:
* This repo contains the Eagle schematics and PCB laout for a functional dynamixel controller. The board is still in early development and the design is subject to change. Clone & build at your own risk :)
* Firmware example for a [XL-320 based balancing bot](https://github.com/hansj66/automaton-xl320/tree/main/hardware/fusion/balance-bot) can be found [here](https://github.com/hansj66/automaton-xl320/blob/main/firmware/esp/balance-bot/src/main.cpp).
* The mechanical design for an AX-12A hexapod is complete, but the firmware is still in early development
* Alexa integration is still in the "ripoff a sample" stage
* The board can be assembled by hand relatively easy, using a stencil, low temperature T3/T4 paste and a hot plate.
* There is no battery monitoring circuitry on the board. Undervoltage detection for a given battery type and automatic shutdown can be achieved by using the MIN_VOLTAGE_LIMIT and SHUTDOWN registers in the dynamixel control table.
* The board is BT and Thread capable, but integration between the nRF and the ESP is currently limited to I2C + one GPIO line.

## Stuff that is subject to change in future revision
* nRF/ESP integration
* Expansion header



![pcb](https://github.com/hansj66/automaton-xl320/blob/main/pictures/pcb.jpg)

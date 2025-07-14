## Teensy/MKS20 Controller Firmware
This code main branch compiles on NXP MCUXpresso and runs on Teensy 3.5. You can communicate with the Teensy using MAP-MATE if you choose. The MKS20 development branch is the currently supported and developed actively, please clone MKS20_DEVELOP and run that on the Sparkdog MKS20 controller board.

## Install
To run the project, you need a Teensy 3.5 or a Sparkdog MKS20 controller. You can create the hex to download using binary utilities and load using Teensy.exe, or JTAG and fully debug by soldering the ARM single wire debug interface onto the Teensy 3.5 or Sparkdog MKS20 controller. Segger J-link and PEMicro Universal are both debuggers I have used.

## Compilation
The project is recommended to be edited ONLY with MCUXpresso IDE v10.3.1 [Build 2233] [2019-02-20], the installation uses arm-none-eabi 7.3.1. toolchain. Other IDE/toolchain versions could cause errors and are not supported.

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
[GPL](http://www.gnu.org/licenses/gpl.html) GPL version 2 or higher.



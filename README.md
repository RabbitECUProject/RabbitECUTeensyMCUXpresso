# Rabbit ECU Teensy MCUXpresso

LEGACY BRANCH - MAIN
This code compiles on NXP MCUXpresso and runs on Teensy 3.5 controller. You can communicate/tune/freeze program with the Teensy using MAP-MATE if you choose. Original Teensy 3.5 MK64 Branch (no longer developing because Teensy 3.5 is discontinued).

ACTIVE BRANCH - RABBIT\_TEENSY\_MKS20
This code compiles on NXP MCUXpresso and runs on Sparkdog MKS2xDuino controller. You can communicate/tune/freeze and firware update with the MKS2xDuino using MAP-MATE if you choose.

## Install

To run the project, you need a Teensy 3.5 or Sparkdog MKS2xDuino controller. You can create the hex to download using binary utilities and load using Teensy.exe, or JTAG and fully debug by soldering the ARM single wire debug interface onto the Teensy 3.5. Segger J-link and PEMicro Universal are both debuggers I have used.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

[GPL](http://www.gnu.org/licenses/gpl.html) GPL version 2 or higher.

## Build Options

Build options are found in BUILD.h

\#define FUEL\_MISSING\_TOOTH\_OUTPUT - for building with a tell-tale output of the missing tooth emulation
#define DEBUG\_TMR\_OFF - for building with MKS2xDuino debugging mode - stops timers interfering with shared JTAG pins
#define BUILD\_SPARKDOG\_TEENSY\_ADAPT - for building using the Teensy 3.5 adapter PCB (no longer supported)
#define BUILD\_RABBIT\_1\_4 - for building Rabbit ECU versions 1.4 and above
#define BUILD\_SPARKDOG\_PF - for building using the Sparkdog PF-DI PCB (Teensy or MKS2xDuino)
#define BUILD\_SPARKDOG\_MKS20 - for building using MKS2x processor
#define PFDIV2FIX - for building Sparkdog PF-DI V2 and above PCB
#define BUILD\_GDI\_SIG\_INVERT - for building Sparkdog PF-DI V3 and above PCB
#define BUILD\_USER - always on
#define BUILD\_BSP\_IAC\_STEPPER - for building with a stepper motor IAC (not currently supported)
#define BUILD\_FME - for building with failure mode effects (safety strategies) on
#define BUILD\_USE\_PPSS\_AS\_PPSM\_OFF - for building inverted PPSM




##Compilation

The project is recommended to be edited ONLY with MCUXpresso IDE v10.3.1 \[Build 2233] \[2019-02-20], the installation uses arm-none-eabi 7.3.1. toolchain. Other IDE/toolchain versions could cause errors and are not supported








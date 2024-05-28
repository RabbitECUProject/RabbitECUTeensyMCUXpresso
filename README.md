# Rabbit ECU Teensy MCIXpresso
This code compiles on NXP MCUXpresso and runs on Teensy 3.5 and Sparkdog MKS2xDuino controller. You can communicate with the Teensy using MAP-MATE if you choose.

There are two branches:
1. Original Teensy 3.5 MK64 Branch (no longer developing because Teensy 3.5 is discontinued)
2. Sparkdog MKS2xDuino MKS2x Branch - currently developing

## Install
To run the project, you need a Teensy 3.5 or Sparkdog MKS2xDuino controller. You can create the hex to download using binary utilities and load using Teensy.exe, or JTAG and fully debug by soldering the ARM single wire debug interface onto the Teensy 3.5. Segger J-link and PEMicro Universal are both debuggers I have used.

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
[GPL](http://www.gnu.org/licenses/gpl.html) GPL version 2 or higher.

## Build Options
Build options are found in BUILD.h

#define FUEL_MISSING_TOOTH_OUTPUT - for building with a tell-tale output of the missing tooth emulation
#define DEBUG_TMR_OFF - for building with MKS2xDuino debugging mode - stops timers interfering with shared JTAG pins
#define BUILD_SPARKDOG_TEENSY_ADAPT - for building using the Teensy 3.5 adapter PCB (no longer supported)
#define BUILD_RABBIT_1_4 - for building Rabbit ECU versions 1.4 and above
#define BUILD_SPARKDOG_PF - for building using the Sparkdog PF-DI PCB (Teensy or MKS2xDuino)
#define BUILD_SPARKDOG_MKS20 - for building using MKS2x processor
#define PFDIV2FIX - for building Sparkdog PF-DI V2 and above PCB
#define BUILD_GDI_SIG_INVERT - for building Sparkdog PF-DI V3 and above PCB
#define BUILD_USER - always on
#define BUILD_BSP_IAC_STEPPER - for building with a stepper motor IAC (not currently supported)
#define BUILD_FME - for building with failure mode effects (safety strategies) on
#define BUILD_USE_PPSS_AS_PPSM_OFF - for building inverted PPSM



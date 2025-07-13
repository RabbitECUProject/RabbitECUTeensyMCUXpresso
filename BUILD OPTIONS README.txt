* FUEL_MISSING_TOOTH_OUTPUT
Connect TMR4 as missing tooth timer for high resolution trigger, e.g 60-1, 60-2, 36-1.

* DEBUG_TMR
Enable this macro to stop the initialisation of Injector A and Injector B. These two signals feed the PF-DI board as injector duty cycle signals. Having them enabled interferes with the JTAG/SWD debug, so they must be off for real-time debugging using the IDE.

* BUILD_SPARKDOG_TEENSY_ADAPT
Enable this macro when building for the Teensy 3.5 adapter controller.

* BUILD_RABBIT_1_4_PLUS
Enable this for Rabbit ECU Version 1.4 and above, where a multiplexer caters for 8 direct fire igniter signals.

* BUILD_SPARKDOG_PF
Enable for the different GPIO configurations used with Sparkdog PF-DI board.

* BUILD_SPARKDOG_MKS20
Enable when using the MKS20 controller.

* PFDIV2FIX
Enabled when using the Sparkdog PF-DI that has errors in the Motor enables.

* BUILD_GDI_SIG_INVERT
Enabled when using the Sparkdog PF-DI V3 and above that require a negative logic on the Injector A and B fuel duty cycle signals.

* BUILD_USER
Always enable this macro.

* BUILD_BSP_IAC_STEPPER
Enable this for IAC stepper motor control. This function has not been supported for a long time.

* BUILD_FME
Enable this macro when failure mode effects are needed for protection of over-speed because of electro-mechanical malfunction.

* BUILD_USE_PPSS_AS_PPSM_OFF
Enable this macro. Used in case the PPSS had to be used as PPSM.
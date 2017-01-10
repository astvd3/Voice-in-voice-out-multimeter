# Voice-in-voice-out-multimeter
A multimeter which responds to voice commands and gives output in the speech form. The whole project is done on a microcontroller platform.

### Install

This project requires AVR GCC Toolchain installed. It can be downloaded from here:
- [AVR-GCC-Toolchain](http://avr-eclipse.sourceforge.net/wiki/index.php/The_AVR_GCC_Toolchain)

### Compile

For compiling the programe, run the following command:

`make`

This will generate the hex file..

(Note: Before doing this you need to specify your Microcontroller Type and Frequency in the makefile)

This hex file can be loaded through any burner similar to `avr-dude`

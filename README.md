polargraph_server_polarshield
=============================

Polargraph Server for ATMEGA2560 based arduino boards, primarily targetting a Polarshield motor controller.

A Polarshield is an add-on board for an Arduino MEGA that provides two stepper drivers, an SD card reader and 
an LCD touchscreen. The firmware may also be configured at compile time to target RAMPS, though without SD reader and touchscreen (yet...).

There are a couple of precompiled binary hex files you can use if you don't want to compile from source.

- polargraph_server_polarshield_22.cpp.hex  Is for versions of the Polarshield or PolargraphSD that shipped with 2.2 inch screens.
- polargraph_server_polarshield_v1.x_2.4.cpp.hex  Is for versions of the Polarshield or PolargraphSD that shipped with a 2.4 inch screen, prior to August 2014.
- polargraph_server_polarshield_v2_2.4.cpp.hex is for versions of the Polarshield or PolargraphSD that shipped after August 2014.

The program has a core part that consists of the following files that are common to all Polargraph Server versions:

- comms.ino
- configuration.ino
- eeprom.ino
- exec.ino
- penlift.ino
- pixel.ino
- util.ino

and 
- polargraph_server_polarshield.ino

which is named for the project.

The other source files include the extended functionality available on ATMEGA2560 boards
and the Polarshield.

The file called impl_ps contains implementations of a few functions, and also
contains the extended impl_executeCommand(...) which is the jumping-off point for those 
extended functions.


Written by Sandy Noble

Released under GNU License version 3.

http://www.polargraph.co.uk

https://github.com/euphy/polargraph_server_polarshield

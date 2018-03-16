polargraph_server_polarshield
=============================

Polargraph Server for NodeMCU-32S, which is based on the Espressif ESP32 chip, combined with an SPI-based 240x320 TFT touchscreen, an SD card reader and two A4988 stepper drivers. This is currently breadboarded.

The display and touch routines are courtesy of Bodmers excellent TFT_eSPI library, which rolls up and optimises some of the Adafruit GFX libraries.

The ESP32 currently has sufficient pins for either:

* Encoders for the two motors, plus endstops, to build a Polargraph Pro type device, OR...
* Another motor axis.

The button and menu drawing routines are spread out over a couple of files:

* buttons.ino - initialise and define the button data
* buttons_actions.ino - the actions that get called when a button is activated
* lcd.ino - touch input handling
* lcd_draw.ino - for drawing to the lcd, layout



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

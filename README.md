Polargraph server for PolarshieldSD v3 
======================================


**Under development! Beware minor dragons!**
----------------------


Change log
-----------

2018.10.21-11:05 **v2.1.4**

Added some extra calls to impl_runBackgroundTasks() in the main loop to allow the touchUI
to be responsive during a draw-from-SD-card. I think this effectively blocks the existing
background task because it runs completely in the high-priority arduino "loop" task. 
I tried a few different combinations of increasing the priority of the background task,
and adding some taskYIELDs in there, but didn't seem to work. This insertion of background
checking into the foreground task is pretty nasty, but it solves the problem in the short
term, and I want to get this functional for machines that I'm sending out today. 
Fixes #3, and #6 was a symptom of the same thing.

Colour schemes are now more easily changeable (030054653a28a887440d54b1f82bb716f3db52d3). 
This work is to support #5.

Touches on the screen now reset the inactivity countdown by default, regardless of 
whether they trigger an event.

**v2.1.3**

Possibly addressed #2 by adding some more places that reset the inactivity countdown.
I'm not completely convinced that's what the problem is, because I can't replicate it.

Added binary files into the mix. Not sure this is a great idea, but the build process
is harder than it used to be, and this is a way to solve it. Look in ```binaries```
for the files, and a guide to doing the flash without compiling, and see also 
http://www.polargraph.co.uk/2018/10/how-to-upload-new-firmware-to-the-polargraphsd-v30/

**v2.1.2**

Replaced my adapted version of Ticker with the regular version of Ticker. My adapted 
version had microseconds added because I was using it to drive the motor loop, but
the motor loop is synchronous/blocking so doesn't need timing.


What this is
------------

This is a port of polargraph_server_polarshield to the Espressif ESP32 chip. I'm using
this part for a few reasons:

* Fast
* 3.3v native, so works with SD cards and many peripherals easily
* Expansion possibilities: wireless, Bluetooth
* Good fit for modern peripherals

The first goal of this project is to achieve feature parity with the old version. 
There are no technical reasons why this shouldn't be straightforward, but that doesn't
mean I'm going to make it easy! 

ESP32 uses a pre-emptively multi-tasking real-time operating system (FreeRTOS), so I'd 
like to configure most features as tasks, rather than trying to keep it all in the 
single-threaded model of most Arduino stuff.

Roadmap:
--------

1. ~~Feature parity - especially norwegian pixel.~~ v2.0.1
2. Remove need to use a computer to set up a machine - 
  * ~~Change machine size using touch UI~~ v2.1.0
  * Set motor specs using touch UI
3. ~~Set bounding box using on-screen controls~~ v2.1.0
  * Spec'd in millimetres
  * Limited by machine size + margin
4. Warnings when machine doesn't match controller (connected on USB)
4. Draw from raw image format on SD card
  * Image bitmap a la norwegian pixel
  * SVG scaled to bounding box area
5. Web UI built in


Hardware:
---------

The PolargraphSD v3 is based on a Polarshield v3 board, which integrates:

    NodeMCU-32S
    2x stepper drivers
    320x240 LCD
    Touch panel
    SD card

You can get the source files for the board here: https://github.com/euphy/polarshieldv3_hardware and view them nicely here: https://cadlab.io/project/1157/master/files.

You can buy pre-built Polargraph hardware at https://polargraph.bigcartel.com/


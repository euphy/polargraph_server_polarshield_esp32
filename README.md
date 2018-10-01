# polargraph_server_polarshield_esp32

** Under development **

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



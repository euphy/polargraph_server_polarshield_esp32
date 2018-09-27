# polargraph_server_polarshield_esp32

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
1. Feature parity - especially norwegian pixel.
2. Remove need to use a computer to set up a machine - 
  * Change machine size using on-screen controls
3. Set bounding box using on-screen controls
4. Draw from real image format on SD card
  * Image bitmap a la norwegian pixel
  * SVG scaled to bounding box area


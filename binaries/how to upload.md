How to upload pre-compiled binary firmware
==========================================

If you can't compile the firmware yourself (for whatever reason,
there's no shame), you can upload the pre-compiled firmware in this 
folder by using these four precompiled binary files.

There are four files that need to be delivered to four specific
memory addresses on the ESP32.

(The two boot*.bin files are in the arduino/hardware/espressif/esp32 
folder. The other two bin files are generated here and appear in
.pioenvs/nodemcu-32s)

|Address    |File                      |
|-----------|--------------------------|
|0x1000     |bootloader_dio_40m.bin    |
|0x8000     |partitions.bin            |
|0xe000     |boot_app0.bin             |
|0x10000    |firmware.bin              |


Using esptool
-------------

You can use [esptool](https://github.com/espressif/esptool) 
(a python tool) to do this. It is installed as part of PlatformIO. 
Oust open a terminal from your IDE and type ```esptool.py``` to 
test if it's installed.

Once you have esptool installed, open a command prompt in this folder (```binaries```), and run:
```
esptool.py --chip esp32 --port "COM9" --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader_dio_40m.bin 0x8000 partitions.bin 0xe000 boot_app0.bin 0x10000 firmware.bin
```
(!Note!  Replace ```COM9``` above with the name of your USB serial port.)


Other way #1
----------

On Windows, you might use the graphical ESP32 Flash Download Tool 
[Flash Download Tools (ESP8266 & ESP32)](https://www.espressif.com/en/products/hardware/esp32/resources).
Enter the four files listed above, along with the addresses. Tick each row to activate it and change the USB port to wherever you have your ESP32 plugged 
into. Hit **START** to upload. 


What happens next
-----------------

When both of the above methods upload, they attempt to connect to the ESP32, and send a signal and wait for a gap.

The signal is shown in a text window as:
```
CONNECT BAUD: 115200
============
....._____....._____....._____....._____..
```

The wavy line progresses across the screen. If it doesn't manage to get connected by the end of the line, it'll simply time out. Try it a few times, but if it doesn't take, you should use a cocktail stick to click the tiny **BOOT** button on the ESP32, next to the micro-USB connector. This bumps it into upload mode and it'll usually connect right away.

Press **reset** to complete.


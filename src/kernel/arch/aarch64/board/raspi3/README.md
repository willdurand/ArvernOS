<!-- doxygen: \page refRaspi3 Raspberry Pi 3 -->

# Raspberry Pi 3

## config file

```
ARCH  = aarch64
BOARD = raspi3

CONFIG_USE_DLMALLOC   = 1
# This board does not have a hardware clock (RTC).
CONFIG_USE_FAKE_CLOCK = 1
```

## Usage on real hardware

1. Download `bootcode.bin` and `start.elf` from [the official GitHub
   repository](https://github.com/raspberrypi/firmware/tree/master/boot)
2. Build this project
3. Copy `bootcode.bin`, `start.elf`, `build/aarch64/raspi3/dist/cmdline.txt` to
   a SD card (FAT32)
4. Rename `build/aarch64/raspi3/dist/ArvernOS-aarch64-raspi3.img` to
   `kernel8.img` and copy it to the SD card as well
5. Insert the card into the Raspberry Pi SD card slot
6. Connect a USB to serial adapter (FTDI) between the Raspberry Pi and a host
   machine [^1] that will run `minicom` or any other serial communication
   program. The serial configuration is: _112500 8N1_.
7. Power up the Raspberry Pi

[^1]: https://pinout.xyz/pinout/uart#

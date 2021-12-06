<!-- doxygen: \page refLicheePiNano Lichee Pi Nano -->

# Lichee Pi Nano

See: https://linux-sunxi.org/LicheePi_Nano

## config file

```
ARCH  = aarch32
BOARD = licheepi-nano

CONFIG_USE_DLMALLOC   = 1
# This board does not have a hardware clock (RTC).
CONFIG_USE_FAKE_CLOCK = 1
```

Note: It is currently not possible to use QEMU with this board.

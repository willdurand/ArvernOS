<!-- doxygen: \page ref_raspi2 Raspberry Pi 2 -->

# Raspberry Pi 2

## config file

```
ARCH  = aarch32
BOARD = raspi2

CONFIG_USE_DLMALLOC   = 1
# This board does not have a hardware clock (RTC).
CONFIG_USE_FAKE_CLOCK = 1
```

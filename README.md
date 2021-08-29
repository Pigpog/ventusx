# ventusx

A free and open-source utility for controlling the advanced features of
the TteSPORTS Ventus X mouse on Linux.

## Usage

Currently, this utility can only control the LEDs, with independent
control of the palm and scroll wheel. Both can be set to on, off,
pulse, or "battle" mode, and their brightness can be adjusted.

```
# control the palm LED
ventusx palm off
ventusx palm on
ventusx palm battle
ventusx palm pulse

# control the scroll wheel LED
ventusx scroll off
ventusx scroll on
ventusx scroll battle
ventusx scroll pulse

# control the brightness of each LED
# valid values are 0 to 255

ventusx palm brightness 0
ventusx palm brightness 255

ventusx scroll brightness 0
ventusx scroll brightness 255

```

## Compiling

Just run `make`.


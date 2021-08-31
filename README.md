# ventusx

A free and open-source utility for controlling the advanced features of
the TteSPORTS Ventus X mouse on Linux.

## Usage

Current features:

 - full control of LEDs, including features not found in the official
   utility such as brightness
 - DPI level
 - Polling rate

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

# set DPI level, between 0 and 63
ventusx dpi 12

# set polling rate to 1000:
ventusx polling 1
# set polling rate to 125:
ventusx polling 8

# bind the "dpi" button to "a":
ventusx bind dpi kbd 4
# bind left click to right click:
ventusx bind left mouse 2
```

## Compiling

Just run `make`.


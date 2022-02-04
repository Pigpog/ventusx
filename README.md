# ventusx

A free and open-source utility for controlling the advanced features of
the TteSPORTS Ventus X mouse on Linux.

## Compiling

Just run `make`.

## Usage

Current features:

 - Full control of LEDs, including features not found in the official
   utility such as brightness
 - DPI level
 - Polling rate
 - Rebinding buttons
 - Lift off distance

### Control the palm LED
```
ventusx palm off
ventusx palm on
ventusx palm battle
ventusx palm pulse
```
### Control the scroll wheel LED
```
ventusx scroll off
ventusx scroll on
ventusx scroll battle
ventusx scroll pulse
```

### Control the brightness of each LED

Valid values are 0 to 255

```
ventusx palm brightness 0
ventusx palm brightness 255

ventusx scroll brightness 0
ventusx scroll brightness 255
```

### Set DPI level, between 0 and 63

```
ventusx dpi 12
```

### Set polling rate

```
ventusx polling 1000
ventusx polling 125
```

### Set lift off distance

Note: only values between 7 and 15 are officially supported

```
ventusx liftoff 7
ventusx liftoff 15
```

### Rebinding buttons

Bind the "dpi" button to "a":

```
ventusx bind dpi kbd a
```

Bind left click to right click:

```
ventusx bind left mouse right
```


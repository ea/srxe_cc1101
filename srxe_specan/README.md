## SRXE CC1101 Spectrum analyzer 

![specan](../photos/specan.png)

This here arduino sketch is basically a clone of old im-me spectrum analyzer adapted for SRXE and standalone CC1101 module.


It supports all three bands (300MHz, 400Mhz and 900MHz) and scans 128 channels at a time. 

There is support for NARROW , WIDE and ULTRAWIDE mode (switch between by pressing `w`, `n` or `u`). Use arrow keys to shift frequencies.

History is drawn as shadow and can be toggled by pressing `m` for "MAX HOLD". 
Persistence can be toggled on or off via `p`. 
You can quickly navigate between the bands by pressing `3`, `4` or `9` for corresponding bands. 
Pressing `space` will cycle through different bandwidth settings. 


### Building 

Building this sketch will require my version of BitBank's SRXE support library (at least until my changes are merged). 



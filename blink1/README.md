blink1 project

6-3-2022, 6-6-2022

The idea is to take a bitstream file that runs entirely
self contained in the PL and load and start it from
the PS.

The end result is the green LED on the EBAZ will blink.

As much as anything it is a driver for the devcfg section
of the Zynq chip on the Ebaz board.

I am using a tested bitstream that blinks the green LED at
about 2 Hz (the red LED remains off).

This began by copying the "timer" project.

The DMA loading of the PL is very fast.

This now works with bit_read generating C source
from the bitstream file.

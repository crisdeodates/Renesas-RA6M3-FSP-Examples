/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the CAC HAL module APIs. 
	On any key press from J-Link RTT Viewer, CAC module starts measuring the 
        accuracy of selected target clock against the selected reference clock.
        Result of the measurement is displayed on J-Link RTT Viewer.

Note: 
1) The user may select any reference clock and clock divider. 

2) The user may select any measurement clock and clock divider.

3) The user needs to ensure that the Main clock/Sub-clock oscillator is available on the board before selecting it.

4) The user must set upper limit and lower limit in configurator based on the selected reference and measurement clocks.

5) Formula to calculate threshold (upper/lower) limits: 
   a. Get Measurement clock frequency with clock divider i.e. say 'M'. 
      for ex. select HOCO(24MHZ on EK-RA4M1) and clock divider as '1', then M = 24MHZ.
   b. Get Reference clock frequency with clock divider i.e. say 'R'.
      for ex. select Main OSC(12MHZ on EK-RA4M1) and clock divider as '32', then R = (12MHZ/32).
   c. Calculate count value, say 'C'.
      i.e. C = (M/R), so C = 64.
   d. Take a tolerance of 10%, say 'T'.
      i.e. T = (C * (10/100)) = C/10 = 6.4.
   e. Upper limit Threshold = (count value + tolerance) = (C + T) = 70.4.
   f. Lower limit Threshold = (count value - tolerance) = (C - T) = 57.6.

6) For clock frequencies, refer "Clocks" tab in configurator or board specific user manual.

7) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22040450
   b. Keil: 	Not Available 
   c. IAR: 	Not Available

8) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

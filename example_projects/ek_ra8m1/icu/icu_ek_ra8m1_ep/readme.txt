/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the ICU HAL module APIs. 
	The project initializes the ICU module.
	User is requested to press the push button to trigger the external irq and this then will start toggling of user LED.
	For MCK-RA4T1, MCK-RA6T3, User is requested to flip the toggle switch (OFF->ON) to trigger the external irq and this then will start toggling of user LED. 

2. Hardware Connection:

	MCK-RA4T1/MCK-RA6T3: Connect to inverter board.

NOTE: 
        Use Switch S1 (push button) on RA6M5,RA6M3,RA6M3G,RA6M4,RA4M3,RA4M2,RA2E1,RA2L1,FPB-RA4E1, FPB-RA6E1, EK-RA4E2, EK-RA6E2, EK-RA8M1.
	Use Switch S2 (push button) on RA6T1, RA6T2 and RA2E2.
	Use Switch S1 (toggle switch) on MCK-RA4T1, MCK-RA6T3.
	
﻿
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000860
   b. Keil:	0x22000430
   c. IAR: 0x2200049c
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

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
	MCK-RA8T1: Pmod BTN (J1:1) to PMOD(CN6:1).

NOTE: 
        Use Switch S1 (push button) on EK-RA6M5, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA2E1, EK-RA2L1, FPB-RA4E1,
	FPB-RA6E1, EK-RA4E2, EK-RA6E2, EK-RA8M1 and EK-RA8D1

	Use Switch S2 (push button) on RSSK-RA6T1, MCK-RA6T2 and EK-RA2E2.

	Use Switch S1 (toggle switch) on MCK-RA4T1, MCK-RA6T3.

	Use Switch S1 (push button) on FPB-RA2E3, P206 <---> P200.

	Use Switch BTN1 (push button on Pmod BTN) on MCK-RA8T1.
﻿
	
﻿
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000860
   b. Keil:	0x22000430
   c. IAR: 0x2200049c
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

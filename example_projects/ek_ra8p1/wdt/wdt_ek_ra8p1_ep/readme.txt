/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
   	This example project demonstrates the typical use of the WDT HAL module APIs. The user can enter '1' through
	J-Link RTT Viewer to initialize the WDT and start the GPT timer. The WDT counter is refreshed periodically
	every 1 second when the GPT timer expires. Refresh status will be printed every 3 seconds. The user can press
	the push button to stop the GPT timer which in turn stops refreshing the WDT timer. Approximately after 2
	seconds, the WDT resets the MCU and turns the LED ON until the user provides the input once again.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Pmod BTN (For MCK-RA8T1. e.g., https://digilent.com/reference/pmod/pmodbtn/start).
	1 x Jumper wire (For FPB-RA2E3).

3. Hardware Connections:
	For EK-RA2A1, EK-RA2E1, EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M4, EK-RA6M5,
	    EK-RA4E2, EK-RA6E2, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1, MCK-RA6T3, FPB-RA8E1, EK-RA4L1, EK-RA2L2:
		Use Switch S1 to stop the WDT from refreshing.

	For EK-RA4W1, EK-RA8E2, EK-RA8P1:
		Use Switch SW1 to stop the WDT from refreshing.

	For EK-RA2E2, RSSK-RA6T1, EK-RA6M3, EK-RA6M3G, MCK-RA6T2, EK-RA2A2, EK-RA8M1, EK-RA8D1:
		Use Switch S2 to stop the WDT from refreshing.

	For FPB-RA2E3:
		Connect wire to short P200/NMI (J3:21) to P206 (J3:18).
		Use Switch S1 to stop the WDT from refreshing.

	For MCK-RA8T1:
		Connect Pmod BTN (J1:1) to Pmod (CN6:1).
		Use Switch BTN1 (push button on Pmod BTN) to stop the WDT from refreshing.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000450
   b. Keil:	Not Available
   c. IAR: 	0x220008fc
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).

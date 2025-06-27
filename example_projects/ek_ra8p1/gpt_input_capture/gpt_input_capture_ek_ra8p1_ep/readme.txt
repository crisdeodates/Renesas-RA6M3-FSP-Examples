/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The EP demonstrates the functionality of GPT Input capture module. GPT3 is used to generate periodic pulses
	of 500msec duration and provided as input to GPT Input capture (GPT5). GPT5 counts the event pulse received 
	at its input. Based on the period and capture event, the time period of pulse is calculated and displayed on
	J-Link RTT Viewer.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Jumper wire.

3. Hardware Connections:
	For EK-RA2A1:
		Connect  P400 <--> P301

	For EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M5:
		Connect  P405 <--> P415

	For FPB-RA6E1:
		Connect  P405 <--> P103
	
	For EK-RA6M4:
		Connect  P105 <--> P512

	For RSSK-RA6T1:
		Connect  P103 <--> P112

	For EK-RA2E1:
		Connect  P500 <--> P105

	For EK-RA2E2:
		Connect  P112 <--> P101

	For FPB-RA4E1, FPB-RA8E1:
		Connect  P105 <--> P113

	For MCK-RA6T2:
		Connect  PE14 <--> PE12

	For EK-RA6E2, EK-RA4E2:
		Connect  P207 <--> P105

	For MCK-RA4T1, MCK-RA6T3:
		Connect  P409 <--> P113

	For EK-RA8M1, EK-RA8D1:
		Connect  P405 <--> P713

	For FPB-RA2E3:
		Connect  P109 <--> P103

	For EK-RA2A2:
		Connect  P104 <--> P502

	For MCK-RA8T1:
		Connect  P113 <--> P300
		
	For EK-RA4L1:
		Connect  P405 (J2:42) <--> P304 (J2:7)

	For EK-RA8E2:
		Connect  P405 (J2:7)  <--> P403 (J2:5)

	For EK-RA2L2: 
		Connect  P103 (J2:36) <--> P112 (J2:30)

	For EK-RA8P1:
		Connect  P403 (J17:7)  <--> P700 (J3:9)

Note: 
1) Generated periodic pulse for RA2A1 is 1000msec.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000430
   b. Keil: 	Not Available 
   c. IAR:  	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

4) For GCC ARM Embedded Toolchain: To enable printing floats to the RTT Viewer, edit the project settings and ensure that
   "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous > Use float with nano printf

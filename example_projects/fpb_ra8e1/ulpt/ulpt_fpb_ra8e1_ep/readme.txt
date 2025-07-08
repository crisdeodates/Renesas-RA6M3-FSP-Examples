/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates the functionality of ULPT in periodic mode and one-shot mode. The user selects 
	the period for both one-shot and periodic timers as 1 second, 2 seconds, or 3 seconds. After selecting the period, 
	the ULPT1 starts in periodic mode and toggles the on-board LED when the ULPT1 underflow interrupt occurs. The ULPT1 
	will stop after underflow three times, and the ULPT0 will start in one-shot mode after that. The MCU will be put into 
	Deep SW Standby mode and wait for the ULPT0 underflow interrupt to reset the MCU internally to Normal Mode and 
	restart the EP.

2. Hardware Requirements:
	Supported RA boards: EK-RA8M1, EK-RA8D1, MCK-RA8T1, FPB-RA8E1
	1 x RA board.
	1 x Type C USB cable for programming and debugging.

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type C USB cable

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22060430
   b. Keil: 	Not Available 
   c. IAR:  	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
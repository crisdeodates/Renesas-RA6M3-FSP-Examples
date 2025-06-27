/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of DMAC driver on Renesas RA MCUs based on Renesas FSP.
	Project initializes 3 DMAC transfer instances. Upon successful initialization, a menu option is displayed.
	On the user input "1", AGT generates 100msec interrupt to trigger a transfer from the source buffer to the port
	control register for 60 times indicated by LED toggling for 60 times. On the user input "2", 32-bit data is
	transferred from the GPT counter register to the destination array and display destination array data on RTT
	Viewer. On the user input "3", 32-bit data is transferred from predefined source array to the port register
	(LED) and the state of LED changes accordingly.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

Note:
1) On starting or resetting the board, at times the DMAC transfer operation takes more time. Due to this the LED
   blinking frequency decreases/pauses. Please reset the board in that case, and it will run as expected.

2) For EK-RA6M3 and EK-RA6M3G: LED blinking pauses in-between and then continues.

3) To send valid input to RA board, the user needs to set in J-Link RTT Viewer as:
   [Input] -> [End of Line] -> Choose [Unix format (LF)]

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000550
   b. Keil:	0x22000970
   c. IAR: 0x22000a50

5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).

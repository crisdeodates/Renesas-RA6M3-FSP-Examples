/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the typical use of the IWDT HAL module. In RTT Viewer, the user can enter '1'
	to initialize the IWDT and start the GPT timer. The IWDT counter is refreshed periodically every 1 second when
	the GPT timer expires. Refresh status will be printed every 2 seconds. Once the user enters '2', the IWDT
	counter stops refreshing and resets the MCU.

	For FPB-RA0E1, FPB-RA0E2: In RTT Viewer, the user can enter '1' to initialize the IWDT and start the TAU timer.
	The IWDT counter is refreshed periodically every 1 second when the TAU timer expires.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

Note:
1) To calculate IWDT underflow time, please refer to the formula in the link:
   https://renesas.github.io/fsp/group___i_w_d_t.html

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000450
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

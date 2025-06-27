/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates Message Queue and Semaphore between tasks and interrupts. Message Queue is
	demonstrated both between tasks and between task and interrupt. GPT timer periodically generates interrupt at
	1000 msec. For the first few seconds, messages are shared between Sender and Receiver Tasks and GPT msgq
	(Message Queue) ISR. Receiver task pends on Message Queue, receives and displays message received on RTT
	Viewer periodically at 500 msec. For the next few seconds, Semaphore Task waits for semaphore until it is
	released by GPT sem (semaphore) ISR. GPT sem ISR releases semaphore periodically at 1000 msec. All the tasks
	run with equal priority level.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

﻿Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000450
   b. Keil:	0x22000850
   c. IAR: 0x22001f5c
	
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).

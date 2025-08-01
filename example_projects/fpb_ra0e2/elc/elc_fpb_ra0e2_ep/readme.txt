/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the ELC HAL module APIs.
        ELC Software Event, GPT8 and GPT9 events are linked using ELC. The start source for GPT8 and GPT9 is ELC Software Event and
        the stop source for GPT8 is GPT9 counter overflow. GPT8 runs in PWM mode and GPT9 runs in one-shot mode.
        On giving valid RTT input, an ELC Software Event is generated that triggers LED blinking.
        LED stops blinking after 5 sec when GPT9 expires.
		
	For FPB-RA0E1, FPB-RA0E2:
	The example project demonstrates the typical use of the ELC HAL module APIs.
	ELC Software Event, TAU0 and TAU1 events are linked using ELC. The start source for TAU1 is ELC Software Event and
	the start source for TAU0 is end of counter TAU1. TAU0 runs in one-shot pulse output and TAU1 runs in delay counter mode.
	On giving valid RTT input, an ELC software Event is generated that turn on LED1 after 1 seconds.
	LED1 turn on for 2 seconds until request pulse width has expires.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	Jumper wire.

3. Hardware Connections:
    	For EK-RA2L1:
		Connect P414 (J1:04) to P505 (J3:06) (LED3).

	For EK-RA2E1:
		Connect P212 to P913 (LED3).

	For EK-RA6M5:
		Connect P414 (J1:32) to P006 (J4:28) (LED1).

	For FPB-RA4E1:
		Connect P104 (J4:21) to P407 (J3:25) (LED2).

	For FPB-RA2E3:
		Connect P102 (J4:10) to P213 (J3:7) (LED1).

	For EK-RA2A2:
		Connect P105 (J4:16) to P307 (J3:16) (LED1).
		
	For FPB-RA0E1:
		Connect P213 (J1:5) to P008 (J2:30) (LED1).
	
	For FPB-RA0E2:
		To use P103 for LED1, the user must close E28.
		Connect P104 (J2:12) to P103 (J2:13) (LED1).

Note:
1) The user is expected to enter data not exceeding 15 bytes in size.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004850
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
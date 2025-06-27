/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the direct access, configuration, and operation of DOC SFRs.
    	The ISR for the DOC is configured through the Interrupts tab of the FSP Configurator.
    	For more information on configuring interrupts, refer to: 
	https://renesas.github.io/fsp/ > RA Flexible Software Package Documentation > Starting Development > e2 studio 
	User Guide > Configuring a Project > Configuring Interrupts from the Stacks Tab 
    	The SFRs and interrupts are configured and enabled to perform a comparison match, using both direct access and 
    	BSP-provided APIs.

	LED Behavior:
	a. For Boards with 3 LEDs:
    	   LED 1 is turned ON only while program control waits for the User Defined ISR to execute.
    	   LED 2 is turned ON only to indicate the successful conclusion of application execution.
    	   LED 3 is turned ON if there is an expected error encountered during operation.

	b. For Boards with 2 LEDs:
	   LED 1 is turned ON only while program control waits for the User Defined ISR to execute.
	   LED 2 is turned ON only indicate the successful conclusion of application execution.
	   LED 1 and LED 2 are turned ON if there is an expected error encountered during operation.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000430
   b. Keil:	Not Available
   c. IAR: 	Not Available

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

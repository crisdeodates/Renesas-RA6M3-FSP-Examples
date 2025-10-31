/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of NetX Secure Crypto on Renesas RA MCUs based on Renesas FSP
	supporting SCE7/SCE9/RSIP-E51A/RSIP-E11A/RSIP-E50D/RSIP-E31A hardware acceleration using AzureRTOS. The example
	project executes AES, ECC, RSA algorithms (including different chaining modes) in multi-threaded environment
	using mutex to enable access to SCE/RSIP. The user can enter any key to enable concurrent execution of the
	different algorithms and status of the execution will be displayed on J-Link RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	LLVM Embedded Toolchain for ARM: Version 18.1.3

3. Hardware Requirements:
	Supported RA Boards:
		SCE7      - EK-RA6M2
		SCE9      - EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5
		RSIP-E51A - EK-RA8M1, EK-RA8D1, MCK-RA8T1, FPB-RA8E1, EK-RA8E2
		RSIP-E11A - EK-RA4L1
		RSIP-E50D - EK-RA8P1, MCK-RA8T2, EK-RA8M2
		RSIP-E31A - EK-RA4C1
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Host PC.

4. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29
		on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x22041190
   b. Keil:	Not Available
   c. IAR:	Not Available

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the Flash_HP HAL module APIs. The example project demonstrate
	the code flash and data flash operations once, the user provides RTT Viewer input from the available commands list.
	To exit from the demo, the user should choose the EXIT option.

2. Hardware Requirements:
	Supported RA boards: CK-RA6M5, EK-RA4E2, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, 
	                     EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3,
	                     MCK-RA8T1, RSSK-RA6T1, FPB-RA8E1, EK-RA4L1, EK-RA8E2, EK-RA4C1
 	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

Note:   
1) The user is expected to enter data of size not exceeding 15 bytes.

2) Operation is not guaranteed for any user input value, other than integer (i.e. float, char, special char)
   through RTT Viewer.

3) Entering any input after EXIT option will result in error trap. Not recommended.

4) The user should refer to the "RASC User Guide for MDK and IAR" section for instructions on using RASC for
   IAR and Keil via link: https://renesas.github.io/fsp/_s_t_a_r_t__d_e_v.html#RASC-MDK-IAR-user-guide

5) After the example project is built successfully, the user should follow these steps in sequence to launch
   the debug session:
   - Open Renesas Device Partition Manager (In e2studio or RASC -> Run -> Renesas Debug Tools -> Renesas Device
     Partition Manager), then choose "Connection Type" as SWD.
   - Initialize device. The user can refer to "Setting up Hardware" section of R11AN0467 application note via link:
     https://www.renesas.com/en/document/apn/security-design-arm-trustzone-using-cortex-m33 to perform the action.
   - Launch the Debug session from the example project.
 
6) EK-RA4L1, EK-RA4C1 boards must have HOCO running and stable in order to program and erase flash.

7) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000ea4
   b. Keil:	0x20000d9c
   c. IAR: 0x20000e44

8) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).

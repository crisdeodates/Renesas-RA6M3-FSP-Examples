/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionalities of USB PHID driver on Renesas RA MCUs based on Renesas FSP.
	The project configures RA board as a Keyboard. On pressing any key from RTTViewer, MCU sends the host PC 
	characters from a - z & numbers 0 - 9.

2. Hardware Requirement:

	micro USB Cable : 2 nos
	
3. Hardware connection :
	
	EK-RA2A1,EK-RA4M1,EK-RA6M1,EK-RA6M2:(support only full speed)
	Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	port (J9) of the board.  Connect the other end of this cable to USB port of the 
	host PC. 
	
	EK-RA6M3,EK-RA6M3G,EK-RA6M4,EK-RA6M5, EK-RA4M3, EK-RA4M2, EK-RA8M1:(support only full speed)
	Jumper J12 placement is pins 2-3
	Connect jumper J15 pins 
	Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	port (J11) of the board.  Connect the other end of this cable to USB port of the 
	host PC. 

	EK-RA4E2, EK-RA6E2:(support only full speed)
	Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	port (J11) of the board.  Connect the other end of this cable to USB port of the 
	host PC.

Note :
1) Open Device Manager, and check 'Human Interface Devices', to see ' USB Input Device' has been detected.

2) Lower case a - z and numbers 0 - 9 is send from MCU to the host PC.﻿

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220009e0
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

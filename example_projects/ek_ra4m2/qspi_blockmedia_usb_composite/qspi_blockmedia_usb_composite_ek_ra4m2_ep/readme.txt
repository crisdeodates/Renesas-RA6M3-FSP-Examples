/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the QSPI as block media on Renesas RA MCU's using the USB composite Class driver(PMSC + PCDC).
	In this example, the application will configure the MCU as a CDC device as well as a Mass storage device on Block media on QSPI.
	Users can use a Serial terminal(like Tera-term) to communicate with the board. Data written by the tera-term will be echoed back
	on the terminal by the RA board. The user at the same time can read/write the data from/to the board using a host machine.

2. Hardware Requirement:

	micro USB Cable : 1no.
	
3. Hardware connection :
	
        EK-RA6M3,EK-RA6M4,EK-RA4M3,EK-RA4M2,EK-RA6M5:
        Jumper j12: Connect pins 2-3
        Connect Jumper j15 pins
        Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	port (J11) of the board. Connect the other end of this cable to a USB port of the 
	host PC. 
 

4. USB Disconnect Behaviour :
        The application also handles removing & connecting the Peripheral USB Cable use case.
        Ex: If the user removes the peripheral USB cable from the board, the Composite USB example project will be running 
        and it waits for the cable to be connected. Once the Cable is connected back, the example project works the same.

Note:
1. Build Download and Run firmware on board. Connect the cable to USB peripheral connector.        
2. Confirm that the PC detects an unformatted media using the "Disk Management" application on Windows.
3. Format the drive to use it as a flash drive.﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000f8c
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

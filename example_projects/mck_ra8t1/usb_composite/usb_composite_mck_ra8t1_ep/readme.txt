/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of USB Composite driver on Renesas RA MCUs based on Renesas FSP using FreeRTOS. 
        In this example, the application will configure the MCU as a CDC device as well as a Mass storage device. 
        Tera term is used to communicate with the CDC interface of the MCU. Data written by the tera term will be echoed back on the terminal by RA board.
        User at the same time can read/write the data from/to the Mass storage device using host machine.

2. Hardware Requirement:

	micro USB Cable : 2 nos or type C USB Cable : 2 nos (For MCK-RA8T1)
	
3. Hardware connection :
	
	EK-RA6M1,EK-RA6M2 :(support only full speed)
	  Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	  port (J9) of the board.  Connect the other end of this cable to USB port of the 
	  host PC.

	EK-RA6M4,EK-RA4M3,EK-RA4M2,EK-RA6M5:(support only full speed)
	  Jumper J12: Connect pins 2-3
	  Connect jumper J15 pins 
	  Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	  port (J11) of the board.  Connect the other end of this cable to USB port of the 
	  host PC. 

	EK-RA6M3 and EK-RA6M3G:(for high speed)
	  Jumper j7: Connect pins 2-3
	  Connect Jumper j17 pins
	  Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	  port (J6) of the board.  Connect the other end of this cable to USB port of the 
	  host PC. 

	EK-RA8M1 (for high speed)
	  Jumper j7: Connect pins 2-3
          Connect Jumper j17 pins
          Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	  port (J31) of the board.  Connect the other end of this cable to USB port of the 
	  host PC.

	EK-RA8D1 (for full speed)
	  Jumper j12: Connect pins 2-3
          Connect Jumper j15 pins
          Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	  port (J11) of the board.  Connect the other end of this cable to USB port of the 
	  host PC.

	MCK-RA8T1:(support only full speed)
	  Jumper jp9: Connect pins 2-3
	  Connect Jumper jp10 pins
	  Connect the micro USB end of the micro USB device cable to Type C USB Full Speed 
	  port (CN14) of the board.  Connect the other end of this cable to USB port of the 
	  host PC.
 

4. USB Disconnect Behaviour :
        The application also handles removing & connecting the Peripheral USB Cable use case.
        Ex: If the user removes the peripheral USB cable from the board, the Composite USB example project will be running 
        and it waits for the cable to be connected. Once the Cable is connected back, the example project works the same.
        
       Note : PMSC Disk content is retained only when the Peripheral USB cable is removed & connected back. 
              If the user removes the power cable of board, the disk content will be lost as the disk is implemented on RAM.
﻿
Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000c18
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

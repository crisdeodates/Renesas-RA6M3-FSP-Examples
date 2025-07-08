/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     This Example Project demonstrates the basic functionality of USB Host Communication Class (HCDC) driver on Renesas RA MCUs
using 2 RA boards. Board1 (with USB HCDC Example Project running on it) communicates with Board2 (with USB PCDC Example Project 
running). Board1 initiates the communication by sending commands to Board2 and Board2 responds by sending the data. Board1 prints 
the received data on the RTTViewer

2. Supported Boards:
	EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4 and EK-RA6M5

3. Hardware Requirements:
    External Hardware : USB OTG cable   - 1 nos
                        USB Cable	- 3 nos
	EK-RA4M2/EK-RA4M3/EK-RA6M3/EK-RA6M3G/EK-RA6M4/EK-RA6M5 - Running USB HCDC Example Project 
	Any RA board running USB PCDC Example Project

4. Configuration changes to switch Speed in configurator: 

    i. Full-Speed : (EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4 and EK-RA6M5 boards)
        USB Speed                      :    Full Speed
        USB Module Number              :    USB_IP0 Port

    ii.  High-Speed :(EK-RA6M3, EK-RA6M3G)
        USB Speed                      :    Hi Speed
        USB Module Number              :    USB_IP1 Port

5. Hardware Connections:
	Connect Board 1, running USB HCDC Example Project, through OTG cable to Board 2, running USB PCDC Example Project.

   Jumper Settings: 

    i. Full-Speed: (EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4 and EK-RA6M5 boards)
        Jumper j12 placement is pins 1-2
        Remove Jumper j15 pins
        Connect USB device to J11 connector with help of micro USB Host cable.

    ii.  High-Speed:(EK-RA6M3, EK-RA6M3G)
        Jumper j7: Connect pins 1-2
        Remove Jumper j17 pins
        Connect USB device to J6 connector with help of micro USB Host cable.

    
Note: 	
1. By default USB HCDC Example Project (i.e., Board 1) runs in Full-speed mode. 
2. RTT Print Buffer size should be configured to 512

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000d6c
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the ADC HAL module APIs with LLVM Embedded Toolchain for Arm.
        The project initializes the ADC in single scan or continuous scan mode based on user selection in RA configurator.
        Once initialized,user can initiate the ADC scan and also stop the scan (in the case of continuous scan mode) using 
	JLinkRTTViewer by sending commands. User provide ADC channel input voltage from 0V to 3.3V with a voltage supply unit 
	at the ADC channel voltage input pin. Once ADC scan is initialized, Window Compare Mode is enabled and compares the	
	ADC channel input voltage with the upper and lower limits. The upper limit and lower limit are configured in RA 
	Configurator. If the ADC channel input voltage is above the upper limit or below the lower limit, it triggers an event 
	and notifies the user to act accordingly. Result and ADC status is displayed on the JLinkRTTViewer.


2. Hardware settings for the project
        Supported Boards - EK-RA8M1
      
        Hardware connections:
		
		ADC Channel Voltage Input Pin:

		Board	 Channel(Pin)
		------------------------------
		EK-RA8M1:
			AN000(P004)

3. Configuration Settings:
        The example project, by default, runs in continuous scan mode on the boards.
	In order to switch the scan mode, change the 'Mode' field of ADC Driver in the RA configurator to Single Scan.
﻿
4. Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000430
   b. Keil: Not Available 
   c. IAR: Not Available
 
5. If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
 

Note: Example project will generate .ihex file as default, it is required to change to .hex file to use with JFlashLite.
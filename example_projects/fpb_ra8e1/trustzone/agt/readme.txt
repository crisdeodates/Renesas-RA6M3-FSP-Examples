/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates the functionality of AGT in periodic mode and one-shot mode. On providing any 
	input on the RTT Viewer, AGT channel 0 starts in one-shot mode. AGT channel 1 starts in periodic mode when AGT 
	channel 0 expires. Timer in periodic mode expires periodically at a time period specified by user and toggles the 
	on-board LED.

2. Software Requirements:
	Refer to the Tools section in the Release Notes for set up suitable Software version.
	https://github.com/renesas/fsp/releases
	
Note:
1) User is expected to enter data of size not exceeding above 15 bytes.
   Operation is not guaranteed for any user input value other than integer,char(i.e. float, special char) through RTT.

2) User should view section 5 of application note R11AN0467 on how to build and run a trustzone project.
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection

3) For the IAR project, user can launch the Workspace using the .eww workspace file in the non-secure project folder. 
   It included multiple workspace which has both secure (_s) and non-secure (_ns) project.

4) For the KEIL project, user can launch the Workspace using the .uvmpw file in keil folder. It included multiple workspace 
   which has both secure (_s) and non-secure (_ns) project.

5) User must build secure project (_s) first, then build non-secure (_ns) project. 

6) After the projects are built successfully, user should follow these three steps in sequence to launch the debug session:
   - Initialize device back to factory default. Refer to the corresponding screen shots in the R11AN0467 to perform the action.
   - Set Trustzone secure / non-secure boundaries. Refer to the corresponding screen shots in the R11AN0467 to perform the action.
   - Launch the Debug session from the non-secure project.

7) For IAR project, User can change setting in the Tools -> Option -> Stack -> Uncheck "Stack pointer(s) not
   valid until program reaches" option to ignore Stack Warning at launch.
﻿
8) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x32062040
   b. Keil:	Not Available
   c. IAR:      0x3206207c     
 
9) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

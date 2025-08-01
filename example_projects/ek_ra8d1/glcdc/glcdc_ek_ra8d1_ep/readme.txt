/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates the basic functionalities of the GLCDC device driver on Renesas RA MCUs based on Renesas FSP. 
	On successful initialization of the GLCDC module, the EP will read the display resolution and frame buffer set in the GLCDC 
	module and draw color bands to cover the entire screen of external LCD connected to RA MCU. User can view raw image in e2studio IDE.
	Errors and Status information will be printed on Jlink RTT Viewer during the execution of the project.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M3G, EK-RA8D1
	1 x RA Board
	1 x Graphics Expansion Board (P/N RTK7EK6M3B00001BU)
	1 x Micro USB cable
	For EK-RA8D1: 1 x Renesas APP-LCD-CONV1 board or Jumper wires (Connect Graphics Expansion Board to RA board) 
	
3. Hardware Connections:
	For EK-RA6M3G:
		Connect Graphic Expandsion Board to J1, please refer to section 8.1 in EK-RA6M3G - User Manual 
		https://www.renesas.com/us/en/document/mat/ek-ra6m3g-v1-users-manual?r=1168091
		Connect the USB Debug port on EK-RA6M3G to the PC using a micro USB cable.
	
	For EK-RA8D1: 
		Set the configuration switches (SW1) as below to avoid potential failures.
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |     ON     |     ON      |    OFF    |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		
		If the user uses Renesas APP-LCD-CONV1 board:
			Connect Graphic Expandsion Board to APP-LCD-CONV1.
			Connect APP-LCD-CONV1 Board to J57.
			Connect the USB Debug port on EK-RA8D1 to the PC using a micro USB cable.
		
		If the user does not have Renesas APP-LCD-CONV1 board:
			The user can use Jumper wires to connect Graphic Expansion Board to EK-RA8D1 board manually, 
			please refer "Hardware Connection" section in markdown file to connect correctly.
			Connect the USB Debug port on EK-RA8D1 to the PC using a micro USB cable.

Note: 
1) The user can also view the "raw image" in memory monitor of the e2studio IDE using the address of g_framebuffer
   and specifying the resolution.
   For EK-RA8D1: Please select format "RGB: 32bpp (8:8:8:8)" to view "raw image" in memory monitor
   
2) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000c80
   b. Keil:	Not Available
   c. IAR: 	Not Available
   
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
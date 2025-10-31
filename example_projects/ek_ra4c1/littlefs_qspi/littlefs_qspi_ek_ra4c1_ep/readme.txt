/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of LittleFS for external flash (e.g., SPI, OSPI, QSPI)
	on RA MCU based on FSP. The project will perform various file system operations based on the user's selection
	from menu options, such as erase entire flash, erase 32 KB flash, format file system, directory management
	(create, get properties, delete), and file management (create, write, read, delete). The terminal application
	is used as the user interface. The menu options and system messages (errors and information messages)
	will be printed on the terminal application during the execution of the project.

2. Hardware Requirements:
	Supported RA board: EK-RA6M5, EK-RA4C1
 	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	For EK-RA6M5:
		Connect the USB Debug port on the RA board to the host PC via a micro USB cable.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

4. Software Configurations:
	By default, to create an empty file or open an existed file with malloc
		LittleFS | Use Malloc        : Enabled
		BSP 	 | Heap size (bytes) : 0x400

	To create an empty file or open an existed file without malloc
		LittleFS | Use Malloc        : Disabled
		BSP 	 | Heap size (bytes) : 0

5. Verifying Operation:
	1. Import the example project. Note that the EP supports the Serial Terminal by default for RA boards that support J-link OB VCOM.
	* By default, the EP supports Serial Terminal for RA boards that support J-link OB VCOM
		* Define USE_VIRTUAL_COM=1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Includes
	
	* To use SEGGER J-Link RTT Viewer, please follow the instructions as below:
		* Define USE_VIRTUAL_COM=0 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Includes 
	2. Generate, build the Example project.
	3. Connect the RA MCU debug port to the host PC via a Type-C USB cable.
	4. Debug or flash the example project to the RA board.
	5. Open the J-Link RTT Viewer on the host PC.
	6. After the main menu is displayed on the J-Link RTT Viewer, the user selects options to perform file system management as desired.
		Type '1' and enter to select Flash Operations Menu.
			Type '1' and enter to erase the entire flash.
			Type '2' and enter to erase the first 32 KB of the flash.
			Type '3' and enter to format the file system.
			Type '4' and enter to go back to the Main Menu.
			Note: After erasing the entire flash or 32 KB flash, the user must format the file system.

		Type '2' and enter to select Directory Operations Menu.
			Type '1' and enter to create a new directory.
			Type '2' and enter to get the root directory properties.
			Type '3' and enter to delete a directory.
			Type '4' and enter to go back to the Main Menu.

		Type '3' and enter to select File Operations Menu.
			Type '1' and enter to create an empty file or open an existed file.
			Type '2' and enter to write a fixed content into a file.
			Type '3' and enter to read the entire file and display the first 1 KB of its content.
			Type '4' and enter to delete a file.
			Type '5' and enter to go back to the Main Menu.

Note:
1) For using the Serial terminal application:
	a) To echo back characters typed in Tera Term, the user needs to enable it through
		[Setup] -> [Terminal...] -> Check [Local echo].
	b) The configuration parameters of the serial port on the terminal application are as follows:
		COM port is port provided by the J-Link on-board.
		Baud rate: 115200 bps
		Data length: 8-bits  
		Parity: none
		Stop bit: 1-bit
		Flow control: none

2) For using the J-Link RTT Viewer:
   If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of USB PHID + PHID Composite Device on Renesas RA MCUs based on Renesas FSP.
	The project configures RA board as a Keyboard and a Mouse.
	On pressing "k" from RTTViewer, MCU will work as a Keyboard. MCU sends the host PC characters from a - z & numbers 0 - 9.
	On pressing "m" from RTTViewer, MCU will work as a Mouse. MCU sends the host PC  the coordinate of the mouse which let it move square on host PC's screen.

2. Hardware Requirement:

	micro USB Cable : 2

3. Hardware connection :

	EK-RA6M2:
	Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed
	port (J9) of the board.  Connect the other end of this cable to USB port of the
	host PC.

	Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed
	port (J11) of the board.  Connect the other end of this cable to USB port of the
	host PC.

Note :
1. Open Device Manager, and check 'Human Interface Devices', to see ' USB Input Device' has been detected.
2. Open Notepad on the PC, lower cases a - z, and numbers 0 - 9 will be shown on the Notepad in case "k" (Keyboard) is chosen
3. On the PC, mouse will move square shape on the screen in case "m" (Mouse) is chosen.
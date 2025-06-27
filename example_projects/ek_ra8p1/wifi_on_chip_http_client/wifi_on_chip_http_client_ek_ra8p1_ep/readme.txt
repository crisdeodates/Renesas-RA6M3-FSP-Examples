/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the functionalities of the HTTP Client within the DA16200 module, employing the FSP
	Integrated Onchip DA16XXX Middleware driver, and leveraging FreeRTOS on Renesas RA MCUs. EP uses the
	US159-DA16200EVZ Ultra Low Power Wi-Fi Pmod™ module to connect to an access point and communicate with a web
	server using HTTP methods (GET, PUT, POST). After sending each request, EP waits for the HTTP response and
	prints it to the RTT Viewer. Additionally, status and failure messages are displayed on the RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.0.0
	e2 studio: Version 2025-04.1
	SEGGER J-Link RTT Viewer: Version 8.44a
	LLVM Embedded Toolchain for ARM: Version 18.1.3

3. Hardware Requirements:
	Supported RA boards: EK-RA6M5, EK-RA2A2, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1,
			     EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA8D1, EK-RA8M1, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1,
			     MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, EK-RA8P1
	1 x Renesas RA board.
	1 x USB cable for programing and debugging.
		- Type-C USB cable: MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, EK-RA8P1.
		- Micro USB cable: the other boards.
	1 x Wi-Fi router Access Point(AP) with internet access. Note: WiFi router AP's credentials should be known.
	1 x US159-DA16200EVZ Ultra Low Power Wi-Fi Pmod™ (Part Number: US159-DA16200MEVZ) (SDK v3.2.9.2).
		Note: Please ensure the Pmod™ SDK is up-to-date before executing the project. 
		      For SDK update instructions, please refer to Section 5, 6, and 7 in the provided link:
		      https://www.renesas.com/en/document/apn/da16200da16600-sdk-update-guide
	
4. Hardware Connections:
	For EK-RA4M1, EK-RA6M1, EK-RA6M2:
	---------
		1) Plug in the US159-DA16200MEVZ to the PMOD A connector on the MCU evaluation kit and be careful to align
		   Pin 1 on the module to Pin 1 on the MCU kit.
		2) Connect the USB Debug port on the RA board to the PC using a Micro USB cable.

	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G:
	---------
		1) Plug in the US159-DA16200MEVZ to the PMOD 1 connector on the MCU evaluation kit and be careful to align
		   Pin 1 on the module to Pin 1 on the MCU kit.
		2) Connect the USB Debug port on the RA board to the PC using a Micro USB cable.

	For EK-RA6M5, EK-RA2L1, EK-RA4E2, EK-RA6E2, EK-RA6M4, EK-RA8D1, EK-RA8M1, FPB-RA4E1, FPB-RA6E1:
	---------
		1) Plug in the US159-DA16200MEVZ to the PMOD 2 connector on the MCU evaluation kit and be careful to align
		   Pin 1 on the module to Pin 1 on the MCU kit.
		2) Connect the USB Debug port on the RA board to the PC using a Micro USB cable.

	For MCK-RA4T1, MCK-RA6T3, EK-RA8P1:
		1) Plug in the US159-DA16200MEVZ to the PMOD 2 connector on the MCU evaluation kit and be careful to align
		   Pin 1 on the module to Pin 1 on the MCU kit.
		2) Connect the USB Debug port on the RA board to the PC using a Type-C USB cable.

	For EK-RA2A2:
	---------
		Connect
		1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> P501/TXD3 (J1:P1)
		2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> P500/RXD3 (J4:P19)
		3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (J18:P6)
		4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (J4:P1)
		5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> P203/RESET (J3:P9)
		Note: If using Pmod2 on EK-RA2A2, it is necessary to solder jumper Ex3.
		Connect the USB Debug port on EK-RA2A2 to the PC using a Micro USB cable.

	For MCK-RA6T2:
	---------
		Connect
		1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> PD05/TXD9 (CN10:P3)
		2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> PD06/RXD9 (CN10:P2)
		3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (CN6:P32)
		4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (CN6:P34)
		5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> PA12/RESET (CN6:P12)
		Connect the USB Debug port on MCK-RA6T2 to the PC using a Type-C USB cable.
	
	For MCK-RA8T1:
	---------
		Connect
		1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> P112/TXD0 (CN2:P1)
		2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> P113/RXD0 (CN1:P33)
		3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (CN1:P30)
		4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (CN1:P23)
		5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> P114/RESET (CN1:P31)
		Connect the USB Debug port on MCK-RA8T1 to the PC using a Type-C USB cable.

Note:	  
1) The segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000490
   b. Keil:     Not Available 
   c. IAR:      Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

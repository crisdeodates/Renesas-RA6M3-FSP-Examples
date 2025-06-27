/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:	
	The example project demonstrates the typical use of the ACMPHS HAL module APIs. DAC0 is used as reference
	voltage source and DAC1 is used as input voltage source for ACMPHS module. DAC0 value is set to 2048 
	(i.e., 1.65V). The user can enter DAC1 value within permitted range. When DAC1 input value is greater than set
	DAC0 reference voltage, the comparator output status is HIGH and on-board LED is turned ON. When DAC1 input is
	less than reference voltage, output status is LOW and the LED is turned OFF.
	
	For FPB-RA8E1, EK-RA8E2:
	The example project demonstrates the typical use of the ACMPHS HAL module APIs. Internal reference voltage
	(Vref) is used as reference voltage source and AN000 is used as input voltage source for ACMPHS module, which
	is supplied by output of DAC0. Internal reference voltage value is set to 1.18V (equivalent to a digital value
	1465). The user can enter DAC0 value within permitted range to provide an analog signal input for AN000. When
	input voltage source is greater than reference voltage, the comparator output status is HIGH and on-board LED
	is turned ON. When input voltage source is less than reference voltage, output status is LOW and the LED is
	turned OFF.
	
2. Hardware Requirements:
	Supported RA Boards: EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, RSSK-RA6T1, MCK-RA6T2, MCK-RA4T1, MCK-RA6T3,
                             EK-RA8M1, EK-RA8D1, MCK-RA8T1, FPB-RA8E1, EK-RA8E2, EK-RA8P1.
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	For MCK-RA6T2, FPB-RA8E1, EK-RA8E2:
		1 x Jumper wire.
	
3. Hardware Connections:
	For MCK-RA6T2:
		Connect PA07 (CN4:11) to PA00 (CN4:9).
	
	For FPB-RA8E1:
		Connect DA0 P014 (J1:34) to IVCMP0_2 (AN000) P004 (J1:20).

	For EK-RA8E2:
		Connect DA0 P014 (J17:2) to IVCMP0_2 (AN000) P004 (J4:12).

Note:
1) For boards that support DAC module:
   a. The user has to input DAC value in between (0 - 2000) or (2100 - 4095).
   b. The user is expected to enter data not exceeding 15 bytes in size.
   c. Operation is not guaranteed for any user input value other than integer (e.g., float, char, special char) through
      RTT Viewer.

2) For FPB-RA8E1, EK-RA8E2:
   a. Internal reference voltage (Vref) is used as reference voltage source, it is set to 1.18V (equivalent to a digital
      value 1465).
   b. The user has to input DAC0 value in between (0 - 1400) or (1600 - 4095) to prevent voltage
      fluctuations caused by electrical noise, which may affect the comparator output.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000450
   b. Keil: 	Not Available
   c. IAR: 	Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

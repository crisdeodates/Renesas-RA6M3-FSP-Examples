/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates Audio Playback with PWM module's functionality. It uses ADPCM module to
	decode the PCM data. The Decoded PCM data is in signed 16-bit format. Application has to convert this signed
	PCM data into unsigned PCM data and provide the converted unsigned data to the audio playback with PWM module.
	The converted unsigned data is provided as input and output is generated as PWM signal. Error and info messages
	will be printed on J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA Boards: EK-RA2A1, EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6M1, EK-RA6M2, EK-RA6M3,
		             EK-RA6M4, RSSK-RA6T1, EK-RA6M5, FPB-RA4E1, FPB-RA6E1, MCK-RA6T2, EK-RA4E2, EK-RA6E2,
			     MCK-RA4T1, MCK-RA6T3, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA2A2, FPB-RA8E1, EK-RA4L1,
			     EK-RA8E2, EK-RA8P1.
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Pmod AMP2 (e.g., https://digilent.com/reference/pmod/pmodamp2/).
	1 x Wired headsets with 3.5mm audio jack.
	Some jumper wires.

3. Hardware Connections:

	EK-RA2A1                          	Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P109       	---->  	AIN  	  (Pin 1)
	VCC --(via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                        	---->  	VCC 	  (Pin 6)
	GND                        	---->  	GAIN	  (Pin 2)
	GND                        	---->  	GND 	  (Pin 5)

	EK-RA2L1                          	Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P111       	---->  	AIN  	  (Pin 1)
	VCC -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                        	---->  	VCC 	  (Pin 6)
	GND                        	---->  	GAIN	  (Pin 2)
	GND                        	---->  	GND 	  (Pin 5)

	EK-RA4M1, EK-RA6M1
	EK-RA6M5, RSSK-RA6T1
	FPB-RA4E1, FPB-RA6E1
	EK-RA4E2, EK-RA6E2
	MCK-RA4T1, MCK-RA6T3               	Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P500       	---->  	AIN  	  (Pin 1)
	VCC -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                        	---->  	VCC 	  (Pin 6)
	GND                        	---->  	GAIN	  (Pin 2)
	GND                        	---->  	GND 	  (Pin 5)

	EK-RA4M2, EK-RA6M3       		Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P107      	---->  	AIN  	  (Pin 1)
	VCC -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                        	---->  	VCC 	  (Pin 6)
	GND                        	---->  	GAIN	  (Pin 2)
	GND                        	---->  	GND 	  (Pin 5)

	EK-RA4W1                          	Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P501     	---->  	AIN  	  (Pin 1)
	VCC -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                        	---->  	VCC 	  (Pin 6)
	GND                        	---->  	GAIN	  (Pin 2)
	GND                        	---->  	GND 	  (Pin 5)

	EK-RA4M3, EK-RA6M2
	EK-RA6M4, EK-RA8M1, EK-RA8D1        	Pmod AMP2                         
	----------------------------------------------------------
	PWM_OUTPUT_PIN P713       	---->  	AIN  	  (Pin 1)
	VCC -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                        	---->  	VCC 	  (Pin 6)
	GND                        	---->  	GAIN	  (Pin 2)
	GND                        	---->  	GND 	  (Pin 5)

	MCK-RA6T2                          	Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN PB04 (CN4:29)	---->  	AIN  	  (Pin 1)
	VCC -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                        	---->  	VCC 	  (Pin 6)
	GND                        	---->  	GAIN	  (Pin 2)
	GND                        	---->  	GND 	  (Pin 5)

	MCK-RA8T1				    	Pmod AMP2
	------------------------------------------------------------------
	PWM_OUTPUT_PIN P411 (CN6:9)        	---->  	AIN  	  (Pin 1)
	VCC (CN3:23) -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC (CN3:24)                        	---->  	VCC 	  (Pin 6)
	GND (CN3:32)                        	----> 	GAIN	  (Pin 2)
	GND (CN3:34)                        	---->  	GND 	  (Pin 5)

	EK-RA2A2                          	Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P110 (J4:9) 	----> 	AIN  	  (Pin 1)
	VCC -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                        	----> 	VCC  	  (Pin 6)
	GND                        	---->  	GAIN	  (Pin 2)
	GND                        	---->  	GND 	  (Pin 5)

	FPB-RA8E1       		  	Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P107 (J1:49)	---->  	AIN  	  (Pin 1)
	VCC -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                        	---->  	VCC 	  (Pin 6)
	GND                        	---->  	GAIN	  (Pin 2)
	GND                        	---->  	GND 	  (Pin 5)

	FPB-RA4L1       		  	Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P107 (J2:49)	---->  	AIN  	  (Pin 1)
	VCC -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                        	---->  	VCC 	  (Pin 6)
	GND                        	---->  	GAIN	  (Pin 2)
	GND                        	---->  	GND 	  (Pin 5)

	EK-RA8E2       		  		Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P107 (J3:21)	---->  	AIN  	  (Pin 1)
	VCC -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                        	---->  	VCC 	  (Pin 6)
	GND                        	---->  	GAIN	  (Pin 2)
	GND                        	---->  	GND 	  (Pin 5)

	EK-RA8P1      		  		Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P713 (J1:35)	---->  	AIN  	  (Pin 1)
	VCC -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                        	---->  	VCC 	  (Pin 6)
	GND                        	---->  	GAIN	  (Pin 2)
	GND                        	---->  	GND 	  (Pin 5)

Note: 
1) The user is expected to enter data not exceeding 15 bytes in size.
   Operation is not guaranteed for any user input value other than integer (e.g., float, char, special character) through RTT.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000450
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

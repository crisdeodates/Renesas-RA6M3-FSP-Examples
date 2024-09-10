/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of AWS https client library with ethernet driver on Renesas RA MCUs 
based on Renesas FSP using FreeRTOS. aws client is used to connect to https adafruit server which is cloud platform. 
On successful connection, menu is displayed enabling user to send GET,PUT,POST requests to adafruit server. On POST/PUT 
request, MCU Die temperature is read via(using) ADC and uploaded to server. On GET request, the last MCU die temperature data 
is read from the adafruit server. JlinkRTTViewer is used to display the status and responses of the requests made to server.

Note: After the initial successful connection, User should run the POST request first and then the GET request prior to running the PUT request.

2. Hardware Requirement:

	i. micro usb cable - 1no or type C usb cable - 1no (For MCK-RA8T1).
       ii. LAN cable - 1no.
      iii. Ethernet Switch - 1no.

3. Hardware Connections:
 	Supported Board EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1:		
	i. Connect EK-RA6M3/EK-RA6M5/EK-RA8M1/EK-RA8D1/MCK-RA8T1 board to Host machine using micro usb cable (using type C usb cable for MCK-RA8T1).
       ii. Connect LAN cable on EK-RA6M3/EK-RA6M5/EK-RA8M1/EK-RA8D1/MCK-RA8T1 at ethernet port and other end connect to ethernet Switch. 
           Switch should have WAN connection to communicate the server over internet. The Switch should be connected to the router 
           which is connected to the Internet.
	
4. Hardware Configurations:

- For EK-RA8D1: Set the configuration switches (SW1) as below to avoid potential failures.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |      OFF    |      OFF     |     OFF    |     ON     |      OFF   |      OFF    |     OFF   |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+	

	CAUTION: Do not enable SW1-4 and SW1-5 together

- For EK-RA8M1: Remove jumper J61 to enable Ethernet B

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000cec
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

NOTE  : On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
        Consequentially, it is necessary to connect the serial programming interface to meet this requirement.

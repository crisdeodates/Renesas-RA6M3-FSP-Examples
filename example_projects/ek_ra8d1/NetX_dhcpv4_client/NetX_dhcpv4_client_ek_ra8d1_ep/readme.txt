/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of Netx Duo DHCPv4 client with an ethernet driver on Renesas 
        RA MCUs based on Renesas FSP using AzureRTOS. DHCP Client is created and started. On successful receiving of 
        IP address from DHCPv4 server, It pings the server to verify the received IP address. On successful completion, 
        the success status and IP address are displayed on the RTT viewer. Error and info messages will be printed on JlinkRTTViewer.
  	
2. Hardware Requirement:

	i. micro usb cable - 1no.
       ii. LAN cable - 1no.
      iii. Ethernet Switch - 1no.

3. Hardware Connections:
 	Supported Board EK-RA6M3/EK-RA6M4/EK-RA6M5/EK-RA8M1/EK-RA8D1:		
	i. Connect RA board to Host machine using micro usb cable.
       ii. connect LAN cable on RA board at ethernet port and other end connect to ethernet switch/router.

4. Hardware Configuration:

- For EK-RA8D1: Set the configuration switches (SW1) as below to avoid potential failures.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |     OFF     |      OFF     |     OFF    |     ON     |     OFF    |     OFF     |     OFF   |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+	

	CAUTION: Do not enable SW1-4 and SW1-5 together

- For EK-RA8M1: Remove jumper J61 to enable Ethernet B
	
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000950
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

NOTE  : On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
        Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
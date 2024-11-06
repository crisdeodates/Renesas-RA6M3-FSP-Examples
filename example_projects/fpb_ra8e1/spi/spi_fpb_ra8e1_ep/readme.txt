/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     	The example project demonstrates the typical use of the SPI HAL module APIs using SPI and DTC modules. The project
        configure SPI channels (Channel 0 and Channel 1) in Master and Slave mode. Once the modules are initialized and
        the SPI channels are configured, Master and Slave can transmit and receive data based on commands from user sent
     	through J-Link RTT Viewer. SPI data transmission and reception are handled by the DTC module.

2. Hardware connections:
     	For EK-RA2A1:
    		MISO  ----> P109 - P104
     		MOSI  ----> P400 - P105
     		RSPCK ----> P110 - P103
    
     	For EK-RA4M1:
     		MISO  ----> P410 - P202
     		MOSI  ----> P411 - P203
     		RSPCK ----> P412 - P204
	
     	For EK-RA4W1:
     		MISO  ----> P100 - P110
     		MOSI  ----> P101 - P109
     		RSPCK ----> P102 - P111

     	For EK-RA6M1:
     		MISO  ----> P100 - P110
     		MOSI  ----> P101 - P109
     		RSPCK ----> P102 - P111

     	For EK-RA6M2:
     		MISO  ----> P410 - P202
     		MOSI  ----> P411 - P203
     		RSPCK ----> P412 - P204

     	For EK-RA6M3:
     		MISO  ----> P410 - P202
     		MOSI  ----> P411 - P203
     		RSPCK ----> P412 - P204

     	For EK-RA6M3G:
     		MISO  ----> P410 - P202
     		MOSI  ----> P411 - P203
     		RSPCK ----> P412 - P204

     	For EK-RA6M4:
     		MISO  ----> P202 - P410 
     		MOSI  ----> P203 - P411
     		RSPCK ----> P204 - P412
	
     	For EK-RA6T1:
     		MISO  ----> P100 - P110 
     		MOSI  ----> P101 - P109
     		RSPCK ----> P102 - P111

     	For EK-RA2L1/EK-RA6M5:
     		MISO  ----> P410 - P202
     		MOSI  ----> P411 - P203
     		RSPCK ----> P412 - P204

     	For FPB-RA6E1:
     		MISO  ----> P410 - P110
     		MOSI  ----> P411 - P109
     		RSPCK ----> P412 - P111

     	For MCK-RA6T2:
     		MISO  ----> PE13 - PB14
     		MOSI  ----> PE14 - PB15
     		RSPCK ----> PE12 - PB13

     	For MCK-RA4T1/MCK-RA6T3/EK-RA4E2:
     		MISO  ----> P206 - P100
     		MOSI  ----> P207 - P101
     		RSPCK ----> P302 - P102

     	For EK-RA6E2:
     		MISO  ----> P110 - P100
     		MOSI  ----> P109 - P101
     		RSPCK ----> P111 - P102

     	For EK-RA8M1:
     		MISO  ----> P700 - P410
     		MOSI  ----> P701 - P411
     		RSPCK ----> P702 - P412
     		Install jumper J61 to disable Ethernet B (Use P700, P701 for SPI channel 0)

     	For EK-RA8D1:
     		MISO  ----> P700 - P410
     		MOSI  ----> P701 - P411
     		RSPCK ----> P702 - P412
     		Turn OFF SW1-3 and SW1-5 to use P700, P701, P702 for SPI channel 0

     	For MCK-RA8T1:
     		MISO  ----> P313 (CN6:3) - P100 (CN2:7)
     		MOSI  ----> P202 (CN6:2) - P101 (CN4:6)
     		RSPCK ----> P203 (CN6:4) - P102 (CN2:5)

     	For FPB-RA8E1:
     		MISO  ----> P609 (J3:13) - P410 (J2:12)
     		MOSI  ----> P611 (J3:12) - P411 (J2:9)
     		RSPCK ----> P610 (J3:14) - P412 (J2:10)

Note: 
1) User is expected to enter data of size not exceeding 64 bytes.
   As SPI bit width is set to 32 bits, each 4 bytes of user data input will be transferred in single SPI transmission.
   The bytes sent for slave should be lesser than the bytes sent for master when using WriteRead operation.
   Operation is not guaranteed for any user input value other than integer,char(i.e. float, special char) through RTT.
﻿
2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22060200
   b. Keil:	0x22060584
   c. IAR:      0x220605a4
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

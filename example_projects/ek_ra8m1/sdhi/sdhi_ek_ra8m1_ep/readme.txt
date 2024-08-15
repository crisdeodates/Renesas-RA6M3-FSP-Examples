/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    This example project demonstrates basic functionalities of sdhi driver with FreeRTOS+FAT file system on Renesas
    RA MCUs based on Renesas FSP. FreeRTOS+FAT uses the underlying Block media driver.
    The Block media driver utilizes SDHI driver to perform file operations on the SD Card.
    Error and info messages will be printed on JlinkRTTViewer.

Configuration changes to switch Bus Width in configurator: (Same for EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G and EK-RA6M4 boards)

    For 4 Bit Width
    Under SDMMC Stack
        Bus Width: 4 Bits
        
    For 1 Bit Width
    Under SDMMC Stack
        Bus Width: 1 Bits

2. Hardware Requirements:
    External Hardware : PMOD SD Full sized SD Card slot

3. Hardware Connections:

For EK-RA6M3 & EK-RA6M3G
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P415(CD)     --->    PMOD SD Pin9(CD)
        P414(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

For EK-RA6M2
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P210(CD)     --->    PMOD SD Pin9(CD)
        P209(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        Tracecut Jumper E15 and solder to E12
        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

For EK-RA6M1
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P210(CD)     --->    PMOD SD Pin9(CD)
        P209(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        Tracecut Jumper E15 and solder to E12
        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

For EK-RA6M4
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P210(CD)     --->    PMOD SD Pin9(CD)
        P209(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

For EK-RA6M5
    1 Bit Width

        P412(CMD)(J1 Pin34) ---> PMOD SD Pin2(MOSI)
        P411(DAT0)(J1 Pin35) ---> PMOD SD Pin3(MISO)
        P413(CLK)(J1 Pin33) ---> PMOD SD Pin4(SCLK)
        GND ---> PMOD SD Pin5(GND)
        +3V3(VCC) ---> PMOD SD Pin6(VCC)
        P210(CD)(J2 Pin17) -→ PMOD SD Pin9(CD)
        P209(WP)(J2 Pin18) ---> PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width.In addition below connections


        P410(DAT1)(J1 Pin36) ---> PMOD SD Pin7(DAT1)
        P206(DAT2)(J2 Pin4) ---> PMOD SD Pin8(DAT2)
        P205(DAT3)(J2 Pin5) ---> PMOD SD Pin1(CS)


For FPB_RA6E1
    1 Bit Width

       P412(CMD)(J3 Pin20) ---> PMOD SD Pin2(MOSI)
       P411(DAT0)(J3 Pin21) ---> PMOD SD Pin3(MISO)
       P413(CLK)(J3 Pin19) ---> PMOD SD Pin4(SCLK)
       GND ---> PMOD SD Pin5(GND)
       +3V3(VCC) ---> PMOD SD Pin6(VCC)
       P210(CD)(J3 Pin35) -→ PMOD SD Pin9(CD)
       P209(WP)(J3 Pin36) ---> PMOD SD Pin10(WP)

    4 Bit Width
       Above mentioned connections for 1 Bit Width. In addition below connections


       P410(DAT1)(J3 Pin22) ---> PMOD SD Pin7(DAT1)
       P206(DAT2)(J3 Pin31) ---> PMOD SD Pin8(DAT2)
       P205(DAT3)(J3 Pin32) ---> PMOD SD Pin1(CS)

For EK_RA8M1
    *Note: User need to remove jumper J61 to use SHDI channel 0.
    Hardware connection for SDHI channel 0 as below:	
    1 Bit Width
       P307(CMD)(J54 Pin5) 	---> PMOD SD Pin2(MOSI)
       P304(DAT0)(J54 Pin9) 	---> PMOD SD Pin3(MISO)
       P308(CLK)(J54 Pin4) 	---> PMOD SD Pin4(SCLK)
       GND 			---> PMOD SD Pin5(GND)
       +3V3(VCC) 		---> PMOD SD Pin6(VCC)
       P306(CD)(J54 Pin7) 	---> PMOD SD Pin9(CD)
       P305(WP)(J54 Pin8) 	---> PMOD SD Pin10(WP)

    4 Bit Width
       Above mentioned connections for 1 Bit Width. In addition below connections

       P303(DAT1)(J54 Pin6)  ---> PMOD SD Pin7(DAT1)
       P302(DAT2)(J54 Pin14) ---> PMOD SD Pin8(DAT2)
       P301(DAT3)(J54 Pin12) ---> PMOD SD Pin1(CS)

Note:
0. Connect PMOD: use short wiring connections (should be shorter than 10cm).
1. Ensure that PMOD SD Pin11(GND) is connected to GND and PMOD SD Pin12(VCC) is connected to 3.3V.
2. Format the SD Card before performing any operation.
3. Data written can also be manually verified in file ra_sdhi.txt.
4. User is expected to execute "safely remove SD Card" option before removing SD Card else further file operations may fail and
   SD Card data may get corrupted.
5. User has to remove and re-insert the SD Card after executing the "safely remove SD Card" & "Initialize FreeRTOS+FAT" option.
6. If SD Card is removed without "safely remove SD Card" option, user is recommended to perform "safely remove SD Card" and
   "Initialize FreeRTOS+FAT" commands in sequence. This will make sure no operations fails after unexpected removal of SD Card.
7. User is expected to enter RTT input data of size not exceeding 20 bytes.
8. Operation is not guaranteed for any user input value other than integer, char(i.e. float, special char) through
   JlinkRTTViewer input.
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000900
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

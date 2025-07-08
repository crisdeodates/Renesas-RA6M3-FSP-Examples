/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    This Example Project demonstrates SCI_I2C Master operation through loop-back with IIC Slave driver.
    6 bytes of data will be transmitted and received continuously on successful initialization. 
    The transmitted data is compared with the received data. If the data matches, on-board LED
    starts blinking. On a data mismatch, LED stays ON.
    Failure messages and status is displayed on RTTViewer.

    LED output Status on Master TX and RX data mismatch(failure) and data match(success)
    a) Failure  - Led is set as ON
    b) Success  - Led blinks for each transaction

2. Hardware Settings:
    Two jumper wires are required to establish loop back connection along IIC lines within the board with pins as mentioned below.

    EK-RA6M3 & EK-RA6M3G
    --------
      Channel 0 has been used by SCI_I2C Master and IIC Slave.
      1) SCI_I2C Master pins
        SCI0 P411 (Jumper J3 Pin 36) ----> SDA
        SCI0 P410 (Jumper J3 Pin 35) ----> SCL
      2) Slave IIC pins
        IIC0 P401 (Jumper J3 Pin 09) ----> SDA
        IIC0 P408 (Jumper J3 Pin 37) ----> SCL

    EK-RA6M2
    --------
      Channel 0 has been used by SCI_I2C Master and IIC Slave.
      1) SCI_I2C Master pins
        SCI0 P411 (Jumper J2 Pin 20) ----> SDA
        SCI0 P410 (Jumper J2 Pin 02) ----> SCL
      2) Slave IIC pins
        IIC0 P401 (Jumper J4 Pin 11) ----> SDA
        IIC0 P400 (Jumper J4 Pin 13) ----> SCL

    EK-RA6M1
    --------
      Channel 0 has been used by SCI_I2C Master and IIC Slave.
      1) SCI_I2C Master pins
        SCI0 P411 (Jumper J2 Pin 20) ----> SDA
        SCI0 P410 (Jumper J4 Pin 14) ----> SCL
      2) Slave IIC pins
        IIC0 P401 (Jumper J1 Pin 03) ----> SDA
        IIC0 P400 (Jumper J1 Pin 01) ----> SCL

    EK-RA6M4
    --------
      Channel 7 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
      1) SCI_I2C Master pins
        SCI7 P613 (Jumper J3 Pin 16) ----> SDA
        SCI7 P614 (Jumper J3 Pin 17) ----> SCL
      2) Slave IIC pins
        IIC1 P511 (Jumper J2 Pin 38) ----> SDA
        IIC1 P512 (Jumper J2 Pin 37) ----> SCL

    RSSK-RA6T1
    --------
      Channel 0 has been used by SCI_I2C Master and IIC Slave.
      1) SCI_I2C Master pins
        SCI0 P101 (CN8- pin 2)   ----> SDA
        SCI0 P100 (CN8- pin 3)   ----> SCL
      2) Slave IIC pins
        IIC0 P401 (CN6- pin 1)   ----> SDA
        IIC0 P400 (CN9- pin 1)   ----> SCL

    EK-RA4W1
    --------
      Channel 0 has been used by SCI_I2C Master and IIC Slave.
      1) SCI_I2C Master pins
        SCI0 P101 (Header CN7 pin 26) ----> SDA
        SCI0 P100 (Header CN7 pin 27) ----> SCL
      2) Slave IIC pins
        IIC0 P407 (Header CN7 Pin 01) ----> SDA
        IIC0 P204 (Header CN7 Pin 09) ----> SCL

    EK-RA4M1
    --------
      Channel 0 has been used by SCI_I2C Master and IIC Slave.
      1) SCI_I2C Master pins
        SCI0 P411 (Jumper J2 Pin 04) ----> SDA
        SCI0 P410 (Jumper J2 Pin 02) ----> SCL
      2) Slave IIC pins
        IIC0 P401 (Jumper J2 Pin 08) ----> SDA
        IIC0 P400 (Jumper J2 Pin 37) ----> SCL

    EK-RA2A1
    --------
      Channel 0 has been used by SCI_I2C Master and IIC Slave.
      1) SCI_I2C Master pins
        SCI0 P302 (Jumper J2 Pin 04) ----> SDA
        SCI0 P301 (Jumper J2 pin 02) ----> SCL
      2) Slave IIC pins
        IIC0 P401 (Jumper J1 Pin 03) ----> SDA
        IIC0 P000 (Jumper J1 Pin 01) ----> SCL
		
    EK-RA2L1
    --------
      Channel 0 has been used by SCI_I2C Master and IIC Slave.
      1) SCI_I2C Master pins
        SCI0 P205 (Jumper J1 Pin 15) ----> SDA
        SCI0 P206 (Jumper J1 pin 14) ----> SCL
      2) Slave IIC pins
        IIC0 P401 (Jumper J4 Pin 08) ----> SDA
        IIC0 P400 (Jumper J4 Pin 07) ----> SCL

    EK-RA4M3
    --------
      Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
      1) SCI_I2C Master pins
        SCI9 P602 (Jumper J3 Pin 28) ----> SDA
        SCI9 P601 (Jumper J3 Pin 29) ----> SCL
      2) Slave IIC pins
        IIC1 P511 (Jumper J2 Pin 38) ----> SDA
        IIC1 P512 (Jumper J2 Pin 37) ----> SCL

   EK-RA4M2
   ---------
     Channel 9 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
     1) SCI_I2C Master pins
        SCI9 P602 (Jumper J3 Pin 28) ----> SDA
        SCI9 P601 (Jumper J3 Pin 29) ----> SCL 
     2) Slave IIC pins
        IIC0 P401 (J1 Pin 03) ----> SDA
        IIC0 P400 (J1 Pin 04) ----> SCL
   
   EK-RA2E1
   ---------
     Channel 2 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
     1) SCI_I2C Master pins
        SCI2 P302 (Jumper J27 Pin 02) ----> SDA
        SCI2 P301 (Jumper J27 Pin 01) ----> SCL 
     2) Slave IIC pins
        IIC0 P407 (J2 Pin 09) ----> SDA
        IIC0 P408 (J2 Pin 08) ----> SCL

   EK-RA6M5
   ---------
     Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
     1) SCI_I2C Master pins
        SCI9 P602 (Jumper J3 Pin 29) ----> SDA
        SCI9 P601 (Jumper J3 Pin 30) ----> SCL 
     2) Slave IIC pins
        IIC1 P511  (Jumper J4 Pin 39)  ----> SDA
        IIC1 P512  (Jumper J4 Pin 38)  ----> SCL

   FPB-RA6E1
   ----------
     Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
     1) SCI_I2C Master pins
        SCI9 P602 (Jumper J4 Pin 15) ----> SDA
        SCI9 P601 (Jumper J4 Pin 16) ----> SCL
     2) Slave IIC pins
        IIC1 P206 (Jumper J3 Pin 31)  ----> SDA
        IIC1 P205 (Jumper J3 Pin 32)  ----> SCL

   FPB-RA4E1
   -----------
     Channel 4 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
     1) SCI_I2C Master pins
        SCI4 P206 (Jumper J3 Pin31)  ----> SCL
        SCI4 P205 (Jumper J3 Pin 32) ----> SDA
     2) Slave IIC pins
        IIC0 - P400 (Jumper J3 Pin1) ----> SCL 
        IIC0 - P401 (Jumper J3 Pin 2)----> SDA

   MCK-RA6T2
   -----------
     Channel 1 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
     1) SCI_I2C Master pins
        SCI1 PB08 (CN5:3)  ----> SCL
        SCI1 PB09 (CN5:1)  ----> SDA
     2) Slave IIC pins
        IIC0 PB06 (CN4:33) ----> SCL 
        IIC0 PB07 (CN4:31) ----> SDA
  
   EK-RA8M1
   -----------
     Remove Jumper J61 to disable I3C, use pin P401, P400 for SCI_I2C.
     Channel 1 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
     1) SCI_I2C Master pins
        SCI1 P401 (Jumper J56 Pin32)  ----> SCL
        SCI1 P400 (Jumper J56 Pin36)  ----> SDA
     2) Slave IIC pins
        IIC0 P410 (Jumper J51 Pin46)  ----> SCL 
        IIC0 P409 (Jumper J51 Pin49)  ----> SDA

   FPB-RA2E3
   -----------
     Channel 0 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
     1) SCI_I2C Master pins
        SCI1 P100 (Jumper J4 Pin12)  ----> SCL
        SCI1 P101 (Jumper J4 Pin11)  ----> SDA
     2) Slave IIC pins
        IIC0 P400 (Jumper J3 Pin1)  ----> SCL 
        IIC0 P401 (Jumper J3 Pin2)  ----> SDA

Note:
1) For the functioning of SCI_I2C Master and IIC Slave on all the boards except for EK-RA6M3,EK-RA6M3G,FPB-RA6E1,FPB-RA4E1,FPB-RA2E3 external pull up
   resistors of value 3.9 or 4.7k ohms are required to be connected on I2C(SDA/SCL) lines.

2) For EK-RA8M1, value of external pull up resistors is 1.0k ohms.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004c48
   b. Keil:	Not Available 
   c. IAR: 	Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

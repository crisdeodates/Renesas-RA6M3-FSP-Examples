/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

        This project demonstrates the basic functionality of NetX Duo HTTPS client with ethernet driver on 
        Renesas RA MCUs based on Renesas FSP using AzureRTOS. The EP uses the DHCP client to get the IP address 
        from the DHCP server. On successful receiving of IP address from DHCP server and resolving the IP address 
        for the Domain name for "Adafruit IO" using the DNS lookup, On successful completion of DNS lookup, 
        the success status and IP address are displayed. Now HTTP Client is created and started.
        HTTP GET/SET/PUT operations can be performed by the user using the Menu provided on JLink RTT Viewer Console.
        Error and info messages will be printed on Jlink RTTViewer.

2. Hardware Requirement:

	i. Micro USB cable - 1no.
       ii. Ethernet Cable CAT5/6 (LAN cable) - 1no.
      iii. Ethernet Switch to connect to the router or LAN(connected to Internet) - 1no.

3. Hardware Connections:
 	Supported Board EK_RA6M3, EK_RA6M4 and EK_RA6M5:		
	i. Connect EK_RA6M3 board(J10)/ EK_RA6M4 board(J10)/ EK_RA6M5 board(J10) to Host machine using micro USB cable.
       ii. Connect LAN cable to the Ethernet Port of EK-RA6M3 and other end to the Ethernet Port on the switch 
           or Router. Switch connecting to the router or router should have access to the internet.


NOTE  : On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
        Consequentially, it is necessary to connect the serial programming interface to meet this requirement.	

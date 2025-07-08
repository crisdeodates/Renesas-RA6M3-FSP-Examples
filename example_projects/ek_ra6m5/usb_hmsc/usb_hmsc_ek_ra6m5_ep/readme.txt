/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

Project Overview:
    This example project demonstrates basic functionalities of USB Host Mass Storage Class (HMSC) driver
    with FreeRTOS+FAT file system on Renesas RA MCUs based on Renesas FSP. FreeRTOS+FAT uses the underlying
    Block media driver. The Block media driver utilizes USB HMSC driver to perform file operations on the
    USB storage devices. Error and info messages will be printed on JlinkRTTViewer.


Configuration changes to switch Speed in configurator: (same for EK-RA6M3 and EK-RA6M3G boards)

    High-Speed
        Setting USB module to be used  :    Using USB1 module
        USB Speed                      :    Hi Speed
        USB Module Number              :    USB_IP1 Port

    DMAC setting for High-Speed
    r_usb_basic stack:
        DMA support                  : Enable
        Transfer Source address      : Setting for HS module
        Transfer Destination address : Setting for HS module
    r_dmac stack for transfer and receive
        Transfer size(for both g_transfer0 & g_transfer1) : 4 Bytes
		Activation Source(for g_transfer0)      : USBHS FIFO 1(DMA transfer request 1)
        Activation Source(for g_transfer1)      : USBHS FIFO 0(DMA transfer request 0)

    Full-Speed (Default)
        Setting USB module to be used  :    Using USB0 module
        USB Speed                      :    Full Speed
        USB Module Number              :    USB_IP0 Port

    DMAC setting for Full-Speed (Default DMAC)
    r_usb_basic stack:
        DMA support                  : Enable
        Transfer Source address      : Setting for FS module
        Transfer Destination address : Setting for FS module
    r_dmac stack for transfer and receive
        Transfer size(for both g_transfer0 & g_transfer1) : 2 Bytes
		Activation Source(for g_transfer0)      : USBFS FIFO 1(DMA transfer request 1)
        Activation Source(for g_transfer1)      : USBFS FIFO 0(DMA transfer request 0)


Hardware Connections:
    High-Speed: (For EK-RA6M3/EK-RA6M3G only)
        Jumper j7: Connect pins 1-2
        Remove Jumper j17 pins
        Connect USB device to J6 connector with help of micro USB Host cable.

    Full-Speed:  (For EK-RA4M3/EK-RA6M3/EK-RA6M3G/EK-RA6M4/EK-RA4M2/EK-RA6M5)
        Jumper j12 placement is pins 1-2
        Remove Jumper j15 pins
        Connect USB device to J11 connector with help of micro USB Host cable.

Note:
1. By default USB HMSC Example Project runs in Full-speed mode.
2. Format the USB drive(with FAT32 file system) before performing any operation.
3. Data written can also be manually verified in file ra_usb.txt.
4. User is expected to execute "safely_eject" option before removing USB else further file operations may fail and USB data
5  may get corrupted.
6. If USB is removed without "safely_eject" option, user is recommended to perform "safely_eject" and "initialise USB" commands
   in sequence. This will make sure no USB operations fails after unexpected removal of USB.
7. User is expected to enter RTT input data of size not exceeding 20 bytes.
8. Operation is not guaranteed for any user input value other than integer, char(i.e. float, special char) through
   JlinkRTTViewer input.
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200009d8
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

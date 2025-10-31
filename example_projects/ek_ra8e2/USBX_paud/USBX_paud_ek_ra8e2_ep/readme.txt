/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of USBX Peripheral Audio device driver on Renesas RA MCUs based on
	Renesas FSP using Azure RTOS. RA MCU will be connected as USBX peripheral audio to the host PC. An audio WAV file will 
	be played on the host PC using audio player (example Audacity). The WAV audio data will be transferred from host PC to the 
	MCU using the isochronous OUT, through USBX PAUD module. Once the audio data is received on the MCU, the received audio 
	data will be stored on the MCU SRAM. When the host PC starts recording audio using the audio recorder (example Audacity), 
	the MCU will send the audio data stored in the MCU SRAM to the host PC using the isochronous IN, through USBX PAUD module. 
	The user will be able to play, listen and verify the recorded audio file data. USBX status and any errors will be printed 
	on the J-Link RTT Viewer which will be handled in separate thread.
 
2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	LLVM Embedded Toolchain for ARM: Version 18.1.3
	Audacity v3.1.3 (Audio Player/Recorder)
	     
3. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1,
			     MCK-RA8T1, EK-RA4L1, EK-RA8E2
	1 x Renesas RA board.
	2 x Type-C USB cables for programming and debugging.
	1 x PC with at least 2 USB ports (1 for debug and 1 for emulated USB Audio port).

4. Configuration Settings:
	To select USB Speed in configuration:
        i.  Full-Speed:
            USB Speed                     :    Full Speed
            USB Module Number             :    USB_IP0 Port
            DMA Source Address            :    FS Address
            DMA Destination Address       :    FS Address
            DMAC Transfer Size            :    2 Bytes

        ii. High-Speed:
            USB Speed                     :    Hi Speed
            USB Module Number             :    USB_IP1 Port
            DMA Source Address            :    HS Address
            DMA Destination Address       :    HS Address
            DMAC Transfer Size            :    4 Bytes

5. Hardware Connections:
   	For EK-RA6M1, EK-RA6M2 (Full Speed):
		1. Connect the RA board USB FS port (J9) to the PC via Micro USB cable.
		2. Connect the RA board USB debug port (J11) to the PC via Micro USB cable for EP debugging.
        	
    	For EK-RA4M2, EK-RA4M3, EK-RA6M4 (Full Speed):
        	1. Jumper J12: connect pins 2-3.
        	2. Connect Jumper J15 pins.
		3. Connect the RA board USB FS port (J11) to the PC via Micro USB cable.
		4. Connect the RA board USB debug port (J10) to the PC via Micro USB cable for EP debugging.

    	For EK-RA6M3 (High Speed):
        	1. Jumper J7: connect pins 2-3.
        	2. Connect Jumper J17 pins.
		3. Connect the RA board USB HS port (J6) to the PC via Micro USB cable.
		4. Connect the RA board USB debug port (J10) to the PC via Micro USB cable for EP debugging.
        	
    	For EK-RA6M5, EK-RA8M1 (High Speed):
        	1. Jumper J7: connect pins 2-3.
        	2. Connect Jumper J17 pins.
		3. Connect the RA board USB HS port (J31) to the PC via Micro USB cable.
		4. Connect the RA board USB debug port (J10) to the PC via Micro USB cable for EP debugging.

    	For EK-RA8D1 (High Speed):
        	* Note: The user must turn OFF SW1-6 to use USBHS.
        	1. Jumper J7: connect pins 2-3.
        	2. Connect Jumper J17 pins.
		3. Connect the RA board USB HS port (J31) to the PC via Micro USB cable.
		4. Connect the RA board USB debug port (J10) to the PC via Micro USB cable for EP debugging.

    	For MCK-RA8T1 (Full Speed):
        	1. Jumper JP9: connect pins 2-3.
        	2. Connect Jumper JP10 pins.
		3. Connect the RA board USB FS port (CN14) to the PC via Type-C USB cable.
		4. Connect the RA board USB debug port (CN11) to the PC via Type-C USB cable for EP debugging.

	For EK-RA4L1 (Full Speed):
		1. The user must turn ON S4-4 to select USB device mode.
		2. Jumper J17: connect pins 2-3, connect Jumper J7 pins to use P407 for USBFS VBUS.
		3. Connect the RA board USB FS port (J11) to the PC via Type-C USB cable.
		4. Connect the RA board USB debug port (J10) to the PC via Type-C USB cable for EP debugging.

	For EK-RA8E2 (Full Speed):
		1. Connect the RA board USB FS port (J11) to the PC via Type-C USB cable.
		2. Connect the RA board USB debug port (J10) to the PC via Type-C USB cable for EP debugging.

6. Verifying Operation:
	1. Connect the USBX PAUD device to host PC via two USB Cables: One for USB Debug port and one for USB HS or FS port.

   	2. On the host PC, open Windows Device Manager. 
	   Expand Sound, video and game controllers 
	   	Verify "USB Audio Demonstration" appears.
	   Expand Audio inputs and outputs
	   	Verify "Headphones (USB Audio Demonstration)" and "Microphone (USB Audio Demonstration)" appears.

	3. Open the J-Link RTT Viewer to view the application status messages.

	4. Play an audio file (eg. .wav, .mp3) using an audio player (example Audacity) on host PC.

	5. MCU records the audio received in SRAM upon reception of the audio data from PC.

	6. After audio play is completed, start recording audio using audio recorder (example Audacity) on the host PC.

	7. MCU sends the received data back to PC upon request from PC.

	8. Export the recorded data to a desired format (eg. .wav, .mp3).

	9. Import the exported recorded audio into audio player (example Audacity).

	10. Listen to the recorded audio and verify that the playback matches the recorded audio. 
	    Minor differences may occur if the recording format is different or if only a partial recording was captured.

Note:
1) Need to update source code as below if the user changes USB Speed:
   For High Speed: Uncomment the macro APL_AUDIO_20 in usbx_paud_ep.h file
   For Full Speed: Comment the macro APL_AUDIO_20 in usbx_paud_ep.h file

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22040750
   b. Keil: 	Not Available
   c. IAR:  	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
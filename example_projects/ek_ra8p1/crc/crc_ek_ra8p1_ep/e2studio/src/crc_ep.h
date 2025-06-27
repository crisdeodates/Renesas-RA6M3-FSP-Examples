/***********************************************************************************************************************
 * File Name    : crc_ep.h
 * Description  : Contains macros and function declarations used in hal_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CRC_EP_H_
#define CRC_EP_H_

/* For on board LED */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK)\
	|| defined (BOARD_RA4T1_MCK) ||defined (BOARD_RA8T1_MCK)
#define LED_ON                  (BSP_IO_LEVEL_LOW)
#define LED_OFF                 (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON                  (BSP_IO_LEVEL_HIGH)
#define LED_OFF                 (BSP_IO_LEVEL_LOW)
#endif

/* Length of input buffer to calculate CRC in normal mode */
#define NUM_BYTES               (4U)

/* Size of input buffer */
#define BUF_LEN                 (8U)

/* LED toggle delay */
#define TOGGLE_DELAY            (0x15E)

/* 8 and 16 bit seed value and data length */
#define SEED_VALUE              (0x00000000)
#define EIGHT_BIT_DATA_LEN      (5U)
#define SIXTEEN_BIT_DATA_LEN    (6U)

#define EP_INFO "\r\nThe example project demonstrates the typical use of the CRC HAL module APIs.\r\n"\
                "It demonstrates CRC operation for data transmission in normal mode and reception\r\n"\
                "in snoop mode through SCI interface. If the board does not support snoop mode (EK-RA4E2,\r\n"\
                "EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA4L1), reception is performed in normal mode through SCI\r\n"\
                "interface. If the board does not support snoop mode (FPB-RA0E1, FPB-RA0E2), reception\r\n"\
                "is performed in normal mode through SAU interface. On pressing any key through RTT Viewer,\r\n"\
                "CRC value in normal mode is calculated for 4 bytes of data. The calculated CRC value and input\r\n"\
                "data are transmitted and received through loopback on SCI_UART (except for FPB-RA0E1,\r\n"\
                "FPB-RA0E2 which use SAU_UART).\r\n"\
                "\r\nOnce the transfer is complete, if CRC value for snoop mode is zero and the transmit\r\n"\
                "and receive buffers are equal, the on-board LED blinks as a sign of successful CRC operation.\r\n"\
                "On data mismatch, LED stays ON. Failure and status messages are displayed on RTT Viewer.\r\n"\

/* Check IO-port API return and trap error (if any error occurs) cleans up and display failure details on RTT Viewer */
#define VALIDATE_IO_PORT_API(API)   ({\
                                    if (FSP_SUCCESS != (API))\
                                    {   APP_PRINT("%s API failed at Line number %d", \
                                                    #API, __LINE__);\
                                        cleanup();\
                                        APP_ERR_TRAP(true);\
                                    }\
                                    })

/* Function declarations */
void toggle_led(void);
void cleanup(void);
void deinit_crc(void);
void deinit_uart(void);

#endif /* CRC_EP_H_ */

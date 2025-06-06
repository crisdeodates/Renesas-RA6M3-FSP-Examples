/***********************************************************************************************************************
 * File Name    : can_fd_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CAN_FD_EP_H_
#define CAN_FD_EP_H_

#define CAN_MAILBOX_NUMBER_0            (0U)               //mail box number
#define CAN_CLASSIC_FRAME_DATA_BYTES    (8U)               //Data Length code for classic frame
#define CAN_FD_DATA_LENGTH_CODE         (64)               //Data Length code for FD frame
#define CAN_ID                          (0x1100)           //ID to be updated in transmit frame
/* Acceptance filter array parameters */
#define CANFD_FILTER_ID                 (0x00001000)
#define MASK_ID                         (0x1FFFF000)
#define MASK_ID_MODE                    (1)

#define ZERO                            (0U)               //Array Index value
#define NULL_CHAR                       ('\0')
#define WAIT_TIME                       (500U)             //Wait time value
#define EP_INFO                        "\r\n This Example Project demonstrates CAN FD operations on Renesas RA MCUs using 2 RA boards."\
                                       "\r\nOn pressing any key on the RTT Viewer, data is transmitted from one board to other."\
                                       "\r\nOn data reception, Board2 displays the received data on the RTT Viewer and transmits updated data to Board1."\
                                       "\r\nBoard1 on reception of data, changes CAN frame to CANFD frame and sends data to board2."\
                                       "\r\nOn reception, board2 transmits updated data."\
                                       "\r\nOn successful transmission, Board1 prints the data on to the RTT Viewer.\r\n\n"

/* Enum to select LED that is to be made ON */
typedef enum
{
   LED_CASE_1 = 1,
   LED_CASE_2 = 2,
   LED_CASE_3 = 3,
} led_state_t;

/* Enum to select status of LED */

typedef enum
{
#if defined (BOARD_RA8T1_MCK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK)
   LED_ON = BSP_IO_LEVEL_LOW,
   LED_OFF = BSP_IO_LEVEL_HIGH,
#else
   LED_ON = BSP_IO_LEVEL_HIGH,
   LED_OFF = BSP_IO_LEVEL_LOW,
#endif
} led_status_t;

void canfd_operation(void);
void can_read_operation(void);
void canfd_deinit(void);



#endif /* CAN_FD_EP_H_ */

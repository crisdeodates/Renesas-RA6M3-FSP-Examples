/***********************************************************************************************************************
 * File Name    : usb_pmsc.h
 * Description  : Contains data structures and functions used in usb_pmsc.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_PMSC_H_
#define USB_PMSC_H_

/* Macros used in usb_hmsc Example Project */
#define EP_INFO        "\r\nThis example project demonstrates basic functionalities of USB Peripheral Mass Storage \r\n"\
                       "Class (PMSC) driver with FreeRTOS on Renesas RA MCUs based on Renesas FSP.\r\n"\
                       "FreeRTOS uses the underlying Block media driver which utilizes USB  driver. The PMSC driver\r\n"\
                       "internally configures memory device (sdcard) to pop up as a memory drive on host PC.\r\n"\
                       "Error and info messages will be printed on JlinkRTTViewer.\r\n\n\n"

#define NUM_STRING_DESCRIPTOR               (7U)
#define WAIT_TIME                           (0U)
#define TASK_DELAY                          (1U)

/* Function declaration */
void deinit_usb(void);

#endif /* USB_PMSC_H_ */

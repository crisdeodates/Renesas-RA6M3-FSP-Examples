/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions commonly used in the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* Generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hal_data.h"
/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"

#define BIT_SHIFT_8             (8u)
#define SIZE_64                 (64u)

#define LVL_ERR                 (1u)       /* Error conditions */

#define RESET_VALUE             (0x00)

#define EP_VERSION              ("1.1")
#if (BSP_PERIPHERAL_RTC_C_PRESENT)
#define MODULE_NAME             "r_rtc_c"
#else
#define MODULE_NAME             "r_rtc"
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */

#define BANNER_1	            "\r\n******************************************************************"
#if (BSP_PERIPHERAL_RTC_C_PRESENT)
#define BANNER_2                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module               *"
#else
#define BANNER_2                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */
#define BANNER_3                "\r\n*   Example Project Version %s                                  *"
#define BANNER_4                "\r\n*   Flex Software Pack Version  %d.%d.%d                            *"
#define BANNER_5                "\r\n******************************************************************"
#define BANNER_6                "\r\nRefer to readme.txt file for more details on Example Project and" \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define SEGGER_INDEX            (0)

#define APP_PRINT(fn_, ...)     (SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__))

#define APP_ERR_PRINT(fn_, ...) ({if(LVL_ERR)\
                                SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s",\
                                __FUNCTION__,(fn_),##__VA_ARGS__);})

#define APP_ERR_TRAP(err)       ({if(err) {\
                                SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", (err));\
                                __asm("BKPT #0\n");}}) /* Trap upon the error */

#define APP_READ(read_data)     (SEGGER_RTT_Read (SEGGER_INDEX, (read_data), sizeof(read_data)))

#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())

#endif /* COMMON_UTILS_H_ */

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
#include <SEGGER_RTT/SEGGER_RTT.h>
#include "hal_data.h"
/* SEGGER RTT and error related headers */

#define LVL_ERR                 (1u)       /* Error conditions */
#define LVL_DEBUG               (3u)       /* Debug-level messages */
#define LOG_LEVEL               (LVL_ERR)  /* To see the debug messages, LOG_LEVEL should be set to LVL_DEBUG */

#define BIT_SHIFT_8             (8u)
#define SIZE_64                 (64u)

#define RESET_VALUE             (0x00)

#define EP_VERSION              ("1.1")
#define MODULE_NAME             "USBX_paud"
#define BANNER_INFO	            "\r\n******************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module             *"\
                                "\r\n*   Example Project Version %s                                  *"\
                                "\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
                                "\r\n******************************************************************"\
                                "\r\nRefer to readme.txt file for more details on Example Project and" \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define SEGGER_INDEX            (0)

#define APP_PRINT(fn_, ...)     (SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__))

#define APP_ERR_PRINT(fn_, ...) ({if(LVL_ERR)\
                                SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s", \
                                __FUNCTION__,(fn_),##__VA_ARGS__);})

#define APP_ERR_TRAP(err)       ({if(err) {\
                                SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", (err));\
                                __asm("BKPT #0\n");}}) /* Trap upon the error */

#define APP_READ(read_data)     (SEGGER_RTT_Read (SEGGER_INDEX, (read_data), sizeof(read_data)))

#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())

#define IP_MSG_MAX_SIZE         (BUFFER_SIZE_DOWN-1)
#define TIME_STAMP_MSG_SIZE     (35U)
#define AZURE_RTOS              (1)
#define FREERTOS                (2)

#define BYTE_POOL_SIZE          ((ULONG)256U)

#define APP_DBG_PRINT(fn_, ...) ({if(LOG_LEVEL >= LVL_DEBUG)\
                                SEGGER_RTT_printf (SEGGER_INDEX, "[DBG] In Function: %s(), %s", \
                                __FUNCTION__,(fn_),##__VA_ARGS__);})

#if (BSP_CFG_RTOS == AZURE_RTOS)
#include "tx_api.h"

typedef enum event_id
{
    RTT_INPUT_MESSAGE_TYPE_STR_QUEUED,
    RTT_OUTPUT_MESSAGE_BANNER,
    RTT_OUTPUT_MESSAGE_APP_INFO_STR,
    RTT_OUTPUT_MESSAGE_APP_ERR_STR,
    RTT_OUTPUT_MESSAGE_APP_ERR_TRAP,
    RTT_OUTPUT_MESSAGE_APP_VAL,
}event_id_t;

/* Used to exchange information between application thread and RTT thread */
typedef  __PACKED_STRUCT  rtt_user_display_data
{
    event_id_t msg_id;
    uint32_t msg_data_size;
    uint32_t msg_time;
    CHAR  p_msg[];
}rtt_msg_t;

/* Signature of function passed to tx_thread_info_get */
typedef struct
{
    TX_THREAD*   thread_ptr;
    CHAR *      name;
    UINT        state;
    ULONG       run_count;
    UINT        priority;
    UINT        preemption_threshold;
    ULONG       time_slice;
    TX_THREAD * next_thread;
    TX_THREAD * suspended_thread;
}thread_info_t;

/* Variables */
extern TX_QUEUE g_rtt_op_data_queue;
extern TX_QUEUE g_rtt_ip_data_queue;
extern TX_BYTE_POOL   g_byte_pool;

/* Function definitions */
UINT app_rtt_print_data(event_id_t id, uint32_t size, void * const p_data);
UINT check_for_RTT_user_IP(char *const p_get_data);
UINT rtt_framework_init(void);
void rtt_thread_init_check(void);
void rtt_thread_service_cleanup(void);

#endif /* BSP_CFG_RTOS */
#endif /* COMMON_UTILS_H_ */

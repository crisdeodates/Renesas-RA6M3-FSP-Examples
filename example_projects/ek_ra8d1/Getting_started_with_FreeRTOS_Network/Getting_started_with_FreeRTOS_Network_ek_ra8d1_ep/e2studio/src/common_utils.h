/***********************************************************************************************************************
* File Name    : common_utils.h
* Description  : Contains macros, data structures, and functions commonly used in the EP
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "bsp_api.h"
#include "hal_data.h"

#define MODULE_CLOSE                    (0U)

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macros for the terminal interface. */
#if (USE_VIRTUAL_COM == 1)
  #include "SERIAL_TERM/serial.h"
  #define TERM_BUFFER_SIZE              (SERIAL_RX_MAX_SIZE)
  #define TERM_INIT                     (serial_init)
  #define TERM_PRINTF(fmt, ...)         (serial_printf((fmt), ##__VA_ARGS__))
  #define TERM_READ(buf, len)           (serial_read((buf), (len)))
  #define TERM_HAS_DATA()               (serial_has_data())
  #define TERM_HAS_KEY()                (serial_has_key())
#else
  #include "SEGGER_RTT/SEGGER_RTT.h"
  #define SEGGER_INDEX                  (0)
  #define TERM_BUFFER_SIZE              (BUFFER_SIZE_DOWN)
  #define TERM_INIT                     (SEGGER_RTT_Init)
  #define TERM_PRINTF(fmt, ...)         (SEGGER_RTT_printf(SEGGER_INDEX, (fmt), ##__VA_ARGS__))
  #define TERM_READ(buf, len)           (SEGGER_RTT_Read(SEGGER_INDEX, (buf), (len)))
  #define TERM_HAS_DATA()               (SEGGER_RTT_HasData(SEGGER_INDEX))
  #define TERM_HAS_KEY()                (SEGGER_RTT_HasKey())
#endif

/* Macros for terminal functionality in the RTOS project. */
#if (BSP_CFG_RTOS != 0U)
  #if (BSP_CFG_RTOS == 1U)
    #define TERM_BYTE_POOL_SIZE         (4096U)
  #endif
  #define TERM_OUTPUT_QUEUE_SIZE        (40U)
  #define TERM_INPUT_QUEUE_SIZE         (40U)
#endif

/* Macros commonly used */
#define LVL_ERR                             (1U)       /* error conditions   */
#define RESET_VALUE                         (0x00)
#define NULL_CHAR                           ('\0')

#define APP_PRINT(fn_, ...)                 (TERM_PRINTF((fn_), ##__VA_ARGS__))

#define APP_READ(buf, len)                  (TERM_READ(buf, len))

#define APP_CHECK_DATA                      (TERM_HAS_DATA())

#define APP_CHECK_KEY                       (TERM_HAS_KEY())

#if LVL_ERR
  #define APP_ERR_PRINT(fn_, ...)           \
      (APP_PRINT("\r\n[ERR] In Function: %s(), %s", __FUNCTION__, (fn_), ##__VA_ARGS__))
#else
  #define APP_ERR_PRINT(fn_, ...)
#endif

#define APP_ERR_RET(con, err, fn_, ...)     \
({\
    if (con)\
    {\
        APP_ERR_PRINT((fn_), ##__VA_ARGS__); \
        return (err);\
    }\
})

#define ERROR_TRAP                          \
({ \
    __asm("BKPT #0\n"); \
})

#define APP_ERR_TRAP(err)                   \
({\
    if(err)\
    {\
        APP_PRINT("\r\nReturned Error Code: 0x%x  \r\n", (err));  \
        /* trap upon the error */  \
		    ERROR_TRAP; \
    }\
})

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/* Structure for exchanging information between application threads and the terminal thread */
typedef struct st_term_msg
{
    uint32_t id;
    uint32_t size;
    uint32_t time;
    char msg[];
}term_msg_t;

/***********************************************************************************************************************
 * Public function prototypes
 **********************************************************************************************************************/
/* Terminal API prototype for the RTOS project */
#if (BSP_CFG_RTOS != 0U)
uint32_t term_framework_init(void);
uint32_t term_get_input_queue(char * p_msg, uint32_t * p_size, uint32_t wait);
uint32_t term_get_output_queue(void ** pp_msg_st, uint32_t wait);
uint32_t term_send_input_queue(uint32_t id, void * const p_data, uint32_t size);
uint32_t term_send_output_queue(uint32_t id, void * const p_data, uint32_t size);
#endif /* BSP_CFG_RTOS == 1U */

#endif /* COMMON_UTILS_H_ */
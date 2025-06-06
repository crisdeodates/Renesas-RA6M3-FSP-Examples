/***********************************************************************************************************************
* File Name    : r_sci_b_uart_instance_cfg.h
* Description  : Contains data structures and functions used in r_sci_b_uart_instance_cfg.c
**********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef R_SCI_B_UART_INSTANCE_CFG_H_
#define R_SCI_B_UART_INSTANCE_CFG_H_

#if (USE_SERIAL_TERM == 1)
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"

#if BSP_PERIPHERAL_SCI_B_PRESENT
#include "r_sci_b_uart.h"
#include "../r_uart_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern const uart_instance_t g_serial;
extern sci_b_uart_instance_ctrl_t g_serial_ctrl;
extern const uart_cfg_t g_serial_cfg;
extern const sci_b_uart_extended_cfg_t g_serial_cfg_extend;

/***********************************************************************************************************************
 * Public function prototypes
 **********************************************************************************************************************/
#ifndef serial_callback
void serial_callback(uart_callback_args_t *p_args);
#endif

#endif /* BSP_PERIPHERAL_SCI_B_PRESENT */
#endif /* USE_SERIAL_TERM */
#endif /* R_SCI_B_UART_INSTANCE_CFG_H_ */

/***********************************************************************************************************************
 * File Name    : serial_data.c
 * Description  : Contains declarations of data structures and functions used in serial_terminal.c
 ***********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#if (USE_VIRTUAL_COM == 1)

#include "serial_data.h"
/* This content is copied from hal_data.c */
dtc_instance_ctrl_t g_transfer1_ctrl;

#if (1 == 1)
transfer_info_t g_transfer1_info =
{ .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
  .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
  .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.size = TRANSFER_SIZE_1_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void*) NULL,
  .p_src = (void const*) NULL,
  .num_blocks = 0,
  .length = 0, };

#elif (1 > 1)
/* User is responsible to initialize the array. */
transfer_info_t g_transfer1_info[1];
#else
/* User must call api::reconfigure before enable DTC transfer. */
#endif

const dtc_extended_cfg_t g_transfer1_cfg_extend =
{ .activation_source = VECTOR_NUMBER_SCI9_RXI, };

const transfer_cfg_t g_transfer1_cfg =
{
#if (1 == 1)
  .p_info = &g_transfer1_info,
#elif (1 > 1)
    .p_info              = g_transfer1_info,
#else
    .p_info = NULL,
#endif
  .p_extend = &g_transfer1_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer1 =
{ .p_ctrl = &g_transfer1_ctrl, .p_cfg = &g_transfer1_cfg, .p_api = &g_transfer_on_dtc };
dtc_instance_ctrl_t g_transfer0_ctrl;

#if (1 == 1)
transfer_info_t g_transfer0_info =
{ .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
  .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
  .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .transfer_settings_word_b.size = TRANSFER_SIZE_1_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void*) NULL,
  .p_src = (void const*) NULL,
  .num_blocks = 0,
  .length = 0, };

#elif (1 > 1)
/* User is responsible to initialize the array. */
transfer_info_t g_transfer0_info[1];
#else
/* User must call api::reconfigure before enable DTC transfer. */
#endif

const dtc_extended_cfg_t g_transfer0_cfg_extend =
{ .activation_source = VECTOR_NUMBER_SCI9_TXI, };

const transfer_cfg_t g_transfer0_cfg =
{
#if (1 == 1)
  .p_info = &g_transfer0_info,
#elif (1 > 1)
    .p_info              = g_transfer0_info,
#else
    .p_info = NULL,
#endif
  .p_extend = &g_transfer0_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer0 =
{ .p_ctrl = &g_transfer0_ctrl, .p_cfg = &g_transfer0_cfg, .p_api = &g_transfer_on_dtc };
sci_b_uart_instance_ctrl_t g_uart_ctrl;

sci_b_baud_setting_t g_uart_baud_setting =
        {
        /* Baud rate calculated with 0.160% error. */.baudrate_bits_b.abcse = 0,
          .baudrate_bits_b.abcs = 0, .baudrate_bits_b.bgdm = 1, .baudrate_bits_b.cks = 0, .baudrate_bits_b.brr = 64, .baudrate_bits_b.mddr =
                  (uint8_t) 256,
          .baudrate_bits_b.brme = false };

/** UART extended configuration for UARTonSCI HAL driver */
const sci_b_uart_extended_cfg_t g_uart_cfg_extend =
{ .clock = SCI_B_UART_CLOCK_INT, .rx_edge_start = SCI_B_UART_START_BIT_FALLING_EDGE, .noise_cancel =
          SCI_B_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_B_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting = &g_uart_baud_setting, .flow_control =
          SCI_B_UART_FLOW_CONTROL_RTS,
#if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
  .flow_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
  .rs485_setting =
  { .enable = SCI_B_UART_RS485_DISABLE,
    .polarity = SCI_B_UART_RS485_DE_POLARITY_HIGH,
    .assertion_time = 1,
    .negation_time = 1, } };

/** UART interface configuration */
const uart_cfg_t g_uart_cfg =
{ .channel = 9, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          serial_callback,
  .p_context = NULL, .p_extend = &g_uart_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == g_transfer0)
                .p_transfer_tx       = NULL,
#else
  .p_transfer_tx = &g_transfer0,
#endif
#if (RA_NOT_DEFINED == g_transfer1)
                .p_transfer_rx       = NULL,
#else
  .p_transfer_rx = &g_transfer1,
#endif
#undef RA_NOT_DEFINED
  .rxi_ipl = (12),
  .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
#if defined(VECTOR_NUMBER_SCI9_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI9_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI9_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI9_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI9_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI9_TEI,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI9_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI9_ERI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif
        };

/* Instance structure to use this module. */
const uart_instance_t g_uart =
{ .p_ctrl = &g_uart_ctrl, .p_cfg = &g_uart_cfg, .p_api = &g_uart_on_sci_b };

#endif /* USE_VIRTUAL_COM */

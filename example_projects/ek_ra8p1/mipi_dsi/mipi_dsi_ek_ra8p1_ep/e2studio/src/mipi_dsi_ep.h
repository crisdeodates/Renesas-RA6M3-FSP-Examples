/***********************************************************************************************************************
 * File Name    : mipi_dsi_ep.h
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef MIPI_DSI_EP_H_
#define MIPI_DSI_EP_H_

#include "common_utils.h"

#define MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG      ((mipi_cmd_id_t) 0xFE)
#define MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE    ((mipi_cmd_id_t) 0xFD)
#define BYTES_PER_PIXEL                              (4)
#define COLOR_BAND_COUNT                             (8)
#define BLUE                                         (0x000000FF)
#define LIME                                         (0xFF00FF00)
#define RED                                          (0x00FF0000)
#define BLACK                                        (0x00000000)
#define WHITE                                        (0xFFFFFFFF)
#define YELLOW                                       (0xFFFFFF00)
#define AQUA                                         (0xFF00FFFF)
#define MAGENTA                                      (0x00FF00FF)
#define RESET_FLAG                                   (false)
#define SET_FLAG                                     (true)
#define INITIAL_VALUE                                ('\0')
#define RTT_SELECT_5S                                (1U)
#define RTT_SELECT_15S                               (2U)
#define RTT_SELECT_30S                               (3U)
#define RTT_SELECT_DISABLE_ULPS                      (4U)
#define GPT_UNITS_SECONDS                            (1)
#define GPT_DESIRED_PERIOD_5SEC                      (5)
#define GPT_DESIRED_PERIOD_15SEC                     (15)
#define GPT_DESIRED_PERIOD_30SEC                     (30)
#define ULPS_EXIT_PERIOD_30SEC                       (30)
#if defined (BOARD_RA8P1_EK)
#define PIN_DISPLAY_INT                              (BSP_IO_PORT_01_PIN_11)
#define PIN_DISPLAY_RST                              (BSP_IO_PORT_06_PIN_06)
#define PIN_DISPLAY_BACKLIGHT                        (BSP_IO_PORT_05_PIN_14)
#else
#define PIN_DISPLAY_INT                              (BSP_IO_PORT_05_PIN_10)
#define PIN_DISPLAY_RST                              (BSP_IO_PORT_10_PIN_01)
#define PIN_DISPLAY_BACKLIGHT                        (BSP_IO_PORT_04_PIN_04)
#endif
#define ENTER_ULPS_DELAY                             (8000000)
#define MIPI_DSI_MENU               "\r\nSet time to enter Ultra-Low Power State (ULPS):"\
                                    "\r\n1. Input '1' to enter ULPS after 5s."\
                                    "\r\n2. Input '2' to enter ULPS after 15s."\
                                    "\r\n3. Input '3' to enter ULPS after 30s."\
                                    "\r\n4. Input '4' to always display (Default).\r\n"\
                                    "\r\nNote: After entering Ultra Low Power State (ULPS), "\
                                    "please touch the screen to exit this mode"\
                                    "\r\n      or wait 30s after backlight turn off to automatically "\
                                    "exit this mode.\r\n"\
                                    "\r\nUser Input:  "
#define MIPI_DSI_INFO_5S            "Enter ULPS after 5 seconds of display\r\n"
#define MIPI_DSI_INFO_15S           "Enter ULPS after 15 seconds of display\r\n"
#define MIPI_DSI_INFO_30S           "Enter ULPS after 30 seconds of display\r\n"
#define MIPI_DSI_INFO_DISABLE_ULPS  "Always display\r\n"

typedef struct
{
    unsigned char        size;
    unsigned char        buffer[10];
    mipi_cmd_id_t        cmd_id;
    mipi_dsi_cmd_flag_t  flags;
} lcd_table_setting_t;

extern const lcd_table_setting_t g_lcd_init_focuslcd[];
void mipi_dsi_push_table (const lcd_table_setting_t *table);
void mipi_dsi_start_display ();
void mipi_dsi_entry(void);

#endif /* MIPI_DSI_EP_H_ */

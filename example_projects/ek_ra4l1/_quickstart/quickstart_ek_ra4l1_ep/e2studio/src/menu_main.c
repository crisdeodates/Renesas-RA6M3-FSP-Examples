/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : menu_main.c
 * Description  : The main menu controller.
 *********************************************************************************************************************/

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"


#include "menu_main.h"
#include "common_utils.h"
#include "common_init.h"
#include "menu_main.h"
#include "r_typedefs.h"
#include "board_hw_cfg.h"
#include "menu_slcd.h"
#include "lp_mode.h"

#define MODULE_NAME     "\r\n\x1b[2m\x1b[37mWelcome to Quick Start Example Project for %s!\r\n"

#define SUB_OPTIONS     "\r\n> Select from the options in the menu below:\r\n" \
                        "\r\nMENU"

int8_t g_selected_menu = 0;


static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};


/* Table of menu functions */
static st_menu_fn_tbl_t s_menu_items[] =
{
    {"Kit Information"               , false,               true , kis_display_menu},
    {"Quad-SPI Speed Test"           , false,               true , ext_display_menu},
    {"Segment LCD Demonstration"     , false,               true , slcd_display_menu},
    {"Low Power Modes Demonstration" , false,               true , lpm_main_display_menu},
    {"Next Steps"                    , false,               true , ns_display_menu },
    {""                              , false,               true , NULL }
};



/**********************************************************************************************************************
 * Function Name: main_display_menu
 * Description  : .
 * Return Value : The main menu controller.
 *********************************************************************************************************************/
int8_t main_display_menu(void)
{
    TURN_GREEN_ON_CFG_;

    int8_t c = -1;

    int8_t menu_limit = 0;

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console(s_print_buffer);
    sprintf (s_print_buffer, MODULE_NAME, FULL_NAME);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console(s_print_buffer);
    sprintf (s_print_buffer, SUB_OPTIONS);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console(s_print_buffer);


    for (int8_t test_active = 0; (NULL != s_menu_items[test_active].p_func); test_active++ )
    {
        sprintf (s_print_buffer, "\r\n %d. %s", (test_active + 1), s_menu_items[menu_limit++ ].p_name);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);
    }

    /* ignoring -Wpointer-sign is OK for a constant string */
    print_to_console("\r\n");

    while ((0 != c))
    {
        c = input_from_console ();
        if (0 != c)
        {
            /* Cast, as compiler will assume calc is int */
            c = (int8_t) (c - '0');

            g_selected_menu = c;

            if ((c > 0) && (c <= menu_limit))
            {
                s_menu_items[c - 1].p_func ();
                break;
            }
        }

    }


    /* Cast, as compiler will assume calc is int */
    return ((int8_t) (c - '0'));
}
/**********************************************************************************************************************
 End of function main_display_menu
 *********************************************************************************************************************/


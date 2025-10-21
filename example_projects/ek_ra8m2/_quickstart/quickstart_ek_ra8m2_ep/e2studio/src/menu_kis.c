/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : menu_kis.c
 * Version      : 1.0
 * Description  : Kit Information Screen.
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 14/10/2025 1.00     First Release
 *********************************************************************************************************************/

 /**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "menu_kis.h"
#include "jlink_console.h"

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

#define MODULE_NAME     "\r\n%d. KIT INFORMATION\r\n"

#define SUB_OPTIONS     "\r\n\x1b[2m\x1b[37m a) Kit name:                           %s "                               \
                        "\r\n\x1b[2m\x1b[37m b) Kit ordering part number:           %s "                               \
                        "\r\n\x1b[2m\x1b[37m c) RA Device part number:              %s"                                \
                        "\r\n\x1b[2m\x1b[37m d) RA MCU 128-bit Unique ID (hex):     "                                  \
                        "\x1b[32m%08x\x1b[37m-\x1b[32m%08x\x1b[37m-\x1b[32m%08x\x1b[37m-\x1b[32m%08x\x1b[37m"          \
                        "\r\n\x1b[2m\x1b[37m e) RA MCU Die temperature (F/C):       \x1b[32m%d.%02d/%d.%02d\x1b[37m"   \
                        "\r\n\x1b[2m\x1b[37m f) Blue LED blinking frequency (Hz):   \x1b[32m%d\x1b[37m"                \
                        "\r\n\x1b[2m\x1b[37m g) Blue LED blinking intensity (%%%%):   \x1b[32m%d\x1b[37m"              \


static char_t s_print_buffer[BUFFER_LINE_LENGTH] = "";
static const TickType_t s_ticks_to_wait = 100 / portTICK_PERIOD_MS;


/**********************************************************************************************************************
 * Function Name: kis_display_menu
 * Description  : .
 * Return Value : The Kit information demo screen
 *********************************************************************************************************************/
test_fn kis_display_menu(void)
{
    int8_t c = -1;
    uint16_t wn_mcu_temp_f = 0;
    uint16_t fr_mcu_temp_f = 0;
    uint16_t wn_mcu_temp_c = 0;
    uint16_t fr_mcu_temp_c = 0;
    volatile bool_t mode = true;
    bool_t new_mode = false;
    bsp_unique_id_t const * p_uid = R_BSP_UniqueIdGet ();

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    print_to_console(s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    wn_mcu_temp_f = g_board_status.temperature_f.whole_number;
    fr_mcu_temp_f = g_board_status.temperature_f.mantissa;
    wn_mcu_temp_c = g_board_status.temperature_c.whole_number;
    fr_mcu_temp_c = g_board_status.temperature_c.mantissa;

    sprintf (s_print_buffer, SUB_OPTIONS, FULL_NAME, PART_NUMBER, DEVICE_NUMBER,
            (int_t)p_uid->unique_id_words[0],  (int_t)p_uid->unique_id_words[1], // typedef uint_t in stdint.h
            (int_t)p_uid->unique_id_words[2],  (int_t)p_uid->unique_id_words[3], // as signed long long int64_t;
            wn_mcu_temp_f, fr_mcu_temp_f, wn_mcu_temp_c, fr_mcu_temp_c,
            g_pwm_rates_data[g_board_status.led_frequency], g_pwm_dcs_data[g_board_status.led_intensity]);

    print_to_console(s_print_buffer);
    vTaskDelay (s_ticks_to_wait);

    sprintf (s_print_buffer, MENU_RETURN_INFO);

    print_to_console(s_print_buffer);

    /* provide small delay so board_status should be up to date */
    vTaskDelay (s_ticks_to_wait);
    xEventGroupSetBits (g_update_console_event_group, STATUS_DISPLAY_MENU_KIS);

    new_mode = jlink_set_mode(mode);
    start_key_check();

    while ((CONNECTION_ABORT_CRTL != c))
    {
        if (key_pressed())
        {
            /* c will be in the range of 0 to  g_menu_limit (which is int8_t) so safe to case into int8_t */
            c = (int8_t) get_detected_key();

            if (0 == c)
            {
                c = -1;
            }

            if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
            {
                break;
            }
            start_key_check();
        }
        vTaskDelay(10);
    }
    jlink_restore_mode(new_mode);

    xEventGroupClearBits (g_update_console_event_group, STATUS_DISPLAY_MENU_KIS);
    return (0);
}
/**********************************************************************************************************************
 End of function kis_display_menu
 *********************************************************************************************************************/


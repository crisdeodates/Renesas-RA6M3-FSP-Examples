/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : board_mon_thread_entry.c
 * Version      : 1.0
 * Description  : Board status monitor thread.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "board_mon_thread.h"
#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "board_cfg.h"
#include "board_hw_cfg.h"
#include "lp_mode.h"
#include "update_slcd.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define BUTTON_DEBOUNCE_RATE (500)

/* Constant data for ADC conversion */
#define ADC_EXAMPLE_VCC_VOLT                             (3.3)      /* 3.3 Volt reference                   */
#define ADC_EXAMPLE_TEMPERATURE_RESOLUTION               (12U)      /* Set ADC to 12-bit resolution         */
#define ADC_EXAMPLE_REFERENCE_CALIBRATION_TEMPERATURE    (125.0)    /* Reference temperature at calibration */

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/* NONE */

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
bool g_usb_configured = true;

uint32_t g_pwm_dcs[3] =     { LED_INTENSITY_10,
                              LED_INTENSITY_50,
                              LED_INTENSITY_90
                            };
uint32_t g_pwm_rates[3] =   { BLINK_FREQ_1HZ,
                              BLINK_FREQ_5HZ,
                              BLINK_FREQ_10HZ
                            };

st_board_status_t g_board_status = { };

uint8_t g_pwm_dcs_data[] =   { 10, 50, 90 };
uint8_t g_pwm_rates_data[] = {  1,  5, 10 };

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static EventBits_t s_ux_bits;

static uint32_t s_temperature_read_interval = 10;

static const TickType_t s_ticks_to_wait = (50 / portTICK_PERIOD_MS);

static uint16_t s_adc_data     = 0;
static uint16_t s_old_adc_data = 0;

static uint16_t s_new_value                 = 0;

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = { };

static void test_temperature_change();
static fsp_err_t read_die_temperature (float * celcius_temperature);

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: board_mon_thread_entry
 * Description  : Board Monitor entry function
 *                Note: pvParameters contains TaskHandle_t
 * Argument     : pvParameters - unused but required by I/F
 * Return Value : NONE
 *********************************************************************************************************************/
void board_mon_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    uint16_t wn_mcu_temp_f = 0;
    uint16_t fr_mcu_temp_f = 0;
    uint16_t wn_mcu_temp_c = 0;
    uint16_t fr_mcu_temp_c = 0;

    while (1)
    {
        s_ux_bits = xEventGroupWaitBits (g_update_console_event, STATUS_UPDATE_KIS_INFO, pdFALSE, pdTRUE, 1);

        if (true == g_usb_configured)
        {
            if (0 == (--s_temperature_read_interval))
            {
                s_temperature_read_interval = 10;

                /* Check for change in core temperature */
                /* Event will be set is a change is detected */
                test_temperature_change ();
            }

            /* Update temperature on segment LCD */
            if (((s_ux_bits & (STATUS_UPDATE_TEMP_INFO)) == (STATUS_UPDATE_TEMP_INFO)) &&
                (!(STATUS_LPM_MODE_START == (s_ux_bits & (STATUS_LPM_MODE_START)))))
            {
                set_temp_on_slcd (&g_board_status);
            }

            /* Update temperature on console */
            if ((s_ux_bits & (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_TEMP_INFO))
                    == (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_TEMP_INFO))
            {
                wn_mcu_temp_f = g_board_status.temperature_f.whole_number;
                fr_mcu_temp_f = g_board_status.temperature_f.mantissa;
                wn_mcu_temp_c = g_board_status.temperature_c.whole_number;
                fr_mcu_temp_c = g_board_status.temperature_c.mantissa;

                /* Update temperature to console */
                char_t * p_temp_buffer = pvPortMalloc (128);
                sprintf (p_temp_buffer, "%s%s%s%d.%02d/%d.%02d%s\r\n%s", gp_cursor_store, gp_cursor_temp, gp_green_fg,
                            wn_mcu_temp_f, fr_mcu_temp_f, wn_mcu_temp_c, fr_mcu_temp_c, gp_green_fg, gp_cursor_restore);

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console ((void*)p_temp_buffer);
                vPortFree (p_temp_buffer);
            }

            if ((s_ux_bits & (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_INTENSE_INFO))
                    == (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_INTENSE_INFO))
            {
                /* Update Switch SW1 */
                s_new_value = g_board_status.led_intensity;

                sprintf (s_print_buffer, "%s%s%s%d%s\r\n%s", gp_cursor_store, gp_cursor_intensity, gp_green_fg,
                            g_pwm_dcs_data[s_new_value], gp_white_fg, gp_cursor_restore);

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console ((void*)s_print_buffer);
            }

            if ((s_ux_bits & (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_FREQ_INFO))
                    == (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_FREQ_INFO))
            {
                /* Update Switch SW2 */
                s_new_value = g_board_status.led_frequency;

                sprintf (s_print_buffer, "%s%s%s%d%s\r\n%s", gp_cursor_store, gp_cursor_frequency, gp_green_fg,
                            g_pwm_rates_data[s_new_value], gp_white_fg, gp_cursor_restore);

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console ((void*)s_print_buffer);
            }
        }

        if ((s_ux_bits & (STATUS_UPDATE_TEMP_INFO)) == (STATUS_UPDATE_TEMP_INFO))
        {
            xEventGroupClearBits (g_update_console_event, STATUS_UPDATE_TEMP_INFO);
        }

        if ((s_ux_bits & (STATUS_UPDATE_INTENSE_INFO)) == (STATUS_UPDATE_INTENSE_INFO))
        {
            led_duty_cycle_update ();

            /* Clear Event */
            xEventGroupClearBits (g_update_console_event, (STATUS_UPDATE_INTENSE_INFO));
        }

        if ((s_ux_bits & (STATUS_UPDATE_FREQ_INFO)) == (STATUS_UPDATE_FREQ_INFO))
        {
            R_GPT_PeriodSet (g_blinker.p_ctrl, g_pwm_rates[g_board_status.led_frequency]);

            /* Clear Event */
            xEventGroupClearBits (g_update_console_event, (STATUS_UPDATE_FREQ_INFO));
        }
        vTaskDelay (s_ticks_to_wait);
    }
}
/**********************************************************************************************************************
 End of function board_mon_thread_entry
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: button_irq15_callback
 * Description  : SW1 interrupt handler.
 * Argument     : p_args
 * Return Value : None
 *********************************************************************************************************************/
void button_irq15_callback(external_irq_callback_args_t *p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult                  = pdFAIL;

    EventBits_t uxBits;

    /* Void the unused args */
    FSP_PARAMETER_NOT_USED(p_args);

    uxBits = xEventGroupGetBitsFromISR (g_update_console_event);

    if ((uxBits & (STATUS_UPDATE_INTENSE_INFO)) != (STATUS_UPDATE_INTENSE_INFO))
    {
        /* Cast, as compiler will assume calculation is int */
        g_board_status.led_intensity = (uint16_t) ((g_board_status.led_intensity + 1) % 3);

        xResult = xEventGroupSetBitsFromISR(g_update_console_event, STATUS_UPDATE_INTENSE_INFO,
                                            &xHigherPriorityTaskWoken);

        /* Was the message posted successfully? */
        if (pdFAIL != xResult)
        {
            /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
             switch should be requested.  The macro used is port specific and will
             be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
             the documentation page for the port being used. */
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
    if (STATUS_LPM_MODE_MENU == (uxBits & (STATUS_LPM_MODE_MENU)))
    {
        /* Check source flag state */
        if (LPM_SWITCH_MODE == g_s_lp_bits)
        {
            return;
        }

        /* Set detection flags */
        g_s_lp_bits |= LPM_SWITCH_MODE;
    }
}
/**********************************************************************************************************************
 End of function button_irq15_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: button_irq0_callback
 * Description  : SW2 Interrupt handler.
 * Argument     : p_args
 * Return Value : None
 *********************************************************************************************************************/
void button_irq0_callback(external_irq_callback_args_t *p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult                  = pdFAIL;

    EventBits_t uxBits;

    /* Void the unused args */
    FSP_PARAMETER_NOT_USED(p_args);

    uxBits = xEventGroupGetBitsFromISR (g_update_console_event);

    if ((uxBits & (STATUS_UPDATE_FREQ_INFO)) != (STATUS_UPDATE_FREQ_INFO))
    {
        /* Cast, as compiler will assume calc is int */
        g_board_status.led_frequency = (uint16_t) ((g_board_status.led_frequency + 1) % 3);

        xResult = xEventGroupSetBitsFromISR(g_update_console_event, STATUS_UPDATE_FREQ_INFO, &xHigherPriorityTaskWoken);

        /* Was the message posted successfully? */
        if (pdFAIL != xResult)
        {
            /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
             switch should be requested.  The macro used is port specific and will
             be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
             the documentation page for the port being used. */
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }

    if (STATUS_LPM_MODE_MENU == (uxBits & (STATUS_LPM_MODE_MENU)))
    {
        /* Check source flag state */
        if (LPM_LEAVE_MODE == g_s_lp_bits)
        {
            return;
        }

        /* Set detection flags */
        g_s_lp_bits |= LPM_LEAVE_MODE;
    }
}
/**********************************************************************************************************************
 End of function button_irq0_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) functions
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: test_temperature_change
 * Description  : Read the Temperature and if it is different from the previous reading, trigger an update.
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
static void test_temperature_change()
{
    float32_t mcu_temp_f = 0.00;
    float32_t mcu_temp_c = 0.00;
    float32_t tmp = 0.00;
    uint16_t d = 0;
    uint16_t r = 0;

    /* Read TSN calibration data (value written at manufacture, does not change at runtime) */
    if (ADC_CALIBRATION_NA == g_adc_info_rtn.calibration_data)
    {
        /* Unable to read TSN calibration value - not supported */
        g_board_status.temperature_f.whole_number = 0;
        g_board_status.temperature_f.mantissa     = 0;

        g_board_status.temperature_c.whole_number = 0;
        g_board_status.temperature_c.mantissa     = 0;
    }
    else
    {
        /* Read MCU die temperature */
        read_die_temperature(&mcu_temp_c);

        /* Convert celsius to fahrenheit using formula */
        /* fahrenheit = (celsius x 9/5) + 32 */
        mcu_temp_f = (mcu_temp_c * (float32_t)1.8) + (float32_t)32.0;

        /* As sprintf does not support floating point convert result to d.r */
        tmp = mcu_temp_f * 100.0f;

        /* Truncate the whole number to leave only the integer part */
        d = (uint16_t) (mcu_temp_f / 1.00f);

        /* Truncate the mantissa to leave only the integer part */
        r = (uint16_t) (tmp / 1.00f);

        /* Cast to maintain size of uint16_t */
        r = (uint16_t) (r % (d * 100U));

        g_board_status.temperature_f.whole_number = d;
        g_board_status.temperature_f.mantissa     = r;

        tmp = mcu_temp_c * 100.0f;

        /* Truncate the whole number to leave only the integer part */
        d = (uint16_t) (mcu_temp_c / 1.00f);

        /* Truncate the mantissa to leave only the integer part */
        r = (uint16_t) (tmp / 1.00f);

        /* Cast to maintain size of uint16_t */
        r = (uint16_t) (r % (d * 100U));

        g_board_status.temperature_c.whole_number = d;
        g_board_status.temperature_c.mantissa     = r;
    }

    s_adc_data = g_board_status.adc_temperature_data;

    /* If there's a change in temperature then update the UI */
    if (s_old_adc_data != s_adc_data)
    {
        s_old_adc_data = s_adc_data;
        xEventGroupSetBits (g_update_console_event, STATUS_UPDATE_TEMP_INFO);
    }
}
/**********************************************************************************************************************
 End of function test_temperature_change
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: read_die_temperature
 * Description  : Read the die temperature
 * Argument     : celcius_temperature - pointer to the temperature
 * Return Value : FSP_SUCCESS or the error value. The temperature pointed to by the parameter is updated
 *********************************************************************************************************************/
static fsp_err_t read_die_temperature(float * celcius_temperature)
{
    uint16_t adc_tsn_result;

    float32_t v1;
    float32_t vs;
    float32_t slope;
    float32_t cal125;

    fsp_err_t err;

    err = R_ADC_Read(&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, (uint16_t *) &adc_tsn_result); /* Cast to uint16_t */

    if (FSP_SUCCESS != err)
    {
        return (err);
    }

    adc_info_t p_adc_info;
    err = R_ADC_InfoGet(&g_adc_ctrl, &p_adc_info);

    g_board_status.adc_temperature_data = adc_tsn_result;

    cal125 = (float32_t) R_TSN->TSCDR; /* Cast uint32_t calibration data to float for calculation */
    slope  = (float32_t) p_adc_info.slope_microvolts; /* cast int16_t calibration data to float for calculation */

    /* cast uint32_t calibration data to float for calculation */
    v1 = (float32_t) ((ADC_EXAMPLE_VCC_VOLT * cal125) / pow(2, ADC_EXAMPLE_TEMPERATURE_RESOLUTION));

    /* cast uint32_t calibration data to float for calculation */
    vs = (float32_t) ((ADC_EXAMPLE_VCC_VOLT * adc_tsn_result) / pow(2, ADC_EXAMPLE_TEMPERATURE_RESOLUTION));

    /* cast uint32_t calibration data to float for calculation */
    *celcius_temperature = (float32_t) (((1000000 * (vs - v1)) / slope) + \
            ADC_EXAMPLE_REFERENCE_CALIBRATION_TEMPERATURE);

    return (err);
}
/**********************************************************************************************************************
 End of function read_die_temperature
 *********************************************************************************************************************/

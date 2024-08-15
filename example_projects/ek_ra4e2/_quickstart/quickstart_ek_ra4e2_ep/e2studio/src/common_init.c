/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Common init function.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "common_init.h"
#include "board_cfg.h"

#define NUM_RATES             (sizeof(pwm_rates) / sizeof(pwm_rates[0]))
#define NUM_DCS               (sizeof(pwm_dcs) / sizeof(pwm_dcs[0]))
#define NUM_SWITCH            (sizeof(s_irq_pins) / sizeof(s_irq_pins[0]))

typedef struct irq_pins
{
    const external_irq_instance_t * const p_irq;
} st_irq_pins_t;

typedef struct pwm_pins
{
    const timer_instance_t * const p_timer;
    const gpt_io_pin_t              pin;
} st_pwm_pins_t;

adc_info_t  g_adc_info_rtn;

static st_irq_pins_t s_irq_pins[] =
{
        {&g_external_irq10_ds},
        {&g_external_irq9},
};

static st_pwm_pins_t s_pwm_pins[] =
{
    {&g_gpt_blue, GPT_IO_PIN_GTIOCA},
};


/**********************************************************************************************************************
 * Function Name: adc_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t adc_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* When selecting the temperature sensor output, the internal reference voltage,
     * do not use continuous scan mode or group scan mode. */

    fsp_err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    /* Read TSN cal data (value written at manufacture, does not change at runtime) */
    fsp_err = R_ADC_InfoGet(&g_adc_ctrl, &g_adc_info_rtn);

    return fsp_err;
}
/**********************************************************************************************************************
 End of function adc_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: icu_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t icu_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < NUM_SWITCH; i++ )
    {
        fsp_err = R_ICU_ExternalIrqOpen(s_irq_pins[i].p_irq->p_ctrl, s_irq_pins[i].p_irq->p_cfg);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }

        fsp_err = R_ICU_ExternalIrqEnable(s_irq_pins[i].p_irq->p_ctrl);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }
    return fsp_err;
}
/**********************************************************************************************************************
 End of function icu_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: gpt_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t gpt_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < 1; i++ )
    {
        fsp_err = R_GPT_Open(s_pwm_pins[i].p_timer->p_ctrl, s_pwm_pins[i].p_timer->p_cfg);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    fsp_err = R_GPT_Open(g_blinker.p_ctrl, g_blinker.p_cfg);
    {
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    fsp_err = R_GPT_PeriodSet(g_blinker.p_ctrl, g_pwm_rates[g_board_status.led_frequency]);
    {
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    fsp_err = R_GPT_Start(g_blinker.p_ctrl);
    if (FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR

        /* Close the GPT timer */
        R_GPT_Close(g_blinker.p_ctrl);

        return fsp_err;
    }

    return fsp_err;
}
/**********************************************************************************************************************
 End of function gpt_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: common_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = adc_initialize();
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = icu_initialize();
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = gpt_initialize();

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    led_duty_cycle_update();

    R_GPT_Start(s_pwm_pins[0].p_timer->p_ctrl);

    return fsp_err;
}
/**********************************************************************************************************************
 End of function common_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_duty_cycle_update
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void led_duty_cycle_update(void)
{
    R_GPT_DutyCycleSet(s_pwm_pins[0].p_timer->p_ctrl, g_pwm_dcs[g_board_status.led_intensity], s_pwm_pins[0].pin);
}
/**********************************************************************************************************************
 End of function led_duty_cycle_update
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: gpt_blinker_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void gpt_blinker_callback(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        timer_status_t status = {0};

        R_GPT_StatusGet(s_pwm_pins[0].p_timer->p_ctrl, &status);

        if (TIMER_STATE_COUNTING == status.state)
        {
            R_GPT_Stop(s_pwm_pins[0].p_timer->p_ctrl);
        }
        else
        {
            R_GPT_Start(s_pwm_pins[0].p_timer->p_ctrl);
        }
    }
}
/**********************************************************************************************************************
 End of function gpt_blinker_callback
 *********************************************************************************************************************/


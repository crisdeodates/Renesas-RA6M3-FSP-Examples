/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Common init function.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#include "common_init.h"
#include "board_cfg.h"

static const struct
{
    const timer_instance_t * const p_timer;
    const gpt_io_pin_t              pin;
} pwm_pins[] =
{
    {&g_gpt_red, GPT_IO_PIN_GTIOCA},
    {&g_gpt_green, GPT_IO_PIN_GTIOCB},
    {&g_gpt_blue, GPT_IO_PIN_GTIOCA},
};

static uint32_t cur_rate = 0;
uint32_t pwm_rates[3] = {BLINK_FREQ_1HZ, BLINK_FREQ_5HZ, BLINK_FREQ_10HZ};


void gpt_blink_callback(timer_callback_args_t *p_args)
{
    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
        timer_status_t status = {0};

        R_GPT_StatusGet(pwm_pins[0].p_timer->p_ctrl, &status);

        for(uint32_t i = 0; i < g_bsp_leds.led_count; i++)
        {
            if (TIMER_STATE_COUNTING == status.state)
            {
                R_GPT_Stop(pwm_pins[i].p_timer->p_ctrl);
            }
            else
            {
                R_GPT_Start(pwm_pins[i].p_timer->p_ctrl);
            }
        }
    }
}

static fsp_err_t GPT_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for(uint32_t i = 0; i < g_bsp_leds.led_count; i++)
    {
        fsp_err = R_GPT_Open(pwm_pins[i].p_timer->p_ctrl, pwm_pins[i].p_timer->p_cfg);
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;
    }

    fsp_err = R_GPT_Open(g_blinker.p_ctrl, g_blinker.p_cfg);
    {
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;
    }
    fsp_err = R_GPT_PeriodSet(g_blinker.p_ctrl, pwm_rates[cur_rate]);
    {
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;
    }

    fsp_err = R_GPT_Start(g_blinker.p_ctrl);
    if(FSP_SUCCESS != fsp_err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        /* Close the GPT timer */
        R_GPT_Close(g_blinker.p_ctrl);

        return fsp_err;
    }

    return fsp_err;
}


fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = GPT_Initialize();
    if(FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }
    return fsp_err;
}

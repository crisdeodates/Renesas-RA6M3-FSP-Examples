/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Common init function.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_init.h"
#include "board_cfg.h"



#define NUM_RATES             (sizeof(pwm_rates) / sizeof(pwm_rates[0]))
#define NUM_SWITCH            (sizeof(irq_pins) / sizeof(irq_pins[0]))

int g_curr_led_freq = BLINK_FREQ_1HZ;

static const struct
{
    const external_irq_instance_t * const p_irq;
} irq_pins[] =
{
        {&g_external_irq06},
};

static const struct
{
    const timer_instance_t * const p_timer;
    const gpt_io_pin_t              pin;
} pwm_pins[] =
{
    {&g_gpt_red, GPT_IO_PIN_GTIOCB},
};


static uint32_t cur_rate = 0;

int curr_led_freq = 0;
uint32_t pwm_rates[3] = {BLINK_FREQ_1HZ, BLINK_FREQ_5HZ, BLINK_FREQ_10HZ};


void button_irq06_callback(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    if(++cur_rate > (NUM_RATES - 1))
        cur_rate = 0;

    R_GPT_PeriodSet(g_blinker.p_ctrl, pwm_rates[cur_rate]);

    g_curr_led_freq = (int)pwm_rates[cur_rate];
}


static fsp_err_t ICU_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for(uint32_t i = 0; i < NUM_SWITCH; i++)
    {
        fsp_err = R_ICU_ExternalIrqOpen(irq_pins[i].p_irq->p_ctrl, irq_pins[i].p_irq->p_cfg);
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;

        fsp_err = R_ICU_ExternalIrqEnable(irq_pins[i].p_irq->p_ctrl);
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;
    }

    return fsp_err;
}

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

fsp_err_t adc_reading(uint16_t *p_adc_data)
{
    fsp_err_t err = FSP_SUCCESS;
    adc_status_t p_status;

    err = R_ADC_ScanStart (g_adc.p_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    do
    {
        err = R_ADC_StatusGet (g_adc.p_ctrl, &p_status);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

    }
    while (ADC_STATE_IDLE != p_status.state);

    /* Read die temperature */
    err = R_ADC_Read (g_adc.p_ctrl, ADC_CHANNEL_TEMPERATURE, p_adc_data);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_ADC_ScanStop (g_adc.p_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return FSP_SUCCESS;
}

static fsp_err_t ADC_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
    if(FSP_SUCCESS != fsp_err)
        return fsp_err;

    R_ADC0->VREFAMPCNT |= ((3 << 1) | (3 << 3));

    fsp_err = R_ADC_Calibrate(&g_adc_ctrl, NULL);
    if(FSP_SUCCESS != fsp_err)
        return fsp_err;

    fsp_err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
    if(FSP_SUCCESS != fsp_err)
        return fsp_err;

    return fsp_err;
}

fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = GPT_Initialize();
    if(FSP_SUCCESS != fsp_err)
        return fsp_err;

    fsp_err = ICU_Initialize();
    if(FSP_SUCCESS != fsp_err)
        return fsp_err;

    fsp_err = ADC_Initialize();
    if(FSP_SUCCESS != fsp_err)
        return fsp_err;

    return fsp_err;
}

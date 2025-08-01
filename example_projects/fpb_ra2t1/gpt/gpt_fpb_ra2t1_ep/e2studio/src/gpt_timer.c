/***********************************************************************************************************************
 * File Name    : gpt_timer.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "gpt_timer.h"

/*******************************************************************************************************************//**
 * @addtogroup r_gpt_ep
 * @{
 **********************************************************************************************************************/

/* Boolean flag to determine one-shot mode timer is expired or not */
bool volatile g_one_shot_expired  = false;
/* Store Timer open state */
uint8_t g_timer_open_state = RESET_VALUE;

/***********************************************************************************************************************
 * @brief       Initialize GPT timer
 * @param[in]   p_timer_ctl     Timer instance control structure
 * @param[in]   p_timer_cfg     Timer instance Configuration structure
 * @param[in]   timer_mode      Mode of GPT Timer
 * @retval      FSP_SUCCESS     Upon successful open of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS on Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t init_gpt_timer(timer_ctrl_t * const p_timer_ctl, timer_cfg_t const * const p_timer_cfg, uint8_t timer_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize GPT Timer */
    err = R_GPT_Open(p_timer_ctl, p_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n** R_GPT_Open API FAILED **\r\n");
        return err;
    }
    if (PERIODIC_MODE_TIMER == timer_mode)
    {
        g_timer_open_state = PERIODIC_MODE;
    }
    else if (PWM_MODE_TIMER == timer_mode)
    {
        g_timer_open_state = PWM_MODE;
    }
    else
    {
        g_timer_open_state = ONE_SHOT_MODE;
    }
    return err;
}

/***********************************************************************************************************************
 * @brief       Start GPT timers in Periodic, One-Shot, PWM mode
 * @param[in]   p_timer_ctl     Timer instance control structure
 * @retval      FSP_SUCCESS     Upon successful start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS on Unsuccessful start
 **********************************************************************************************************************/
fsp_err_t start_gpt_timer (timer_ctrl_t * const p_timer_ctl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Starts GPT timer */
    err = R_GPT_Start(p_timer_ctl);
    if (FSP_SUCCESS != err)
    {
        /* In case of GPT_open is successful and start fails, requires a immediate cleanup.
         * Since, cleanup for GPT open is done in start_gpt_timer, Hence cleanup is not required */
        APP_ERR_PRINT ("\r\n ** R_GPT_Start API failed ** \r\n");
    }
    return err;
}

/***********************************************************************************************************************
 *  @brief       Set duty cycle of PWM timer
 *  @param[in]   duty_cycle_percent
 *  @retval      FSP_SUCCESS on correct duty cycle set
 *  @retval      FSP_INVALID_ARGUMENT on invalid info
 **********************************************************************************************************************/
fsp_err_t set_timer_duty_cycle(uint8_t duty_cycle_percent)
{
    fsp_err_t err                           = FSP_SUCCESS;
    uint32_t duty_cycle_counts              = RESET_VALUE;
    uint32_t current_period_counts          = RESET_VALUE;
    timer_info_t info                       = {(timer_direction_t)RESET_VALUE, RESET_VALUE, RESET_VALUE};

    /* Get the current period setting */
    err = R_GPT_InfoGet(&g_timer_pwm_ctrl, &info);
    if (FSP_SUCCESS != err)
    {
        /* GPT Timer InfoGet Failure message */
        APP_ERR_PRINT ("\r\n** R_GPT_InfoGet API failed **\r\n");
    }
    else
    {
        /* Update period counts locally */
        current_period_counts = info.period_counts;

        /* Calculate the desired duty cycle based on the current period. Note that if the period could be larger than
         * UINT32_MAX / 100, this calculation could overflow. A cast to uint64_t is used to prevent this. The cast is
         * not required for 16-bit timers */
        duty_cycle_counts = (uint32_t) ((uint64_t) (current_period_counts * duty_cycle_percent) / GPT_MAX_PERCENT);
#if defined(BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA6T2_MCK) ||\
    defined (BOARD_RA4T1_MCK)
        duty_cycle_counts = (current_period_counts - duty_cycle_counts);
#endif

        /* Duty Cycle Set API set the desired intensity on the on-board LED */
        err = R_GPT_DutyCycleSet(&g_timer_pwm_ctrl, duty_cycle_counts, TIMER_PIN);
        if (FSP_SUCCESS != err)
        {
            /* GPT Timer DutyCycleSet Failure message */
            /* In case of GPT_open is successful and DutyCycleSet fails, requires a immediate cleanup.
             * Since, cleanup for GPT open is done in timer_duty_cycle_set,Hence cleanup is not required */
            APP_ERR_PRINT ("\r\n** R_GPT_DutyCycleSet API failed **\r\n");
        }
    }
    return err;
}

/***********************************************************************************************************************
 *  @brief      Process input string to integer value
 *  @param[in]  None
 *  @retval     integer value of input string
 **********************************************************************************************************************/
uint32_t process_input_data(void)
{
    unsigned char buf[BUF_SIZE] = {INITIAL_VALUE};
    uint32_t num_bytes          = RESET_VALUE;
    uint32_t value              = RESET_VALUE;

    while (RESET_VALUE == num_bytes)
    {
        if (APP_CHECK_DATA)
        {
            num_bytes = APP_READ(buf);
            if (RESET_VALUE == num_bytes)
            {
                APP_PRINT("\r\nInvalid Input\r\n");
            }
        }
    }

    /* Conversion from input string to integer value */
    value =  (uint32_t) (atoi((char *)buf));

    return value;
}

/***********************************************************************************************************************
 * @brief      Close the GPT HAL driver
 * @param[in]  p_timer_ctl     Timer instance control structure
 * @retval     None
 **********************************************************************************************************************/
void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Timer Close API call */
    err = R_GPT_Close(p_timer_ctl);
    if (FSP_SUCCESS != err)
    {
        /* GPT Close failure message */
        APP_ERR_PRINT ("\r\n** R_GPT_Close FAILED **\r\n");
    }

    /* Reset open state of timer */
    g_timer_open_state = RESET_VALUE;
}

/***********************************************************************************************************************
 * @brief      Print GPT Timer menu option
 * @param[in]  None
 * @retval     None
 **********************************************************************************************************************/
void print_timer_menu(void)
{
    APP_PRINT ("\r\nMenu Options");
    APP_PRINT ("\r\n1. Enter 1 for Periodic mode");
    APP_PRINT ("\r\n2. Enter 2 for PWM mode");
    APP_PRINT ("\r\n3. Enter 3 for One-Shot mode\r\n");
    APP_PRINT ("User Input:  ");
}

/***********************************************************************************************************************
 *  @brief      The user defined GPT callback in One-Shot mode
 *  @param[in]  p_args  updates timer event.
 *  @retval     None
 **********************************************************************************************************************/
void user_gpt_one_shot_callback(timer_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        if (TIMER_EVENT_CYCLE_END  == p_args->event)
        {
            /* Set boolean flag on One-Shot mode timer expired */
            g_one_shot_expired = true;
        }
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_gpt_ep)
 **********************************************************************************************************************/

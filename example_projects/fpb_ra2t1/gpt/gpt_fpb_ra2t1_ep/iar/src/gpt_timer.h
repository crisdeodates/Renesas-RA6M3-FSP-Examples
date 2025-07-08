/***********************************************************************************************************************
 * File Name    : gpt_timer.h
 * Description  : Contains Macros and function declarations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef GPT_TIMER_H_
#define GPT_TIMER_H_

/* Macros definitions */
#define GPT_MAX_PERCENT             (100U)          /* Max Duty Cycle percentage */
#define BUF_SIZE                    (16U)           /* Size of buffer for RTT input data */
#define PERIODIC_MODE_TIMER         (1U)            /* To perform GPT Timer in Periodic mode */
#define PWM_MODE_TIMER              (2U)            /* To perform GPT Timer in PWM mode */
#define ONE_SHOT_MODE_TIMER         (3U)            /* To perform GPT Timer in One-Shot mode */
#define INITIAL_VALUE               ('\0')
#define TIMER_UNITS_MILLISECONDS    (1000U)         /* Timer unit in millisecond */
#define CLOCK_TYPE_SPECIFIER        (1ULL)          /* Type specifier */

/* GPT Timer Pin for boards */
#if defined (BOARD_RA6E2_EK) || defined (BOARD_RA2E3_FPB) || defined (BOARD_RA2T1_FPB)
#define TIMER_PIN                   (GPT_IO_PIN_GTIOCA)
#else
#define TIMER_PIN                   (GPT_IO_PIN_GTIOCB)
#endif

#if defined (BOARD_RA2A1_EK) || defined (BOARD_RA2A2_EK) || defined (BOARD_RA2E2_EK) || defined (BOARD_RA4W1_EK) \
    || defined (BOARD_RA4E2_EK) || defined (BOARD_RA4L1_EK) || defined (BOARD_RA6E2_EK) || defined (BOARD_RA6E1_FPB) \
    || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA2L2_EK) || defined (BOARD_RA2T1_FPB)
#define GPT_MAX_PERIOD_COUNT        (0XFFFF)        /* Max Period Count for 16-bit Timer */
#else
#define GPT_MAX_PERIOD_COUNT        (0XFFFFFFFF)    /* Max Period Count for 32-bit Timer */
#endif

#define PERIODIC_MODE	            (1U)            /* To check status of GPT Timer in Periodic mode */
#define PWM_MODE	                (2U)            /* To check status of GPT Timer in PWM mode */
#define ONE_SHOT_MODE               (3U)            /* To check status of GPT Timer in One-Shot mode */

#define EP_INFO    "\r\nThe project initializes GPT module in Periodic, PWM or One-Shot mode based on the user input"\
                   "\r\nfrom the displayed menu options. In Periodic mode, the user can enter the time period within"\
                   "\r\nthe permitted ranges to change the frequency of the user LED. In PWM mode, the user can enter"\
                   "\r\nthe duty cycle within the specified range to adjust the intensity of the user LED. In One-Shot"\
                   "\r\nmode, output will be displayed on J-link RTT Viewer when timer expires.\r\n"

/* Function declarations */
fsp_err_t init_gpt_timer(timer_ctrl_t * const p_timer_ctl, timer_cfg_t const * const p_timer_cfg, uint8_t timer_mode);
fsp_err_t start_gpt_timer(timer_ctrl_t * const p_timer_ctl);
fsp_err_t set_timer_duty_cycle(uint8_t duty_cycle_percent);
uint32_t  process_input_data(void);
void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl);
void print_timer_menu(void);

#endif /* GPT_TIMER_H_ */

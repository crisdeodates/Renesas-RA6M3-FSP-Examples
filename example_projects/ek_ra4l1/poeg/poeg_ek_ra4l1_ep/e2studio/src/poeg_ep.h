/***********************************************************************************************************************
 * File Name    : poeg_ep.h
 * Description  : Contains macros and functions declaration
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef POEG_EP_H_
#define POEG_EP_H_
#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macros for commands to be received through RTT input */
#define POEG_TRIGGER_ON_GTETRG_INPUT_PIN_LEVEL (1U)
#define POEG_OUTPUT_DISABLE_USING_SOFTWARE_API (2U)
#define POEG_OUTPUT_DISABLE_ON_GTIOC_OUTPUT_LEVEL (3U)
#if !defined (BOARD_RA4L1_EK)
#define POEG_OUTPUT_DISABLE_ON_ACMPHS_EVENTS (4U)
#define POEG_RESET (5U)
#else
#define POEG_RESET (4U)
#endif

#define POEG_CHANNEL_0 (1U)
#define POEG_CHANNEL_1 (2U)

/* Macro definition */
#define DAC_MAX_VAL (4095U)     /* Max count value for 12 bit DAC */
#if defined (BOARD_RA8E1_FPB)
#define LOW_VAL  (1400U)        /* Lower value of fluctuating range*/
#define HIGH_VAL (1600U)        /* Higher value of fluctuating range*/
#define V_REF    (3.3f)         /* Reference Voltage*/
#else
#define DAC_REF_VAL (1024U)     /* Reference value for 12 bit DAC */
#define LOW_VAL (2000U)         /* Lower value of fluctuating range*/
#define HIGH_VAL (2100U)        /* Higher value of fluctuating range*/
#endif
#define CHANNEL_ZERO (0U)       /* Channel 0 of ACMPHS */

#if defined(BOARD_RA6T2_MCK)
#define LED_PIN (BSP_IO_PORT_04_PIN_00)
#define GTETRG_PIN "PA12"
#elif defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T3_MCK)
#define LED_PIN (BSP_IO_PORT_01_PIN_13)
#elif defined(BOARD_RA8D1_EK)
#define LED_PIN (BSP_IO_PORT_06_PIN_00)
#define GTETRG_PIN "P401"
#elif defined(BOARD_RA8M1_EK)
#define LED_PIN (BSP_IO_PORT_06_PIN_00)
#define GTETRG_PIN "P614"
#elif defined(BOARD_RA8T1_MCK)
#define LED_PIN (BSP_IO_PORT_10_PIN_12)
#elif defined(BOARD_RA8E1_FPB)
#define LED_PIN (BSP_IO_PORT_04_PIN_08)
#elif defined(BOARD_RA4L1_EK)
#define LED_PIN (BSP_IO_PORT_06_PIN_09)
#else
#define LED_PIN (BSP_IO_PORT_04_PIN_00)
#endif
#ifndef GTETRG_PIN
#define GTETRG_PIN "P104"
#endif

#define SET_FLAG    (1)
#define CLEAR_FLAG  (0)

#define NULL_CHAR   ('\0')

#if !defined (BOARD_RA4L1_EK)
#define EP_INFO                "\r\nThe example project demonstrates the modes of POEG module"\
                               "\r\n1. POEG Trigger Pin mode-GPT Output disable on GTETRG pin level"\
                               "\r\n2. POEG Software API mode-GPT Output disable by software API"\
                               "\r\n3. POEG Trigger Output Level mode-GPT Output disable on GTIOC level"\
                               "\r\n4. POEG Trigger ACMPHS mode-GPT Output disable on comparator events"\
                               "\r\n5. Enable Port output using POEG_Reset API."\
                               "\r\nUsing J-Link RTT Viewer input, user can choose the POEG method.\r\n"\

#define MENU_INFO              "\r\nPOEG Application Menu\r\n"\
                               "1 : Disable GPT output pin using POEG trigger pin\r\n"\
                               "2 : Disable GPT output pin using POEG software API\r\n"\
                               "3 : Disable GPT output pin using POEG trigger GPT output level\r\n"\
                               "4 : Disable GPT output pin using POEG trigger ACMPHS Mode\r\n"\
                               "5 : Reset the POEG Channel\r\n"

#if defined (BOARD_RA8E1_FPB)
#define MENU_INFO_1            "** Reset of application is needed prior to continuous selection of the\r\n"\
                               "menu option 4 to restart the DAC0 and ACMPHS **\r\n"
#else
#define MENU_INFO_1            "** Reset of application is needed prior to continuous selection of the\r\n"\
                               "menu option 4 to restart the DAC0 and DAC1 and ACMPHS **\r\n"
#endif

#else
#define EP_INFO                "\r\nThe example project demonstrates the modes of POEG module"\
                               "\r\n1. POEG Trigger Pin mode-GPT Output disable on GTETRG pin level"\
                               "\r\n2. POEG Software API mode-GPT Output disable by software API"\
                               "\r\n3. POEG Trigger Output Level mode-GPT Output disable on GTIOC level"\
                               "\r\n4. Enable Port output using POEG_Reset API."\
                               "\r\nUsing J-Link RTT Viewer input, user can choose the POEG method.\r\n"\

#define MENU_INFO              "\r\nPOEG Application Menu\r\n"\
                               "1 : Disable GPT output pin using POEG trigger pin\r\n"\
                               "2 : Disable GPT output pin using POEG software API\r\n"\
                               "3 : Disable GPT output pin using POEG trigger GPT output level\r\n"\
                               "4 : Reset the POEG Channel\r\n"
#endif

#define MENU_INFO_2            "** Reset of application is needed prior to continuous selection of the\r\n"\
                               "menu option 1 **\r\n"\
                               "User Input :  "

#define RESET_MENU_INFO         "\r\nPOEG RESET Menu\r\n"\
                               "1 :  RESET POEG Channel 0 \r\n"\
                               "2 :  RESET POEG Channel 1 \r\n"\
                               "User Input :  "

/* Status of LED */

#if defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T1_RSSK) || defined(BOARD_RA6T2_MCK) || \
    defined(BOARD_RA6T3_MCK) || defined(BOARD_RA8T1_MCK)
#define   LED_ON  (BSP_IO_LEVEL_LOW)
#define   LED_OFF (BSP_IO_LEVEL_HIGH)
#else
#define   LED_ON  (BSP_IO_LEVEL_HIGH)
#define   LED_OFF (BSP_IO_LEVEL_LOW)
#endif

void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl);
fsp_err_t init_poeg0_module(void);
fsp_err_t init_poeg1_module(void);
fsp_err_t init_gpt_timer_pwm1(void);
fsp_err_t init_gpt_timer_pwm2(void);
fsp_err_t init_gpt_timer_pwm3(void);
fsp_err_t deinit_poeg0_module(void);
fsp_err_t deinit_poeg1_module(void);
void deinit_poeg_module(void);
#if !defined (BOARD_RA4L1_EK)
fsp_err_t enable_acmphs_mode_modules(void);
void deinit_dac_and_acmphs(void);
void deinit_acmphs(void);
void deinit_dac(dac_instance_ctrl_t * p_ctrl);
extern comparator_info_t g_stabilize_time;
#endif
fsp_err_t poeg_ouput_disable_and_reset_options(void);
fsp_err_t reset_poeg_module(void);

#endif /* POEG_EP_H_ */

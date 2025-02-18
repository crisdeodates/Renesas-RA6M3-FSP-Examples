/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2025 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : board_hw_cfg.h
 * Version      : 1.0
 * Description  : .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2018 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "bsp_api.h"
#include "common_init.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#ifndef BOARD_HW_CFG_H_
#define BOARD_HW_CFG_H_

#ifndef BOARD_INIT_H
#include "board_leds.h"
#endif
extern const bsp_leds_t g_bsp_leds;

/* OFF stage Configuration */
#define OFF_CFG_                                     (0U)
/* ON stage Configuration */
#define ON_CFG_                                      (1U)

/* BLUE LED Configuration */
#define BLUE_CFG_      (BSP_LED_LED1)
/* GREEN LED Configuration */
#define GREEN_CFG_     (BSP_LED_LED2)
/* RED LED Configuration */
#define RED_CFG_       (BSP_LED_LED3)

/* TURN RED LED ON Macro */
#define TURN_RED_ON_CFG_    R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[RED_CFG_], ON_CFG_);\
                            R_BSP_PinAccessDisable();
/* TURN RED LED OFF Macro */
#define TURN_RED_OFF_CFG_   R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[RED_CFG_], OFF_CFG_);\
                            R_BSP_PinAccessDisable();
/* TURN GREEN LED ON Macro */
#define TURN_GREEN_ON_CFG_  R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[GREEN_CFG_], ON_CFG_);\
                            R_BSP_PinAccessDisable();
/* TURN GREEN LED OFF Macro */
#define TURN_GREEN_OFF_CFG_ R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[GREEN_CFG_], OFF_CFG_); \
                            R_BSP_PinAccessDisable();
/* TURN BLUE LED ON Macro */
#define TURN_BLUE_ON_CFG_   R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[BLUE_CFG_], ON_CFG_);\
                            R_BSP_PinAccessDisable();
/* TURN BLUE LED OFF Macro */
#define TURN_BLUE_OFF_CFG_  R_BSP_PinAccessEnable(); \
                            R_BSP_PinWrite(g_bsp_leds.p_leds[BLUE_CFG_], OFF_CFG_);\
                            R_BSP_PinAccessDisable();

/* TURN GREEN LED ON to represent system OK Macro */
#define SYSTEM_OK_CFG_      R_BSP_PinAccessEnable();\
                            TURN_GREEN_ON_CFG_; \
                            TURN_RED_OFF_CFG_; \
                            R_BSP_PinAccessDisable();

/* TURN RED LED ON to represent system ERROR Macro
 * Note GCC compile time __asm used */
#define SYSTEM_ERROR_CFG_   R_BSP_PinAccessEnable(); \
                            TURN_GREEN_OFF_CFG_; \
                            TURN_RED_ON_CFG_; \
                            R_BSP_PinAccessDisable();\
                            __asm("BKPT #0\n");




/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/*
 * Stores a test configuration name, stack instance reference, pin and required pin cfg.
 * Stack instance shall be casted to required type pointer by test function.
 */
typedef struct test_fn_cfg
{
    char_t             * p_name;
    void               * p_inst;
    bsp_io_port_pin_t    pin;
    uint8_t              int_channel;
    IRQn_Type            irqn_type;
    const ioport_cfg_t * p_pin_cfg; /*<! Pointer to required pin configuration */
} st_test_fn_cfg_t;

/* Table of test functions */
typedef struct test_fn_tbl
{
    char_t           * p_name;  /*<! Name of Test */
    test_fn            p_func;  /*<! Pointer to Test Function */
    st_test_fn_cfg_t * p_cfg_1; /*<! Pointer to user configurations to be used by the function */
    st_test_fn_cfg_t * p_cfg_2; /*<! Pointer to additional, optional, user config. for use by the function.
                                     Set to NULL if not used. */
} st_test_fn_tbl_t;


/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* BOARD_HW_CFG_H_ */

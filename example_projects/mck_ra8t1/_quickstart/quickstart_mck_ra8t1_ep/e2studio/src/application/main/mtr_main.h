/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name   : mtr_main.h
* Description : Definitions for the application layer
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 19.01.2024 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef MTR_MAIN_H
#define MTR_MAIN_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#define SW_ON                    (0)                            /* Active level of SW */
#define SW_OFF                   (1)                            /* Inactive level of SW */
#define SW1_ON                   (1)                            /* Active level of SW */
#define SW1_OFF                  (0)                            /* Inactive level of SW */
#define SW2_ON                   (0)                            /* Active level of SW */
#define SW2_OFF                  (1)                            /* Inactive level of SW */
#define CHATTERING_CNT           (10)                           /* Counts to remove chattering */
#define MTR_LED_ON               (BSP_IO_LEVEL_LOW)             /* Active level of LED */
#define MTR_LED_OFF              (BSP_IO_LEVEL_HIGH)            /* Inactive level of LED */
#define MTR_MAX_SPEED_RPM        (1200)
#define STOP_RPM                 (400)
#define MTR_AD12BIT_DATA         (4095.0f)                      /* A/D 12Bit data */
#define VR1_SCALING              (-(MTR_MAX_SPEED_RPM + 100 ) / (MTR_AD12BIT_DATA * 0.5f))
                                                                /* Scaling factor for position reference (A/D) */
                                                                /* (max position(180 degrees) + margin) / A/D(12 bits) */
#define ADJUST_OFFSET            (0x7FF)                        /* Adjusting offset for reference */
#define MTR_FLG_CLR              (0)                            /* For flag clear */
#define MTR_FLG_SET              (1)                            /* For flag set */

#define MTR_ADCH_VR1             (7)                            /* A/D channel of vr1 */

#define MTR_PORT_SW1             (BSP_IO_PORT_10_PIN_15)        /* Input port of SW1 */
#define MTR_PORT_SW2             (BSP_IO_PORT_10_PIN_13)        /* Input port of SW2 */

#define MTR_PORT_LED1            (BSP_IO_PORT_10_PIN_12)        /* Output port of LED1 */
#define MTR_PORT_LED2            (BSP_IO_PORT_10_PIN_14)        /* Output port of LED2 */
/* This line is commented out, because LED3 is not assigned on MCB-RA6T3/RA4T1/RA8T1. */
// #define MTR_PORT_LED3            (BSP_IO_PORT_13_PIN_03)        /* Output port of LED3 */

/***********************************************************************************************************************
* global functions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : mtr_init
* Description   : Initialization for Motor Control
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_init( void );

/***********************************************************************************************************************
* Function Name : mtr_main
* Description   : Initialization and main routine
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_main(void);

#endif /* MTR_MAIN_H */

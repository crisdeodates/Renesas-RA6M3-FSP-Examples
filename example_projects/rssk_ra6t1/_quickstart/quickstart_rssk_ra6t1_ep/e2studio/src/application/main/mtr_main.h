/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name   : mtr_main.h
* Description : Definitions for the application layer
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 28.10.2018 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/* Guard against multiple inclusion */
#ifndef MTR_MAIN_H
#define MTR_MAIN_H

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#define SW_ON                    (0)                        /* Active level of SW */
#define SW_OFF                   (1)                        /* Inactive level of SW */
#define CHATTERING_CNT           (10)                       /* Counts to remove chattering */
#define MTR_LED_ON               (0)                        /* Active level of LED */
#define MTR_LED_OFF              (1)                        /* Inactive level of LED */
#define MTR_MAX_SPEED_RPM        (2650)
#define STOP_RPM                 (400)
#define VR1_SCALING              (-(MTR_MAX_SPEED_RPM + 50 ) / (MTR_AD12BIT_DATA * 0.5f))
                                                            /* Scaling factor for position reference (A/D) */
                                                            /* (max position(180 degrees) + margin) / A/D(12 bits) */
#define ADJUST_OFFSET            (0x7FF)                    /* Adjusting offset for reference */
#define MTR_FLG_CLR              (0)                        /* For flag clear */
#define MTR_FLG_SET              (1)                        /* For flag set */

/* RA6T1 RSSK Definitions */
#define MTR_AD12BIT_DATA         (4095.0f)                  /* A/D 12Bit data */
#define MTR_ADCH_VR1             (17)                       /* A/D channel of vr1 */

#define MTR_PORT_SW1             (R_PORT3->PIDR_b.PIDR2)    /* Input port of SW1 */
#define MTR_PORT_SW2             (R_PORT3->PIDR_b.PIDR1)    /* Input port of SW2 */

#define MTR_PORT_LED1            (R_PORT4->PODR_b.PODR8)    /* Output port of LED1 */
#define MTR_PORT_LED2            (R_PORT4->PODR_b.PODR7)    /* Output port of LED2 */
#define MTR_PORT_LED3            (R_PORT2->PODR_b.PODR7)    /* Output port of LED3 */

/***********************************************************************************************************************
* global functions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : mtr_init
* Description   : Initialization for Motor Control
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_init (void);

/***********************************************************************************************************************
* Function Name : mtr_main
* Description   : Initialization and main routine
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_main (void);

#endif /* MTR_MAIN_H */

/***********************************************************************************************************************
 * File Name    : led.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#include "led.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/***********************************************************************************************************************
Export global variables
***********************************************************************************************************************/
extern const bsp_leds_t g_bsp_leds;


/***********************************************************************************************************************
* Function Name: R_LED_BLUE_OFF
* Description  : This function performs the processing in the code
* Arguments    : void
* Return Value : none
***********************************************************************************************************************/
void R_LED_BLUE_OFF(void ) // @suppress("API function naming")
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], BSP_IO_LEVEL_LOW);
}

/***********************************************************************************************************************
* Function Name: R_LED_BLUE_ON
* Description  : This function performs the processing in the code
* Arguments    : void
* Return Value : none
***********************************************************************************************************************/
void R_LED_BLUE_ON(void ) // @suppress("API function naming")
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], BSP_IO_LEVEL_HIGH);
}

/***********************************************************************************************************************
 * File Name    : lpm_app_peripheral.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "lpm_app.h"

/*******************************************************************************************************************//**
 * @addtogroup lpm_app
 * @{
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @brief       This function toggles user LED.
 * @param[IN]   leds
 * @retval      fsp_err_t
 **********************************************************************************************************************/
fsp_err_t user_led_toggle(bsp_leds_t leds)
{
    fsp_err_t err = FSP_SUCCESS;
	/* Toggle user LED */
	err = R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[LED_NO_0], BSP_IO_LEVEL_HIGH);
	/* Handle error */
	if (FSP_SUCCESS == err)
	{
		/* Delay 250 milliseconds */
		R_BSP_SoftwareDelay(250, BSP_DELAY_UNITS_MILLISECONDS);

		/* Toggle user LED */
		err = R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[LED_NO_0], BSP_IO_LEVEL_LOW);

		/* Delay 250 milliseconds */
		R_BSP_SoftwareDelay(250, BSP_DELAY_UNITS_MILLISECONDS);
	}

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function turn on user LED.
 * @param[IN]   leds
 * @retval      fsp_err_t
 **********************************************************************************************************************/
fsp_err_t user_led_on(bsp_leds_t leds)
{
    fsp_err_t err = FSP_SUCCESS;

	/* Toggle user LED */
	err = R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[LED_NO_0], BSP_IO_LEVEL_HIGH);

    return err;
}
/*******************************************************************************************************************//**
 * @} (end addtogroup lpm_app)
 **********************************************************************************************************************/

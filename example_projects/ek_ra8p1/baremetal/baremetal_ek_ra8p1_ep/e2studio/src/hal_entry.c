/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

extern bsp_leds_t g_bsp_leds;   /* LED structure used to blink on board LED */

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used. This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_pack_version_t version = {RESET_VALUE};

    /* Get API version for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Print Example Project information on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);
    APP_PRINT("\r\nThis project demonstrates the direct access, configuration, and operation of DOC SFRs.");
    APP_PRINT("\r\nThe SFRs and interrupts are configured directly to perform a comparison match.");
    APP_PRINT("\r\nThe ISR for the DOC is configured through the Interrupts tab of the FSP Configurator.");

    if (g_bsp_leds.led_count == 3){
        APP_PRINT("\r\nLED 1 is turned ON while program control waits for the User Defined ISR to execute.");
        APP_PRINT("\r\nLED 2 is turned ON to indicate the successful conclusion of application execution.");
        APP_PRINT("\r\nLED 3 is turned ON if there is an expected error encountered during operation.");
    }
    else if (g_bsp_leds.led_count == 2)
    {
        APP_PRINT("\r\nLED 1 is turned ON only while program control waits for the User Defined ISR to execute.");
        APP_PRINT("\r\nLED 2 is turned ON only indicate the successful conclusion of application execution.");
        APP_PRINT("\r\nLED 1 and LED 2 are turned ON if there is an expected error encountered during operation.");
    }
    else
    {
        /* Do nothing */
    }

    /* Run the example project code */
    baremetal_ep();

    APP_PRINT("\r\nReached Application End.\r\n");

    while(1)
    {
        /* Halt the CPU */
        __WFI();
    }

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process. This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take
         * more than 6us */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup */

        /* Configure pins */
        R_IOPORT_Open (&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);

#if BSP_CFG_SDRAM_ENABLED

        /* Setup SDRAM and initialize it. Must configure pins first */
        R_BSP_SdramInit(true);
#endif
    }
}

#if BSP_TZ_SECURE_BUILD

FSP_CPP_HEADER
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build
 * (Remove this if it is not required to build) */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
FSP_CPP_FOOTER

#endif

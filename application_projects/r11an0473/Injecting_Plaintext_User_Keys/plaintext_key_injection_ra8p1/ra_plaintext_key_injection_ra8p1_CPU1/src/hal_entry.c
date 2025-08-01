/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include "hal_data.h"

#include "common_utils.h"
#include "example_AES.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

void g_rm_littlefs_spi_flash0_callback(rm_littlefs_spi_flash_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
}

/*****************************************************************************************
 *  @brief      app_lfs_init: format and mount the LittleFS system for key management
 *  @retval     fsp_err_t
 *  **************************************************************************************/
static fsp_err_t app_lfs_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int lfs_err = RESET_VALUE;

    err = RM_LITTLEFS_SPI_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);

    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nRM_LITTLEFS_SPI_FLASH_Open API FAILED\r\n");
        return err;
    }

    lfs_err = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if (LFS_ERR_OK != lfs_err)
    {
        /* Format the file system */
        lfs_err = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
        if (LFS_ERR_OK != lfs_err)
        {
            APP_ERR_PRINT("\r\n**lfs_format API FAILED!!!**\r\n");
            return (fsp_err_t)lfs_err;
        }

        /* Mount the file system again */
        lfs_err = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
        if (LFS_ERR_OK != lfs_err)
        {
            APP_ERR_PRINT("\r\n**lfs_mount API FAILED after formatting!!!**\r\n");
            return (fsp_err_t)lfs_err;
        }
    }

    return err;
}

static fsp_err_t app_aes_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = app_lfs_init();
    if (FSP_SUCCESS == err)
    {
        APP_PRINT("\r\nLFS initialization successful.\r\n");
    }
    else 
    {
        APP_ERR_PRINT( "\r\nLFS initialization failed.\r\n");
    }

    err = psacrypto_AES256CBC_example_NIST();
    if (FSP_SUCCESS == err)
    {
        APP_PRINT( "\r\nRSIP-E50D AES key injection operation is successful.\r\n");
    }
    else
    {
        APP_ERR_PRINT( "\r\nRSIP-E50D AES key injection operation failed.\r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    app_aes_operation();

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);

#if BSP_CFG_SDRAM_ENABLED

        /* Setup SDRAM and initialize it. Must configure pins first. */
        R_BSP_SdramInit(true);
#endif
    }
}

#if BSP_TZ_SECURE_BUILD

FSP_CPP_HEADER
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
FSP_CPP_FOOTER

#endif

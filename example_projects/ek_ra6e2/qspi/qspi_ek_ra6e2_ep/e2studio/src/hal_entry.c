/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "qspi_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup qspi_ep
 * @{
 **********************************************************************************************************************/

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/*
 * private functions
 */
static fsp_err_t get_flash_status(void);
static void deinit_qspi(const spi_flash_protocol_t spi_protocol_mode);
static fsp_err_t qpi_mode_set(void);


/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void) {
    fsp_err_t err                                  = FSP_SUCCESS;
    uint8_t   data_write[PAGE_WRITE_SIZE]          = {RESET_VALUE,};
    uint8_t   verify_written_data[PAGE_WRITE_SIZE] = {RESET_VALUE,};
    uint8_t   data_sreg[SREG_SIZE]                 = STATUS_REG_PAYLOAD;
    fsp_pack_version_t version                     = {RESET_VALUE};
    uint8_t *p_mem_addr                            = (uint8_t *)QSPI_DEVICE_START_ADDRESS;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Banner information */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    if (SPI_FLASH_PROTOCOL_QPI == g_qspi_cfg.spi_protocol)
    {
        /*
         * this needs to be done since QPI is set by user in configuration
         * and it sets QPI only in MCU but not in flash device
         * so as a system (MCU + QSPI flash device) QPI mode does not get set by
         * simply calling only R_QSPI_Open in QPI mode.
         * Rather QPI mode enabling has to be done in Flash device as well
         * So opening the driver in extended SPI mode only
         * and QPI mode is enabled when qpi_mode_set sub-function is called
         */
        spi_flash_cfg_t l_qspi_cfg;

        memcpy((spi_flash_cfg_t *)&l_qspi_cfg, (spi_flash_cfg_t *)&g_qspi_cfg, sizeof (spi_flash_cfg_t));

        l_qspi_cfg.spi_protocol = SPI_FLASH_PROTOCOL_EXTENDED_SPI;

        APP_PRINT ("\r\n ** user selected QPI Mode in RA configuration tool ** \r\n");

        /* open QSPI with local configuration  */
        err = R_QSPI_Open(&g_qspi_ctrl, &l_qspi_cfg);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("R_QSPI_Open Failed\r\n");
            APP_ERR_TRAP(err);
        }
    }
    else
    {
        APP_PRINT ("\r\n ** user selected extended SPI Mode in RA Configuration tool ** \r\n");

        /* open QSPI in extended SPI mode */
        err = R_QSPI_Open(&g_qspi_ctrl, &g_qspi_cfg);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("R_QSPI_Open Failed\r\n");
            APP_ERR_TRAP(err);
        }
    }

    /* write enable for further operations */
    err = R_QSPI_DirectWrite(&g_qspi_ctrl, &(g_qspi_cfg.write_enable_command), ONE_BYTE, false);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("R_QSPI_DirectWrite Failed\r\n");
        /* close QSPI module which is currently in extended SPI mode only */
        deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);
        APP_ERR_TRAP(err);
    }
    else
    {
        err = get_flash_status();
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("Failed to get status for QSPI operation\r\n");
            /* close QSPI module which is currently in extended SPI mode only */
            deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);
            APP_ERR_TRAP(err);
        }
    }

    /*
     * write QSPI flash status register
     * This is required to make sure the device is ready for general
     * read write operation,
     * This performs settings such as physical reset,WP hardware pin disable,
     * block protection lock bits clearing.
     * for more details please refer AT25SF128A data sheet.
     */
    err = R_QSPI_DirectWrite(&g_qspi_ctrl, data_sreg, SREG_SIZE, false);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("R_QSPI_DirectWrite Failed\r\n");
        /* close QSPI module which is currently in extended SPI mode only */
        deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);
        APP_ERR_TRAP(err);
    }
    else
    {
        err = get_flash_status();
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("Failed to get status for QSPI operation\r\n");
            /* close QSPI module which is currently in extended SPI mode only */
            deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);
            APP_ERR_TRAP(err);
        }
    }

    /*
     * Verifying data written to QSPI flash status register
     * Step 1: - send command byte - 0x05
     * through R_QSPI_DirectWrite with last argument set as true
     * Step 2 - read data through R_QSPI_DirectRead
     */
    uint8_t sreg_data = RESET_VALUE;
    err = R_QSPI_DirectWrite(&g_qspi_ctrl, &(g_qspi_cfg.status_command), ONE_BYTE, true);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("R_QSPI_DirectWrite Failed\r\n");
        /* close QSPI module which is currently in extended SPI mode only */
        deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);
        APP_ERR_TRAP(err);
    }

    /*
     *  we should not call function get_flash_status here
     * because the CS line should not get interrupted between write read
     *
     * Also MCU <SFMCD register> is set as 0 when status register is read
     * to resume in ROM access mode hence API direct read returns error as part
     * of parameter check itself
     */
    err = R_QSPI_DirectRead(&g_qspi_ctrl, &sreg_data, ONE_BYTE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("R_QSPI_DirectRead Failed\r\n");
        /* close QSPI module which is currently in extended SPI mode only */
        deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);
        APP_ERR_TRAP(err);
    }
    else
    {
        /* check for status check operation here */
        err = get_flash_status();
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("Failed to get status for QSPI operation\r\n");
            /* close QSPI module which is currently in extended SPI mode only */
            deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);
            APP_ERR_TRAP(err);
        }
    }

    /* verify read status register data */
    if (SET_SREG_VALUE != sreg_data)
    {
        APP_ERR_PRINT("Failed to get value set in the status register \r\n");
        /* close QSPI module which is currently in extended SPI mode only */
        deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);
        APP_ERR_TRAP(err);
    }


    if (SPI_FLASH_PROTOCOL_QPI == g_qspi_cfg.spi_protocol)
    {
        /* set QPI mode in flash and MCU device */
        err = qpi_mode_set();
        if (FSP_SUCCESS!=err)
        {
            APP_ERR_PRINT ("qpi_mode_set failed\r\n");
            /* close QSPI module which is currently in extended SPI mode only */
            deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);
            APP_ERR_TRAP(err);
        }
    }

    /* Erase Flash for one sector */
    err = R_QSPI_Erase(&g_qspi_ctrl, p_mem_addr, SECTOR_SIZE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("R_QSPI_Erase Failed\r\n");
        deinit_qspi(g_qspi_cfg.spi_protocol);
        APP_ERR_TRAP(err);
    }
    else
    {
        err = get_flash_status();
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("Failed to get status for QSPI operation\r\n");
            deinit_qspi(g_qspi_cfg.spi_protocol);
            APP_ERR_TRAP(err);
        }

        /* validating erase */
        for (uint16_t mem_index = RESET_VALUE; mem_index < SECTOR_SIZE; mem_index++)
        {
            if (DEFAULT_MEM_VAL != p_mem_addr[mem_index])
            {
                APP_ERR_PRINT ("\r\n Verification for erase Failed \r\n");
                deinit_qspi(g_qspi_cfg.spi_protocol);
                APP_ERR_TRAP(err);
            }
        }
    }

    /* Fill the data write buffer further to be written in QSPI flash device */
    for (uint16_t write_index = RESET_VALUE; write_index < PAGE_WRITE_SIZE ; write_index++)
    {
        data_write[write_index] = (uint8_t)write_index;
    }

    /* Write data to QSPI Flash */
    err = R_QSPI_Write(&g_qspi_ctrl, data_write, (uint8_t *)QSPI_FLASH_ADDRESS(PAGE_FIRST), PAGE_WRITE_SIZE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("R_QSPI_Write Failed\r\n");
        deinit_qspi(g_qspi_cfg.spi_protocol);
        APP_ERR_TRAP(err);
    }
    else
    {
        err = get_flash_status();
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("Failed to get status for QSPI operation\r\n");
            deinit_qspi(g_qspi_cfg.spi_protocol);
            APP_ERR_TRAP(err);
        }
    }

    /* Read data from QSPI memory region */
    memcpy(verify_written_data,  (uint8_t *)QSPI_FLASH_ADDRESS(PAGE_FIRST), PAGE_WRITE_SIZE);

    /* Verify the written data */
    if ( FSP_SUCCESS == (fsp_err_t)(memcmp(verify_written_data, data_write, PAGE_WRITE_SIZE)) )
    {
        APP_PRINT ("\r\n ** QSPI operation successful -> Data read matches with written data ** \r\n");
    }
    else
    {
        APP_ERR_PRINT ("\r\nQSPI operation Failed -> Data read does not match with written data\r\n");
    }

    /* close QSPI module */
    deinit_qspi(g_qspi_cfg.spi_protocol);
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
    if (BSP_WARM_START_RESET == event) {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event) {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

/*******************************************************************************************************************//**
 *  @brief       wait for QSPI flash device status register to get idle till operation is in progress
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS or any other possible error codes
 **********************************************************************************************************************/
static fsp_err_t get_flash_status(void)
{
    spi_flash_status_t status = {.write_in_progress = true};
    int32_t time_out          = (INT32_MAX);
    fsp_err_t err             = FSP_SUCCESS;

    do
    {
        /* Get status from QSPI flash device */
        err = R_QSPI_StatusGet(&g_qspi_ctrl, &status);
        if (FSP_SUCCESS!= err)
        {
            APP_ERR_PRINT("R_QSPI_StatusGet Failed\r\n");
            return err;
        }

        /* Decrement time out to avoid infinite loop in case of consistent failure */
        --time_out;

        if ( RESET_VALUE >= time_out)
        {
            APP_PRINT("\r\n ** Timeout : No result from QSPI flash status register ** \r\n");
            return FSP_ERR_TIMEOUT;
        }

    }while (false != status.write_in_progress);

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       Close QSPI module
 *  @param[IN]   spi_protocol mode
 *  @retval      None
 **********************************************************************************************************************/
static void deinit_qspi(const spi_flash_protocol_t spi_protocol_mode)
{
    fsp_err_t error = FSP_SUCCESS;

        /* if QPI is active mode then Exit QPI mode from flash device before QSPI close */
        if (SPI_FLASH_PROTOCOL_QPI == spi_protocol_mode)
        {
            #ifdef QSPI_DEVICE_AT25SF128A
                APP_PRINT ("\r\n ** QPI mode is not supported ** \r\n");
            #elif QSPI_DEVICE_MX25L
                uint8_t data_exit_qpi = QSPI_MX25L_CMD_EXIT_QPI_MODE;

                APP_PRINT ("\r\n ** Exit QPI mode before Closing QSPI module ** \r\n");

                error = R_QSPI_DirectWrite(&g_qspi_ctrl, &data_exit_qpi, ONE_BYTE, false);
                if (FSP_SUCCESS != error)
                {
                    APP_ERR_PRINT("R_QSPI_DirectWrite Failed\r\n");
                }
            #endif
        }


    APP_PRINT ("\r\n ** Closing QSPI module ** \r\n");

    /* close QSPI module */
    error = R_QSPI_Close(&g_qspi_ctrl);
    if (FSP_SUCCESS != error)
    {
        APP_ERR_PRINT("R_QSPI_Close Failed\r\n");
    }

    APP_PRINT("\r\n\r\n *****############## demo ends here ########## *******\r\n\r\n")
}

/*******************************************************************************************************************//**
 *  @brief       set QPI Mode in flash device and MCU
 *  @param[IN]   none
 *  @retval      FSP_SUCCESS or any other possible error codes
 **********************************************************************************************************************/
static fsp_err_t qpi_mode_set(void)
{
    fsp_err_t err = FSP_SUCCESS;

    APP_PRINT ("\r\n ** setting QPI mode: sending QPI enabling command byte to flash ** \r\n");

    /* write enable once again section 9-1 states that
     * we should do it before sending 0x35 to flash device
     */
    err = R_QSPI_DirectWrite(&g_qspi_ctrl, &(g_qspi_cfg.write_enable_command), ONE_BYTE, false);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("R_QSPI_DirectWrite Failed\r\n");
        return err;
    }
    else
    {
        err = get_flash_status();
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("Failed to get status for QSPI operation\r\n");
            return err;
        }
    }

    /* send QPI mode enable command in flash device
     * Note - no status register read after this operation
     * because flash device has gone in QPI mode
     * and MCU at this point is in extended SPI mode only.
     * vice versa same is applicable while exiting QPI mode too.
     */
    #ifdef QSPI_DEVICE_AT25SF128A
        APP_PRINT ("\r\n ** QPI mode is not supported  ** \r\n");
        err = FSP_ERR_UNSUPPORTED;
    #elif QSPI_DEVICE_MX25L
        uint8_t data_qpi_en = QSPI_MX25L_CMD_ENTER_QPI_MODE;
        err = R_QSPI_DirectWrite(&g_qspi_ctrl, &data_qpi_en, ONE_BYTE, false);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("R_QSPI_DirectWrite Failed\r\n");
            return err;
        }

        APP_PRINT ("\r\n ** setting QPI mode:  setting QPI mode in MCU  ** \r\n");

        /* Command byte transferred to flash-> NOW  set the QPI protocol in MCU run time */
        err = R_QSPI_SpiProtocolSet(&g_qspi_ctrl, SPI_FLASH_PROTOCOL_QPI);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("R_QSPI_SpiProtocolSet Failed\r\n");
        }
    #endif

    return err;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup qspi_ep)
 **********************************************************************************************************************/

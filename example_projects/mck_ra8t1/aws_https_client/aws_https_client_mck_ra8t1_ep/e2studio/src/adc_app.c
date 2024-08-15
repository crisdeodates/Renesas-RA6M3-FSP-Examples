/***********************************************************************************************************************
 * File Name    : adc_app.c
 * Description  : This file contains the User functions to configure the adc module
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <user_app_thread.h>
#include "adc_app.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup aws_https_client_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief      Initializes the ADC module by opening and start scanning of the internal mcu die temperature sensor channel
 *
 * @param[in]  None
 * @retval     FSP_SUCCESS                  Upon successful ADC Initialization.
 * @retval     Any other Error Code         Upon unsuccesful ADC Initialization.
 **********************************************************************************************************************/
fsp_err_t hal_adc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ADC module */
    err = R_ADC_Open(g_adc.p_ctrl, g_adc.p_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in R_ADC_Open API ** \r\n");
        return err;
    }

    /* Configures the ADC scan parameters */
    err = R_ADC_ScanCfg (g_adc.p_ctrl, g_adc.p_channel_cfg);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT ("** Failed in R_ADC_ScanCfg API ** \r\n");
        hal_adc_deinit();
        return err;
    }

    /* Start the ADC scan*/
    err = R_ADC_ScanStart (g_adc.p_ctrl);

    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT ("** Failed in R_ADC_ScanStart API ** \r\n");
        hal_adc_deinit();
        return err;
    }
    APP_PRINT("\r\n ADC module initialization successful");
    return err;
}

/*******************************************************************************************************************//**
 * @brief      Reads the internal mcu die temperature value
 *
 * @param[in]  None
 * @retval     ADC Temperature value in celsius
 **********************************************************************************************************************/
float adc_data_read(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t adc_data = RESET_VALUE;
    /* Read die temperature */
    err = R_ADC_Read (g_adc.p_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_data);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in R_ADC_Read API ** \r\n");
        hal_adc_deinit();
        APP_ERR_TRAP(err);
    }

    /* return the conversion of mcu die temperature in celsius */
    return (ADCTEMP_AS_C(adc_data));
}

/*******************************************************************************************************************//**

* @brief      de-initializes the ADC module
*
* @param[in]  None
* @retval     FSP_SUCCESS                  Upon successful ADC Initialization.
* @retval     Any other Error Code         Upon unsuccesful ADC Initialization.
**********************************************************************************************************************/
void hal_adc_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_ADC_Close(g_adc.p_ctrl);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT(" ** Failed in R_ADC_Close API \r\n**");
    }
}

/*******************************************************************************************************************//**
 * @} (end defgroup aws_https_client_ep)
 **********************************************************************************************************************/

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
#include "sdadc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup sdadc_ep
 * @{
 **********************************************************************************************************************/

#if defined(BOARD_RA2A1_EK)
    #define BSP_FEATURE_SDADC_VERSION           (1U)
#else
    #define BSP_FEATURE_SDADC_VERSION           (2U)
#endif

void R_BSP_WarmStart(bsp_warm_start_event_t event);

#if (BSP_FEATURE_SDADC_VERSION == 1U)
/*
 * static variables declaration
 */
/* Flag to notify successful calibration completion */
static volatile bool b_is_calibration_complete = false;

float vref_volt[LEN] = {VREF_VOLT_0_8_V, VREF_VOLT_1_0_V, VREF_VOLT_1_2_V,
                        VREF_VOLT_1_4_V, VREF_VOLT_1_6_V, VREF_VOLT_1_8_V,
                        VREF_VOLT_2_0_V, VREF_VOLT_2_2_V};
#else
#define VREF_VOLT           (0.69F)
#define VREF_FULL_SCALE     (VREF_VOLT * 2.0)
extern const sdadc_b_scan_cfg_t g_sdadc_channel_cfg;
#endif

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    int32_t channel_conversion_result = RESET_VALUE;
    uint32_t timeout_value = UINT32_MAX;
    float voltageOut = RESET_VALUE;
    char dataBuff[BUFF_SIZE] = {RESET_VALUE};


    /* version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Open the SDADC channel */
#if (BSP_FEATURE_SDADC_VERSION == 1U)
    err = R_SDADC_Open (&g_sdadc_ctrl, &g_sdadc_cfg);
#else
    err = R_SDADC_B_Open (&g_sdadc_ctrl, &g_sdadc_cfg);
#endif
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* SDADC module open failed */
        APP_ERR_PRINT("\r\n ** SDADC module Open failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Configures the enabled channels of the ADC */
#if (BSP_FEATURE_SDADC_VERSION == 1U)
    err = R_SDADC_ScanCfg (&g_sdadc_ctrl, &g_sdadc_channel_cfg);
#else
    err = R_SDADC_B_ScanCfg (&g_sdadc_ctrl, &g_sdadc_channel_cfg);
#endif
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** SDADC configure channel fails ** \r\n");
        /* close the SDADC module */
#if (BSP_FEATURE_SDADC_VERSION == 1U)
        if (FSP_SUCCESS != R_SDADC_Close (&g_sdadc_ctrl))
#else
        if (FSP_SUCCESS != R_SDADC_B_Close (&g_sdadc_ctrl))
#endif
        {
            APP_ERR_PRINT("\r\n ** SDADC module close failed ** \r\n");
        }
        APP_ERR_TRAP(err);
    }

#if (BSP_FEATURE_SDADC_VERSION == 1U)
    /* Calibrate the differential channel which is channel 1 here. */
    sdadc_calibrate_args_t calibrate_args;
    calibrate_args.mode = SDADC_CALIBRATION_INTERNAL_GAIN_OFFSET;
    calibrate_args.channel = ADC_CHANNEL_2;

    err = R_SDADC_Calibrate(&g_sdadc_ctrl, &calibrate_args);

    /* handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** SDADC module calibration failed ** \r\n");
        /* close the SDADC module */
        if(FSP_SUCCESS != R_SDADC_Close(&g_sdadc_ctrl))
        {
            APP_ERR_PRINT("\r\n ** SDADC module close failed ** \r\n");
        }
        APP_ERR_TRAP(err);
    }

    /* Wait for calibration to complete. */
    adc_status_t status;
    status.state = ADC_STATE_SCAN_IN_PROGRESS;
    while (ADC_STATE_SCAN_IN_PROGRESS == status.state && timeout_value != RESET_VALUE)
    {
        R_SDADC_StatusGet (&g_sdadc_ctrl, &status);
        timeout_value--;
    }
#endif

#if (BSP_FEATURE_SDADC_VERSION == 1U)
    /* Start the scan if the calibration is completed successfully*/
    if (b_is_calibration_complete)
    {

#endif
        APP_PRINT("\r\n ** SDADC Scan start initiated ** \r\n");

        /* In software trigger mode, start a scan by calling R_SDADC_ScanStart() */
#if (BSP_FEATURE_SDADC_VERSION == 1U)
        err = R_SDADC_ScanStart (&g_sdadc_ctrl);
#else
        err = R_SDADC_B_ScanStart (&g_sdadc_ctrl);
#endif
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n ** SDADC scanstart failed ** \r\n");
            /* close the SDADC module */
#if (BSP_FEATURE_SDADC_VERSION == 1U)
            if (FSP_SUCCESS != R_SDADC_Close (&g_sdadc_ctrl))
#else
            if (FSP_SUCCESS != R_SDADC_B_Close (&g_sdadc_ctrl))
#endif
            {
                APP_ERR_PRINT("\r\n ** SDADC module close failed ** \r\n");
            }
            APP_ERR_TRAP(err);

        }

        APP_PRINT("\r\n ** SDADC Scan start is successful ** \r\n");
#if (BSP_FEATURE_SDADC_VERSION == 1U)
    }
    else
    {
        APP_ERR_PRINT("\r\n ** Calibration is not successful ** \r\n");
        /* close the SDADC module */
        if (FSP_SUCCESS != R_SDADC_Close (&g_sdadc_ctrl))
        {
            APP_ERR_PRINT("\r\n ** SDADC module close failed ** \r\n");
        }
        APP_ERR_TRAP(true);
    }
#endif

#if (BSP_FEATURE_SDADC_VERSION == 2U)
    /* Wait for setup time to complete. */
    adc_status_t status;
    status.state = ADC_STATE_IDLE;
    while (ADC_STATE_SCAN_IN_PROGRESS != status.state && timeout_value != RESET_VALUE)
    {
        R_SDADC_B_StatusGet (&g_sdadc_ctrl, &status);
        timeout_value--;
    }
#endif

    while(true)
    {

        /* Read converted data from channel 0. */
#if (BSP_FEATURE_SDADC_VERSION == 1U)
        err = R_SDADC_Read32(&g_sdadc_ctrl, ADC_CHANNEL_0, (uint32_t*)&channel_conversion_result);
#else
        err = R_SDADC_B_Read32(&g_sdadc_ctrl, ADC_CHANNEL_0, (uint32_t*)&channel_conversion_result);
#endif
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n ** SDADC Read from channel 0 failed ** \r\n");
            /* close the SDADC module */
#if (BSP_FEATURE_SDADC_VERSION == 1U)
            if (FSP_SUCCESS != R_SDADC_Close (&g_sdadc_ctrl))
#else
            if (FSP_SUCCESS != R_SDADC_B_Close (&g_sdadc_ctrl))
#endif
            {
                APP_ERR_PRINT("\r\n ** SDADC module close failed ** \r\n");
            }
            APP_ERR_TRAP(err);
        }

#if (BSP_FEATURE_SDADC_VERSION == 1U)
        /*
         * Conversion of SDADC out to voltage
         *single-ended input  = vref_voltage × (ADCDATA / 2^24) + 0.2 V: page number 999 of ra2a1 manual
         */

        sdadc_extended_cfg_t const * p_cfg_extend_temp = g_sdadc_cfg.p_extend;
        voltageOut=(float)((float)channel_conversion_result/SDADC_RESOLUTION);
        voltageOut = (float)((float)vref_volt[p_cfg_extend_temp->vref_voltage]* voltageOut)+ OFFSET_VOLTAGE;
#else
        /* Input voltage for the SDADC24(differential ended) = (2 * vref_voltage / Gain) × (ADC_DATA / 2^24) */
        voltageOut = (float)(VREF_FULL_SCALE / GAIN_TOTAL) * (float)((float)channel_conversion_result / SDADC_RESOLUTION);
#endif
        snprintf(dataBuff, sizeof(dataBuff), "%0.4f", voltageOut);
        APP_PRINT("\r\nDigital Value at Channel 0:  %d\r\n",channel_conversion_result);
        APP_PRINT("\r\nVoltage at Channel 0:  %s\r\n",dataBuff);

        /* Read converted data from channel 2 */
#if (BSP_FEATURE_SDADC_VERSION == 1U)
        err = R_SDADC_Read32 (&g_sdadc_ctrl, ADC_CHANNEL_2,(uint32_t*) &channel_conversion_result);
#else
        err = R_SDADC_B_Read32 (&g_sdadc_ctrl, ADC_CHANNEL_2,(uint32_t*) &channel_conversion_result);
#endif
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n ** SDADC Read from channel 2 failed ** \r\n");
            /* close the SDADC module */
#if (BSP_FEATURE_SDADC_VERSION == 1U)
            if (FSP_SUCCESS != R_SDADC_Close (&g_sdadc_ctrl))
#else
            if (FSP_SUCCESS != R_SDADC_B_Close (&g_sdadc_ctrl))
#endif
            {
                APP_ERR_PRINT("\r\n ** SDADC module close failed ** \r\n");
            }
            APP_ERR_TRAP(err);
        }

#if (BSP_FEATURE_SDADC_VERSION == 1U)
        /* mask off any unwanted data from the result and sign extend the result */
        channel_conversion_result = ((channel_conversion_result & MASK) << SHIFT) >> SHIFT;

        /*
         * Input voltage for the SDADC24(differential ended) = (vref_voltage / GTOTAL) × (ADCDATA1 / 2^24)
         * from page number 999 of ra2a1 manual
         */
        voltageOut=(float)((float)channel_conversion_result/SDADC_RESOLUTION);
        voltageOut = (float)(((float)vref_volt[p_cfg_extend_temp->vref_voltage]/GAIN_TOTAL) * voltageOut);
#else
        /* Input voltage for the SDADC24(differential ended) = (2 * vref_voltage / Gain) × (ADC_DATA / 2^24) */
        voltageOut = (float)(VREF_FULL_SCALE / GAIN_TOTAL) * (float)((float)channel_conversion_result / SDADC_RESOLUTION);
#endif

        snprintf(dataBuff, sizeof(dataBuff), "%0.4f", voltageOut);
        APP_PRINT("\r\nDigital Value at Channel 2:  %d\r\n",channel_conversion_result);
        APP_PRINT("\r\nVoltage at Channel 2:  %s\r\n",dataBuff);

        /* 500 milliseconds Wait time between successive readings */
        R_BSP_SoftwareDelay (SDADC_READ_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

    }
}

/*******************************************************************************************************************//**
 * @brief SDADC callback function.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void g_sdadc_callback(adc_callback_args_t *p_args)
{
    /*Check if calibration is complete and set flag */
    if (ADC_EVENT_CALIBRATION_COMPLETE == p_args->event)
    {
#if (BSP_FEATURE_SDADC_VERSION == 1U)
        b_is_calibration_complete = true;
#endif
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}
/*******************************************************************************************************************//**
 * @} (end addtogroup sdadc_ep)
 **********************************************************************************************************************/

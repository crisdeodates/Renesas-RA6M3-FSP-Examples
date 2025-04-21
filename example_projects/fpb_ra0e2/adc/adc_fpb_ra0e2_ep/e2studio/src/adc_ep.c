/***********************************************************************************************************************
 * File Name    : adc_ep.c
 * Description  : Contains variables and functions used in adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "adc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup adc_ep
 * @{
 **********************************************************************************************************************/

/* Local variables */
volatile bool g_ready_to_read = false;
static uint16_t g_adc_data;
#if (BSP_PERIPHERAL_ADC_PRESENT)
static bool g_window_comp_event = false;
#endif
uint8_t g_volt_str[VOLT_STR_SIZE] = {RESET_VALUE};
float g_adc_volt = {RESET_VALUE};
 
#ifdef BOARD_RA2A1_EK
static uint16_t g_prev_adc_data;
#endif

/* Private function declarations */
/* Open the ADC module, configure and initiate the scan */
static fsp_err_t adc_scan_start(void);

/* Stops the ADC scan if the ADC is continuous scan or sequential mode, then close the module */
static fsp_err_t adc_scan_stop(void);

#if (BSP_PERIPHERAL_ADC_PRESENT)
/* Callback to handle window compare event */
void adc_callback(adc_callback_args_t * p_args);
#endif

#ifdef BOARD_RA2A1_EK
static fsp_err_t adc_deviation_in_output(void);

static fsp_err_t adc_start_calibration(void);
#endif

/*******************************************************************************************************************//**
 * @brief       This function reads the command (input) from RTT and process the command(input).
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful, action of ADC start or ADC stop takes place
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful start or stop
 **********************************************************************************************************************/
fsp_err_t read_process_input_from_RTT(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    uint8_t readBuff[BUFF_SIZE] =  { RESET_VALUE };

    /* Read user input */
    uint32_t rByte = APP_READ(readBuff);

    /* Function returns the converted integral number as an integer value */
    int32_t inputRead = atoi((char *)readBuff);

    if (rByte > RESET_VALUE)
    {
        switch (inputRead)
        {
            case SCAN_START:
            {
               err = adc_scan_start();
            }
            break;
            case SCAN_STOP:
            {
               err = adc_scan_stop();
            }
            break;
            default:
            {
                /* Menu for User Selection */
                APP_PRINT(MENUOPTION1);
                APP_PRINT(MENUOPTION2);
                APP_PRINT(MENUOPTION3);
                APP_PRINT(MENUOPTION4);
            }
            break;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function open the ADC, configures and starts the scan
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful open,configure ADC, on success or calibrate and start ADC scan
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful open or configure or start
 **********************************************************************************************************************/
static fsp_err_t adc_scan_start(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */
#if (BSP_PERIPHERAL_ADC_PRESENT)
    g_window_comp_event = false;
#endif

    if (false == g_ready_to_read)
    {
        /* Open/Initialize ADC module */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
        err = R_ADC_D_Open (&g_adc_ctrl, &g_adc_cfg);
#else
        err = R_ADC_Open (&g_adc_ctrl, &g_adc_cfg);
#endif
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_"ADC_TYPE"_Open API failed ** \r\n");
            return err;
        }
#ifdef BOARD_RA2A1_EK
        /* Set Reference Voltage Circuit Control register */
        R_ADC0->VREFAMPCNT |= ((VREFADCG_VALUE << SHIFT_BY_ONE) | (VREFADCG_ENABLE << SHIFT_BY_THREE));

        /* Calibrate the ADC */
        err = adc_start_calibration();

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** adc_start_calibration function failed ** \r\n");
            return err;
        }
#endif

        /* Configures the ADC scan parameters */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
        err = R_ADC_D_ScanCfg (&g_adc_ctrl, &g_adc_channel_cfg);
#else
        err = R_ADC_ScanCfg (&g_adc_ctrl, &g_adc_channel_cfg);
#endif
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_"ADC_TYPE"_ScanCfg API failed ** \r\n");
            return err;
        }

        /* Start the ADC scan */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
        err = R_ADC_D_ScanStart (&g_adc_ctrl);
#else
        err = R_ADC_ScanStart (&g_adc_ctrl);
#endif
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_"ADC_TYPE"_ScanStart API failed ** \r\n");
            return err;
        }
        /* Disable interrupts */
        R_BSP_IrqDisable((IRQn_Type)ADC_EVENT_SCAN_COMPLETE);

        APP_PRINT("\r\nADC Started Scan\r\n");

        /* Indication to start reading the ADC data */
        g_ready_to_read = true;
    }
    else
    {
        APP_PRINT("\r\nADC Scan already in progress\r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function stops the scanning of ADC
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful stops the ADC scan and closes the ADC
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful stop or close of ADC
 **********************************************************************************************************************/
static fsp_err_t adc_scan_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

#if (BSP_PERIPHERAL_ADC_D_PRESENT)
    /* Stop the scan if ADC scan is started in sequential mode else ignore */
    if((ADC_D_CONVERSION_MODE_ONESHOT != g_adc_cfg_extend.conversion_operation) && (true == g_ready_to_read ))
    {
        /* Stop ADC scan */
        err = R_ADC_D_ScanStop (&g_adc_ctrl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_D_ScanStop API failed ** \r\n");
            return err;
        }
        APP_PRINT("\r\nADC Scan stopped\r\n");

        /* Reset to indicate stop reading the ADC data */
        g_ready_to_read = false;

        /* Close the ADC module */
        err = R_ADC_D_Close (&g_adc_ctrl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_D_Close API failed ** \r\n");
            return err;
        }
    }
    else
    {
        APP_PRINT("\r\nStop command is not supported in Single Scan mode or the user does not press"\
                  " Start Scan in Continuous mode \r\n");
    }
    APP_PRINT("\r\nPress any other key (except 1 and 2) to go back to the main menu\r\n");
    return err;
#else
    /* Stop the scan if ADC scan is started in continuous scan mode else ignore */
    if((ADC_MODE_SINGLE_SCAN != g_adc_cfg.mode) && (true == g_ready_to_read ))
    {
        /* Stop ADC scan */
        err = R_ADC_ScanStop (&g_adc_ctrl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_ScanStop API failed ** \r\n");
            return err;
        }
        APP_PRINT("\r\nADC Scan stopped\r\n");

        /* Reset to indicate stop reading the ADC data */
        g_ready_to_read = false;

        /* Close the ADC module */
        err = R_ADC_Close (&g_adc_ctrl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_Close API failed ** \r\n");
            return err;
        }
    }
    else
    {
        APP_PRINT("\r\nStop command is not supported in Single Scan mode or the user does not press"\
                  " Start Scan in Continuous mode \r\n");
    }
    APP_PRINT("\r\nPress any other key (except 1 and 2) to go back to the main menu\r\n");
    return err;
#endif
}

/*******************************************************************************************************************//**
 * @brief       This function reads the ADC output data from the prescribed channel and checks ADC status
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful stops the ADC scan and closes the ADC
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful stop or close of ADC
 **********************************************************************************************************************/
fsp_err_t adc_read_data(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    /* Read the result */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
    err = R_ADC_D_Read (&g_adc_ctrl, ADC_CHANNEL_1, &g_adc_data);
#else
    err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_2, &g_adc_data);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("** R_"ADC_TYPE"_Read API failed ** \r\n");
        return err;
    }

#ifdef BOARD_RA2A1_EK
    {
        g_adc_volt = (float)((g_adc_data * V_REF)/ADC_16_BIT);
    }
#else
    {
        g_adc_volt = (float)((g_adc_data * V_REF)/ADC_12_BIT);
    }
#endif

    snprintf((char *)g_volt_str, sizeof(g_volt_str), "%0.2f", g_adc_volt);

    APP_PRINT("\r\nThe Voltage Reading from ADC: %d\r\n", g_adc_data);
    APP_PRINT("\r\nThe ADC input voltage: %s\r\n", g_volt_str);

#if (BSP_PERIPHERAL_ADC_D_PRESENT)
    /* In ADC one-shot mode after reading the data, it stops. So reset the g_ready_to_read state to
     * avoid reading unnecessarily. Close the ADC module as it gets opened in start scan command. */
    if (ADC_D_CONVERSION_MODE_ONESHOT == g_adc_cfg_extend.conversion_operation)
    {
        g_ready_to_read = false;

        /* Stop ADC scan */
        err = R_ADC_D_ScanStop (&g_adc_ctrl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {   /* ADC ScanStop message */
            APP_ERR_PRINT("** R_ADC_D_ScanStop API failed ** \r\n");
            APP_ERR_TRAP(err);
        }
#else
     /* In adc single scan mode after reading the data, it stops. So reset the g_ready_to_read state to
      * avoid reading unnecessarily. Close the ADC module as it gets opened in start scan command. */
     if (ADC_MODE_SINGLE_SCAN == g_adc_cfg.mode || g_window_comp_event == true)
     {
        g_ready_to_read = false;

        /* Stop ADC scan */
        err = R_ADC_ScanStop (&g_adc_ctrl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {   /* ADC ScanStop message */
            APP_ERR_PRINT("** R_ADC_ScanStop API failed ** \r\n");
            APP_ERR_TRAP(err);
        }

        if(((g_window_comp_event == true)&&(ADC_MODE_SINGLE_SCAN == g_adc_cfg.mode))||\
            (ADC_MODE_CONTINUOUS_SCAN == g_adc_cfg.mode))
        {

            /* Print temperature status warning to RTT Viewer */
            if(ADC_L_LMT > g_adc_data)
            {
                APP_PRINT("\r\nADC Voltage is below the Lower Limit.\r\n");
            }
            else if(ADC_H_LMT < g_adc_data)
            {
                APP_PRINT("\r\nADC Voltage is above the Upper Limit.\r\n");
            }
        }
#endif

        /* Close the ADC module */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
        err = R_ADC_D_Close (&g_adc_ctrl);
#else
        err = R_ADC_Close (&g_adc_ctrl);
#endif
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_"ADC_TYPE"_Close API failed ** \r\n");
            return err;
        }
        APP_PRINT("\r\nPress any other key(except 1 and 2) to go back to the main menu\r\n");
    }

    /* 1 second wait time between successive readings */
    R_BSP_SoftwareDelay (ADC_READ_DELAY, BSP_DELAY_UNITS_SECONDS);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Close the ADC driver and Handle the return closing API error, to the Application.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void deinit_adc_module(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close the ADC driver */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
    err = R_ADC_D_Close (&g_adc_ctrl);
#else
    err = R_ADC_Close (&g_adc_ctrl);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Close failure message */
        APP_ERR_PRINT("** R_"ADC_TYPE"_Close API failed **  \r\n");
    }
}

#ifdef BOARD_RA2A1_EK
/*******************************************************************************************************************//**
 * @brief       This function initiates the calibration
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful of ADC calibration
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful calibration failure
 **********************************************************************************************************************/
static fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */
    adc_status_t adc_status;         /* To get ADC status */

    /* Initiate ADC calibration */
    err = R_ADC_Calibrate (&g_adc_ctrl, NULL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("** R_ADC_Calibrate API failed ** \r\n");
        return err;
    }

    APP_PRINT("\r\nADC Calibration Started \r\n");

    do
    {
        /* To get the ADC status */
        err = R_ADC_StatusGet (&g_adc_ctrl, &adc_status);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC status Failure message */
            APP_ERR_PRINT("** R_ADC_StatusGet API failed ** \r\n");
            return err;
        }
        /* Wait here till the calibration. It takes 24msec to 780msec based on clock */
    } while (ADC_STATE_IDLE != adc_status.state);

    APP_PRINT("\r\nADC Calibration Successful\r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function checks the deviation between previously measured ADC value and current value. If the
 *              device is observed continuously and exceeds set limit, triggers calibration again
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful stops the ADC scan and closes the ADC
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful stop or close of ADC
 **********************************************************************************************************************/
static fsp_err_t adc_deviation_in_output(void)
{
    static uint8_t count = RESET_VALUE;
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    if (RESET_VALUE != g_prev_adc_data)
    {
        if (((g_prev_adc_data > g_adc_data) && (g_prev_adc_data - g_adc_data > TEMPERATURE_DEVIATION_LIMIT))
                || ((g_prev_adc_data < g_adc_data) && (g_adc_data - g_prev_adc_data > TEMPERATURE_DEVIATION_LIMIT)))
        {
            /* Increment deviation count */
            count++;
            if (TEMPERATURE_DEVIATION_LIMIT < count)
            {
                /* Reset the error count */
                count = RESET_VALUE;

                APP_PRINT("\r\nInitiating calibration of ADC due to deviation in output"\
                          " continuously beyond set limits\r\n");

                /* Stop the scan to initiate calibration */
                err = adc_scan_stop ();
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    /* ADC Failure message */
                    APP_ERR_PRINT("** adc_scan_stop function failed ** \r\n");
                    return err;
                }

                /* Calibrate the ADC again */
                err = adc_start_calibration ();
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    /* ADC Failure message */
                    APP_ERR_PRINT("** adc_start_calibration function failed ** \r\n");
                    return err;
                }

                /* Clear the current reading */
                g_adc_data = RESET_VALUE;
            }
        }
        else
        {
            count = RESET_VALUE;
        }
    }
    return err;
}
#endif

#if (BSP_PERIPHERAL_ADC_PRESENT)
/* Callback procedure for when window A compare event occurs */
void adc_callback(adc_callback_args_t * p_args)
{
    if(ADC_EVENT_WINDOW_COMPARE_A == p_args->event)
    {
        g_window_comp_event = true;
        IRQn_Type irq = R_FSP_CurrentIrqGet();
        R_BSP_IrqDisable(irq);
    }
}
#endif
/*******************************************************************************************************************//**
 * @} (end addtogroup adc_ep)
 **********************************************************************************************************************/

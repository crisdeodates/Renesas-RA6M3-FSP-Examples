/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup r_acmphs_ep
 * @{
 **********************************************************************************************************************/

/* Macro definitions */
#if BSP_PERIPHERAL_DAC_PRESENT
#define DAC_MAX_VAL          (4095U)		/* Max count value for 12 bit DAC */
#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
#define LOW_VAL              (1400U)        /* Lower value of fluctuating range */
#define HIGH_VAL             (1600U)        /* Higher value of fluctuating range */
#define V_REF                (3.3f)         /* Reference Voltage */
#else
#define DAC_REF_VAL          (2048U)        /* Mid count value for 12 bit DAC */
#define LOW_VAL              (2000U)        /* Lower value of fluctuating range */
#define HIGH_VAL             (2100U)        /* Higher value of fluctuating range */
#endif /* Specific DAC value for each board */
#endif /* BSP_PERIPHERAL_DAC_PRESENT */
#define CHANNEL_ZERO		 (0U)			/* Channel 0 of ACMPHS */
#define INITIAL_VALUE        ('\0')

/* Board specific LED */
#if defined (BOARD_RA6M2_EK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK)
#define LED_PIN			(BSP_IO_PORT_01_PIN_06)
#elif defined (BOARD_RA6T2_MCK)
#define LED_PIN         (BSP_IO_PORT_13_PIN_01)
#elif defined (BOARD_RA6M1_EK)
#define LED_PIN			(BSP_IO_PORT_01_PIN_12)
#elif defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA8E1_FPB)
#define LED_PIN         (BSP_IO_PORT_04_PIN_08)
#elif defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define LED_PIN 		(BSP_IO_PORT_01_PIN_00)
#elif defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
#define LED_PIN         (BSP_IO_PORT_01_PIN_07)
#elif defined (BOARD_RA8T1_MCK)
#define LED_PIN         (BSP_IO_PORT_10_PIN_12)
#elif defined (BOARD_RA8E2_EK)
#define LED_PIN         (BSP_IO_PORT_04_PIN_05)
#elif defined (BOARD_RA2T1_FPB)
#define LED_PIN         (BSP_IO_PORT_02_PIN_13)
#endif /* Specific LED for each board */

/* Global variable */
static volatile bool b_comparator_state_flag = true;

/* Private functions */
static void deinit_acmphs(void);
#if BSP_PERIPHERAL_DAC_PRESENT
static void deinit_dac(dac_instance_ctrl_t * p_ctrl);
#endif
static void clean_up(void);

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used. This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err 					 		= FSP_SUCCESS;
    unsigned char rByte[BUFFER_SIZE_DOWN]	= {RESET_VALUE};
    uint8_t num_bytes 				 		= RESET_VALUE;
    comparator_status_t acmphs_status       = {RESET_VALUE};
#if BSP_PERIPHERAL_DAC_PRESENT
    uint16_t dac_val			 	 		= RESET_VALUE;
#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
    uint8_t  volt_str[5]                    = {RESET_VALUE};
    float    dac_volt                       = {RESET_VALUE};
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT */
    fsp_pack_version_t version 		 		= {RESET_VALUE};
    comparator_info_t stabilize_time 		= {RESET_VALUE};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Project information */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3, EP_VERSION);
    APP_PRINT(BANNER_4, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);

#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
    APP_PRINT("\r\n In this project Internal Reference Voltage (Vref) is used as reference voltage source"
              "\r\n and AN000 is used as input voltage source for ACMPHS module, which is supplied by output of DAC0."
              "\r\n Internal Reference Voltage value is set to 1.18V (Equivalent to a digital value 1465)."
              "\r\n The user can enter DAC0 value within permitted range to provide an analog signal input for AN000."
              "\r\n When input voltage source is greater than reference voltage, Comparator output status is HIGH"
              "\r\n and on-board LED is turned ON. When input voltage source is less than reference voltage,"
              "\r\n output status is LOW and the LED is turned OFF.\r\n");
#elif defined (BOARD_RA2T1_FPB)
    APP_PRINT("\r\n In this project Internal Reference Voltage (Vref) is used as reference voltage source"
              "\r\n and CMPIN01 is used as input voltage source for ACMPHS module, which is supplied"
              "\r\n by external DC power supply. Internal Reference Voltage value is set to 1.44V."
              "\r\n The user can input DC power value within permitted range to provide an analog signal input for"
              "\r\n CMPIN01. When input voltage source is greater than reference voltage, Comparator output status"
              "\r\n is HIGH and on-board LED is turned ON. When input voltage source is less than reference voltage,"
              "\r\n output status is LOW and the LED is turned OFF.\r\n");
#else
    APP_PRINT("\r\n In this project DAC0 is used as reference voltage source and DAC1 is used as input"
              "\r\n voltage source for ACMPHS module. DAC0 value is set to 2048 (i.e. 1.65V)."
              "\r\n The user can enter DAC1 value within permitted range. When DAC1 input value is greater"
              "\r\n than set DAC0 reference voltage, Comparator output status is HIGH and on-board LED "
              "\r\n is turned ON. When DAC1 input is less than reference voltage, Output status is LOW "
              "\r\n and the LED is turned OFF.\r\n");
#endif

#if BSP_PERIPHERAL_DAC_PRESENT
    /* Open DAC0 module */
    err = R_DAC_Open(&g_dac0_ctrl, &g_dac0_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Open API FAILED FOR DAC0 ** \r\n");
        APP_ERR_TRAP(err);
    }

#if !(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
    /* Open DAC1 module */
    err = R_DAC_Open(&g_dac1_ctrl, &g_dac1_cfg);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Open API FAILED FOR DAC1 ** \r\n");
        deinit_dac(&g_dac0_ctrl);
        APP_ERR_TRAP(err);
    }
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT */

    /* Open ACMPHS module */
#if (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
    err  = R_ACMPHS_B_Open(&g_comparator_ctrl, &g_comparator_cfg);
#else
    err  = R_ACMPHS_Open(&g_comparator_ctrl, &g_comparator_cfg);
#endif /* BSP_PERIPHERAL_ACMPHS_B_PRESENT */
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_"ACMPHS_TYPE"_Open API FAILED ** \r\n");
#if BSP_PERIPHERAL_DAC_PRESENT
        deinit_dac(&g_dac0_ctrl);
#if !(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
        deinit_dac(&g_dac1_ctrl);
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT */
        APP_ERR_TRAP(err);
    }

#if BSP_PERIPHERAL_DAC_PRESENT
#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
    /* Write 0 on DAC0 module for providing analog input voltage to comparator
     * and avoid garbage to ACMPHS input */
    err = R_DAC_Write(&g_dac0_ctrl, RESET_VALUE);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Write API FAILED FOR DAC0 ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }
#else
    /* Write 2048 (≈1.65V) on DAC0 module for reference voltage to comparator */
    err = R_DAC_Write(&g_dac0_ctrl, DAC_REF_VAL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Write API FAILED FOR DAC0 ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }

    /* Write 0 on DAC1 module for providing analog input voltage to comparator
     * and avoid garbage to ACMPHS input */
    err = R_DAC_Write(&g_dac1_ctrl, RESET_VALUE);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Write API FAILED FOR DAC1 ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
    /* Start conversion on DAC0 module */
    err = R_DAC_Start(&g_dac0_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Start API FAILED FOR DAC0 ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }

    /* Start conversion on DAC1 module */
#if !(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
    err = R_DAC_Start(&g_dac1_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_DAC_Start API FAILED FOR DAC1 ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT */

    /* Get the minimum stabilization wait time */
#if (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
    err = R_ACMPHS_B_InfoGet(&g_comparator_ctrl, &stabilize_time);
#else
    err = R_ACMPHS_InfoGet(&g_comparator_ctrl, &stabilize_time);
#endif /* BSP_PERIPHERAL_ACMPHS_B_PRESENT */
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_"ACMPHS_TYPE"_InfoGet API FAILED ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }

    /* Enable the comparator output */
#if (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
    err = R_ACMPHS_B_OutputEnable(&g_comparator_ctrl);
#else
    err = R_ACMPHS_OutputEnable(&g_comparator_ctrl);
#endif /* BSP_PERIPHERAL_ACMPHS_B_PRESENT */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_"ACMPHS_TYPE"_OutputEnable API FAILED ** \r\n");
        clean_up();
        APP_ERR_TRAP(err);
    }

    while(true)
    {
        num_bytes = RESET_VALUE;
#if BSP_PERIPHERAL_DAC_PRESENT
        dac_val   = RESET_VALUE;
#endif /* BSP_PERIPHERAL_DAC_PRESENT */
        /* Reset buffer */
        memset(rByte,INITIAL_VALUE,sizeof(rByte));

#if BSP_PERIPHERAL_DAC_PRESENT
#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
        APP_PRINT(" \r\n Enter the DAC0 Value between (0 - 1400) or (1600 - 4095) to compare:");
#else
        APP_PRINT(" \r\n Enter the DAC1 Value between (0 - 2000) or (2100 - 4095) to compare:");
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#else
        APP_PRINT(" \r\n Enter any key to perform compare operation:");
#endif /* BSP_PERIPHERAL_DAC_PRESENT */

        while (RESET_VALUE == num_bytes)
        {
            if (APP_CHECK_DATA)
            {
                /* Take User input */
                num_bytes = (uint8_t)APP_READ(rByte);
                if (RESET_VALUE == num_bytes)
                {
                    APP_PRINT(" \r\nInvalid Input\r\n");
                }
            }
        }

#if BSP_PERIPHERAL_DAC_PRESENT
        /* Conversion from input string to integer value */
        dac_val =  (uint16_t) (atoi((char *)rByte));

        APP_PRINT("\r\n Input DAC value: %d\r\n", dac_val);

        /* Check for out of range value */
        if (DAC_MAX_VAL < dac_val)
        {
            APP_ERR_PRINT(" \r\n ** INVALID INPUT, DAC VALUE IS OUT OF RANGE(0 - 4095) ** \r\n");
        }

        /* Check for DAC input range to avoid fluctuating voltage caused due to
         * electrical noise which may alter the comparator output */
        else if (LOW_VAL < dac_val && dac_val < HIGH_VAL)
        {
            APP_ERR_PRINT(" \r\n ** Fluctuating voltage range ** \r\n");
        }
        else
        {
#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
            /* Write to DAC0 channel */
            err = R_DAC_Write(&g_dac0_ctrl, dac_val);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT ("\r\n ** R_DAC_Write API FAILED FOR DAC0 ** \r\n");
                clean_up();
                APP_ERR_TRAP(err);
            }

            /* Conversion from digital value to analog value */
            dac_volt = (float)((dac_val * V_REF)/(DAC_MAX_VAL+1));
            snprintf((char *)volt_str, sizeof(volt_str), "%0.2f", dac_volt);
            APP_PRINT("\r\n Input voltage source for ACMPHS module: %sV", volt_str);
#else
            /* Write to DAC1 channel */
            err = R_DAC_Write(&g_dac1_ctrl, dac_val);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT ("\r\n ** R_DAC_Write API FAILED FOR DAC1 ** \r\n");
                clean_up();
                APP_ERR_TRAP(err);
            }
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT */

            /* Wait for the minimum stabilization wait time */
            R_BSP_SoftwareDelay(stabilize_time.min_stabilization_wait_us, BSP_DELAY_UNITS_MICROSECONDS);

            if (b_comparator_state_flag == true)
            {
                /* Check status of comparator */
#if (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
                err = R_ACMPHS_B_StatusGet(&g_comparator_ctrl, &acmphs_status);
#else
                err = R_ACMPHS_StatusGet(&g_comparator_ctrl, &acmphs_status);
#endif /* BSP_PERIPHERAL_ACMPHS_B_PRESENT */
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT ("\r\n ** R_"ACMPHS_TYPE"_StatusGet API FAILED ** \r\n");
                    clean_up();
                    APP_ERR_TRAP(err);
                }
                /* Clear flag */
                b_comparator_state_flag = false;
            }
            else
            {
                /* Do nothing */
            }

            /* Check if status of comparator is high */
            if (COMPARATOR_STATE_OUTPUT_HIGH == acmphs_status.state)
            {
                APP_PRINT("\r\n Comparator output high \r\n");
#if defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK)\
            || defined (BOARD_RA8T1_MCK)
                err = R_IOPORT_PinWrite(&g_ioport_ctrl, LED_PIN, BSP_IO_LEVEL_LOW);
#else
                err = R_IOPORT_PinWrite(&g_ioport_ctrl, LED_PIN, BSP_IO_LEVEL_HIGH);
#endif

                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT ("\r\n ** R_IOPORT_PinWrite API FAILED ** \r\n");
                    clean_up();
                    APP_ERR_TRAP(err);
                }
            }
            else
            {
                APP_PRINT("\r\n Comparator output low \r\n");
#if defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK)\
            || defined (BOARD_RA8T1_MCK)
                err = R_IOPORT_PinWrite(&g_ioport_ctrl, LED_PIN, BSP_IO_LEVEL_HIGH);
#else
                err = R_IOPORT_PinWrite(&g_ioport_ctrl, LED_PIN, BSP_IO_LEVEL_LOW);
#endif
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT ("\r\n ** R_IOPORT_PinWrite API FAILED ** \r\n");
                    clean_up();
                    APP_ERR_TRAP(err);
                }
            }
#if BSP_PERIPHERAL_DAC_PRESENT
        }
#endif /* BSP_PERIPHERAL_DAC_PRESENT */
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

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take
         * more than 6us. */
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

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build
 * (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
FSP_CPP_FOOTER

#endif

/*****************************************************************************************************************
 *  @brief      Close ACMPHS module.
 *  @param[in]  None
 *  @retval     None
 ****************************************************************************************************************/
static void deinit_acmphs(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close ACMPHS module */
#if (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
    err = R_ACMPHS_B_Close(&g_comparator_ctrl);
#else
    err = R_ACMPHS_Close(&g_comparator_ctrl);
#endif /* BSP_PERIPHERAL_ACMPHS_B_PRESENT */
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** R_"ACMPHS_TYPE"_Close API FAILED ** \r\n");
    }
}

#if BSP_PERIPHERAL_DAC_PRESENT
/*****************************************************************************************************************
 *  @brief      Close corresponding DAC module.
 *  @param[in]  p_ctrl		Pointer to dac_instance_ctrl_t
 *  @retval     None
 ****************************************************************************************************************/
static void deinit_dac(dac_instance_ctrl_t * p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close DAC */
    err = R_DAC_Close(p_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** R_DAC_Close API FAILED ** \r\n");
    }
}
#endif /* BSP_PERIPHERAL_DAC_PRESENT */

/*****************************************************************************************************************
 *  @brief      Close all modules.
 *  @param[in] 	None
 *  @retval     None
 ****************************************************************************************************************/
static void clean_up(void)
{
#if BSP_PERIPHERAL_DAC_PRESENT
    deinit_dac(&g_dac0_ctrl);
#if !(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
    deinit_dac(&g_dac1_ctrl);
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT */
    deinit_acmphs();
}

/*****************************************************************************************************************
 *  @brief      User defined callback
 *  @param[in]  p_args
 *  @retval     None
 ****************************************************************************************************************/
void acmphs_user_callback(comparator_callback_args_t *p_args)
{
    /* Check for the channel 0 of comparator */
    if(CHANNEL_ZERO == p_args->channel)
    {
        /* Toggle the flag */
        b_comparator_state_flag = true;
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup acmphs_ep)
 **********************************************************************************************************************/

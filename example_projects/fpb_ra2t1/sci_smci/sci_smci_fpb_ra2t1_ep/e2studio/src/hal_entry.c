/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include "common_utils.h"
#include "iso7816_3_main.h"
#include "iso7816_T0_protocol.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

#define BUF_SIZE                 (16U)           /* Size of buffer for RTT input data */
#define INITIAL_VALUE            ('\0')
#define T0_CASE_3_SELECT_MF      (1U)
#define T0_CASE_2_GET_CHALLENGE  (2U)
#define DEACTIVATE_SMC_DEVICE    (3U)

static smci_apdu_commands_t t0_apdu_command;
static smci_apdu_response_t t0_apdu_response;

static const uint8_t MF_Identifier[2] = {0x3F, 0x00};

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    /* TODO: add your own code here */
    fsp_pack_version_t version              = {RESET_VALUE};
    fsp_err_t err;
    unsigned char rByte[BUF_SIZE]           = {INITIAL_VALUE};
    uint32_t read_data                      = RESET_VALUE;
    uint16_t status_word = 0;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    err = Smart_Card_Interface_Init();
    assert(err == FSP_SUCCESS);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3,EP_VERSION);
    APP_PRINT(BANNER_4,version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch );
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);
    APP_PRINT(EP_INFO1);
    APP_PRINT(EP_INFO2);
    APP_PRINT(EP_INFO3);

    APP_PRINT("Activating Smart Card Interface...\r\n");
    Smart_Card_Interface_Handler(SMC_DEVICE_ACTIVE);
    APP_PRINT(EP_MENU);

    while(1)
    {
        /* Process input only when the user has provided one */
        if (APP_CHECK_DATA)
        {
            /* Read First byte of data provided by the user */
            APP_READ(rByte);

            /* Conversion from input string to integer value */
            read_data = (uint32_t) (atoi((char *)rByte));

            switch (read_data)
            {
                case T0_CASE_3_SELECT_MF:
                    /* Perform TPDU Case 3 - Select MF */
                    APP_PRINT("Transportation of APDU messages testing by T=0\r\n");
                    APP_PRINT("Case 3 : Select MF\r\n");
                    memset(&t0_apdu_command, 0x00, sizeof(smci_apdu_commands_t));
                    t0_apdu_command.Header.CLA = 0x00;
                    t0_apdu_command.Header.INS = 0xA4;
                    t0_apdu_command.Header.Lc = 0x02;
                    memcpy(t0_apdu_command.Body, MF_Identifier, t0_apdu_command.Header.Lc);
                    err = ISO7816_T0_SEND_APDU_CASE3(&t0_apdu_command, &status_word);
                    assert(err == FSP_SUCCESS);

                    if(status_word == 0x6d00)
                    {
                        APP_PRINT("Instruction code not supported or invalid\r\n");
                    }

                    if(status_word == 0x6e00)
                    {
                       APP_PRINT("Response TPDU: %x Class not support\r\n");
                    }
                    else if(status_word != 0x9000)
                    {
                        APP_PRINT("Command successfully executed (OK).\r\n");
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    APP_PRINT(EP_MENU);
                    break;

                case T0_CASE_2_GET_CHALLENGE:
                    /* Print message to request user to customize .CLA, .INS, .Le fields to match the
                     * card being used. And remove the break statement on line 117. */
                    APP_PRINT("Note: Users have to customize .CLA, .INS, .Le field to match with the card "\
                              "being used.\r\n");
                    APP_PRINT("      And remove the break statement on the line 117 of hal_entry.c.\r\n\n");
                    APP_PRINT(EP_MENU);
                    break;
                    /* Perform TPDU Case 2 - Get Challenge */
                    APP_PRINT("Transportation of APDU messages testing by T=0\r\n");
                    APP_PRINT("Case 2: Get Challenge\r\n");
                    memset(&t0_apdu_command, 0x00, sizeof(smci_apdu_commands_t));
                    memset(&t0_apdu_response, 0x00, sizeof(smci_apdu_response_t));
                    t0_apdu_command.Header.CLA = 0x00;
                    t0_apdu_command.Header.INS = 0x84;
                    t0_apdu_command.Header.Le = 0x22;
                    err = ISO7816_T0_SEND_APDU_CASE2(&t0_apdu_command.Header, &t0_apdu_response);
                    assert(err == FSP_SUCCESS);
                    status_word = (uint16_t)(t0_apdu_response.status_bytes[0]<<8 | t0_apdu_response.status_bytes[1]);

                    if(status_word == 0x6d00)
                    {
                        APP_PRINT("Instruction code not supported or invalid\r\n");
                    }

                    if(status_word == 0x6e00)
                    {
                        APP_PRINT("Response TPDU: %x Class not support\r\n");
                    }
                    else if(status_word == 0x9000)
                    {
                        APP_PRINT("Command successfully executed (OK).\r\n");
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    APP_PRINT(EP_MENU);
                    break;

                case DEACTIVATE_SMC_DEVICE:
                    Smart_Card_Interface_Handler(SMC_DEVICE_DEACTIVE);
                    APP_PRINT("Activation will automatically start when smart card device is ready again\r\n");
                    break;

                default:
                    APP_PRINT(EP_MENU);
                    break;
            }
        }

        Smart_Card_Interface_Handler(REGULAR_POOLING);
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

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if
 * it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
FSP_CPP_FOOTER

#endif

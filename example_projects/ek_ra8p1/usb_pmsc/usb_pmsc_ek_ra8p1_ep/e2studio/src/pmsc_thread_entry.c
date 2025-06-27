/***********************************************************************************************************************
 * File Name    : pmsc_thread_entry.c
 * Description  : Contains data structures and functions used in pmsc_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "pmsc_thread.h"
#include "usb_pmsc.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_pmsc_ep
 * @{
 **********************************************************************************************************************/

/* Global variables */
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t *gp_apl_string_table[];

const usb_descriptor_t g_usb_descriptor =
{
 g_apl_device,                   /* Pointer to the device descriptor */
 g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
 g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
 g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
 gp_apl_string_table,            /* Pointer to the string descriptor table */
 NUM_STRING_DESCRIPTOR
};

/* Event flag to identify the USB event */
static volatile usb_event_info_t *p_usb_event  = NULL;

/* PMSC thread entry function */
/* pvParameters contains TaskHandle_t */
void pmsc_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    volatile fsp_err_t err = FSP_SUCCESS;

    fsp_pack_version_t version = {RESET_VALUE};

    /* Get FSP version information using the version API */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initialize USB module in PMSC (Peripheral Mass Storage Class) mode */
    err = R_USB_Open(&g_basic_ctrl, &g_basic_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nFailed to initialize USB PMSC\r\n");
        APP_ERR_TRAP (err);
    }

    while (true)
    {
        BaseType_t err_queue = pdFALSE;

        /* Wait to receive a USB event from the queue */
        err_queue = xQueueReceive(g_event_queue, &p_usb_event, (portMAX_DELAY));
        if(pdTRUE != err_queue)
        {
            APP_ERR_PRINT("\r\nNo USB event received. Please check the USB connection\r\n");
        }

        /* Handle the received USB event */
        switch(p_usb_event->event)
        {
            case USB_STATUS_CONFIGURED:
            {
                APP_PRINT("USB configured successfully\r\n");
                break;
            }
            case USB_STATUS_DETACH:
            case USB_STATUS_SUSPEND:
            {
                APP_PRINT("USB removed successfully\r\n");
                break;
            }
            default:
            {
                break;
            }
        }

        /* Add delay so that while loop won't get optimized out */
        vTaskDelay(1);
    }
}

/*******************************************************************************************************************//**
 * @brief       Callback function for FreeRTOS+PMSC triggered when USB is inserted or removed.
 * @param[IN]   p_event_info    Pointer to the USB event information structure.
 * @param[IN]   handler         USB handler (unused).
 * @param[IN]   on_off          USB on/off status (unused).
 * @retval      None
 **********************************************************************************************************************/
void pmsc_freertos_callback(usb_event_info_t * p_event_info, usb_hdl_t handler, usb_onoff_t on_off)
{

    FSP_PARAMETER_NOT_USED (handler);
    FSP_PARAMETER_NOT_USED (on_off);
    xQueueSendFromISR(g_event_queue, (const void *)&p_event_info, NULL);
}

/*******************************************************************************************************************//**
 * @brief       De-initialize the USB module by closing it via HAL API. Errors are handled internally with proper
 *              messages. Application is responsible for further handling.
 * @param       None
 * @retval      None
 **********************************************************************************************************************/
void deinit_usb(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close the opened USB module */
    err = R_USB_Close(&g_basic_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("** R_USB_Close API failed **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_pmsc_ep)
 **********************************************************************************************************************/

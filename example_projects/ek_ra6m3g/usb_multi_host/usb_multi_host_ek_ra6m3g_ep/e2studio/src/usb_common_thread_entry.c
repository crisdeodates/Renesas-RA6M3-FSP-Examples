/***********************************************************************************************************************
 * File Name    : usb_common_thread_entry.c.
 * Version      : .
 * Description  : Manage USB common operations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <usb_common_thread.h>
#include "usb_hcdc_thread.h"
#include "usb_hcdc_hmsc_app.h"

/**********************************************************************************************************************
 Private variables and functions
 *********************************************************************************************************************/
static void usb_app_common_callback(usb_event_info_t *p_event_info, usb_hdl_t cur_task, usb_onoff_t usb_state);
static void usb_manage_task_state(eTaskState hcdc_state_change, eTaskState hmsc_state_change);
static usb_callback_t *g_usb_host_apl_callback[NUM_OF_APP_CALLBACK];
static EventBits_t g_usb_status;

/**********************************************************************************************************************
 Public variables and functions
 *********************************************************************************************************************/
extern usb_callback_t *g_usb_apl_callback[USB_NUM_USBIP];

/**********************************************************************************************************************
 * @brief     USB Common Thread entry function.
 * @param[IN] pointer to pvParameters
 * @retval    None
 *********************************************************************************************************************/
void usb_common_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    fsp_pack_version_t version = { RESET_VALUE };

    /* version get API for FLEX pack information */
    R_FSP_VersionGet (&version);
    xSemaphoreGive(g_app_print_semaphore);
    xSemaphoreGive(g_user_input_semaphore);

#if (USE_VIRTUAL_COM == 1)
    jlink_console_init ();
#endif /* End of USE_VIRTUAL_COM == 1 */

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initializes lower layer media device */
    init_freertos_plus_fat ();

    /* Assign app Callback */
    g_usb_host_apl_callback[0] = usb_app_hcdc_callback;           // HCDC App Callback
    g_usb_host_apl_callback[1] = g_basic0_cfg.p_usb_apl_callback; // HMSC App Callback
    g_usb_apl_callback[g_basic0_cfg.module_number] = usb_app_common_callback;

    while (true)
    {
        APP_PRINT("\r\n!! Waiting for a device to connect... !!\r\n");

        /* Wait indefinitely for MSC or CDC connection event */
        g_usb_status = xEventGroupWaitBits (g_usb_event_group,
                                            IS_MSC_DEVICE_CONNECTED | IS_CDC_DEVICE_CONNECTED,
                                            pdFALSE,
                                            pdFALSE,
                                            portMAX_DELAY);

        /* Wait another 200ms for a second MSC or CDC connection event (in case both are connected) */
        g_usb_status = xEventGroupWaitBits (g_usb_event_group,
                                            IS_MSC_DEVICE_CONNECTED | IS_CDC_DEVICE_CONNECTED,
                                            pdFALSE,
                                            pdTRUE,
                                            DELAY_200_TICK);

        /* Check if MSC device connected */
        if (g_usb_status & IS_MSC_DEVICE_CONNECTED)
        {
            /* Check if CDC device connected */
            if (g_usb_status & IS_CDC_DEVICE_CONNECTED)
            {
                /* Both MSC and CDC devices connected */
                /* Resumes the CDC and MSC tasks if they are not already running */
                xEventGroupSetBits (g_usb_event_group, USB_COMPOSITE_DEVICE_ENABLE);
                usb_manage_task_state (eReady, eReady);
            }
            else
            {
                /* Only MSC device connected */
                /* Suspend CDC task and resume the MSC task if they are not already running */
                usb_manage_task_state (eSuspended, eReady);
            }
        }
        else
        {
            /* Suspend MSC task and resume the CDC task if they are not already running */
            usb_manage_task_state (eReady, eSuspended);

        }

        while (check_usb_connection (true, true))
        {
            vTaskDelay (1);
            ;
        }

        vTaskDelay (1);
    }
} /* End of function usb_common_thread_entry() */

/**********************************************************************************************************************
 * @brief       This function checks if the specified USB devices (CDC and/or MSC) are connected.
 * @param[IN]   check_for_cdc_connect Boolean flag to check for a CDC device connection.
 * @param[IN]   check_for_msc_connect Boolean flag to check for a MSC device connection.
 * @retval      true if at least one of the specified devices is connected, false otherwise.
 *********************************************************************************************************************/
bool check_usb_connection(bool check_for_cdc_connect, bool check_for_msc_connect)
{
    EventBits_t usb_status = xEventGroupGetBits(g_usb_event_group);
    EventBits_t check_mask = RESET_VALUE;

    if (check_for_cdc_connect)
    {
        check_mask |= IS_CDC_DEVICE_CONNECTED;
    }

    if (check_for_msc_connect)
    {
        check_mask |= IS_MSC_DEVICE_CONNECTED;
    }

    if (RESET_VALUE != (usb_status & check_mask))
    {
        return true;
    }
    else
    {
        return false;
    }
} /* End of function check_usb_connection() */

/**********************************************************************************************************************
 * @brief       This function handles the management of USB tasks (CDC and MSC threads).
 * @param[IN]   hcdc_state_change    State change for the CDC task (eReady or eSuspended).
 * @param[IN]   hmsc_state_change    State change for the MSC task (eReady or eSuspended).
 * @retval      None.
 *********************************************************************************************************************/
static void usb_manage_task_state(eTaskState hcdc_state_change, eTaskState hmsc_state_change) {

    eTaskState current_hcdc_state;
    eTaskState current_hmsc_state;

    if (usb_hcdc_thread != NULL)
    {
        current_hcdc_state = eTaskGetState (usb_hcdc_thread);

        if (hcdc_state_change == eReady && current_hcdc_state != eReady)
        {
            vTaskResume (usb_hcdc_thread);
        }
        else if (hcdc_state_change == eSuspended && current_hcdc_state != eSuspended)
        {
            vTaskSuspend (usb_hcdc_thread);
        }
    }

    if (usb_hmsc_thread != NULL)
    {
        current_hmsc_state = eTaskGetState (usb_hmsc_thread);

        if (hmsc_state_change == eReady && current_hmsc_state != eReady)
        {
            vTaskResume (usb_hmsc_thread);
        }
        else if (hmsc_state_change == eSuspended && current_hmsc_state != eSuspended)
        {
            vTaskSuspend (usb_hmsc_thread);
        }
    }
} /* End of function usb_manage_task_state() */

/**********************************************************************************************************************
 * @brief       This function is a common callback for FreeRTOS+HCDC and FreeRTOS+FAT when a USB event occurs
 *              from the device.
 * @param[IN]   p_event_info    Triggered event
 * @param[IN]   cur_task        current task handle
 * @param[IN]   usb_state       State of USB
 * @retval      None.
 *********************************************************************************************************************/
static void usb_app_common_callback(usb_event_info_t *p_event_info, usb_hdl_t cur_task, usb_onoff_t usb_state)
{
    FSP_PARAMETER_NOT_USED(cur_task);
    FSP_PARAMETER_NOT_USED(usb_state);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult;

    if (USB_CLASS_REQUEST == p_event_info->type)
    {
        if ((USB_CDC_SET_LINE_CODING == (p_event_info->setup.request_type & USB_BREQUEST))
                || (USB_CDC_GET_LINE_CODING == (p_event_info->setup.request_type & USB_BREQUEST))
                || (USB_CDC_SET_CONTROL_LINE_STATE == (p_event_info->setup.request_type & USB_BREQUEST)))
        {
            g_usb_host_apl_callback[0] (p_event_info, cur_task, usb_state); // HCDC Callback
        }
        else
        {
            g_usb_host_apl_callback[1] (p_event_info, cur_task, usb_state); // HMSC Callback
        }
    }
    else
    {
        if (USB_CLASS_HCDC == p_event_info->type)
        {
            if (USB_STATUS_CONFIGURED == p_event_info->event)
            {
                /* Set the IS_CDC_DEVICE_CONNECTED bit in the g_usb_event_group event group
                 to signal that a USB CDC (Communications Device Class) device has been inserted. */
                xResult = xEventGroupSetBitsFromISR(g_usb_event_group, IS_CDC_DEVICE_CONNECTED, &xHigherPriorityTaskWoken);
                if (xResult != pdFAIL)
                {
                    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                }
            }
            g_usb_host_apl_callback[0] (p_event_info, cur_task, usb_state); // HCDC Callback
        }
        else
        {
            if (USB_STATUS_CONFIGURED == p_event_info->event)
            {
                xResult = xEventGroupSetBitsFromISR(g_usb_event_group, USB_MSC_CMD_COMPLETE, &xHigherPriorityTaskWoken);
                if (xResult != pdFAIL)
                {
                    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                }
            }
            g_usb_host_apl_callback[1] (p_event_info, cur_task, usb_state); // HMSC Callback
        }
    }
} /* End of function usb_app_common_callback() */

/**********************************************************************************************************************
 * End  Of File
 *********************************************************************************************************************/

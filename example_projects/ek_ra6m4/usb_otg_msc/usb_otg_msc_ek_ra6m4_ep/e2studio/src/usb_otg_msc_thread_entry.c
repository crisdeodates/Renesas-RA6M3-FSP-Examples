/***********************************************************************************************************************
* File Name    : usb_otg_msc_thread_entry.c
* Description  : Manages USB OTG MSC operations, including mode switching, device attach/detach events, and FAT thread
*                notifications.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "usb_otg_msc_thread.h"
#include "usb_otg_msc.h"
#include "freertos_plus_fat_internal_data.h"
#include "fat_operation.h"
#include "common_app.h"

/***********************************************************************************************************************
 * Exported Global Variables
 **********************************************************************************************************************/
extern TaskHandle_t usb_otg_msc_thread;
extern TaskHandle_t fat_thread;

volatile bool g_usb_host_active = false;

/***********************************************************************************************************************
 * Private Function Prototypes
 **********************************************************************************************************************/
static uint32_t usb_otg_start_up(void);
static uint32_t usb_otg_change_mode(usb_otg_msc_t * cur_mode, usb_otg_msc_t * target_mode);

usb_otg_msc_t USB_OTG_HOST_MODE =
{
  .ctrl = &g_basic_hmsc_ctrl,
  .cfg = &g_basic_hmsc_cfg,
  .mode = OTG_HOST,
};

usb_otg_msc_t USB_OTG_DEVICE_MODE =
{
  .ctrl = &g_basic_pmsc_ctrl,
  .cfg = &g_basic_pmsc_cfg,
  .mode = OTG_DEVICE,
};

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Callback for the USB OTG ID external interrupt.
 *
 * This function is triggered by an external interrupt on the USB OTG ID pin.
 * It starts a debounce timer to handle the state change.
 *
 * @param[in]  p_args Pointer to the external IRQ callback arguments (not used).
 * @retval     None
 **********************************************************************************************************************/
void external_irq_usb_otg_id_callback(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Start the debounce timer for the USB OTG ID pin */
    xTimerStartFromISR(g_timer_external_irq_usb_otg_debounce, NULL);
}

/*******************************************************************************************************************//**
 * @brief Callback for the USB OTG ID debounce timer.
 *
 * This function reads the state of the USB ID pin after the debounce delay and
 * notifies the appropriate task based on the current USB mode (host or device).
 *
 * @param[in]  xTimer Timer handle (not used).
 * @retval     None
 **********************************************************************************************************************/
void timer_external_irq_usb_otg_debounce_callback(TimerHandle_t xTimer)
{
    FSP_PARAMETER_NOT_USED(xTimer);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Read the current state of the USB OTG ID pin */
    uint32_t pin_state = R_BSP_PinRead(USB_OTG_ID_PIN);

    if (BSP_IO_LEVEL_LOW == pin_state && false == g_usb_host_active)
    {
        /* Notify the USB OTG thread to switch to USB host mode */
        xTaskNotifyFromISR(usb_otg_msc_thread, USB_OTG_CHANGE_TO_HOST, eSetBits, &xHigherPriorityTaskWoken);
    }
    else if (BSP_IO_LEVEL_HIGH == pin_state && true == g_usb_host_active)
    {
        /* Notify the FAT thread to close all disks before switching to USB device mode */
        xTaskNotifyFromISR(fat_thread, FAT_ALL_DISK_REQUEST_CLOSE, eSetBits, &xHigherPriorityTaskWoken);
    }
    else
    {
        __NOP();
    }

    /* Trigger a context switch if a higher priority task was woken */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*******************************************************************************************************************//**
 * @brief Callback for FreeRTOS USB PMSC events.
 *
 * This function handles events related to the USB PMSC (e.g., device configuration or detachment)
 * and notifies the USB OTG thread accordingly.
 *
 * @param[in]  p_event_info Pointer to USB event information.
 * @param[in]  handler      USB handler (not used).
 * @param[in]  on_off       USB on/off status (not used).
 * @retval     None
 **********************************************************************************************************************/
void freertos_basic_usb_pmsc_callback(usb_event_info_t *p_event_info, usb_hdl_t handler, usb_onoff_t on_off)
{
    FSP_PARAMETER_NOT_USED(handler);
    FSP_PARAMETER_NOT_USED(on_off);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Handle USB PMSC events */
    switch (p_event_info->event)
    {
        case USB_STATUS_CONFIGURED:
            /* Notify the USB OTG thread that a device was attached */
            xTaskNotifyFromISR(usb_otg_msc_thread, USB_OTG_DEVICE_ATTACH, eSetBits, &xHigherPriorityTaskWoken);
            break;

        case USB_STATUS_DETACH:
            /* Notify the USB OTG thread that a device was detached */
            xTaskNotifyFromISR(usb_otg_msc_thread, USB_OTG_DEVICE_DETACH, eSetBits, &xHigherPriorityTaskWoken);
            break;

        default:
            break;
    }

    /* Trigger a context switch if a higher priority task was woken */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*******************************************************************************************************************//**
 * @brief USB OTG MSC thread entry function.
 *
 * This function manages USB OTG operations, including switching between USB Host and Device modes, handling device
 * attach and detach events, and communicating with other threads to synchronize operations.
 *
 * @param[in]  pvParameters Pointer to thread parameters (unused).
 * @retval     None
 **********************************************************************************************************************/
void usb_otg_msc_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    fsp_err_t status = FSP_SUCCESS;
    BaseType_t os_status = pdTRUE;
    uint32_t notified = RESET_VALUE;

    /* Wait until the terminal framework is initialized */
    term_framework_init_check();
    PRINT_EP_BANNER;
    PRINT_EP_INFO;

    /* Perform USB OTG startup procedures */
    status = usb_otg_start_up();
    ERROR_TRAP(FSP_SUCCESS != status, status, "USB OTG startup failed!");

    while (true)
    {
        /* Wait for any notifications indicating USB OTG events */
        os_status = xTaskNotifyWait(NOTIFY_NO_CLEAR, NOTIFY_ALL_CLEAR, &notified, portMAX_DELAY);
        if (pdTRUE != os_status)
        {
            PRINT_ERR_STR("Failed to receive notification for USB OTG events.");
            continue;
        }

        /* Handle USB OTG mode change to Device */
        if (USB_OTG_CHANGE_TO_DEVICE == (notified & USB_OTG_CHANGE_TO_DEVICE))
        {
            g_usb_host_active = false;
            usb_otg_change_mode(&USB_OTG_HOST_MODE, &USB_OTG_DEVICE_MODE);
            ERROR_TRAP(FSP_SUCCESS != status, status, "Failed to switch USB to Device mode.");
            PRINT_INFO_STR("********************************************************");
            PRINT_INFO_STR("The USB mode changed to Device.\r\n");
        }

        /* Handle USB OTG mode change to Host */
        if (USB_OTG_CHANGE_TO_HOST == (notified & USB_OTG_CHANGE_TO_HOST))
        {
            g_usb_host_active = true;
            usb_otg_change_mode(&USB_OTG_DEVICE_MODE, &USB_OTG_HOST_MODE);
            ERROR_TRAP(FSP_SUCCESS != status, status, "Failed to switch USB to Host mode.");
            PRINT_INFO_STR("********************************************************");
            PRINT_INFO_STR("The USB mode changed to Host.\r\n");

            /* Notify the FAT thread to open all disks */
            os_status = xTaskNotify(fat_thread, FAT_ALL_DISK_REQUEST_OPEN, eSetBits);
            ERROR_TRAP(pdTRUE != os_status, os_status, "Failed to notify FAT thread to open all disks.");
        }

        /* Handle USB Device attach event */
        if (USB_OTG_DEVICE_ATTACH == (notified & USB_OTG_DEVICE_ATTACH))
        {
            PRINT_INFO_STR("USB Device mode: Attached to a USB host.");
        }

        /* Handle USB Device detach event */
        if (USB_OTG_DEVICE_DETACH == (notified & USB_OTG_DEVICE_DETACH))
        {
            PRINT_INFO_STR("USB Device mode: Detached from the USB host.");
        }

        /* Yield the MCU to other tasks */
        vTaskDelay(USB_OTG_MSC_THREAD_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief Perform USB OTG startup initialization.
 *
 * This function initializes USB OTG in Device mode, sets up the USB OTG ID pin interrupt, and checks the state of
 * the USB ID pin to determine the initial mode (Host or Device).
 *
 * @retval FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static uint32_t usb_otg_start_up(void)
{
    fsp_err_t status = FSP_SUCCESS;
    BaseType_t os_status = pdTRUE;

    /* Open USB in Device mode to set up the PMSC thread */
    status = R_USB_Open(&g_basic_pmsc_ctrl, &g_basic_pmsc_cfg);
    ERROR_RET(FSP_SUCCESS != status, status, "Failed to open USB in Device mode.");

    PRINT_INFO_STR("********************************************************");
    PRINT_INFO_STR("USB initialized in Device mode.\r\n");

    /* Open the USB OTG ID pin interrupt */
    status = R_ICU_ExternalIrqOpen(&g_external_irq_usb_otg_id_ctrl, &g_external_irq_usb_otg_id_cfg);
    ERROR_RET(FSP_SUCCESS != status, status, "Failed to initialize USB OTG ID pin interrupt.");

    /* Enable the USB OTG ID pin interrupt */
    status = R_ICU_ExternalIrqEnable(&g_external_irq_usb_otg_id_ctrl);
    ERROR_RET(FSP_SUCCESS != status, status, "Failed to enable USB OTG ID pin interrupt.");

    /* Check the state of the USB OTG ID pin */
    if (BSP_IO_LEVEL_LOW == R_BSP_PinRead(USB_OTG_ID_PIN))
    {
        os_status = xTaskNotify(usb_otg_msc_thread, USB_OTG_CHANGE_TO_HOST, eSetBits);
        ERROR_RET(pdTRUE != os_status, FSP_ERR_ASSERTION, "Failed to notify USB OTG thread to switch to Host mode.");
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief Handle USB OTG change mode.
 *
 * This function closes the current USB instance and opens the target USB instance.
 *
 * @retval FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static uint32_t usb_otg_change_mode(usb_otg_msc_t * cur_mode, usb_otg_msc_t * target_mode)
{
    fsp_err_t status = FSP_SUCCESS;

    /* Close the current USB instance
     * Do not check error close USB Host instance due to FSP limitation when using both HMSC and PMSC in one project
     * It will be updated after FSP resolving the limitation */
    R_USB_Close(cur_mode->ctrl);

    if (OTG_HOST == target_mode->mode)
    {
        static bool g_usb_host_active_first_time = false;

        if (!g_usb_host_active_first_time)
        {
            /* First-time initialization of USB Host mode */
            g_usb_host_active_first_time = true;

            /* Initialize FreeRTOS FAT for USB Host media */
            status = RM_FREERTOS_PLUS_FAT_Open(&g_rm_freertos_plus_fat_usb_ctrl, &g_rm_freertos_plus_fat_usb_cfg);
            ERROR_RET(FSP_SUCCESS != status, status, "Failed to initialize FreeRTOS FAT for USB Host.");

            return status;
        }
    }

    /* Open the target USB instance */
    status = R_USB_Open(target_mode->ctrl, target_mode->cfg);
    ERROR_RET(FSP_SUCCESS != status, status, "Failed to open USB.");

    return status;
}

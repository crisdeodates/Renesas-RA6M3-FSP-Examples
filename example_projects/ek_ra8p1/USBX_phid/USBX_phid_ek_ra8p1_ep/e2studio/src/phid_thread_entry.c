/***********************************************************************************************************************
 * File Name    : phid_thread_entry.c
 * Description  : Contains macros, data structures and functions used in the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "usbx_phid_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_phid_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Macro definitions
 *****************************************************************************/

/*******************************************************************************************************************//**
 * Exported global variables and functions (to be accessed by other files)
 **********************************************************************************************************************/

/* Num Lock, Caps Lock status */
extern uint8_t g_device_framework_full_speed[];
extern uint8_t g_string_framework[];
extern uint8_t g_language_id_framework[];
extern uint8_t g_apl_report[REPORT_DESCRIPTOR_LENGTH];
extern bool g_write_flag;

/*******************************************************************************************************************//**
 * Global variables and functions
 **********************************************************************************************************************/
UINT usbx_status_callback(ULONG status);

/*******************************************************************************************************************//**
 * Private global variables and functions
 **********************************************************************************************************************/
static UX_SLAVE_CLASS_HID_PARAMETER g_ux_device_class_hid_parameter;
static UX_SLAVE_CLASS_HID_EVENT hid_event;
static uint32_t g_ux_pool_memory[MEMPOOL_SIZE / VALUE_4];
uint8_t g_rtt_input_data[BUFFER_SIZE_DOWN] = {RESET_VALUE};
static UX_SLAVE_CLASS_HID * volatile g_hid = UX_NULL;
static UCHAR key = RESET_VALUE;
static bool b_print_status = false;

static UINT usbx_phid_operation(void);
static void de_init_module(void);
static void ux_hid_instance_activate(void * instance);
static void ux_hid_instance_deactivate(void * instance);

/* PHID Thread entry function */
void phid_thread_entry(void)
{
    UINT status = UX_SUCCESS;
    UINT err_status = UX_SUCCESS;
    /* To check FSP API return status */
    fsp_err_t err = FSP_SUCCESS;
    ULONG actual_flags = RESET_VALUE;

    /* Initialize the RTT Thread */
    rtt_thread_init_check();

    /* Print the banner and EP info */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Initialize the USB system */
    status = ux_system_initialize((CHAR*) g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, RESET_VALUE);
    /* Handle error */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_system_initialize API failed");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_system initialized successfully");

    /* Initialize USB stack */
    status = ux_device_stack_initialize(NULL, RESET_VALUE, g_device_framework_full_speed,
                                        DEVICE_FRAME_LENGTH_SPEED,
                                        g_string_framework,
                                        STRING_FRAMEWORK_LENGTH,
                                        g_language_id_framework,
                                        LANGUAGE_ID_FRAME_WORK_LENGTH,
                                        &usbx_status_callback);
    /* Handle error */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_initialize API failed");
        /* De-initialize of USB stack */
        err_status = ux_system_uninitialize();
        if(UX_SUCCESS != err_status)
        {
            PRINT_ERR_STR("ux_system_uninitialize API failed");
        }
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_device stack initialized successfully");
    /* The activate command is used when the host has sent a SET_CONFIGURATION command
     * and this interface has to be mounted. Both Bulk endpoints have to be mounted
     * and the PHID thread needs to be activated */
    g_ux_device_class_hid_parameter.ux_slave_class_hid_instance_activate = ux_hid_instance_activate;

    /* The deactivate command is used when the device has been extracted.
     * The device endpoints have to be dismounted and the PHID thread canceled */
    g_ux_device_class_hid_parameter.ux_slave_class_hid_instance_deactivate = ux_hid_instance_deactivate;

    /* Add hid report address and descriptor length */
    g_ux_device_class_hid_parameter.ux_device_class_hid_parameter_report_address = g_apl_report;
    g_ux_device_class_hid_parameter.ux_device_class_hid_parameter_report_length = REPORT_DESCRIPTOR_LENGTH;

    /* ux_device stack class registration */
    status = ux_device_stack_class_register(_ux_system_slave_class_hid_name, _ux_device_class_hid_entry, CONFIG_NUMB,
                                            INTERFACE_NUMB0, (void*) &g_ux_device_class_hid_parameter);
    /* Handle error */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_class_register API failed");
        /* De-initialize of USB stack */
        err_status = ux_device_stack_uninitialize();
        if(UX_SUCCESS != err_status)
        {
            PRINT_ERR_STR("ux_device_stack_uninitialize API failed");
        }
        err_status = ux_system_uninitialize();
        if(UX_SUCCESS != err_status)
        {
            PRINT_ERR_STR("ux_system_uninitialize API failed");
        }
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_device stack class registered successfully");

    /* Set the first key to 'a' which is 04 */
    key = FIRST_KEY;

    /* Reset the HID event structure */
    ux_utility_memory_set(&hid_event, RESET_VALUE, sizeof(UX_SLAVE_CLASS_HID_EVENT));

    /* Open USB driver */
    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_USB_OPEN API failed");
        /* De-initialize of USB stack */
        err_status = ux_device_stack_class_unregister(_ux_system_slave_class_hid_name, _ux_device_class_hid_entry);
        if (UX_SUCCESS != err_status)
        {
            PRINT_ERR_STR("ux_device_stack_class_unregister API failed");
        }
        err_status = ux_device_stack_uninitialize();
        if (UX_SUCCESS != err_status)
        {
            PRINT_ERR_STR("ux_device_stack_uninitialize API failed");
        }
        err_status = ux_system_uninitialize();
        if (UX_SUCCESS != err_status)
        {
            PRINT_ERR_STR("ux_system_uninitialize API failed");
        }
        ERROR_TRAP(err);
    }

    PRINT_INFO_STR("USB driver opened successfully");

    /* Wait for enumeration event */
    status = tx_event_flags_get(&g_hid_event_flags0, HID_ACTIVATE_FLAG, TX_OR, &actual_flags, TX_WAIT_FOREVER);
    if ((actual_flags & HID_ACTIVATE_FLAG) && (TX_SUCCESS == status))
    {
        PRINT_INFO_STR("USB enumeration completed");
    }
    else if (!(actual_flags & HID_ACTIVATE_FLAG))
    {
        PRINT_INFO_STR("USB instance has been deactivated");
    }

    while (true)
    {
        /* Perform the PHID operation */
        status = usbx_phid_operation();
        /* Handle error */
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("usbx_phid_operation failed");
            /* De-initialize of USB stack and basic driver */
            de_init_module();
            ERROR_TRAP(status);
        }
        tx_thread_sleep(ONE);
    }
}

/*******************************************************************************************************************//**
 * @brief       Get instance.
 * @param[IN]   instance   Pointer to the memory where the instance information will be stored.
 * @retval      None
 **********************************************************************************************************************/
static void ux_hid_instance_activate (void * instance)
{
    UINT tx_status = TX_SUCCESS;
    /* Save the HID instance */
    g_hid = (UX_SLAVE_CLASS_HID *) instance;
    /* Set event flag to notify that instance activation */
    tx_status = tx_event_flags_set(&g_hid_event_flags0, HID_ACTIVATE_FLAG, TX_OR);
    if (TX_SUCCESS != tx_status)
    {
        PRINT_ERR_STR("Failed to set event flag in ux_hid_instance_activate");
        ERROR_TRAP(tx_status);
    }
}
/*******************************************************************************************************************//**
 * End of function ux_hid_instance_activate
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief       Clear instance.
 * @param[IN]   instance    Pointer to the memory where the instance information will be stored.
 * @retval      None
 **********************************************************************************************************************/
static void ux_hid_instance_deactivate (void * instance)
{
    UINT tx_status = TX_SUCCESS;
    FSP_PARAMETER_NOT_USED(instance);
    /* Clear the HID instance */
    g_hid = UX_NULL;
    /* Set event flag to notify that the HID instance has been deactivated */
    tx_status = tx_event_flags_set(&g_hid_event_flags0, HID_DEACTIVATE_FLAG, TX_OR);
    if (TX_SUCCESS != tx_status)
    {
        PRINT_ERR_STR("Failed to set event flag in ux_hid_instance_deactivate");
        ERROR_TRAP(tx_status);
    }
}
/*******************************************************************************************************************//**
 * End of function ux_hid_instance_deactivate
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief     USB callback function invoked on USB status changes.
 * @param[in] status        USB status value passed by the USBX stack.
 * @retval    UX_SUCCESS    Returned to indicate the callback was handled successfully.
 **********************************************************************************************************************/
UINT usbx_status_callback (ULONG status)
{
    UINT tx_status = TX_SUCCESS;
    switch (status)
    {
        case UX_DEVICE_ATTACHED:
        {
            /* Set event flag to notify that the USB device has been attached */
            tx_status = tx_event_flags_set(&g_hid_event_flags0, HID_FLAG, TX_OR);
            if (TX_SUCCESS != tx_status)
            {
                PRINT_ERR_STR("Failed to set event flag for USB device attachment");
            }
            break;
        }

        case UX_DEVICE_REMOVED:
        {
            /* Set event flag to notify that the USB device has been removed */
            tx_status = tx_event_flags_set(&g_hid_event_flags0, ~HID_FLAG, TX_AND);
            if (TX_SUCCESS != tx_status)
            {
                PRINT_ERR_STR("Failed to set event flag for USB device removal");
            }
            break;
        }

        default:
        {
            break;
        }
    }
    return tx_status;
}
/*******************************************************************************************************************//**
 * End of function usbx_status_callback
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief     Handles the PHID operation by sending key data to the host via USB.
 *            The key input is taken from the RTT Viewer.
 * @param[in] None
 * @retval    UX_SUCCESS on success or a error code on failure.
 **********************************************************************************************************************/
UINT usbx_phid_operation (void)
{
    UINT status = UX_SUCCESS;
    UINT tx_status = TX_SUCCESS;
    ULONG actual_flags = RESET_VALUE;

    /* Wait for USB connection event */
    tx_status = tx_event_flags_get(&g_hid_event_flags0, HID_FLAG, TX_OR, &actual_flags, TX_WAIT_FOREVER);
    if ((TX_SUCCESS == tx_status) && (actual_flags & HID_FLAG))
    {

        if (true != b_print_status)
        {
            PRINT_INFO_STR("USB device is attached");
            b_print_status = true;    /* Update status flag */
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        b_print_status = false; /* Clear the flag */

        PRINT_INFO_STR("USB device is removed. Connect to host PC before operating keyboard.");
        return actual_flags;
    }
    /* Check whether user input has given */
    if (false == g_write_flag)
    {
        PRINT_INFO_STR("Provide any key as input from a-z or 0-9");
        /* Wait for RTT input */
        do
        {
            status = check_for_rtt_user_ip((uint8_t*) &g_rtt_input_data);
        }
        while (status != TX_SUCCESS);
    }
    else
    {
        /* Then insert a key into the keyboard event. Length is fixed to 8 */
        hid_event.ux_device_class_hid_event_length = EVENT_LENGTH;

        /* First byte is a modifier byte */
        hid_event.ux_device_class_hid_event_buffer[ZERO] = RESET_VALUE;

        /* Second byte is reserved */
        hid_event.ux_device_class_hid_event_buffer[ONE] = RESET_VALUE;

        /* The 6 next bytes are keys. We only have one key here */
        hid_event.ux_device_class_hid_event_buffer[TWO] = key;

        /* Set the keyboard event */
        status = ux_device_class_hid_event_set(g_hid, &hid_event);
        /* Handle error */
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("ux_device_class_hid_event_set API failed");
            return status;
        }
        /* Next event has the key depressed */
        hid_event.ux_device_class_hid_event_buffer[TWO] = RESET_VALUE;

        /* Length is fixed to 8 */
        hid_event.ux_device_class_hid_event_length = EVENT_LENGTH;

        /* Set the keyboard event */
        status = ux_device_class_hid_event_set(g_hid, &hid_event);

        /* Handle error */
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("ux_device_class_hid_event_set API failed");
            return status;
        }
        tx_thread_sleep(DELAY_100_MS);

        /* Are we at the end of alphabet? */
        if (key != FIRST_KEY + END_KEY)
        {
            /* Next key */
            key++;
        }
        else
        {
            /* Start over again */
            key = FIRST_KEY;
            memset(g_rtt_input_data, RESET_VALUE, sizeof(g_rtt_input_data));
            /* Clear the flag bit */
            g_write_flag = false;
        }
    }
    return status;
}
/*******************************************************************************************************************//**
 * End of function usbx_phid_operation
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief     Un-initializes the USB stack and closes the basic USB driver.
 * @param[in] None
 * @retval    None
 **********************************************************************************************************************/
static void de_init_module(void)
{
    fsp_err_t err = FSP_SUCCESS;
    UINT err_status = UX_SUCCESS;

    /* De-initialize of USB stack */
    err_status = ux_device_stack_class_unregister(_ux_system_slave_class_hid_name, _ux_device_class_hid_entry);
    if (UX_SUCCESS != err_status)
    {
        PRINT_ERR_STR("ux_device_stack_class_unregister API failed");
    }
    err_status = ux_device_stack_uninitialize();
    if (UX_SUCCESS != err_status)
    {
        PRINT_ERR_STR("ux_device_stack_uninitialize API failed");
    }
    err_status = ux_system_uninitialize();
    if (UX_SUCCESS != err_status)
    {
        PRINT_ERR_STR("ux_system_uninitialize API failed");
    }
    err = R_USB_Close(&g_basic0_ctrl);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_USB_Close API failed");
    }
}
/*******************************************************************************************************************//**
 * End of function de_init_module
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @} (end defgroup usbx_phid_ep)
 **********************************************************************************************************************/

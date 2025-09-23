/***********************************************************************************************************************
 * File Name    : hcdc_acm_thread_entry.c
 * Description  : Contains macros and functions used in hcdc_acm_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hcdc_acm_thread.h"
#include "ux_api.h"
#include "ux_host_class_cdc_acm.h"
#include "usbx_hcdc_acm_ep.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_hcdc_acm_ep
 * @{
 **********************************************************************************************************************/

 /* Private function */
static UINT ux_host_usr_event_notification(ULONG event, UX_HOST_CLASS * host_class, VOID * instance);

/* A pointer to store CDC-ACM instance */
static UX_HOST_CLASS_CDC_ACM * p_cdc_acm = UX_NULL;

static ULONG g_write_actual_length   = RESET_VALUE;
static ULONG g_read_actual_length    = RESET_VALUE;
static uint8_t g_read_buf[MAX_PACKET_SIZE]  = {RESET_VALUE};
static uint8_t g_write_buf[DATA_LEN] = {RESET_VALUE};
static uint32_t g_ux_pool_memory[MEMPOOL_SIZE / VALUE_4];

/* HCDC ACM Thread entry function */
void hcdc_acm_thread_entry(void)
{
    uint32_t status = RESET_VALUE ;
    ULONG    actual_flags = RESET_VALUE ;
    uint16_t count = RESET_VALUE;
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the RTT Thread */
    rtt_thread_init_check();

    /* Print the banner and EP info */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* ux_system_initialization */
    status = ux_system_initialize(g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, RESET_VALUE);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_system_initialize API failed");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_system_initialize successfully");

    /* ux_host_stack initialization */
    status = ux_host_stack_initialize(ux_host_usr_event_notification);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_host_stack_initialize API failed");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_host_stack_initialize successfully");

    /* Open USB driver */
    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_USB_Open API failed");
        ERROR_TRAP(err);
    }

    /* Fill the write buffer */
    for (count = RESET_VALUE; count < DATA_LEN; count++)
    {
        g_write_buf[count] = (uint8_t) count;
    }

    while (true)
    {
        /* Retrieves event flags from the specified event flags group */
        tx_event_flags_get(&g_cdcacm_activate_event_flags0, CDCACM_FLAG, TX_OR, &actual_flags, TX_WAIT_FOREVER);
        if (UX_NULL != p_cdc_acm)
        {
            /* Read the data from device */
            status = ux_host_class_cdc_acm_read(p_cdc_acm, g_read_buf, READ_LEN, &g_read_actual_length);
            if (UX_SUCCESS != status)
            {
                PRINT_ERR_STR("ux_host_class_cdc_acm_read API failed");
                ERROR_TRAP(status);
            }

            /* Print data received from peripheral device */
            PRINT_INFO_STR("USBX_PCDC has requested data from USBX_HCDC");

            /* Send the data to device */
            status = ux_host_class_cdc_acm_write(p_cdc_acm, g_write_buf, DATA_LEN, &g_write_actual_length);
            if (UX_SUCCESS != status)
            {
                PRINT_ERR_STR("ux_host_class_cdc_acm_write API failed");
                ERROR_TRAP(status);
            }

            /* Clear the buffer */
            memset(g_read_buf, RESET_VALUE, sizeof(g_read_buf));

            /* USB receives the data echoed back */
            status = ux_host_class_cdc_acm_read(p_cdc_acm, g_read_buf, DATA_LEN, &g_read_actual_length);
            if (UX_SUCCESS != status)
            {
                PRINT_ERR_STR("ux_host_class_cdc_acm_read API failed");
                ERROR_TRAP(status);
            }

            /* Print read data */
            PRINT_INFO_STR("Loop-back data from USBX_HCDC received");
            /* Compare loop-back data */
            if(SUCCESS == (memcmp(g_read_buf, g_write_buf, DATA_LEN)))
            {
                PRINT_INFO_STR("USBX HCDC Data compared successfully\r\n");
            }
            else
            {
                PRINT_ERR_STR("USBX HCDC data received is incorrect\r\n");
                ERROR_TRAP(status);
            }

            /* Wait time is provided here to print data through RTT thread framework */
            tx_thread_sleep(WAIT_TIME);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function checks the USB device status and notifies to perform operation.
 * @param[IN] none
 * @retval    none
 **********************************************************************************************************************/
static UINT ux_host_usr_event_notification(ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
    if (_ux_utility_memory_compare(_ux_system_host_class_cdc_acm_name, host_class,
                                   _ux_utility_string_length_get(_ux_system_host_class_cdc_acm_name)) == UX_SUCCESS)
    {
        /* Check if there is a device insertion */
        if (UX_FSP_DEVICE_INSERTION == event)
        {
            p_cdc_acm = (UX_HOST_CLASS_CDC_ACM *) instance;
            /* Check interface data class */
            if (p_cdc_acm->ux_host_class_cdc_acm_interface->ux_interface_descriptor.bInterfaceClass !=
                    UX_HOST_CLASS_CDC_DATA_CLASS)
            {
                if (UX_NULL != p_cdc_acm->ux_host_class_cdc_acm_next_instance)
                {
                    /* It seems the DATA class is on the second interface. Or we hope */
                    p_cdc_acm = p_cdc_acm->ux_host_class_cdc_acm_next_instance;

                    /* Check again this interface, if this is not the data interface, we give up */
                    if (p_cdc_acm->ux_host_class_cdc_acm_interface->ux_interface_descriptor.bInterfaceClass !=
                        UX_HOST_CLASS_CDC_DATA_CLASS)
                    {
                        /* We did not find a proper data interface */
                        p_cdc_acm = UX_NULL;
                    }
                }
            }

            if (UX_NULL != p_cdc_acm)
            {
                /* This sets or clears event flags in an event flags group */
                tx_event_flags_set(&g_cdcacm_activate_event_flags0, CDCACM_FLAG, TX_OR);
            }
        }
        /* Check if there is a device removal */
        else if (UX_FSP_DEVICE_REMOVAL == event)
        {
            /* This sets or clears event flags in an event flags group */
            tx_event_flags_set(&g_cdcacm_activate_event_flags0, ~CDCACM_FLAG, TX_AND);
            p_cdc_acm = UX_NULL;
        }
        else
        {
            /* Do nothing */
        }
    }
    return UX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usbx_hcdc_acm_ep)
 **********************************************************************************************************************/

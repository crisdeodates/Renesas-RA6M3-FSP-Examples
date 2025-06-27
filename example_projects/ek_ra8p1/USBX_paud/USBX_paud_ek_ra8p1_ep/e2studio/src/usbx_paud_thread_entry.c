/***********************************************************************************************************************
 * File Name    : usbx_paud_thread_entry.c
 * Description  : Contains macros and functions used in usbx_paud_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usbx_paud_thread.h"
#include "common_utils.h"
#include "usbx_paud_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_paud_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 Exported global variables and functions (to be accessed by other files)
 **********************************************************************************************************************/
extern uint8_t g_device_framework_full_speed[];
extern uint8_t g_device_framework_hi_speed[];
extern uint8_t g_string_framework[];
extern uint8_t g_language_id_framework[];
static uint8_t g_write_buf[USB_MAX_PACKET_SIZE_IN];

/*******************************************************************************************************************//**
 Private global variables and functions
 **********************************************************************************************************************/
static volatile uint32_t g_read_alternate_setting   = USB_APL_OFF;
static volatile uint32_t g_write_alternate_setting  = USB_APL_OFF;
static volatile uint8_t  g_read_wp                  = RESET_VALUE;
static volatile uint32_t g_read_frame_num           = RESET_VALUE;
static volatile uint32_t g_read_counter             = RESET_VALUE;
static volatile uint32_t g_write_counter            = RESET_VALUE;
static volatile uint32_t g_length                   = RESET_VALUE;
static UX_DEVICE_CLASS_AUDIO * volatile g_p_audio   = UX_NULL;
static ULONG  actual_flags                          = RESET_VALUE;
volatile ULONG data_length_playback                 = RESET_VALUE;
volatile ULONG real_byte_count                      = RESET_VALUE;
static UINT g_flag                                  = RESET_VALUE;
static uint32_t g_counter                           = VALUE_0;

static int16_t  g_control_volume[VALUE_2];
static uint16_t g_control_mute[VALUE_2];
static uint32_t g_ux_pool_memory[MEMPOOL_SIZE / VALUE_4];
static unsigned char GLOBAL_BUFF[GBUFF_SIZE];

#ifdef APL_AUDIO_20
static UX_DEVICE_CLASS_AUDIO20_CONTROL_GROUP g_audio20_control_group;
static UX_DEVICE_CLASS_AUDIO20_CONTROL       g_audio20_control[VALUE_2];
static UINT apl_audio20_request_process (UX_DEVICE_CLASS_AUDIO * p_audio, UX_SLAVE_TRANSFER * p_transfer);
#else /* APL_AUDIO_20 */
static UX_DEVICE_CLASS_AUDIO10_CONTROL_GROUP g_audio_control_group;
static UX_DEVICE_CLASS_AUDIO10_CONTROL       g_audio_control[VALUE_2];
static UINT apl_audio10_request_process (UX_DEVICE_CLASS_AUDIO * p_audio, UX_SLAVE_TRANSFER * p_transfer);
#endif /* APL_AUDIO_20 */

/* Private functions */
static UINT apl_status_change_cb (ULONG status);
static void apl_audio_read_change (UX_DEVICE_CLASS_AUDIO_STREAM * p_stream, ULONG alternate_setting);
static void apl_audio_read_done (UX_DEVICE_CLASS_AUDIO_STREAM * p_stream, ULONG actual_length);
static void apl_audio_write_change (UX_DEVICE_CLASS_AUDIO_STREAM * p_stream, ULONG alternate_setting);
static void apl_audio_write_done (UX_DEVICE_CLASS_AUDIO_STREAM * p_stream, ULONG actual_length);
static void apl_audio_instance_activate (void * p_instance);
static void apl_audio_instance_deactivate (void * p_instance);

/* USBX PAUD entry function */
void usbx_paud_thread_entry(void)
{
    rtt_thread_init_check();
    fsp_pack_version_t version  = { RESET_VALUE };
    UINT status                 = UX_SUCCESS;
    UX_DEVICE_CLASS_AUDIO_STREAM_PARAMETER audio_stream_parameter[VALUE_2];
    UX_DEVICE_CLASS_AUDIO_PARAMETER        audio_parameter;

    /* Version get API for FLEX Pack version */
    R_FSP_VersionGet (&version);

    /* Print the banner and EP info */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* ux_system_initialization */
    status = ux_system_initialize((CHAR *) g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, VALUE_0);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_system_initialize API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_system initialized successfully");

    /* ux_device stack initialization */
    status = _ux_device_stack_initialize(g_device_framework_hi_speed,
                                         VALUE_275,
                                         g_device_framework_full_speed,
                                         VALUE_226,
                                         g_string_framework,
                                         VALUE_93,
                                         g_language_id_framework,
                                         VALUE_2,
                                         apl_status_change_cb);
    /* Handle error */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("_ux_device_stack_initialize API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_device stack initialized successfully");

    /* Read parameters and CB Initialization */
    audio_stream_parameter[INDEX_0].ux_device_class_audio_stream_parameter_callbacks.ux_device_class_audio_stream_change
    = apl_audio_read_change;
    audio_stream_parameter[INDEX_0].ux_device_class_audio_stream_parameter_callbacks. \
    ux_device_class_audio_stream_frame_done = apl_audio_read_done;
    audio_stream_parameter[INDEX_0].ux_device_class_audio_stream_parameter_thread_stack_size    = STACK_SIZE;
    audio_stream_parameter[INDEX_0].ux_device_class_audio_stream_parameter_max_frame_buffer_nb  = NUM_OF_FRAME;
    audio_stream_parameter[INDEX_0].ux_device_class_audio_stream_parameter_max_frame_buffer_size
    = USB_MAX_PACKET_SZIE_OUT;
    audio_stream_parameter[INDEX_0].ux_device_class_audio_stream_parameter_thread_entry         =
            ux_device_class_audio_read_thread_entry;

    /* Write parameters and CB Initialization */
    audio_stream_parameter[INDEX_1].ux_device_class_audio_stream_parameter_callbacks.ux_device_class_audio_stream_change
    = apl_audio_write_change;
    audio_stream_parameter[INDEX_1].ux_device_class_audio_stream_parameter_callbacks. \
    ux_device_class_audio_stream_frame_done = apl_audio_write_done;
    audio_stream_parameter[INDEX_1].ux_device_class_audio_stream_parameter_thread_stack_size     = STACK_SIZE;
    audio_stream_parameter[INDEX_1].ux_device_class_audio_stream_parameter_max_frame_buffer_nb   = NUM_OF_FRAME;
    audio_stream_parameter[INDEX_1].ux_device_class_audio_stream_parameter_max_frame_buffer_size =
            USB_MAX_PACKET_SIZE_IN;
    audio_stream_parameter[INDEX_1].ux_device_class_audio_stream_parameter_thread_entry          =
            ux_device_class_audio_write_thread_entry;

    /* Activate, deactivate CB Initialization */
    audio_parameter.ux_device_class_audio_parameter_callbacks.ux_slave_class_audio_instance_activate    =
            apl_audio_instance_activate;
    audio_parameter.ux_device_class_audio_parameter_callbacks.ux_slave_class_audio_instance_deactivate  =
            apl_audio_instance_deactivate;
    audio_parameter.ux_device_class_audio_parameter_callbacks.ux_device_class_audio_control_process
#ifdef APL_AUDIO_20
        = apl_audio20_request_process;
#else
        = apl_audio10_request_process;
#endif

    audio_parameter.ux_device_class_audio_parameter_streams_nb = VALUE_2;
    audio_parameter.ux_device_class_audio_parameter_streams    = &audio_stream_parameter[INDEX_0];

    /* ux_device stack class registration */
    status = ux_device_stack_class_register(_ux_system_slave_class_audio_name,
                                            _ux_device_class_audio_entry,
                                            VALUE_1,
                                            VALUE_0,
                                            (void *) &audio_parameter);
    /* Handle error */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_class_register API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_device stack class registered successfully");

#ifdef APL_AUDIO_20
    /* Audio Class 2.0 parameter settings */
    g_audio20_control[INDEX_0].ux_device_class_audio20_control_cs_id               = A20_CS_ID;
    g_audio20_control[INDEX_0].ux_device_class_audio20_control_sampling_frequency  = A20_SAMPLING_FREQUENCY;
    g_audio20_control[INDEX_0].ux_device_class_audio20_control_fu_id               = A20_FU_ID_0;
    g_audio20_control[INDEX_0].ux_device_class_audio20_control_mute[INDEX_0]       = A20_MUTE;
    g_audio20_control[INDEX_0].ux_device_class_audio20_control_volume_min[INDEX_0] = A20_VOL_MIN_0;
    g_audio20_control[INDEX_0].ux_device_class_audio20_control_volume_max[INDEX_0] = A20_VOL_MAX_0;
    g_audio20_control[INDEX_0].ux_device_class_audio20_control_volume[INDEX_0]     = A20_SET_VOL_0;

    g_audio20_control[INDEX_1].ux_device_class_audio20_control_cs_id               = A20_CS_ID;
    g_audio20_control[INDEX_1].ux_device_class_audio20_control_sampling_frequency  = A20_SAMPLING_FREQUENCY;
    g_audio20_control[INDEX_1].ux_device_class_audio20_control_fu_id               = A20_FU_ID_1;
    g_audio20_control[INDEX_1].ux_device_class_audio20_control_mute[INDEX_0]       = A20_MUTE;
    g_audio20_control[INDEX_1].ux_device_class_audio20_control_volume_min[INDEX_0] = A20_VOL_MIN_1;
    g_audio20_control[INDEX_1].ux_device_class_audio20_control_volume_max[INDEX_0] = A20_VOL_MAX_1;
    g_audio20_control[INDEX_1].ux_device_class_audio20_control_volume[INDEX_0]     = A20_SET_VOL_1;

    g_audio20_control_group.ux_device_class_audio20_control_group_controls_nb      = VALUE_2;
    g_audio20_control_group.ux_device_class_audio20_control_group_controls         = &g_audio20_control[INDEX_0];
#else /* APL_AUDIO_20 */
    /* Audio Class 1.0 parameter settings */
    g_audio_control[INDEX_0].ux_device_class_audio10_control_fu_id               = A10_FU_ID_0;
    g_audio_control[INDEX_0].ux_device_class_audio10_control_mute[INDEX_0]       = A10_MUTE_0;
    g_audio_control[INDEX_0].ux_device_class_audio10_control_volume[INDEX_0]     = A10_VOL_0;
    g_audio_control[INDEX_0].ux_device_class_audio10_control_volume_min[INDEX_0] = A10_VOL_MIN_0;
    g_audio_control[INDEX_0].ux_device_class_audio10_control_volume_max[INDEX_0] = A10_VOL_MAX_0;
    g_audio_control[INDEX_0].ux_device_class_audio10_control_volume_res[INDEX_0] = A10_VOL_RES_0;
    g_audio_control[INDEX_0].ux_device_class_audio10_control_ep_addr             = A10_EP_ADDR_OUT;

    g_audio_control[INDEX_1].ux_device_class_audio10_control_fu_id                = A10_FU_ID_1;
    g_audio_control[INDEX_1].ux_device_class_audio10_control_mute[INDEX_0]        = A10_MUTE_1;
    g_audio_control[INDEX_1].ux_device_class_audio10_control_volume[INDEX_0]      = A10_VOL_1;
    g_audio_control[INDEX_1].ux_device_class_audio10_control_volume_min[INDEX_0]  = A10_VOL_MIN_1;
    g_audio_control[INDEX_1].ux_device_class_audio10_control_volume_max[INDEX_0]  = A10_VOL_MAX_1;
    g_audio_control[INDEX_1].ux_device_class_audio10_control_volume_res[INDEX_0]  = A10_VOL_RES_1;
    g_audio_control[INDEX_1].ux_device_class_audio10_control_ep_addr              = A10_EP_ADDR_IN;

    g_audio_control_group.ux_device_class_audio10_control_group_controls_nb       = VALUE_2;
    g_audio_control_group.ux_device_class_audio10_control_group_controls          = &g_audio_control[INDEX_0];
#endif /* APL_AUDIO_20 */

    /* Clear write buffer */
    memset(g_write_buf, VALUE_0, USB_MAX_PACKET_SIZE_IN);

    /* Open USB driver */
    status = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    /* Handle error */
    if (FSP_SUCCESS != status)
    {
        PRINT_ERR_STR("R_USB_Open API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("USB driver opened successfully");

    PRINT_INFO_STR("Insert the USB cable");

    /* Event flags are used to register USB events such as ATTACH and REMOVE */
    /* Wait until the USB device cable is inserted */
    status = tx_event_flags_get (&g_msc_event_flags0, USB_PLUG_IN, TX_AND_CLEAR, &actual_flags, TX_WAIT_FOREVER);
    if(USB_PLUG_IN == actual_flags)
    {
        PRINT_INFO_STR("USB device is plugged in");
    }
    /* Reset the event flag */
    actual_flags = RESET_VALUE;

    while (true)
    {
        /* Check if USB is plugged out */
        status = tx_event_flags_get (&g_msc_event_flags0, USB_PLUG_OUT, TX_AND_CLEAR, &actual_flags, TX_NO_WAIT);
        if(USB_PLUG_OUT == actual_flags)
        {
            /* Reset the event flag */
            actual_flags = RESET_VALUE;
            PRINT_INFO_STR("USB device is unplugged. Connect USB device cable for EP to work.");

            /* Reset read flags */
            g_read_wp                   = VALUE_0;
            g_read_frame_num            = VALUE_0;
            g_read_alternate_setting    = USB_APL_OFF;
            g_write_alternate_setting   = USB_APL_OFF;

            /* Event flags are used to register USB events such as ATTACH and REMOVE */
            /* Wait until USB device cable is inserted */
            status = tx_event_flags_get (&g_msc_event_flags0, USB_PLUG_IN, TX_AND_CLEAR, &actual_flags, \
                                         TX_WAIT_FOREVER);
            if(USB_PLUG_IN == actual_flags)
            {
                PRINT_INFO_STR("USB device is plugged in");
            }
            /* Reset the event flag */
            actual_flags = RESET_VALUE;
        }
        tx_thread_sleep (1);
    }
}

/*******************************************************************************************************************//**
 * @brief       USB callback function for USB status change
 * @param[IN]   status              USB status
 * @retval      UX_SUCCESS          Upon successful
 **********************************************************************************************************************/
static UINT apl_status_change_cb (ULONG status)
{
    switch (status)
    {
        case UX_DEVICE_ATTACHED:
        {
            /* Set USB PLUG-IN event */
            tx_event_flags_set(&g_msc_event_flags0, USB_PLUG_OUT_CLEAR, TX_AND);
            tx_event_flags_set(&g_msc_event_flags0, USB_PLUG_IN, TX_OR);
            break;
        }

        case UX_DEVICE_REMOVED:
        {
            /* Set USB SUSPENDED event */
            tx_event_flags_set(&g_msc_event_flags0, USB_PLUG_IN_CLEAR, TX_AND);
            tx_event_flags_set(&g_msc_event_flags0, USB_PLUG_OUT, TX_OR);
            break;
        }

        default:
        {
            /* Do nothing */
        }
    }
    return (UX_SUCCESS);
}
/*******************************************************************************************************************//**
 * End of function apl_status_change_cb
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief       Callback function called when switching alternate setting value of OUT transfer
 * @param[IN]   p_stream            Pointer to UX_DEVICE_CLASS_AUDIO_STREAM structure
 *              alternate_setting   Alternate setting value
 * @retval      UX_SUCCESS          Upon successful
 *              Any other error code apart from UX_SUCCESS  Upon unsuccessful
 **********************************************************************************************************************/
static void apl_audio_read_change (UX_DEVICE_CLASS_AUDIO_STREAM * p_stream, ULONG alternate_setting)
{
    UINT ux_err               = UX_SUCCESS;
    if (USB_APL_ON == alternate_setting)
    {
        ux_err = ux_device_class_audio_stream_get(g_p_audio, 0, &p_stream);
        if (UX_SUCCESS != ux_err)
        {
            PRINT_ERR_STR("ux_device_class_audio_stream_get operation failed");
        }
        else
        {
            ux_err = ux_device_class_audio_reception_start(p_stream);
            if (UX_SUCCESS != ux_err)
            {
                PRINT_ERR_STR("ux_device_class_audio_reception_start operation failed");
            }
        }
    }
    else
    {
        if (USB_APL_ON == g_read_alternate_setting)
        {
            /* Alternate Setting 1 --> 0 */
            /* Resetting counters */
            g_read_frame_num    = RESET_VALUE;
            g_read_wp           = RESET_VALUE;
            g_length            = RESET_VALUE;
            if(g_flag == VALUE_1)
            {
                PRINT_INFO_STR("USB read completed");
                g_flag = RESET_VALUE;
                g_read_counter = VALUE_0;
                g_counter = VALUE_0;
            }
        }
    }

    g_read_alternate_setting = alternate_setting;
}
/*******************************************************************************************************************//**
 * End of function apl_audio_read_change
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief       Callback function called when completing of OUT transfer reception
 * @param[IN]   p_stream            Pointer to UX_DEVICE_CLASS_AUDIO_STREAM structure
 *              actual_length       Actual Length
 * @retval      UX_SUCCESS          Upon successful
 *              Any other error code apart from UX_SUCCESS  Upon unsuccessful
 **********************************************************************************************************************/
static void apl_audio_read_done (UX_DEVICE_CLASS_AUDIO_STREAM * p_stream, ULONG actual_length)
{
    UINT    ux_err              = UX_SUCCESS;
    UCHAR * p_buffer            = NULL;
    ULONG   length              = VALUE_0;

    data_length_playback = actual_length;

    if (USB_APL_ON == g_read_alternate_setting)
    {
        ux_err = ux_device_class_audio_read_frame_get(p_stream, &p_buffer, &length);
        if (UX_SUCCESS == ux_err)
        {
            if (g_counter < GBUFF_SIZE - VALUE_1)
            {
                for (UINT index = RESET_VALUE; index < length; index++)
                {
                    GLOBAL_BUFF[g_counter++] =  *(p_buffer + index);
                }
            }
            g_read_frame_num++;
            g_length = g_length + length;
            g_read_counter++ ;
            if(length > VALUE_0 && g_read_counter == VALUE_1)
            {
                PRINT_INFO_STR("USB read started");
                g_flag = VALUE_1;
            }

            ux_err = ux_device_class_audio_read_frame_free(p_stream);
            if (UX_SUCCESS != ux_err)
            {
                PRINT_ERR_STR("ux_device_class_audio_read_frame_free operation failed");
            }
        }
        else
        {
            PRINT_ERR_STR("ux_device_class_audio_read_frame_get operation failed");
        }
    }
}
/*******************************************************************************************************************//**
 * End of function apl_audio_read_done
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief       Callback function called when switching alternate setting value of IN transfer
 * @param[IN]   p_stream            Pointer to UX_DEVICE_CLASS_AUDIO_STREAM structure
 *              actual_length       Actual Length
 * @retval      UX_SUCCESS          Upon successful
 *              Any other error code apart from UX_SUCCESS  Upon unsuccessful
 **********************************************************************************************************************/
static void apl_audio_write_change (UX_DEVICE_CLASS_AUDIO_STREAM * p_stream, ULONG alternate_setting)
{
    UINT ux_err              = UX_SUCCESS;

    if (USB_APL_ON == alternate_setting)
    {
        ux_err = ux_device_class_audio_frame_write(p_stream, g_write_buf, USB_MAX_PACKET_SIZE_IN);
        if (UX_SUCCESS == ux_err)
        {
            ux_err = ux_device_class_audio_transmission_start(p_stream);
            if (UX_SUCCESS != ux_err)
            {
                PRINT_ERR_STR("ux_device_class_audio_transmission_start operation failed");
            }
        }
        else
        {
            PRINT_ERR_STR("ux_device_class_audio_frame_write operation failed");
        }
    }
    else
    {
        if (USB_APL_ON == g_write_alternate_setting)
        {
            if(g_flag == VALUE_1)
            {
                PRINT_INFO_STR("USB write completed");
                g_flag = RESET_VALUE;
                g_write_counter = VALUE_0;
            }
        }
    }

    g_write_alternate_setting = alternate_setting;
}
/*******************************************************************************************************************//**
 * End of function apl_audio_write_change
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief       Callback function called when completing of IN transfer transmission
 * @param[IN]   p_stream            Pointer to UX_DEVICE_CLASS_AUDIO_STREAM structure
 *              actual_length       Actual Length
 * @retval      None
 **********************************************************************************************************************/
static void apl_audio_write_done (UX_DEVICE_CLASS_AUDIO_STREAM * p_stream, ULONG actual_length)
{
    static uint32_t write_counter = VALUE_0;
    FSP_PARAMETER_NOT_USED(actual_length);
    UINT ux_err                   = UX_SUCCESS;

    if (USB_APL_ON == g_write_alternate_setting)
    {
        ux_err=ux_device_class_audio_frame_write(p_stream, GLOBAL_BUFF + write_counter, USB_MAX_PACKET_SIZE_IN);
        if (ux_err != UX_SUCCESS)
        {
            PRINT_ERR_STR("ux_device_class_audio_frame_write is failed");
        }
        else
        {
            write_counter = write_counter + USB_MAX_PACKET_SIZE_IN;
            write_counter %= GBUFF_SIZE ;
            g_write_counter++;
            if(g_write_counter == VALUE_1)
            {
                PRINT_INFO_STR("USB write started");
                g_flag = VALUE_1;
            }
        }
    }
}
/*******************************************************************************************************************//**
 * End of function apl_audio_write_done
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief       Get instance
 * @param[in]   p_instance          Pointer to the area store the instance pointer
 * @retval      None
 **********************************************************************************************************************/
static void apl_audio_instance_activate (void * p_instance)
{
    g_p_audio = (UX_DEVICE_CLASS_AUDIO *) p_instance;
}
/*******************************************************************************************************************//**
 * End of function apl_audio_read_instance_activate
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief       Clear instance
 * @param[in]   p_instance          Pointer to area store the instance pointer
 * @retval      None
 **********************************************************************************************************************/
static void apl_audio_instance_deactivate (void * p_instance)
{
    FSP_PARAMETER_NOT_USED(p_instance);
    g_p_audio = UX_NULL;
}
/*******************************************************************************************************************//**
 * End of function apl_audio_read_instance_deactivate
 **********************************************************************************************************************/

#ifdef APL_AUDIO_20
/*******************************************************************************************************************//**
 * @brief       Audio20 control request processing
 * @param[in]   p_audio             Pointer to audio instance
 *              p_transfer          Pointer to UX_SLAVE_TRANSFER structure
 * @retval      UX_SUCCESS          Upon successful
 *              Any other error code apart from UX_SUCCESS  Upon unsuccessful
 **********************************************************************************************************************/
static UINT apl_audio20_request_process (UX_DEVICE_CLASS_AUDIO * p_audio, UX_SLAVE_TRANSFER * p_transfer)
{
    UINT    ux_err               = UX_SUCCESS;
    uint8_t number               = VALUE_0;

    ux_err = ux_device_class_audio20_control_process(p_audio, p_transfer, &g_audio20_control_group);
    if (UX_SUCCESS == ux_err)
    {
        /* Request handled, check changes */
        number = (uint8_t) g_audio20_control_group.ux_device_class_audio20_control_group_controls_nb;

        for (uint8_t index = RESET_VALUE; index < number; index++)
        {
            switch (g_audio20_control[index].ux_device_class_audio20_control_changed)
            {
                case UX_DEVICE_CLASS_AUDIO20_CONTROL_MUTE_CHANGED:
                {
                    g_control_mute[index] = g_audio20_control[index].ux_device_class_audio20_control_mute[INDEX_0];
                    break;
                }

                case UX_DEVICE_CLASS_AUDIO20_CONTROL_VOLUME_CHANGED:
                {
                    g_control_volume[index] = g_audio20_control[index].ux_device_class_audio20_control_volume[INDEX_0];
                    break;
                }

                default:
                {
                    break;
                }
            }
        }
    }
    else
    {
        PRINT_ERR_STR("ux_device_class_audio20_control_process failed");
    }

    return (UX_SUCCESS);
}
/*******************************************************************************************************************//**
 * End of function apl_audio20_request_process
 **********************************************************************************************************************/
#else /* APL_AUDIO_20 */
/*******************************************************************************************************************//**
 * @brief       Audio10 control request processing
 * @param[in]   p_audio             Pointer to audio instance
 *              p_transfer          Pointer to UX_SLAVE_TRANSFER structure
 * @retval      UX_SUCCESS          Upon successful
 *              Any other error code apart from UX_SUCCESS  Upon unsuccessful
 **********************************************************************************************************************/
static UINT apl_audio10_request_process (UX_DEVICE_CLASS_AUDIO * p_audio, UX_SLAVE_TRANSFER * p_transfer)
{
    UINT    ux_err               = UX_SUCCESS;
    uint8_t number               = VALUE_0;

    ux_err = ux_device_class_audio10_control_process(p_audio, p_transfer, &g_audio_control_group);
    if (UX_SUCCESS == ux_err)
    {
        /* Request handled, check changes */
        number = (uint8_t) g_audio_control_group.ux_device_class_audio10_control_group_controls_nb;

        for (uint8_t index = RESET_VALUE; index < number; index++)
        {
            switch (g_audio_control[index].ux_device_class_audio10_control_changed)
            {
                case UX_DEVICE_CLASS_AUDIO10_CONTROL_MUTE_CHANGED:
                {
                    g_control_mute[index] = g_audio_control[index].ux_device_class_audio10_control_mute[INDEX_0];
                    break;
                }

                case UX_DEVICE_CLASS_AUDIO10_CONTROL_VOLUME_CHANGED:
                {
                    g_control_volume[index] = g_audio_control[index].ux_device_class_audio10_control_volume[INDEX_0];
                    break;
                }

                default:
                {
                    break;
                }
            }
        }
    }
    else
    {
        PRINT_ERR_STR("ux_device_class_audio10_control_process failed");
    }
    return (UX_SUCCESS);
}
/*******************************************************************************************************************//**
 * End of function apl_audio10_request_process
 **********************************************************************************************************************/
#endif /* APL_AUDIO_20 */

/*******************************************************************************************************************//**
 * @} (end addtogroup usbx_paud_ep)
 **********************************************************************************************************************/

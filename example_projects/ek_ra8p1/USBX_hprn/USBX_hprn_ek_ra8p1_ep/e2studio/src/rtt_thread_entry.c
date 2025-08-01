/***********************************************************************************************************************
 * File Name    : rtt_thread_entry.c
 * Description  : Contains macros and functions used in rtt_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "rtt_thread.h"
#include "common_utils.h"
#include "usbx_hprn_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_hprn_ep
 * @{
 **********************************************************************************************************************/

/* Local functions */
static void process_rtt_op_msg(VOID);
static UINT memory_allocate_rtt(TX_BYTE_POOL *pool, rtt_msg_t **p_buf, uint32_t size);

extern TX_THREAD rtt_thread;

/* RTT_Thread entry function */
void rtt_thread_entry(void)
{
    UINT err = TX_SUCCESS;
    CHAR rtt_buffer[IP_MSG_MAX_SIZE]         = {'\0'};
    CHAR timestamp_info[TIME_STAMP_MSG_SIZE] = {'\0'};

    /* Block allocation will be done and address based message exchange happens */
    rtt_msg_t *p_data = NULL;

    /* Print the current time */
    snprintf(&timestamp_info[0], sizeof(timestamp_info), "Timestamp (Ticks) = 0x%08lx:\r\n", tx_time_get());

    APP_DBG_PRINT(timestamp_info);

    /* Initialize RTT framework */
    err = rtt_framework_init();
    if (TX_SUCCESS != err)
    {
        APP_PRINT("\r\nERROR: rtt_framework_init Failed\r\n");
        APP_ERR_TRAP(err);
    }

    while (1)
    {
        /* Check for user input */
        if (APP_CHECK_DATA)
        {
            UINT read_bytes = APP_READ(rtt_buffer);

            err = memory_allocate_rtt(&g_byte_pool, &p_data, sizeof(rtt_msg_t) + read_bytes);
            if (TX_SUCCESS != err)
            {
                APP_PRINT("Error in processing, please check again\r\n");
            }
            else
            {
                /* Send data to user thread */
                p_data->msg_id = RTT_INPUT_MESSAGE_TYPE_STR_QUEUED;
                p_data->msg_data_size = strlen(rtt_buffer)+1;
                memcpy(p_data->p_msg, rtt_buffer, p_data->msg_data_size);
#if (AZURE_RTOS == 1)
                tx_queue_send(&g_rtt_ip_data_queue, (rtt_msg_t *)&p_data, TX_WAIT_FOREVER);
#endif
                memset(rtt_buffer, '\0', sizeof(rtt_buffer));
            }
        }
        /* Print any message on RTT output if any sent by user thread
         * Here event based checking is there */
        process_rtt_op_msg();
#if (AZURE_RTOS == 1)
        /* Large sleep is needed to signal to higher priority threads */
        tx_thread_sleep (1);
#endif
    }
}

/*******************************************************************************************************************//**
 * @brief     This function receives messages from user thread through tx_queue_receive
 *            and display messages to RTT Viewer. Also released print message allocated block.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void process_rtt_op_msg(VOID)
{
#if (AZURE_RTOS == 1)
    UINT err = TX_SUCCESS;

    rtt_msg_t *rtt_op_data = NULL;
    CHAR timestamp_info[TIME_STAMP_MSG_SIZE] = {'\0'};

    /* Pump out all information to print */
    do
    {
        err = tx_queue_receive(&g_rtt_op_data_queue, (VOID *)&rtt_op_data, 1);

        if(err == TX_SUCCESS)
        {
            /* Print the current time */
            snprintf(&timestamp_info[0], sizeof(timestamp_info),\
                     "Timestamp (Ticks) = 0x%08lx:\r\n", (ULONG)rtt_op_data->msg_time);
            APP_DBG_PRINT(timestamp_info);

            /* Message received. Take action and release buffer */
            switch(rtt_op_data->msg_id)
            {
                case RTT_OUTPUT_MESSAGE_BANNER:
                {
                    /* Version get API for FLEX pack information */
                    fsp_pack_version_t version = {RESET_VALUE};
                    R_FSP_VersionGet(&version);
                    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
                              version.version_id_b.patch );
                    APP_PRINT(EP_INFO);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_INFO_STR:
                {
                    APP_PRINT("\r\nINFO: %s\r\n", rtt_op_data->p_msg);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_ERR_STR:
                {
                    APP_PRINT("\r\nERROR: %s\r\n", rtt_op_data->p_msg);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_ERR_TRAP:
                {
                    /* Assign error code to local buffer */
                    UINT err_val = *(UINT*)rtt_op_data->p_msg;
                    /* Halt the application */
                    APP_ERR_TRAP(err_val);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_NEXT_LINE:
                {
                    APP_PRINT("\r\n", rtt_op_data->p_msg[0]);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_PRINT_DATA:
                {
                    /* Print data from printer buffer to RTT/printer one by one */
                    APP_PRINT("%c", rtt_op_data->p_msg[0]);
                }
                break;

                default:
                {
                    APP_PRINT("\r\nRTT Printing Received Unknown Message ID");
                }
                break;
            }

            /* Send a newline and return carriage requesting flushing the buffer */
            if(rtt_op_data->msg_id != RTT_OUTPUT_MESSAGE_APP_PRINT_DATA)
            {
                APP_PRINT("\r\n");
            }

            /* Release print message allocated block */
            err= tx_byte_release((void *) rtt_op_data);
        }
    }while( err != TX_QUEUE_EMPTY );
#endif
    return;
}

/*******************************************************************************************************************//**
 * @brief     This function is called by User/Application Thread.
 *            This is called by each user thread wherever the user wishes to print message to RTT interface like
 *            RTT Viewer.
 * @param[IN] id        Message id
 * @param[IN] size      Message size
 * @param[IN] p_data    Message
 * @retval    Any other error code apart from TX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
UINT app_rtt_print_data(event_id_t id, uint32_t size, void * const p_data)
{
#if (AZURE_RTOS == 1)
    rtt_msg_t *p_display_data = NULL;
    UINT err = TX_SUCCESS;

    /* Allocate memory for RTT display message data structure */
    err = memory_allocate_rtt(&g_byte_pool, &p_display_data, sizeof(rtt_msg_t) + size);
    if (TX_SUCCESS != err)
    {
        APP_PRINT("\r\nError: memory_allocate_rtt failed.\r\n");
        rtt_thread_service_cleanup();
    }

    p_display_data->msg_id = id;
    p_display_data->msg_time = tx_time_get();
    p_display_data->msg_data_size = size;
    memcpy(p_display_data->p_msg, p_data, size);

    /* Send allocated address */
    err = tx_queue_send(&g_rtt_op_data_queue, (rtt_msg_t *)&p_display_data, TX_WAIT_FOREVER);
#endif
    return err;

}

/*******************************************************************************************************************//**
 * @brief     This function allocates memory for message transfer through queue.
 * @param[IN] pool        Pointer to byte memory pool
 * @param[IN] p_buf       Pointer to user display data.
 * @param[IN] size        Total size
 * @retval    Any other error code apart from TX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static UINT memory_allocate_rtt(TX_BYTE_POOL *pool, rtt_msg_t **p_buf, uint32_t size)
{
#if (AZURE_RTOS == 1)
    UINT err = TX_SUCCESS;

    rtt_msg_t *buf = NULL;

    /* Block allocate for user input */
    err = tx_byte_allocate(pool, (VOID **)&buf, size, TX_WAIT_FOREVER);

    if (TX_SUCCESS ==err)
    {
        /* Assign buffer address */
        *p_buf = buf;
    }
    return err;
#endif
}

/*******************************************************************************************************************//**
 * @brief      User input read function. To be called by user thread for receiving RTT input.
 *             message once received through queue (be it RTT sender or receiving queue) is being released.
 *             Note: sender queue for RTT thread is user input from RTT Viewer.
 *                   Receiver queue for RTT thread is when the user wants to print/display any message to RTT Viewer.
 * @param[OUT] p_get_data        Pointer to get data buffer
 * @retval     Any other error code apart from TX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
UINT check_for_rtt_user_ip(char *const p_get_data)
{
#if (AZURE_RTOS == 1)
    UINT err = TX_SUCCESS;

    rtt_msg_t *r_data1 = NULL;

    err = tx_queue_receive(&g_rtt_ip_data_queue, (VOID *)&r_data1 , 0xFF);
    if (TX_SUCCESS != err)
    {
        return err;
    }

    if ( (NULL != r_data1) && ( RTT_INPUT_MESSAGE_TYPE_STR_QUEUED == r_data1->msg_id) )
    {
        /* Copy data over here */
        memcpy(p_get_data, (CHAR *)r_data1->p_msg, r_data1->msg_data_size);
    }

    /* Release print message allocated block */
    err= tx_byte_release(r_data1);

    return err;
#endif
}

/*******************************************************************************************************************//**
 * @brief      Wait for the RTT Thread to initialize and sleep before sending data to it.
 *             Mandatory to call in user thread.
 * @param[IN]  None
 * @retval     None
 **********************************************************************************************************************/
VOID rtt_thread_init_check(VOID)
{
#if (AZURE_RTOS == 1)
    UINT err = TX_SUCCESS;

    thread_info_t rtt_thread_info =
    {
     .thread_ptr = &rtt_thread,
    };

    do{
        err = tx_thread_info_get(rtt_thread_info.thread_ptr,
                                 TX_NULL,
                                 &rtt_thread_info.state,
                                 &rtt_thread_info.run_count,
                                 TX_NULL,
                                 TX_NULL,
                                 TX_NULL,
                                 TX_NULL,
                                 TX_NULL);

        if( rtt_thread_info.run_count >= 1 )
        {
            break;
        }
        else
        {
            /* Sleep to allow other threads to run */
            tx_thread_sleep(1);
        }
    }while(err == TX_SUCCESS);
#endif
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usbx_hprn_ep)
 **********************************************************************************************************************/

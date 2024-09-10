/**********************************************************************************************************************
 * File Name    : rtt_thread_entry.c
 * Description  : Contains entry function of RTT Thread.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *********************************************************************************************************************/

#include <rtt_thread.h>
#include "sntp_client_thread.h"
#include "sntp_client_ep.h"
#include "common_utils.h"
#include "vir_time.h"

/**********************************************************************************************************************
 * Private local function prototypes
 *********************************************************************************************************************/
static void process_rtt_op_msg(VOID);
static UINT memory_allocate_rtt(TX_BYTE_POOL *pool, rtt_msg_t **p_buf, uint32_t size);

extern TX_THREAD rtt_thread;

/**********************************************************************************************************************
 * Function Name: rtt_thread_entry
 * Description  : This function runs rtt thread.
 * Arguments    : None
 * Return value : TX_SUCCESS   Upon successful create timer
 *                Any other Error code apart from TX_SUCCESS
 *********************************************************************************************************************/
void rtt_thread_entry(void)
{
    UINT err = TX_SUCCESS;
    CHAR rtt_buffer[IP_MSG_MAX_SIZE]         = {'\0'};
    CHAR timestamp_info[TIME_STAMP_MSG_SIZE] = {'\0'};

    /* Block allocation will be done and address based message exchange happens */
    rtt_msg_t *p_data = NULL;

    /* Print the current time */
    snprintf(&timestamp_info[0], sizeof(timestamp_info),"Timestamp (Ticks) = 0x%08lx:\r\n", tx_time_get());

    APP_DBG_PRINT(timestamp_info);

    /* Initialize rtt framework */
    err = rtt_framework_init();
    if (TX_SUCCESS != err)
    {
        APP_PRINT("\r\n ERROR: RTT framework init Failed\r\n");
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
                memcpy(p_data->p_msg,rtt_buffer,p_data->msg_data_size);
#if (BSP_CFG_RTOS == AZURE_RTOS)
                tx_queue_send(&g_rtt_ip_data_queue,(rtt_msg_t *)&p_data , TX_WAIT_FOREVER);
#endif

                memset(rtt_buffer,'\0',sizeof(rtt_buffer));
            }

        }

        /* Print any message on RTT output
         * if queue has data, then same
         * will be processed. */
        process_rtt_op_msg();
#if (BSP_CFG_RTOS == AZURE_RTOS)
        /* Large Sleep is needed to signal to higher priority threads.*/
        tx_thread_sleep (1);
#endif
    }
}
/**********************************************************************************************************************
 * End of function rtt_thread_entry
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: process_rtt_op_msg
 * Description  : This function receives messages from user thread through tx_queue_receive
 *                and display messages to RTT viewer. Also released print message allocated block.
 * Arguments    : None
 * Return value : TX_SUCCESS   Upon successful create timer
 *                Any other Error code apart from TX_SUCCESS
 *********************************************************************************************************************/
static void process_rtt_op_msg(VOID)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    UINT err = TX_SUCCESS;

    rtt_msg_t *rtt_op_data = NULL;
    CHAR timestamp_info[TIME_STAMP_MSG_SIZE] = {'\0'};

    /* Pump out all information to print */
    do
    {
        err = tx_queue_receive(&g_rtt_op_data_queue,(VOID *)&rtt_op_data , 1);

        if(err == TX_SUCCESS)
        {
            /* Print the current time */
            snprintf(&timestamp_info[0], sizeof(timestamp_info),"Timestamp (Ticks) = 0x%08lx:\r\n", rtt_op_data->msg_time);

            APP_DBG_PRINT(timestamp_info);

            /* Message received. Take action and release buffer */
            switch(rtt_op_data->msg_id)
            {
                case RTT_OUTPUT_MESSAGE_BANNER:
                {
                    /* Version get API for FLEX pack information */
                    fsp_pack_version_t version = {RESET_VALUE};
                    R_FSP_VersionGet(&version);
                    APP_PRINT(BANNER_INFO,EP_VERSION,version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch );
                    APP_PRINT(EP_INFO);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_INFO_STR:
                {
                    APP_PRINT("\r\nINFO : %s",rtt_op_data->p_msg);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_ERR_STR:
                {
                    APP_PRINT("\r\nERROR : %s",rtt_op_data->p_msg);
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

                case RTT_OUTPUT_MESSAGE_APP_GET_SERVER_STR:
                {
                    UCHAR server_url[SERVER_URL_LEN] = {'\0'};
                    memcpy(server_url, (UCHAR*)rtt_op_data->p_msg, sizeof(server_url));
                    APP_PRINT("\r\nINFO : Getting server IP address from %s URL...",server_url);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_PRINT_SERVER_STR:
                {
                    UCHAR server_url[SERVER_URL_LEN] = {'\0'};
                    memcpy(server_url, (UCHAR*)rtt_op_data->p_msg, sizeof(server_url));
                    APP_PRINT("\r\nINFO : server IP address acquired from the SNTP URL:  %s successfully!",server_url);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_PRINT_DATE_TIME:
                {
                    vtime_t print_time = {RESET_VALUE};
                    memcpy(&print_time, (vtime_t *)rtt_op_data->p_msg, sizeof(vtime_t));
                    /* Print Date and Time in user readable format */
                    APP_PRINT("Date (DD/MM/YYYY): %02d/%02d/%d \nTime (HH:MM:SS): %02d : %02d : %02d \r\n", print_time.tm_mday,
                            print_time.tm_mon , print_time.tm_year, print_time.tm_hour,
                            print_time.tm_min, print_time.tm_sec);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_IP:
                {
                    union IP client_ip;
                    char ipv4_address[64] = {'\0'};
                    /* Assign ipv4 address to local buffer */
                    ULONG client_addr = *(ULONG*)rtt_op_data->p_msg;
                    client_ip.ip_address = client_addr;
                    sprintf(ipv4_address, "%d.%d.%d.%d", client_ip.IP_t.ip_a, client_ip.IP_t.ip_b, client_ip.IP_t.ip_c, client_ip.IP_t.ip_d);
                    APP_PRINT("\r\nINFO : SNTP Client IP address is : %s", ipv4_address);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_PRINT_SERVER_IP:
                {
                    union IP server_ip;
                    char ipv4_address[64] = {'\0'};
                    /* Assign ipv4 address to local buffer */
                    ULONG server_addr = *(ULONG*)rtt_op_data->p_msg;
                    server_ip.ip_address = server_addr;
                    sprintf(ipv4_address, "%d.%d.%d.%d", server_ip.IP_t.ip_a, server_ip.IP_t.ip_b, server_ip.IP_t.ip_c, server_ip.IP_t.ip_d);
                    APP_PRINT("\r\nINFO : SNTP Server IP address is : %s", ipv4_address);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_PRINT_DNS_SERVER_IP:
                {
                    union IP dns_server_ip;
                    char ipv4_address[64] = {'\0'};
                    /* Assign ipv4 address to local buffer */
                    ULONG dns_server_addr = *(ULONG*)rtt_op_data->p_msg;
                    dns_server_ip.ip_address = dns_server_addr;
                    sprintf(ipv4_address, "%d.%d.%d.%d", dns_server_ip.IP_t.ip_a, dns_server_ip.IP_t.ip_b, dns_server_ip.IP_t.ip_c, dns_server_ip.IP_t.ip_d);
                    APP_PRINT("\r\nINFO : Successfully pinged DNS Server : %s", ipv4_address);
                }
                break;

                default:
                {
                    APP_PRINT("\r\n RTT Printing Received Unknown Message ID");
                }
                break;
            }

            /* Send a newline and return carriage requesting flushing the buffer */
            APP_PRINT("\r\n");

            /* Release print message allocated block */
            err= tx_byte_release((void *) rtt_op_data);
        }
    }while( err != TX_QUEUE_EMPTY );
#endif
    return;
}
/**********************************************************************************************************************
 * End of function process_rtt_op_msg
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: app_rtt_print_data
 * Description  : This function is called by User/Application Thread.
 *                This is called by each user thread wherever user wishes to print message to RTT interface like RTT viewer.
 * Arguments    : id        message id
 *                size      message size
 *                p_data    message
 * Return value : Any Other Error code apart from TX_SUCCESS on Unsuccessful operation.
 *********************************************************************************************************************/
UINT app_rtt_print_data(event_id_t id, uint32_t size, void * const p_data)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    rtt_msg_t *p_display_data = NULL;
    UINT err = TX_SUCCESS;

    /* Allocate memory for rtt display message data structure */
    err = memory_allocate_rtt(&g_byte_pool,
                              &p_display_data,
                              sizeof(rtt_msg_t) + size);
    if (TX_SUCCESS != err)
    {
        APP_PRINT("\r\nError : memory_allocate_rtt failed.\r\n");
        rtt_thread_service_cleanup();
    }

    p_display_data->msg_id = id;
    p_display_data->msg_time = tx_time_get();
    p_display_data->msg_data_size = size;
    memcpy(p_display_data->p_msg, p_data, size);

    /* Send Allocated address */
    err = tx_queue_send(&g_rtt_op_data_queue,
                        (rtt_msg_t *)&p_display_data,
                        TX_WAIT_FOREVER);
    return err;
#endif
}
/**********************************************************************************************************************
 * End of function app_rtt_print_data
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: memory_allocate_rtt
 * Description  : This function allocates memory for message transfer through queue.
 * Arguments    : pool        pointer to byte memory pool
 *                p_buf       pointer to user display data
 *                size        total size
 * Return value : Any Other Error code apart from TX_SUCCESS on Unsuccessful operation.
 *********************************************************************************************************************/
static UINT memory_allocate_rtt(TX_BYTE_POOL *pool, rtt_msg_t **p_buf, uint32_t size)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
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
/**********************************************************************************************************************
 * End of function memory_allocate_rtt
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: check_for_rtt_user_ip
 * Description  : user input read function. To be called by user thread for receiving RTT input.
 *                message once received through queue (be it RTT sender or receiving queue) is being released.
 *                Note : sender queue for RTT thread is user input from RTT viewer.
 *                       Receiver queue for RTT thread is when user wants to print/display any message to RTT viewer.
 * Arguments    : p_get_data        pointer to get data buffer
 * Return value : Any Other Error code apart from TX_SUCCESS on Unsuccessful operation.
 *********************************************************************************************************************/
UINT check_for_rtt_user_ip(uint8_t * p_get_data)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    UINT err = TX_SUCCESS;

    rtt_msg_t *r_data1 = NULL;

    err = tx_queue_receive(&g_rtt_ip_data_queue,(VOID *)&r_data1 , 0xFF);
    if (TX_SUCCESS != err)
    {
        return err;
    }

    if ( (NULL != r_data1) && ( RTT_INPUT_MESSAGE_TYPE_STR_QUEUED == r_data1->msg_id) )
    {
        /* Conversion from input string to integer value */
        *p_get_data =  (uint8_t)(atoi((char *)r_data1->p_msg));
    }

    /* Release print message allocated block */
    err= tx_byte_release(r_data1);

    return err;
#endif
}
/**********************************************************************************************************************
 * End of function check_for_rtt_user_ip
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: rtt_thread_init_check
 * Description  : Wait for the RTT Thread to initialize and sleep before sending data to it.
 *                Mandatory to call in user thread.
 * Arguments    : None
 * Return value : None
 *********************************************************************************************************************/
VOID rtt_thread_init_check(VOID)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    UINT err = TX_SUCCESS;

    thread_info_t rtt_thread_info =
    {
     .thread_ptr = &rtt_thread,
    };

    do{
        err = tx_thread_info_get(rtt_thread_info.thread_ptr,
                                 TX_NULL ,
                                 &rtt_thread_info.state,
                                 &rtt_thread_info.run_count,
                                 TX_NULL ,
                                 TX_NULL ,
                                 TX_NULL ,
                                 TX_NULL ,
                                 TX_NULL );

        if( rtt_thread_info.run_count >= 1 )
        {
            break;
        }
        else
        {
            /* Let go of program control */
            tx_thread_sleep(1);
        }
    }while(err==TX_SUCCESS);
#endif
}
/**********************************************************************************************************************
 * End of function rtt_thread_init_check
 *********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name    : terminal.c
 * Description  : Contains data structures and functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "terminal.h"
#include "common_utils.h"

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
#if (USE_VIRTUAL_COM == 1)
static volatile uint8_t g_rx_count = RESET_VALUE;
static uint8_t g_tx_buffer [TX_MAX_SIZE] = {RESET_VALUE};
#endif /* USE_VIRTUAL_COM */
static uint8_t g_rx_buffer [RX_MAX_SIZE] = {RESET_VALUE};
static TX_BYTE_POOL g_terminal_byte_pool;
static uint8_t g_byte_pool_memory [TERMINAL_BYTE_POOL_SIZE] = {RESET_VALUE};

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/
extern TX_THREAD terminal_thread;

#if (USE_VIRTUAL_COM == 1)
/*******************************************************************************************************************//**
 *  @brief      SCI UART callback function.
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void serial_callback(uart_callback_args_t *p_args)
{
    /* Check correct the SCI UART channel */
    if (g_serial_cfg.channel == p_args->channel)
    {
        /* Handle by the SCI UART event */
        switch (p_args->event)
        {
            case UART_EVENT_TX_COMPLETE:
                tx_event_flags_set(&g_terminal_event_flag, UART_EVENT_TX_COMPLETE, TX_OR);
                break;

            case UART_EVENT_RX_CHAR:
                switch(p_args->data)
                {
                    /* If Enter is pressed by the user, set flag to process the data */
                    case CHAR_CR:
                        if (RESET_VALUE != g_rx_count)
                        {
                            g_rx_count = RESET_VALUE;
                            tx_event_flags_set(&g_terminal_event_flag, UART_EVENT_RX_COMPLETE, TX_OR);
                        }
                        break;

                    /* Read all data provided by the user until enter button is pressed */
                    default:
                        if (RX_MAX_SIZE > g_rx_count)
                        {
                            g_rx_buffer[g_rx_count ++] = (uint8_t)p_args->data;
                        }
                        break;
                }
                break;

            default :
                break;
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief      This function will initialize the SCI UART module which transfer the EP log to the host PC.
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation.
 *  @retval     Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
UINT serial_init(void)
{
    UINT status = (UINT)FSP_SUCCESS;
    /* Initialize SCI UART channel with baud rate 115200 */
#if (BSP_PERIPHERAL_SCI_PRESENT)
    status = (UINT)R_SCI_UART_Open(&g_serial_ctrl, &g_serial_cfg);
#else
    status = (UINT)R_SCI_B_UART_Open(&g_serial_ctrl, &g_serial_cfg);
#endif

    return status;
}

/*******************************************************************************************************************//**
 *  @brief      This function will de-initialize SCI UART module.
 *  @param      None
 *  @retval     None
 **********************************************************************************************************************/
void serial_deinit(void)
{
    /* De-initialize SCI UART if it opened */
    if (g_serial_ctrl.open)
    {
#if (BSP_PERIPHERAL_SCI_PRESENT)
        R_SCI_UART_Close(&g_serial_ctrl);
#else
        R_SCI_B_UART_Close(&g_serial_ctrl);
#endif
    }
}
#endif

/*******************************************************************************************************************//**
 *  @brief      This function allocates heap memory for the terminal frame.
 *  @param      None
 *  @retval     None
 **********************************************************************************************************************/
UINT terminal_framework_init(void)
{
    UINT status = TX_SUCCESS;

    /* Create a memory byte pool for the terminal framework */
    status = tx_byte_pool_create(&g_terminal_byte_pool, TERMINAL_BYTE_POOL_NAME,
                                 (VOID *)g_byte_pool_memory, TERMINAL_BYTE_POOL_SIZE);

    return status;
}

/*******************************************************************************************************************//**
 *  @brief      This function will transfer a message to the host PC.
 *  @param[in]  p_buf          Pointer to message buffer.
 *  @retval     FSP_SUCCESS    Upon successful operation.
 *  @retval     Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
UINT terminal_printf(char * format, ...)
{
    UINT    status  = TX_SUCCESS;
    INT     ret     = RESET_VALUE;
    va_list aptr;

    /* Initialize the argument list */
    va_start(aptr, format);

    /* Write a formatted string to a buffer using argument list */
#if (USE_VIRTUAL_COM == 1)
    ret = vsprintf((char *)g_tx_buffer, format, aptr);
#else
    ret = SEGGER_RTT_vprintf(SEGGER_INDEX, format, &aptr);
#endif

    /* Cleanup for an APTR object initialized by va_start */
    va_end(aptr);

    /* Check VSPRINTF function return code */
    if (RESET_VALUE > ret)
    {
        return (UINT)FSP_ERR_INVALID_ARGUMENT;
    }

#if (USE_VIRTUAL_COM == 1)
    ULONG   event   = RESET_VALUE;
    ULONG   wait    = RESET_VALUE;
    INT     len     = RESET_VALUE;

    /* Get the message length of the transmit buffer */
    len = strlen((char *)g_tx_buffer);
    if (RESET_VALUE == len)
    {
        return (UINT)FSP_ERR_INVALID_SIZE;
    }

    /* Set the waiting time value according to the message length */
    wait = (ULONG)len;

    /* Clean UART_EVENT_TX_COMPLETE event flag */
    status = tx_event_flags_set(&g_terminal_event_flag, ~UART_EVENT_TX_COMPLETE, TX_AND);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    /* Write message to UART interface */
#if (BSP_PERIPHERAL_SCI_PRESENT)
    status = (UINT)R_SCI_UART_Write(&g_serial_ctrl, g_tx_buffer, len);
#else
    status = (UINT)R_SCI_B_UART_Write(&g_serial_ctrl, g_tx_buffer, len);
#endif
    if ((UINT)FSP_SUCCESS != status)
    {
        return status;
    }

    /* Waiting for UART_EVENT_TX_COMPLETE event */
    status = tx_event_flags_get(&g_terminal_event_flag, UART_EVENT_TX_COMPLETE, TX_OR_CLEAR, &event, wait);
    if (TX_SUCCESS != status)
    {
        return status;
    }
#endif /* USE_VIRTUAL_COM */

    return status;
}

/*******************************************************************************************************************//**
 * @brief      This function gets and converts data to an integer from the terminal input queue.
 *             To be called by the User/Application Thread.
 * @param[out] p_data    Pointer to data buffer
 * @retval     Any other error code apart from TX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
UINT terminal_get_input_queue(uint8_t * p_data)
{
    UINT status = TX_SUCCESS;
    terminal_msg_t * p_terminal_msg = NULL;

    /* Waiting for an input queue */
    status = tx_queue_receive(&g_terminal_ip_queue, (VOID *)&p_terminal_msg , TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    if (TERMINAL_INPUT_MESSAGE == p_terminal_msg->id)
    {
        /* Conversion from input string to integer value */
        *p_data =  (uint8_t)(atoi(p_terminal_msg->msg));
    }

    /* Free the memory allocated for this terminal frame */
    status = tx_byte_release(p_terminal_msg);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function prepares the terminal message structure and sends it to the terminal output queue.
 * @param[in] id        Message id.
 * @param[in] size      Message size.
 * @param[in] p_data    Pointer to the message string.
 * @retval    Any other error code apart from TX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
UINT terminal_send_output_queue(terminal_event_t id, uint32_t size, void * const p_data)
{
    UINT status = TX_SUCCESS;
    terminal_msg_t * p_terminal_msg = NULL;

    /* Allocate memory for terminal message structure and input message */
    status = tx_byte_allocate(&g_terminal_byte_pool, (VOID **)&p_terminal_msg,\
                              sizeof(terminal_msg_t) + size, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    /* Prepare the terminal message structure */
    p_terminal_msg->id = id;
    p_terminal_msg->time = tx_time_get();
    p_terminal_msg->size = size;
    memcpy(p_terminal_msg->msg, p_data, size);

    /* Send the terminal message pointer to output queue */
    status = tx_queue_send(&g_terminal_op_queue, (VOID *)&p_terminal_msg, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    return TX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function takes input data from the PC then puts it into the terminal input queue.
 * @param[in] None
 * @retval    Any other error code apart from TX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
UINT terminal_input_handle(void)
{
    UINT status = TX_SUCCESS;
    terminal_msg_t * p_terminal_msg = NULL;

#if (USE_VIRTUAL_COM == 1)
    ULONG event = RESET_VALUE;

    /* Check RX event to detect a new input from UART interface */
    tx_event_flags_get(&g_terminal_event_flag, UART_EVENT_RX_COMPLETE, TX_OR_CLEAR, &event, TX_NO_WAIT);
    if (TX_SUCCESS != status && TX_NO_EVENTS != status)
    {
        return status;
    }

    /* Handle terminal input */
    if(UART_EVENT_RX_COMPLETE == (event & UART_EVENT_RX_COMPLETE))
    {
#else
    if (APP_CHECK_DATA)
    {
        APP_READ(g_rx_buffer);
#endif
        /* Allocate memory for terminal message structure and input message */
        status = tx_byte_allocate(&g_terminal_byte_pool, (VOID **)&p_terminal_msg,\
                                  sizeof(terminal_msg_t) + sizeof(g_rx_buffer), TX_WAIT_FOREVER);

        if (TX_SUCCESS != status)
        {
            terminal_printf("\r\nERROR: tx_byte_allocate failed\r\n");
            return status;
        }

        /* Copy terminal receive buffer to terminal message buffer */
        memcpy(p_terminal_msg->msg, g_rx_buffer, sizeof(g_rx_buffer));

        /* Clean terminal receive buffer for new input */
        memset(g_rx_buffer, RESET_VALUE, sizeof(g_rx_buffer));

        /* Set terminal message structure information */
        p_terminal_msg->id = TERMINAL_INPUT_MESSAGE;
        p_terminal_msg->time = tx_time_get();
        p_terminal_msg->size = sizeof(terminal_msg_t) + sizeof(g_rx_buffer);

        /* Send terminal message structure pointer to input queue */
        status = tx_queue_send(&g_terminal_ip_queue, (VOID *)&p_terminal_msg, TX_WAIT_FOREVER);
        if (TX_SUCCESS != status)
        {
            terminal_printf("\r\nERROR: tx_queue_send failed\r\n");
            return status;
        }
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function transfers a message to the PC based on the terminal frame taken from the terminal
 *            output queue.
 * @param[in] None
 * @retval    Any other error code apart from TX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
UINT terminal_output_handle(void)
{
    UINT status = TX_SUCCESS;
    UINT err_code = RESET_VALUE;
    terminal_msg_t * p_terminal_msg = NULL;
    fsp_pack_version_t version = {RESET_VALUE};
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,\
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,\
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}\
                         };
    dir_property_t dir = {.file = RESET_VALUE, .size = RESET_VALUE, .subdir = RESET_VALUE};

    while (true)
    {
        /* Retrieve a message from output queue */
        status = tx_queue_receive(&g_terminal_op_queue, (VOID *)&p_terminal_msg, TX_NO_WAIT);
        if (TX_SUCCESS == status)
        {
            switch (p_terminal_msg->id)
            {
                case TERMINAL_OUTPUT_EP_BANNER:
                    /* Get FSP package information */
                    R_FSP_VersionGet(&version);

                    /* Print the EP banner to terminal application on PC */
                    terminal_printf(BANNER_INFO, EP_VERSION, version.version_id_b.major,\
                                    version.version_id_b.minor, version.version_id_b.patch);
                    break;

                case TERMINAL_OUTPUT_EP_INFO:
                    /* Print the EP information to terminal application on PC */
                    terminal_printf(EP_INFO);
                    break;

                case TERMINAL_OUTPUT_APP_INFO_STR:
                    terminal_printf("%s", p_terminal_msg->msg);
                    break;

                case TERMINAL_OUTPUT_APP_ERR_STR:
                    terminal_printf("\r\nERROR: %s\r\n", p_terminal_msg->msg);
                    break;

                case TERMINAL_OUTPUT_APP_ERR_TRAP:
                    err_code = *(UINT*)p_terminal_msg->msg;
                    terminal_printf("\r\nReturned error code: 0x%x\r\n", err_code);
                    APP_ERR_TRAP(err_code);
                    break;

                case TERMINAL_OUTPUT_APP_MENU:
                    terminal_printf(MENU_OPTION);
                    break;

                case TERMINAL_OUTPUT_APP_ENTRY_INFO:
                    entry = *(entry_info_t*)p_terminal_msg->msg;
                    if (FX_DIRECTORY == (entry.attr & FX_DIRECTORY))
                    {
                        /* Print directory information */
                        terminal_printf("%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %s %s\r\n",
                                  entry.time.month, entry.time.date, entry.time.year,
                                  entry.time.hour, entry.time.min, entry.time.sec,
                                  "<DIR>          ", entry.name);
                    }
                    else if (FX_ARCHIVE == (entry.attr & FX_ARCHIVE))
                    {
                        /* Print file information */
                        terminal_printf("%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %15d %s\r\n",
                                  entry.time.month, entry.time.date, entry.time.year,
                                  entry.time.hour, entry.time.min, entry.time.sec,
                                  entry.size, entry.name);
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    break;

                case TERMINAL_OUTPUT_APP_DIR_PROPERTY:
                    dir = *(dir_property_t*)p_terminal_msg->msg;
                    /* Print directory property */
                    terminal_printf("         %6u File(s) %16u bytes\r\n", dir.file, dir.size);
                    terminal_printf("         %6u Dir(s)\r\n\r\n", dir.subdir);
                break;

                default:
                    break;
            }

            /* Release terminal message allocated block */
            status = tx_byte_release((void *)p_terminal_msg);
            if (TX_SUCCESS != status)
            {
                /* Print a error message to terminal application on PC */
                terminal_printf("\r\nERROR: tx_byte_release failed\r\n");
                return status;
            }
        }

        /* In case queue is empty*/
        else if (TX_QUEUE_EMPTY == status)
        {
            return TX_SUCCESS;
        }

        /* In case the API is error */
        else
        {
            /* Print a error message to terminal application on PC */
            terminal_printf("\r\nERROR: tx_queue_receive failed\r\n");
            return status;
        }
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief      This function waits for the terminal thread to initialize and sleeps before sending data to it.
 * @param[in]  None
 * @retval     None
 **********************************************************************************************************************/
UINT terminal_thread_init_check(void)
{
    UINT status = TX_SUCCESS;
    ULONG run_count = RESET_VALUE;

    /* Get terminal thread run count */
    while (true)
    {
        status = tx_thread_info_get(&terminal_thread,
                                    TX_NULL,
                                    TX_NULL,
                                    &run_count,
                                    TX_NULL,
                                    TX_NULL,
                                    TX_NULL,
                                    TX_NULL,
                                    TX_NULL);
        if (TX_SUCCESS != status)
        {
            return status;
        }

        if (RESET_VALUE < run_count)
        {
            return TX_SUCCESS;
        }

        tx_thread_sleep(ONE_SLEEP_TICK);
    }
}

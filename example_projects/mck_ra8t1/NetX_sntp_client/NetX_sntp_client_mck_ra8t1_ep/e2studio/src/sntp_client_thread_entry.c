/**********************************************************************************************************************
 * File Name    : sntp_client_thread_entry.c
 * Description  : Contains functions called from the sntp_client main thread
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *********************************************************************************************************************/

#include "sntp_client_thread.h"
#include "common_utils.h"
#include "sntp_client_ep.h"
#include "vir_time.h"
/**********************************************************************************************************************
 * @addtogroup NetX_sntp_client_ep
 * @{
 *********************************************************************************************************************/

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory) */
NX_PACKET_POOL g_packet_pool0;
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;

/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0 */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0 */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* DHCP instance */
NX_DHCP g_dhcp_client0;

/* DNS instance */
NX_DNS g_dns0;

/* SNTP client instance */
NX_SNTP_CLIENT g_sntp_client0;

/* Private function declarations */
static UINT ip0_init(void);
static UINT packet_pool0_init(void);
static UINT dhcpv4_client_init(ULONG * dns_server_addr);
static UINT dns_client_init(ULONG dns_server_addr);
static UINT sntp_client_init(void);
static void sntp_client_deinit(void);
static UINT network_init(void);
static UINT sntp_client_run(sntp_client_mode_t mode, UCHAR * server_url);
static UINT get_print_sntp_time(void);
static UINT fetch_ntp_server_ip_address(ULONG *server_addr, UCHAR * server_url);
VOID time_update_callback(NX_SNTP_TIME_MESSAGE * time_update_ptr, NX_SNTP_TIME * local_time);

/**********************************************************************************************************************
 * Function Name: sntp_client_thread_entry
 * Description  : SNTP Client Thread entry function.
 * Arguments    : None
 * Return value : NX_SUCCESS   Upon  successfully
 *                Any Other Error code apart from NX_SUCCESS
 *********************************************************************************************************************/
void sntp_client_thread_entry(void)
{
    /* To capture the status(Success/Failure) of each Function/API */
    UINT      status        = NX_SUCCESS;
    UINT      err           = SUCCESS;

    /* Variable to capture retry count */
    int8_t   retry_count    = SERVER_URL_MAX_VAL;

    /* Variable to capture sntp time update event */
    ULONG actual_events            = RESET_VALUE;

    /* Buffer to store the ntp server pool */
    UCHAR * server_url_str[SERVER_URL_MAX_VAL] = {(UCHAR *)"0.pool.ntp.org",
                                                  (UCHAR *)"1.pool.ntp.org",
                                                  (UCHAR *)"2.pool.ntp.org",
                                                  (UCHAR *)"3.pool.ntp.org"};

    /* Initialize the RTT Thread */
    rtt_thread_init_check();

    /* Print EP and Banner info */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Initialize the timer and display MCU's system time */
    err = timer_init_and_display_time();
    if(SUCCESS != err)
    {
        PRINT_ERR_STR("timer_init_and_display_time function Failed.");
        ERROR_TRAP(err);
    }

    /* Initialize the network stack and print assigned IP address, obtained using DHCP */
    status = network_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("network_init function Failed.");
        ERROR_TRAP(status);
    }

    /* Initialize SNTP client */
    status = sntp_client_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("sntp_client_init function Failed.");
        ERROR_TRAP(status);
    }

    /* Initialize SNTP related services for 1st server from ntp pool server list */
    status = sntp_client_run(INIT_MODE, server_url_str[RESET_VALUE]);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("sntp_client_run function Failed.");

        /* Delete the SNTP client service */
        sntp_client_deinit();
        ERROR_TRAP(status);
    }

    /* Wait for the server update event */
    status = tx_event_flags_get(&g_sntp_flags, SNTP_UPDATE_EVENT, TX_OR_CLEAR, &actual_events, INIT_MODE_EVENT_WAIT_TIME);

    /* Check if we have valid event */
    if ((TX_SUCCESS == status) && (SNTP_UPDATE_EVENT == actual_events))
    {
        PRINT_INFO_STR("SNTP time update event received.");

        /* Receive and process network time */
        status = get_print_sntp_time();
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("get_print_sntp_time function Failed.");

            /* Delete the SNTP client service */
            sntp_client_deinit();
            ERROR_TRAP(status);
        }
    }
    else
    {
        PRINT_INFO_STR("no network time received in init mode -->  ## retry mode on");

        /* Retry for all available servers from server list */
        while (-- retry_count)
        {
            /* Initialize SNTP related services in retry mode */
            status = sntp_client_run(RETRY_MODE, server_url_str[retry_count]);
            if(NX_SUCCESS != status)
            {
                PRINT_ERR_STR("sntp_client_run function Failed.");

                /* Delete the SNTP client service */
                sntp_client_deinit();
                ERROR_TRAP(status);
            }

            /* Reset the event variable */
            actual_events = RESET_VALUE;

            /* Wait for the server update event */
            status = tx_event_flags_get(&g_sntp_flags, SNTP_UPDATE_EVENT, TX_OR_CLEAR, &actual_events, RETRY_MODE_EVENT_WAIT_TIME);
            if ((TX_SUCCESS == status) && (SNTP_UPDATE_EVENT == actual_events))
            {
                PRINT_INFO_STR("SNTP time update event received in retry mode.");

                /* Receive and process network time */
                status = get_print_sntp_time();
                if(NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("get_print_sntp_time function Failed.");

                    /* Delete the SNTP client service */
                    sntp_client_deinit();
                    ERROR_TRAP(status);
                }
                break;
            }
        }
        /* Check for all available ntp servers tried in retry mode */
        if(RESET_VALUE >= retry_count)
        {
            PRINT_INFO_STR("No time update received in retry mode.");
        }
    }

    /* Stop SNTP client service */
    status = nx_sntp_client_stop(&g_sntp_client0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_stop API Failed.");

        /* Delete the SNTP client service */
        sntp_client_deinit();
        ERROR_TRAP(status);
    }

    /* Delete the SNTP client service */
    sntp_client_deinit();
    PRINT_INFO_STR("Stopped and Deleted SNTP client services.");
    PRINT_INFO_STR("SNTP client EP completed.");

    while(true)
    {
        tx_thread_sleep(1);
    }
}
/**********************************************************************************************************************
 * End of function sntp_client_thread_entry
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: packet_pool0_init
 * Description  : This function creates the packet pool.
 * Arguments    : None
 * Return value : NX_SUCCESS   Upon  successfully
 *                Any Other Error code apart from NX_SUCCESS
 *********************************************************************************************************************/
static UINT packet_pool0_init(void)
{
    /* To capture the status(Success/Failure) of each Function/API */
    UINT status = NX_SUCCESS;

    /* Create the packet pool */
    status = nx_packet_pool_create(&g_packet_pool0,
                                   "g_packet_pool0 Packet Pool",
                                   G_PACKET_POOL0_PACKET_SIZE,
                                   &g_packet_pool0_pool_memory[0],
                                   G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));

    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_pool_create API Failed.");
    }
    return status;
}
/**********************************************************************************************************************
 * End of function packet_pool0_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ip0_init
 * Description  : This function creates the ip instance and enables the ARP, UPD, ICMP.
 * Arguments    : None
 * Return value : NX_SUCCESS   Upon  successfully
 *                Any Other Error code apart from NX_SUCCESS
 *********************************************************************************************************************/
static UINT ip0_init(void)
{
    /* To capture the status(Success/Failure) of each Function/API */
    UINT status = NX_SUCCESS;

    /* Create the ip instance */
    status = nx_ip_create(&g_ip0,
                          "g_ip0 IP Instance",
                          G_IP0_ADDRESS,
                          G_IP0_SUBNET_MASK,
                          &g_packet_pool0,
                          g_netxduo_ether_0,
                          &g_ip0_stack_memory[0],
                          G_IP0_TASK_STACK_SIZE,
                          G_IP0_TASK_PRIORITY);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_create API Failed.");
        return status;
    }

    /* Enable the ARP */
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[RESET_VALUE], G_IP0_ARP_CACHE_SIZE);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_arp_enable API Failed.");
        return status;
    }

    /* Enable the UDP */
    status = nx_udp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_udp_enable API Failed.");
        return status;
    }

    /* Enable the Internet Control Message Protocol */
    status = nx_icmp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_icmp_enable API Failed.");
        return status;
    }
    PRINT_INFO_STR("Checking for Ethernet link...");

    /* Check the connection status */
    ULONG current_state;
    status = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, LINK_ENABLE_WAIT_TIME);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_status_check API Failed.");
        return status;
    }
    PRINT_INFO_STR("Ethernet Link is Up!");
    return status;
}
/**********************************************************************************************************************
 * End of function ip0_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: dhcpv4_client_init
 * Description  : This function initializes the dhcpv4 client by creating the dhcp client instance
 *                and allocates required the packet pool for it. DHCP server assigns the client ip
 *                address after server starts running successfully.
 * Arguments    : *dns_server_addr   DNS server address.
 * Return value : NX_SUCCESS   Upon  successfully
 *                Any Other Error code apart from NX_SUCCESS
 *********************************************************************************************************************/
static UINT dhcpv4_client_init(ULONG * dns_server_addr)
{
    /* To capture the status(Success/Failure) of each Function/API */
    UINT  status                         = NX_SUCCESS;
    ULONG requested_status               = RESET_VALUE;
    UCHAR dns_buffer[4*MAX_DNS_SERVERS]  = {NULL_CHAR};
    UINT dns_buffer_size                 = IP_V4_SIZE;
    ULONG *dns_server_ptr                = NULL;

    /* Create the DHCP instance */
    status = nx_dhcp_create (&g_dhcp_client0, &g_ip0, "g_dhcp_client0");
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_create API Failed.");
        return status;
    }

    /* Set the DHCP Packet Pool */
    status = nx_dhcp_packet_pool_set (&g_dhcp_client0, &g_packet_pool0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_packet_pool_set API Failed.");
        return status;
    }

    /* Start DHCP service */
    status = nx_dhcp_start (&g_dhcp_client0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_start API Failed.");
        return status;
    }

    /* Wait until an IP address is acquired via DHCP */
    status = nx_ip_status_check (&g_ip0, NX_IP_ADDRESS_RESOLVED, &requested_status, LINK_ENABLE_WAIT_TIME);
    if (NX_IP_ADDRESS_RESOLVED != requested_status)
    {
        PRINT_ERR_STR("Error in acquiring the IP address.");
        return status;
    }
    PRINT_INFO_STR("Acquired the IP address via DHCP successfully!");

    /* Query the DHCP Client for the DNS Server address */
    status = nx_dhcp_user_option_retrieve(&g_dhcp_client0, NX_DHCP_OPTION_DNS_SVR, dns_buffer, &dns_buffer_size);
    while(NX_DHCP_DEST_TO_SMALL == status)
    {
        if((MAX_DNS_SERVERS*IP_V4_SIZE) > dns_buffer_size)
        {
            dns_buffer_size = dns_buffer_size+IP_V4_SIZE;
            status = nx_dhcp_user_option_retrieve(&g_dhcp_client0, NX_DHCP_OPTION_DNS_SVR, dns_buffer, &dns_buffer_size);
        }
        else
        {
            break;
        }
    }

    if(NX_SUCCESS == status)
    {
        dns_server_ptr = (ULONG *)(dns_buffer);
        for(uint8_t cnt = RESET_VALUE; cnt < (dns_buffer_size/IP_V4_SIZE); cnt++, dns_server_ptr++)
        {
            /* Print DNS server address */
            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DNS_SERVER_IP, sizeof(ULONG *), dns_server_ptr);

            /* Retrieve the DNS Server Address */
            *dns_server_addr = *dns_server_ptr;
        }
    }
    return status;
}
/**********************************************************************************************************************
 * End of function dhcpv4_client_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: dns_client_init
 * Description  : This function initializes the dns client by creating an instance and allocates the required packet pool.
 *                It adds the IPv4 server address to the Client list.
 * Arguments    : dns_server_addr   DNS server address.
 * Return value : NX_SUCCESS   Upon  successfully
 *                Any Other Error code apart from NX_SUCCESS
 *********************************************************************************************************************/
static UINT dns_client_init(ULONG dns_server_addr)
{
    /* To capture the status(Success/Failure) of each Function/API */
    UINT status = NX_SUCCESS;

    /* Create the DNS instance */
    status = nx_dns_create(&g_dns0, &g_ip0, (UCHAR *)"g_dns0");
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dns_create API failed.");
        return status;
    }

    /* Create the DNS packet pool */
    status = nx_dns_packet_pool_set(&g_dns0, &g_packet_pool0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dns_packet_pool_set API Failed.");
        return status;
    }

    /* Add an IPv4 server address to the Client list */
    status = nx_dns_server_add(&g_dns0, dns_server_addr);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dns_server_add API Failed.");
    }
    return status;
}
/**********************************************************************************************************************
 * End of function dns_client_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: fetch_ntp_server_ip_address
 * Description  : This function gets and prints sntp server address.
 * Arguments    : *server_url    server url.
 *                *server_addr   sntp server address
 * Return value : NX_SUCCESS or Any Other Error code apart from NX_SUCCESS upon unsuccessful sntp server address fetch.
 *********************************************************************************************************************/
static UINT fetch_ntp_server_ip_address(ULONG * server_addr, UCHAR * server_url)
{
    UINT status = NX_SUCCESS;

    /* Print the message for getting server IP from server url */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_GET_SERVER_STR, SERVER_URL_LEN, server_url);

    /* Get the IP address from SNTP server */
    status = nx_dns_host_by_name_get(&g_dns0, server_url, server_addr, NETWORK_TIMEOUT);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dns_host_by_name_get API Failed.");
        return status;
    }
    else
    {
        /* Print the message for acquired server IP from server url */
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_SERVER_STR, SERVER_URL_LEN, server_url);
    }
    return status;
}
/**********************************************************************************************************************
 * End of function fetch_ntp_server_ip_address
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: sntp_client_init
 * Description  : This function creates the sntp client and setups the time update callback function.	
 * Arguments    : None
 * Return value : NX_SUCCESS   Upon  successfully
 *                Any Other Error code apart from NX_SUCCESS
 *********************************************************************************************************************/
static UINT sntp_client_init(void)
{
    UINT status   = NX_SUCCESS;

    /* Create the SNTP instance */
    status = nx_sntp_client_create(&g_sntp_client0,
                        &g_ip0,
                        G_SNTP_CLIENT0_INDEX_NETWORK_INTERFACE,
                        &g_packet_pool0,
                        NULL,
                        NULL,
                        NULL);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_create API Failed.");
        return status;
    }

    /* Setup time update callback function */
    status = nx_sntp_client_set_time_update_notify(&g_sntp_client0, time_update_callback);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_set_time_update_notify API Failed.");
        return status;
    }
    PRINT_INFO_STR("SNTP client created successfully.");
    return status;
}
/**********************************************************************************************************************
 * End of function sntp_client_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: sntp_client_deinit
 * Description  : This function deletes the sntp client.
 * Arguments    : None
 * Return value : None
 *********************************************************************************************************************/
static void sntp_client_deinit(void)
{
    UINT status   = NX_SUCCESS;
    status = nx_sntp_client_delete(&g_sntp_client0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_delete API Failed.");
    }
}
/**********************************************************************************************************************
 * End of function sntp_client_deinit
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: time_update_callback
 * Description  : The application defined handler for notifying SNTP time update event.
 * Arguments    : *time_update_ptr   time update
 *                *local_time        local time
 * Return value : None
 *********************************************************************************************************************/
VOID time_update_callback(NX_SNTP_TIME_MESSAGE * time_update_ptr, NX_SNTP_TIME * local_time)
{
    NX_PARAMETER_NOT_USED(time_update_ptr);
    NX_PARAMETER_NOT_USED(local_time);

    /* Set event flag */
    tx_event_flags_set(&g_sntp_flags, SNTP_UPDATE_EVENT, TX_OR);
}
/**********************************************************************************************************************
 * End of function time_update_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: sntp_client_run
 * Description  : This function runs SNTP client, fetches ntp server time.
 * Arguments    : mode           sntp client mode i.e. init or retry
 *                *server_url    server url.
 * Return value : NX_SUCCESS or Any Other Error code apart from NX_SUCCESS upon unsuccessful sntp client run.
 *********************************************************************************************************************/
static UINT sntp_client_run(sntp_client_mode_t mode, UCHAR * server_url)
{
    UINT   status              = NX_SUCCESS;
    ULONG  base_seconds        = RESET_VALUE;
    ULONG  base_fraction       = RESET_VALUE;
    ULONG  sntp_server_ip_addr = RESET_VALUE;

    if (RETRY_MODE == mode)
    {
        status = nx_sntp_client_stop(&g_sntp_client0);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_sntp_client_stop API Failed.");
            return status;
        }
        PRINT_INFO_STR("Stopped SNTP client services.");
        PRINT_INFO_STR("Re-checking for SNTP server IP address.");
    }

    /* Obtain server address */
    status = fetch_ntp_server_ip_address(&sntp_server_ip_addr, server_url);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("fetch_ntp_server_ip_address function Failed.");
        return status;
    }

    /* Print SNTP server IP address */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_SERVER_IP, sizeof(ULONG *), &sntp_server_ip_addr);

#if UNICAST
    /* Use the IPv4 service to set up the Client and set the IPv4 SNTP server */
    status = nx_sntp_client_initialize_unicast(&g_sntp_client0, sntp_server_ip_addr);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_initialize_unicast API Failed.");
        return status;
    }
#else
    status = nx_sntp_client_initialize_broadcast(&g_sntp_client0,  NX_NULL, sntp_server_ip_addr);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_initialize_broadcast API Failed.");
        return status;
    }
#endif

    base_seconds =  0xd2c96b90;  /* Jan 24, 2012 UTC */
    base_fraction = 0xa132db1e;

    /* Apply to the SNTP Client local time */
    status = nx_sntp_client_set_local_time(&g_sntp_client0, base_seconds, base_fraction);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_set_local_time API Failed.");
        return status;
    }

    /* Run whichever service the client is configured for */
#if UNICAST
    status = nx_sntp_client_run_unicast(&g_sntp_client0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_run_unicast API Failed.");
        return status;
    }
#else
    status = nx_sntp_client_run_broadcast(&g_sntp_client0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_run_broadcast API Failed.");
        return status;
    }
#endif  /* USE_UNICAST */

    PRINT_INFO_STR("SNTP client initialization done and is running.");
    return status;
}
/**********************************************************************************************************************
 * End of function sntp_client_run
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_print_sntp_time
 * Description  : This function gets and prints network time.
 * Arguments    : None
 * Return value : NX_SUCCESS or Any Other Error code apart from NX_SUCCESS upon unsuccessful gets and prints network time
 *********************************************************************************************************************/
static UINT get_print_sntp_time(void)
{
    fsp_err_t err        = FSP_SUCCESS;
    UINT   status        = NX_SUCCESS;
    UINT   server_status = NX_FALSE;
    ULONG  seconds       = RESET_VALUE;
    ULONG  fraction      = RESET_VALUE;

    /* Check for valid SNTP server status */
    status = nx_sntp_client_receiving_updates(&g_sntp_client0, &server_status);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_receiving_updates API Failed.");
        return status;
    }

    /* We have communicated with the server let us re validate this */
    if (NX_TRUE == server_status)
    {
        CHAR write_time_data[MAX_BYTES] = {NULL_CHAR};

        /* We have a valid update. Get the SNTP Client local time */
        status = nx_sntp_client_get_local_time_extended(&g_sntp_client0, &seconds, &fraction, &write_time_data[0], 100U);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_sntp_client_get_local_time_extended API Failed.");
            return status;
        }

        /* Convert an NTP Time to Date and Time string */
        status = nx_sntp_client_utility_display_date_time(&g_sntp_client0, &write_time_data[0],sizeof(write_time_data));
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_sntp_client_utility_display_date_time API Failed.");
            return status;
        }

        /* Update client time stamp query to system block */
        err = update_time_stamp_to_system_block(write_time_data);
        if(FSP_SUCCESS != err)
        {
            PRINT_ERR_STR("update_time_stamp_to_system_block function Failed.");
            return err;
        }
    } /* Server status NX TRUE check if condition */
    return status;
}
/**********************************************************************************************************************
 * End of function get_print_sntp_time
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: network_init
 * Description  : This function initializes the NetX system, Packet pool, IP, DNS client, dhcpv4 client
 *                and prints client IP address
 * Arguments    : None
 * Return value : NX_SUCCESS or Any Other Error code apart from NX_SUCCESS upon unsuccessful network initialization
 *********************************************************************************************************************/
static UINT network_init(void)
{
    UINT status = NX_SUCCESS;

    /* Client IP address and Network Mask */
    ULONG client_ip_address = RESET_VALUE;
    ULONG network_mask      = RESET_VALUE;
    ULONG dns_addr          = RESET_VALUE;

    /* Initialize NetX */
    nx_system_initialize();

    /* Initialize Packet Pool */
    status = packet_pool0_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("packet_pool0_init function Failed.");
        return status;
    }

    /* Initialize IP */
    status = ip0_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip0_init function Failed.");
        return status;
    }

    /* Initialize DHCP v4 Client */
    status = dhcpv4_client_init(&dns_addr);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcpv4_client_init function Failed.");
        return status;
    }

    /* Retrieve IP address and network mask */
    status = nx_ip_address_get (&g_ip0, &client_ip_address, &network_mask);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_address_get API Failed.");
        return status;
    }

    /* Print the client IP address */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_IP, sizeof(ULONG *), &client_ip_address);

    /* Initialize DNS client */
    status = dns_client_init(dns_addr);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dns_client_init function Failed.");
    }
    return status;
}
/**********************************************************************************************************************
 * End of function network_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @} (end addtogroup NetX_sntp_client_ep)
 *********************************************************************************************************************/

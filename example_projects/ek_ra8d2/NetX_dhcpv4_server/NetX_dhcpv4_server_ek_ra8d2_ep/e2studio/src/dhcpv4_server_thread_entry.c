/***********************************************************************************************************************
 * File Name    : dhcpv4_server_thread_entry.c
 * Description  : Contains entry function of DHCPV4 Server.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "dhcpv4_server_thread.h"
#include "common_utils.h"
#include "dhcpv4_server_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup NetX_dhcpv4_server_ep
 * @{
 **********************************************************************************************************************/

/* Function declarations */
static void nx_common_init0(void);
static void packet_pool_init0(void);
static void ip_init0(void);
static void dhcpv4_server_init0(void);
static void terminate_nullchar(char * dest_str, char * input_str);

/* Global variables */
/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory) */
NX_PACKET_POOL g_packet_pool0;
#if (BSP_PERIPHERAL_ESWM_PRESENT)
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))]
                                   BSP_ALIGN_VARIABLE(4);
#else
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))]
                                   BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;
#endif
UINT active_client_count = RESET_VALUE;
/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0 */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0")
                           BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0 */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* DHCP server instance */
NX_DHCP_SERVER g_dhcp_server0;
uint8_t g_dhcp_server0_stack_memory[G_DHCP_SERVER0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_dhcp_server0")
                                    BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* Variable to capture server state */
static bool b_serverReady = false;

/* DHCPV4 Server thread entry function */
void dhcpv4_server_thread_entry(void)
{
    UINT    status                                   = NX_SUCCESS;
    UINT    addresses_added                          = RESET_VALUE;
    UINT    err                                      = TX_SUCCESS;
    CHAR    client_name[NX_DHCP_CLIENT_HOSTNAME_MAX] = {NULL_CHAR};
    ULONG   client_ip                                = RESET_VALUE;
    uint8_t read_data                                = RESET_VALUE;
    size_t  client_data_len                          = RESET_VALUE;

    /* Initialize the RTT Thread */
    rtt_thread_init_check();
    /* Print the banner and EP info */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Initialize the NetX system */
    nx_common_init0();

    /* Initialize the packet pool */
    packet_pool_init0();

    /* Create the IP instance */
    ip_init0();

    /* Initialize the dhcpv4 server */
    dhcpv4_server_init0();

    PRINT_INFO_STR("Network Initialization completed successfully.");

    /* Create a pool of IP addresses from the given range */
    status = nx_dhcp_create_server_ip_address_list(&g_dhcp_server0, INTERFACE_INDEX,
                                                   START_IP_ADDRESS_LIST, END_IP_ADDRESS_LIST, &addresses_added);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_create_server_ip_address_list failed.");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("Successfully created pool of available IP addresses for the specified DHCP server to assign.");

    /* Set network parameters returned by the DHCP server */
    status = nx_dhcp_set_interface_network_parameters(&g_dhcp_server0, INTERFACE_INDEX, NX_DHCP_SUBNET_MASK,
                                                      NX_DHCP_ROUTER_IP_ADDRESS, NX_DHCP_DNS_IP_ADDRESS);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_set_interface_network_parameters failed.");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("Successfully sets default values for network critical parameters for the specified interface.");

    /* Print the DHCPv4 Server menu options */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

    while (true)
    {
        /* Wait for user input */
        do
        {
            err = check_for_RTT_user_IP(&read_data);
        } while (err != TX_SUCCESS);

        switch (read_data)
        {
            case DHCPV4_SERVER_START :
            {
                if(false == b_serverReady)
                {
                    /* Start DHCP Server processing */
                    status = nx_dhcp_server_start(&g_dhcp_server0);
                    if(NX_SUCCESS != status)
                    {
                        PRINT_ERR_STR("nx_dhcp_server_start failed.");
                        ERROR_TRAP(status);
                    }
                    PRINT_INFO_STR("DHCPV4 server started.");

                    /* Set the server ready flag to true */
                    b_serverReady = true;
                }
                else
                {
                    PRINT_INFO_STR("DHCPV4 server is already running...");
                }
            }
            break;

            case DHCPV4_SERVER_STOP :
            {
                if(true == b_serverReady)
                {
                    /* Stop DHCP Server processing */
                    status = nx_dhcp_server_stop(&g_dhcp_server0);
                    if(NX_SUCCESS != status)
                    {
                        PRINT_ERR_STR("nx_dhcp_server_stop failed.");
                        ERROR_TRAP(status);
                    }
                    PRINT_INFO_STR("DHCPV4 server stopped.");

                    /* Set the server ready flag to false */
                    b_serverReady = false;
                }
                else
                {
                    PRINT_INFO_STR("DHCPV4 server is not started.");
                }
            }
            break;

            case PRINT_DHCPV4_CLIENT_INFO :
            {
                if(true == b_serverReady)
                {
                    /* Print the DHCPV4 client info */
                    /* Update number of active clients connected, client name and client IP */
                    active_client_count = g_dhcp_server0.nx_dhcp_number_clients;
                    memset(client_name, NULL_CHAR, sizeof(client_name));
                    terminate_nullchar(client_name,
                                       g_dhcp_server0.client_records[active_client_count-1].nx_dhcp_client_name);
                    client_data_len = strlen(client_name) +1;
                    client_ip = g_dhcp_server0.client_records[active_client_count-1].nx_dhcp_assigned_ip_address;
                    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_CLIENT_NUM, sizeof(UINT *), &active_client_count);
                    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_NAME, client_data_len, client_name);
                    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_IP, sizeof(ULONG *), &client_ip);
                }
                else
                {
                    PRINT_INFO_STR("DHCPV4 server is not started.");
                }
            }
            break;

            default :
            {
                PRINT_INFO_STR("Invalid Menu Option Selected");
            }
            break;
        }
        /* Print the DHCPv4 Server menu options */
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

        tx_thread_sleep (1);
    }
}

/*******************************************************************************************************************//**
 * @brief     Initialization of NetX system.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void nx_common_init0(void)
{
    /* Initialize the NetX system */
    nx_system_initialize();
}

/*******************************************************************************************************************//**
 * @brief     Create the packet pool.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void packet_pool_init0(void)
{
    /* Create the packet pool */
    UINT status = nx_packet_pool_create(&g_packet_pool0,
                  "g_packet_pool0 Packet Pool",
                  G_PACKET_POOL0_PACKET_SIZE,
                  &g_packet_pool0_pool_memory[0],
                  G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_pool_create failed.");
        ERROR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief     Create the IP instance and enables ARP, UDP, ICMP.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void ip_init0(void)
{
    UINT status = NX_SUCCESS;

    /* Create the IP instance */
    status = nx_ip_create(&g_ip0,
                          "g_ip0 IP Instance",
                          G_IP0_ADDRESS,
                          G_IP0_SUBNET_MASK,
                          &g_packet_pool0,
                          g_netxduo_ether_0,
                          &g_ip0_stack_memory[0],
                          G_IP0_TASK_STACK_SIZE,
                          G_IP0_TASK_PRIORITY);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_create failed.");
        ERROR_TRAP(status);
    }

    /* Enables address resolution protocol (ARP) */
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_arp_enable failed.");
        ERROR_TRAP(status);
    }

    /* Enable UDP */
    status = nx_udp_enable(&g_ip0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_udp_enable failed.");
        ERROR_TRAP(status);
    }

    /* Enable ICMP */
    status = nx_icmp_enable(&g_ip0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_icmp_enable failed.");
        ERROR_TRAP(status);
    }

    /* Wait for the link to be enabled */
    ULONG current_state;
    PRINT_INFO_STR("Checking Ethernet Link...");

    status = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, LINK_ENABLE_WAIT_TIME);
    if ((NX_SUCCESS != status) || (NX_IP_LINK_ENABLED != current_state))
    {
        PRINT_ERR_STR("nx_ip_status_check failed.");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("Ethernet link is up.");
}

/*******************************************************************************************************************//**
 * @brief     Create the DHCPv4 Server instance.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void dhcpv4_server_init0(void)
{
    /* Create the DHCP Server instance */
    UINT status = nx_dhcp_server_create(&g_dhcp_server0,
                                        &g_ip0,
                                        g_dhcp_server0_stack_memory,
                                        G_DHCP_SERVER0_TASK_STACK_SIZE,
                                        "g_dhcp_server0",
                                        &g_packet_pool0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_server_create failed.");
        ERROR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief      Terminate with null char from input string.
 * @param[in]  input_str  Input string.
 * @param[out] dest_str   Output string.
 * @retval     None.
 **********************************************************************************************************************/
static void terminate_nullchar(char * dest_str, char * input_str)
{
    int8_t index = RESET_VALUE;

    do
    {
        dest_str[index] = input_str[index];
        ++index;
    } while (input_str[index]!='\0');

    dest_str[index] = '\0';
}

/*******************************************************************************************************************//**
 * @} (end addtogroup NetX_dhcpv4_server_ep)
 **********************************************************************************************************************/

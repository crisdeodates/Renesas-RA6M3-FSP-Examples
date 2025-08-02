/***********************************************************************************************************************
 * File Name    : wifi_thread_entry.c
 * Description  : Contains data structures and functions used in wifi_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "wifi_ep.h"
#include "FreeRTOS.h"
#include "rm_wifi_api.h"
#include "rm_wifi_onchip_silex.h"

/*******************************************************************************************************************//**
 * @addtogroup wifi_on_chip_udp_freertos_ep
 * @{
 **********************************************************************************************************************/

extern SocketPrameters_t    g_socket;
extern WIFINetworkParams_t  g_net_params;

static void connectAP();
static void pingIP(uint8_t *p_ip_addr);

/* WiFi thread entry function */
void wifi_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    fsp_err_t err = FSP_SUCCESS;
    int32_t transfer_err = RESET_VALUE;
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    uint8_t input_buff[BUFF_LEN] = {RESET_VALUE};
    uint8_t ip_addr[IP_BUFF_LEN] = {RESET_VALUE};       /* IP address for ping operation */
    fsp_pack_version_t version = {RESET_VALUE};

    WIFIIPConfiguration_t ipInfo;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the J-Link RTT Viewer */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, \
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Opening WiFi module */
    wifi_err = WIFI_On();
    /* Handle error */
    if(eWiFiSuccess != wifi_err)
    {
        APP_ERR_PRINT("\r\n** WIFI_On API failed **\r\n");
        APP_ERR_TRAP(wifi_err);
    }

    /* Connect to AP either using Wi-Fi scan or manual entry */
    connectAP();

    /* Get IP configuration */
    wifi_err = WIFI_GetIPInfo(&ipInfo);
    /* Handle error */
    if(eWiFiSuccess != wifi_err)
    {
        APP_ERR_PRINT("\r\n** WIFI_GetIPInfo API failed **\r\n");
        wifi_deinit();
        APP_ERR_TRAP(wifi_err);
    }
    APP_PRINT("\r\nMCU IP address: %d.%d.%d.%d\r\n",\
              ipInfo.xIPAddress.ulAddress[INDEX_ZERO], ipInfo.xIPAddress.ulAddress[INDEX_ONE],\
              ipInfo.xIPAddress.ulAddress[INDEX_TWO], ipInfo.xIPAddress.ulAddress[INDEX_THREE]);

    /* Ping a IP address to check Internet connectivity */
    pingIP(ip_addr);

    /* Starting UDP client-server communication */
    APP_PRINT("\r\nStart UDP server in PC and start listening."\
              "\r\nSuggestion: UDP server in PC should listen at a port number greater than 10000.");

    /* Creating UDP socket in client mode */
    err = udp_socket_create();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nUDP socket create in client mode failed." \
                      "\r\nCheck network connectivity.\r\n");
        wifi_deinit();
        APP_ERR_TRAP(err);
    }
    APP_PRINT("\r\nUDP Socket created in client mode on MCU.\r\n");

    /* User input for UDP server IP address and its validation */
    APP_PRINT("\r\nEnter UDP server IP address to connect:\r\n");

    /* Ping UDP IP address to check connectivity */
    pingIP(ip_addr);

    g_socket.ip_addr_server = (uint32_t) (ip_addr[INDEX_ZERO] + (ip_addr[INDEX_ONE] << SHIFT_BY_8) + \
                              (ip_addr[INDEX_TWO] << SHIFT_BY_16) + \
                              (ip_addr[INDEX_THREE] << SHIFT_BY_24));

    /* User input for UDP server port number */
    APP_PRINT("\r\nEnter UDP server port number to connect:");
    APP_PRINT("\r\nNote: Please give free port number, otherwise UDP connect would fail and "\
              "application would also fail.\r\n");
    while(g_socket.port < UDP_PORT_MIN || g_socket.port > UDP_PORT_MAX)
    {
        user_input((char *)input_buff);
        g_socket.port = (uint32_t)atoi((char *)input_buff);
        if(g_socket.port < UDP_PORT_MIN || g_socket.port > UDP_PORT_MAX)
        {
            APP_ERR_PRINT("\r\nUDP port out of range. Re-enter port number(valid range 4096 - 65535)\r\n");
        }
    }

    /* Connecting to UDP server */
    err = udp_socket_connect(g_socket.ip_addr_server, g_socket.port);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nConnection failed. \r\nCheck UDP server settings and network connectivity.\r\n");
        wifi_deinit();
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\nUDP socket connected.\r\n");

    /* Sending message to server */
    strcpy((char *) g_socket.send_buff, "Hello");
    APP_PRINT("\r\nSending: %s", g_socket.send_buff);

    /* Capture number of bytes */
    transfer_err = udp_send_data(g_socket.send_buff);
    /* Handle error */
    if(RESET_VALUE >= transfer_err)
    {
        APP_ERR_PRINT("\r\nCheck UDP server settings and network connectivity.");
        wifi_deinit();
        APP_ERR_TRAP(FSP_ERR_WIFI_FAILED);
    }

    while(FSP_SUCCESS == network_connectivity_check())
    {
        /* Receiving message from server */
        memset(g_socket.recv_buff, RESET_VALUE, sizeof(g_socket.recv_buff));
        APP_PRINT("\r\nWaiting for message from server...\r\n");

        /* Capture number of bytes */
        transfer_err = udp_recv_data(g_socket.recv_buff);
        /* Handle error */
        if(RESET_VALUE <= transfer_err)
        {
            APP_PRINT("\r\nReceived: %s", g_socket.recv_buff);
        }
    }

    APP_PRINT("\r\nUDP Server disconnected. Restart the EP\r\n");

    while(true)
    {
        vTaskDelay(ONE_SEC_DELAY);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function connects to an Access point using either performing a network scan or by allowing
 *              the user to enter valid credentials
 * @param[IN]   None.
 * @retval      None.
 **********************************************************************************************************************/
static void connectAP()
{
    uint8_t input_buff[BUFF_LEN] = {RESET_VALUE};
    uint8_t index_menu_option = RESET_VALUE;
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    WIFIReturnCode_t connect_status = eWiFiFailure;

    /* Connecting to WiFi AP */
    do
    {
        /* Menu for user selection */
        APP_PRINT(MENU_OPTIONS);
        user_input((char *)input_buff);
        index_menu_option = (uint8_t)atoi((char *) input_buff);

        switch(index_menu_option)
        {
            /* Scan for available APs and connect to a desired one */
            case SCAN_AP:
#if defined (BOARD_RA2E2_EK)
                APP_PRINT("\r\nWiFi Access Point scanning is not available for this board."\
                          "\r\nPlease press 2 for entering WiFi Access Point's credentials manually.\r\n");
#else
                /* Scan for APs and connect to user chosen one */
                wifi_err = scan_and_select();
                /* Handle error */
                if(eWiFiSuccess != wifi_err)
                {
                    APP_PRINT("\r\nWiFi Access Point scanning failed. Retry."\
                              "\r\nIf multiple failures, please enter WiFi AP credentials.\r\n");
                }
                else
                {
                    /* Connecting to user selected WiFi AP */
                    wifi_err = WIFI_ConnectAP(&g_net_params);
                    /* Handle error */
                    if(eWiFiSuccess != wifi_err)
                    {
                        APP_ERR_PRINT("\r\n** WIFI_ConnectAP API failed **\r\n"\
                                      "\r\nIncorrect Access Point credentials entered. Try again.\r\n");
                    }
                    else
                    {
                        /* Status check of WiFi connectivity */
                        connect_status = WIFI_IsConnected(NULL);
                        if(eWiFiSuccess != connect_status)
                        {
                            APP_PRINT("\r\nWiFi not connected.");
                        }
                    }
                }
#endif
                break;
            /* Connect to a known access point manually */
            case ENTER_SSID:
                /* User manually enter WiFi AP credentials to connect */
                wifi_err = manual_connect();
                /* Handle error */
                if(eWiFiSuccess != wifi_err)
                {
                    APP_ERR_PRINT("\r\nIncorrect Access Point credentials entered. Try again.\r\n");
                }
                else
                {
                    connect_status = eWiFiSuccess;
                }
                break;
            default:
                APP_PRINT("\r\nInvalid Input.\r\n");
                break;
        }
    }while(eWiFiSuccess != connect_status);

    APP_PRINT("\r\nWiFi connected.\r\n");
}

/*******************************************************************************************************************//**
 * @brief       This function pings and resolves DNS for a IP specified.
 * @param[OUT]  IP address.
 * @retval      None.
 **********************************************************************************************************************/
static void pingIP(uint8_t *p_ip_addr)
{
    uint8_t input_buff[BUFF_LEN] = {RESET_VALUE};
    WIFIReturnCode_t wifi_err = eWiFiFailure;
    uint8_t retry_count = RESET_VALUE;
    uint8_t retry_attempt = RESET_VALUE;

    /* Ping demonstration */
    while(eWiFiSuccess != wifi_err && WIFI_MAX_TRY >= retry_attempt)
    {
        /* User input for IP address or URL to ping */
        APP_PRINT("\r\nEnter the IP Address to ping:\r\n");
        user_input((char *)input_buff);

        while(WIFI_MAX_TRY > retry_count)
        {
            APP_PRINT("\r\nPinging. Please wait...\r\n");
            /* DNS lookup for URL or IP */
            wifi_err = dns_query((char *)input_buff, p_ip_addr);
            /* Handle error */
            if(eWiFiSuccess != wifi_err)
            {
                APP_PRINT("\r\nDNS lookup failed\r\n");
            }
            /* Only if DNS lookup is successful, ping would be performed */
            else
            {
                /* Ping URL or IP address */
                wifi_err = ping(p_ip_addr);
                /* Handle error */
                if(eWiFiSuccess != wifi_err)
                {
                    APP_PRINT("\r\nPing Unsuccessful.\r\n");
                }
                else
                    break;
            }
            retry_count++;
            APP_PRINT("\r\nRetrying...\r\n");
        }
        if(eWiFiSuccess != wifi_err)
        {
            retry_attempt++;
            APP_PRINT("Check IP/URL entered.\r\n");
        }
        retry_count = RESET_VALUE;
    }

    if(WIFI_MAX_TRY <= retry_attempt)
    {
        APP_ERR_PRINT("\r\nFailed maximum retry count for ping operation. Reset MCU.\r\n");
        wifi_deinit();
        APP_ERR_TRAP(wifi_err);
    }
    else
    {
        APP_PRINT("\r\nPing Successful to %s[%d.%d.%d.%d]\r\n", input_buff, p_ip_addr[INDEX_ZERO], \
                  p_ip_addr[INDEX_ONE], p_ip_addr[INDEX_TWO], p_ip_addr[INDEX_THREE]);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup wifi_on_chip_udp_freertos_ep)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name    : dhcpv6_server.h
 * Description  : Contains data structures and functions used in DHCPv6_server.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DHCP_V6_SERVER_H_
#define DHCP_V6_SERVER_H_

#define LINK_ENABLE_WAIT_TIME           (1000U)

/* RTT input selection options to start, and print leased IPs info */
#define DHCPV6_SERVER_START             (1U)
#define PRINT_DHCPV6_LEASED_IP_INFO     (2U)

#define DHCPV6_SERVER_MENU              "\r\nDHCPV6 SERVER Menu options :"\
                                        "\r\n1. Enter 1 to start dhcpv6 server"\
                                        "\r\n2. Enter 2 to display dhcpv6 leased IPs info"\
                                        "\r\nUser Input : \r\n"\

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)     (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)         (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

#define EP_INFO        "\r\nThis project demonstrates the basic functionality of Netx Duo DHCPv6 server\r\n"\
                       "with ethernet driver on Renesas RA MCUs based on Renesas FSP using AzureRTOS.\r\n"\
                       "DHCPv6 Server is created and based on user input, user can start, stop and print \r\n"\
                       "DHCPv6 clients info.\r\n"\
                       "On successful completion of each operation, the success status is displayed\r\n"\
                       "on RTT viewer. Error and info messages will be printed on J-Link RTT Viewer.\r\n\n\n"

#define FILL_NXD_IPV6_ADDRESS(ipv6,f0,f1,f2,f3,f4,f5,f6,f7) (\
		{do {\
			(ipv6).nxd_ip_address.v6[0] = (((uint32_t)(f0) << 16) & 0xFFFF0000) | ((uint32_t)(f1) & 0x0000FFFF);\
            (ipv6).nxd_ip_address.v6[1] = (((uint32_t)(f2) << 16) & 0xFFFF0000) | ((uint32_t)(f3) & 0x0000FFFF);\
            (ipv6).nxd_ip_address.v6[2] = (((uint32_t)(f4) << 16) & 0xFFFF0000) | ((uint32_t)(f5) & 0x0000FFFF);\
            (ipv6).nxd_ip_address.v6[3] = (((uint32_t)(f6) << 16) & 0xFFFF0000) | ((uint32_t)(f7) & 0x0000FFFF);\
            (ipv6).nxd_ip_version       = NX_IP_VERSION_V6;\
            } while(0);})

/* Structure to store leased IPs info */
typedef struct st_ipv6_info
{
    NXD_ADDRESS ipv6_address;
    USHORT mac_high;
    ULONG mac_low;
} ipv6_info_t;

/* Function declarations */
void str_ipv6(UCHAR * str, NXD_ADDRESS ipv6);
void str_mac(UCHAR * str, USHORT mac_msw, ULONG mac_lsw);

#endif /* DHCP_V6_SERVER_H_ */

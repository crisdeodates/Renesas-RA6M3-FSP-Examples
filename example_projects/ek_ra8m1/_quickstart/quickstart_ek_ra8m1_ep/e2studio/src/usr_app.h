/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name    : usr_app.h
 * Description  : Contains macros, data structures and functions used  in the Application
 **********************************************************************************************************************/

#ifndef USR_APP_H_
#define USR_APP_H_

#if( ipconfigUSE_DHCP != 0 )
    #include "FreeRTOS_DHCP.h"
#endif

#define USR_TEST_DOMAIN_NAME "www.renesas.com"
#define USR_TEST_PING_IP     "172.217.160.174"
#define USR_PING_COUNT       (4)

#define SUCCESS                     (0)
#define PRINT_UP_MSG_DISABLE        (0x01)
#define PRINT_DOWN_MSG_DISABLE      (0x02)
#define PRINT_NWK_USR_MSG_DISABLE   (0x04)


#define ETHERNET_LINK_DOWN  (0x01)
#define ETHERNET_LINK_UP    (0x00)
#define IP_LINK_DOWN        (0x02)
#define IP_LINK_UP          (0x00)


typedef struct st_ping_data
{
    uint32_t sent;     // Ping Request
    uint32_t received; // Ping Response
    uint32_t lost;     // Ping failure
} st_ping_data_t;

extern bool_t get_resolved_ip_address (uint8_t * ipv4a, uint8_t * ipv4b, uint8_t * ipv4c, uint8_t * ipv4d);
extern void update_ipconfig_to_static_ip (void);

#if (ipconfigUSE_DHCP != 0)
/**********************************************************************************************************************
 * Function Name: updateDhcpResponseToUsr
 * Description  : .
 * Arguments    : None
 * Return Value : None.
 *********************************************************************************************************************/
extern void update_dhcp_response_to_usr (void);
#endif

#if (ipconfigUSE_DHCP != 0)
/**********************************************************************************************************************
 * Function Name: xApplicationDHCPHook_Multi
 * Description  : This is the User Hook for the DHCP Response. xApplicationDHCPHook() is called by DHCP Client Code
 *                when DHCP handshake messages are exchanged from the Server.
 * Arguments    : eDHCPPhase Different Phases of DHCP Phases
 *              : ulIPAddress the Offered IP Address
 * Return Value : Returns DHCP Answers
 *********************************************************************************************************************/
eDHCPCallbackAnswer_t xApplicationDHCPHook_Multi( eDHCPCallbackPhase_t eDHCPPhase,
                                                          struct xNetworkEndPoint * pxEndPoint,
                                                          IP_Address_t * pxIPAddress );
#endif

#if (ipconfigDHCP_REGISTER_HOSTNAME == 1)
    /* DHCP has an option for clients to register their hostname.  It doesn't
    have much use, except that a device can be found in a router along with its
    name. If this option is used the callback below must be provided by the
    application writer to return a const string, denoting the device's name. */
/**********************************************************************************************************************
 * Function Name: pcApplicationHostnameHook
 * Description  : .
 * Arguments    : None
 * Return Value : .
 *********************************************************************************************************************/
extern const char_t * pcApplicationHostnameHook (void);
#endif /* ipconfigDHCP_REGISTER_HOSTNAME */

/**********************************************************************************************************************
 * Function Name: ulApplicationGetNextSequenceNumber
 * Description  : .
 * Arguments    : ulSourceAddress
 *              : usSourcePort
 *              : ulDestinationAddress
 *              : usDestinationPort
 * Return Value : .
 *********************************************************************************************************************/
uint32_t ulApplicationGetNextSequenceNumber (uint32_t ulSourceAddress,
                                                uint16_t usSourcePort,
                                                uint32_t ulDestinationAddress,
                                                uint16_t usDestinationPort);
/**********************************************************************************************************************
 * Function Name: ulRand
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
extern uint32_t ulRand ();

/**********************************************************************************************************************
 * Function Name: send_ping
 * Description  : .
 * Argument     : pcIPAddress
 * Return Value : .
 *********************************************************************************************************************/
extern BaseType_t send_ping (const char * pcIPAddress);

/**********************************************************************************************************************

 * Function Name: print_pingResult
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
extern void print_pingResult (void);

/**********************************************************************************************************************
 * Function Name: dns_querry_func
 * Description  : .
 * Argument     : domain
 * Return Value : .
 *********************************************************************************************************************/
extern void dns_querry_func (char_t * domain);


#endif /* USR_APP_H_ */

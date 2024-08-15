/***********************************************************************************************************************
* Copyright (c) 2021 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : net_thread_entry.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#include "net_thread.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "common_data.h"
#include "common_init.h"
#include "common_utils.h"
#include "usr_app.h"
#include "r_typedefs.h"


/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define STATIC_IP_MAC_ADDRESS        {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}
#define STATIC_IP_ADDRESS            {192, 168,  10, 142}
#define STATIC_IP_GATEWAY_ADDRESS    {192, 168,   0,   1}
#define STATIC_IP_NET_MASK           {255, 255, 255,   0}
#define STATIC_IP_NET_MASK_BOOT      {255, 255, 255, 255}

#define ETHERNET_BUFFER_SIZE                (512)
#define ETHERNET_SIMULTANEOUS_CONNECTIONS   (20)
#define ETHERNET_ECHO_SERVER_STACK_SIZE     (360)


#define STATIC_IP_DNS_SERVER_ADDRESS {192, 168,   0,   1}


#if ENABLE_TEST_PACKETS
#define ETHER_TEST_CONTINUE_PROMPT_ENABLE_PRV_          (0)
#define ETHER_TEST_MAXIMUM_ETHERNET_FRAME_SIZE_PRV_     (1514)
#define ETHER_TEST_TRANSMIT_ETHERNET_FRAME_SIZE_PRV_    (60)

/* mac addresses identical for loopback */
#define ETHER_TEST_SOURCE_MAC_ADDRESS_PRV_              (0x00, 0x11, 0x22, 0x33, 0x44, 0x55)
#define ETHER_TEST_DESTINATION_MAC_ADDRESS_PRV_         (0x00, 0x11, 0x22, 0x33, 0x44, 0x55)
#define ETHER_TEST_FRAME_TYPE_PRV_                      (0x00, 0x2E)
#define ETHER_TEST_EMPTY_PAYLOAD_PRV_                   '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', \
                                                        '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', \
                                                        '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', \
                                                        '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',

#define ETHER_TEST_PAYLOAD_STRING_PRV_   "I am the senate."
#define ETHER_TEST_PAYLOAD_SIZE_PRV_     (46)


/* Domain for the DNS Host lookup is used in this Example Project.
 * The project can be built with different *domain_name to validate the DNS client
 */
char_t *domain_name = USR_TEST_DOMAIN_NAME;
#endif


#if( ipconfigUSE_DHCP != 0 )
/* DHCP populates these IP address, Sub net mask and Gateway Address. So start with this is zeroed out values
 * The MAC address is Test MAC address.
 */
uint8_t g_mac_address[6]        = STATIC_IP_MAC_ADDRESS;
uint8_t g_ip_address[4]         = STATIC_IP_ADDRESS;
uint8_t g_gateway_address[4]    = STATIC_IP_GATEWAY_ADDRESS;
uint8_t g_dns_server_address[4] = STATIC_IP_DNS_SERVER_ADDRESS;

/* Work arround to fix feature of FreeRTOS-Plus-TCP
Before, in FreeRTOSv202107.00/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/FreeRTOS_UDP_IP.c, Line 318
vARPRefreshCacheEntry() was called unconditionally. And everything worked.

Now, in file FreeRTOSv202112.00/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/FreeRTOS_UDP_IP.c, Line 322
xCheckRequiresARPResolution() is called first instead. 
Inside, it checks if' 'SourceIPAddress' belongs to the same subnet. 
Obviously, when network is not initialized yet, this check fails, so network never gets initialized.
*/
//uint8_t g_net_mask[4]          = STATIC_IP_NET_MASK;
uint8_t g_net_mask[4]          = STATIC_IP_NET_MASK_BOOT;

#else
/* Static IP configuration, when DHCP mode is not used for the Example Project.
 * This needs to be populated by the user according to the Network Settings of your LAN.
 * This sample address taken from the LAN where it is tested. This is different for different LAN.
 * get the Address using the PC IPconfig details.
 */
uint8_t g_mac_address[6]       = STATIC_IP_MAC_ADDRESS;
uint8_t g_ip_address[4]        = STATIC_IP_ADDRESS;
uint8_t g_net_mask[4]          = STATIC_IP_NET_MASK;
uint8_t g_gateway_address[4]   = STATIC_IP_GATEWAY_ADDRESS;
uint8_t g_dns_server_address[4] = STATIC_IP_DNS_SERVER_ADDRESS;
#endif


NetworkAddressingParameters_t xNetworkAddressing;
NetworkAddressingParameters_t g_xnd = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE};

NetworkInterface_t xInterfaces[ 1 ];
NetworkEndPoint_t xEndPoints[ 1 ];

/* IP address of the PC or any Device on the LAN/WAN where the Ping request is sent.
 * Note: Users needs to change this according to the LAN settings of your Test PC or device
 * when running this project.
 */
char_t g_remote_ip_address[] = USR_TEST_PING_IP;



static uint32_t s_usr_print_ability = RESET_VALUE;

static uint8_t s_static_ip_mac_address[6]      = STATIC_IP_MAC_ADDRESS;
static uint8_t s_static_ip_address[4]          = STATIC_IP_ADDRESS;
static uint8_t s_static_ip_netmask[4]          = STATIC_IP_NET_MASK;
static uint8_t s_static_ip_gatewayaddress[4]   = STATIC_IP_GATEWAY_ADDRESS;
static uint8_t s_static_ip_dnsserveraddress[4] = STATIC_IP_DNS_SERVER_ADDRESS;

static uint32_t  s_dhcp_in_use      = RESET_VALUE;
static st_ping_data_t s_ping_data   = {RESET_VALUE, RESET_VALUE, RESET_VALUE};

static uint32_t is_network_up (void);
static void update_ipconfig (void);

extern bool_t wsStart (uint16_t usPortNumber);
extern NetworkInterface_t * pxFSP_Eth_FillInterfaceDescriptor (BaseType_t xEMACIndex, NetworkInterface_t * pxInterface);

/**********************************************************************************************************************
 * Function Name: update_ipconfig_to_static_ip
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void update_ipconfig_to_static_ip(void)
{
    memcpy(&g_mac_address, &s_static_ip_mac_address, 6);
    memcpy(&g_ip_address, &s_static_ip_address, 4);
    memcpy(&g_net_mask, &s_static_ip_netmask, 4);
    memcpy(&g_gateway_address, &s_static_ip_gatewayaddress, 4);
    memcpy(&g_dns_server_address, &s_static_ip_dnsserveraddress, 4);

}
/**********************************************************************************************************************
 End of function update_ipconfig_to_static_ip
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: ulRand
 * Description  : Generates 32 bit Random number
 * Argument     :
 * Return Value : Random Number
 *********************************************************************************************************************/
uint32_t ulRand()
{
    /* example of a 32-bit random number generator.
     * Here rand() returns a 15-bit number. so create 32 bit Random number using 15 bit rand()
     * Cast to uint32_t as fn rand() returns int.
     */
    uint32_t ulResult =
            ((((uint32_t) rand()) & 0x7fffuL)) |         /* */
            ((((uint32_t) rand()) & 0x7fffuL) << 15) |   /* */
            ((((uint32_t) rand()) & 0x0003uL) << 30);    /* */

    return ulResult;
}
/**********************************************************************************************************************
 End of function ulRand
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: ulApplicationGetNextSequenceNumber
 * Description  : Generates 32 sequence number
 * Arguments    : ulSourceAddress
 *              : usSourcePort
 *              : ulDestinationAddress
 *              : usDestinationPort
 * Return Value : Sequence Number
 *********************************************************************************************************************/
uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress, uint16_t usSourcePort,
        uint32_t ulDestinationAddress, uint16_t usDestinationPort)
{
    /* Here we need to get random number for the sequence number.
     * This is just for testing purpose, so software rand() is okay.
     * This can also be tied to the TRNG.
     */

    return ((ulSourceAddress + ulDestinationAddress + usSourcePort + usDestinationPort) && ulRand());
}
/**********************************************************************************************************************
 End of function ulApplicationGetNextSequenceNumber
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: send_ping
 * Description  : Send ICMP Ping request  based on the user input IP Address.
 * Argument     : pcIPAddress IP address to Ping
 * Return Value : Sequence Number
 *********************************************************************************************************************/
BaseType_t send_ping( const char *pcIPAddress)
{
    uint32_t ulIPAddress = RESET_VALUE;

    /*
     * The pcIPAddress parameter holds the destination IP address as a string in
     * decimal dot notation (for example, 192.168.0.200). Convert the string into
     * the required 32-bit format.
     */
    ulIPAddress = FreeRTOS_inet_addr(pcIPAddress);

    /*
     * Send a ping request containing 8 data bytes.  Wait (in the Blocked state) a
     * maximum of 100ms for a network buffer into which the generated ping request
     * can be written and sent.
     */
    return (FreeRTOS_SendPingRequest(ulIPAddress, 8, 100 / portTICK_PERIOD_MS));
}
/**********************************************************************************************************************
 End of function send_ping
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: vApplicationPingReplyHook
 * Description  : User Hook for the Ping Reply. vApplicationPingReplyHook() is called by the TCP/IP
 *                stack when the stack receives a ping reply.
 * Arguments    : eStatus
 *              : usIdentifier Ping reply status and Identifier
 * Return Value : None
 *********************************************************************************************************************/
void vApplicationPingReplyHook (ePingReplyStatus_t eStatus, uint16_t usIdentifier)
{
    FSP_PARAMETER_NOT_USED(usIdentifier);

    switch ( eStatus )
    {
        /* A valid ping reply has been received */
        case eSuccess    :
            s_ping_data.received++;
            break;

            /* A reply was received but it was not valid. */
        case eInvalidData :
        default:
            s_ping_data.lost++;
            break;
    }
}
/**********************************************************************************************************************
 End of function vApplicationPingReplyHook
 *********************************************************************************************************************/

extern bool_t check_status_og(int8_t index, e_test_status_t * p_state);

/**********************************************************************************************************************
 * Function Name: net_thread_entry
 * Description  : This is the User Thread for the EP.
 * Argument     : pvParameters Thread specific parameters
 * Return Value : None
 *********************************************************************************************************************/
void net_thread_entry(void *pvParameters)
{
    BaseType_t status = pdFALSE;
    EventBits_t uxBits;

    FSP_PARAMETER_NOT_USED(pvParameters);

    while (1)
    {
        uxBits = xEventGroupWaitBits(g_update_console_event,  STATUS_ENABLE_ETHERNET, pdFALSE, pdTRUE, 1);

        if ((uxBits & (STATUS_ENABLE_ETHERNET)) == (STATUS_ENABLE_ETHERNET))
        {
            xEventGroupClearBits(g_update_console_event, STATUS_ENABLE_ETHERNET);
            break;
        }
    }


    /* Event to start Ethernet has occurred  */
#if (1 == ipconfigIPv4_BACKWARD_COMPATIBLE)
    /* FreeRTOS IP initializes the IP stack  */
    status = FreeRTOS_IPInit (ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);
    /* Error Handler */
    if (pdFALSE == status)
    {
        __BKPT(0);
    }
#else


    /* IF the following function should be declared in the NetworkInterface.c
     * linked in the project. */
    pxFSP_Eth_FillInterfaceDescriptor( 0, &( xInterfaces[ 0 ] ) );
    FreeRTOS_FillEndPoint( &( xInterfaces[ 0 ] ), &( xEndPoints[ 0 ] ), g_ip_address, g_net_mask, g_gateway_address, g_dns_server_address, g_mac_address );

    #if ( ipconfigUSE_DHCP != 0 )
        {
            xEndPoints[ 0 ].bits.bWantDHCP = pdTRUE;
        }
    #endif /* ipconfigUSE_DHCP */

    status = FreeRTOS_IPInit_Multi();

    if (pdFALSE == status)
    {
        print_to_console((uint8_t *)"FreeRTOS_IPInit_Multi error\r\n");
    }
#endif

//   We should use these xTaskNotifyWait, but they do not appear to be configured
//   status = xTaskNotifyWait(pdFALSE, pdFALSE, &ip_status, portMAX_DELAY);
//   status = xTaskNotifyWait(pdFALSE, pdFALSE, &ip_status, portMAX_DELAY);     //https://reasoftware.atlassian.net/browse/RAP-2926
//    We cold try waiting till s_dhcp_in_use in-stead.
#if ( ipconfigUSE_DHCP != 0 )
    {
        while(s_dhcp_in_use == false)
        {
            vTaskDelay(10);
        }
    }
#endif /* ipconfigUSE_DHCP */

    while (true)
    {
        {

            /* Check if Both the Ethernet Link and IP link are UP */
            if (SUCCESS == is_network_up())
            {
                /* s_usr_print_ability is added to avoid multiple UP messages or Down Messages repeating*/
                if (!(PRINT_UP_MSG_DISABLE & s_usr_print_ability))
                {
                    s_usr_print_ability |= PRINT_UP_MSG_DISABLE;
                }

                if (!(PRINT_NWK_USR_MSG_DISABLE & s_usr_print_ability))
                {
    #if( ipconfigUSE_DHCP != 0 )
                    /* Display the New IP credentials obtained from the DHCP server */
                    update_dhcp_response_to_usr();
    #endif
                    /* Updated IP credentials on to the RTT console */
                    update_ipconfig();

                }

                if (!(PRINT_NWK_USR_MSG_DISABLE & s_usr_print_ability))
                {
                    s_usr_print_ability |= PRINT_NWK_USR_MSG_DISABLE;

                    /* DISABLE WEB SERVER FOR 8M1 */
                    wsStart(80);
                }
                xEventGroupSetBits(g_update_console_event, STATUS_ETHERNET_LINKUP);

            }
            else
            {
                xEventGroupClearBits(g_update_console_event, STATUS_ETHERNET_LINKUP);

                if (!(PRINT_DOWN_MSG_DISABLE & s_usr_print_ability))
                {
                    s_usr_print_ability |= PRINT_DOWN_MSG_DISABLE;
                }
            }
            vTaskDelay(100);
        }
    }
}
/**********************************************************************************************************************
 End of function net_thread_entry
 *********************************************************************************************************************/

#if( ipconfigUSE_DHCP != 0 )
/**********************************************************************************************************************
 * Function Name: xApplicationDHCPHook_Multi
 * Description  : This is the User Hook for the DHCP Response. xApplicationDHCPHook_Multi() is called by DHCP Client Code
 *                when DHCP handshake messages are exchanged from the Server.
 * Arguments    : eDHCPPhase Different Phases of DHCP Phases
 *              : ulIPAddress the Offered IP Address
 * Return Value : Returns DHCP Answers
 *********************************************************************************************************************/
eDHCPCallbackAnswer_t xApplicationDHCPHook_Multi(eDHCPCallbackPhase_t eDHCPPhase,
        struct xNetworkEndPoint * pxEndPoint,
        IP_Address_t * pxIPAddress)
{
    FSP_PARAMETER_NOT_USED(pxEndPoint);
    eDHCPCallbackAnswer_t eReturn = eDHCPContinue;

    /*
     * This hook is called in a couple of places during the DHCP process, as identified by the eDHCPPhase parameter.
     */
    switch (eDHCPPhase)
    {
        case eDHCPPhasePreDiscover  :

            /*
             *  A DHCP discovery is about to be sent out.  eDHCPContinue is returned to allow the discovery to go out.
             *  If eDHCPUseDefaults had been returned instead then the DHCP process would be stopped and the statically
             *  configured IP address would be used.
             *  If eDHCPStopNoChanges had been returned instead then the DHCP process would be stopped and whatever the
             *  current network configuration was would continue to be used.
             */
            break;

        case eDHCPPhasePreRequest  :

            /* An offer has been received from the DHCP server, and the offered IP address is passed in the ulIPAddress
             * parameter.
             */

            /* The sub-domains don't match, so continue with the DHCP process so the offered IP address is used.
             * Update the Structure, the DHCP state Machine is not updating this */
            xNetworkAddressing.ulDefaultIPAddress = pxIPAddress->ulIP_IPv4;

            /* Inform system that dhcp has returned with valid ip address */
            s_dhcp_in_use = 1;
            break;

        default :

            /*
             * Cannot be reached, but set eReturn to prevent compiler warnings where compilers
             * are disposed to generating one.
             */
            break;
    }

    return eReturn;
}
/**********************************************************************************************************************
 End of function xApplicationDHCPHook_Multi
 *********************************************************************************************************************/#endif /*  ipconfigUSE_DHCP != 0  */

/**********************************************************************************************************************
 * Function Name: update_ipconfig
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void update_ipconfig(void)
{
#if( ipconfigUSE_DHCP != 0 )
    if (s_dhcp_in_use)
    {
        /* Cast to required type */
        g_net_mask[3] = (uint8_t)((g_xnd.ulNetMask & 0xFF000000) >> 24); /* Maintain data  */
        g_net_mask[2] = (uint8_t)((g_xnd.ulNetMask & 0x00FF0000) >> 16); /* size integrity */
        g_net_mask[1] = (uint8_t)((g_xnd.ulNetMask & 0x0000FF00) >> 8);  /* between mask   */
        g_net_mask[0] = (uint8_t)((g_xnd.ulNetMask & 0x000000FF));       /* and assignment */

        /* Cast to required type */
        g_gateway_address[3] = (uint8_t)((g_xnd.ulGatewayAddress & 0xFF000000) >> 24); /* Maintain data  */
        g_gateway_address[2] = (uint8_t)((g_xnd.ulGatewayAddress & 0x00FF0000) >> 16); /* size integrity */
        g_gateway_address[1] = (uint8_t)((g_xnd.ulGatewayAddress & 0x0000FF00) >>  8); /* between mask   */
        g_gateway_address[0] = (uint8_t)((g_xnd.ulGatewayAddress & 0x000000FF));       /* and assignment */

        /* Cast to required type */
        g_dns_server_address[3] = (uint8_t)((g_xnd.ulDNSServerAddress & 0xFF000000) >> 24); /* Maintain data  */
        g_dns_server_address[2] = (uint8_t)((g_xnd.ulDNSServerAddress & 0x00FF0000) >> 16); /* size integrity */
        g_dns_server_address[1] = (uint8_t)((g_xnd.ulDNSServerAddress & 0x0000FF00) >>  8); /* between mask   */
        g_dns_server_address[0] = (uint8_t)((g_xnd.ulDNSServerAddress & 0x000000FF));       /* and assignment */

        /* Cast to required type */
        g_ip_address[3] = (uint8_t)((g_xnd.ulDefaultIPAddress & 0xFF000000) >> 24); /* Maintain data  */
        g_ip_address[2] = (uint8_t)((g_xnd.ulDefaultIPAddress & 0x00FF0000) >> 16); /* size integrity */
        g_ip_address[1] = (uint8_t)((g_xnd.ulDefaultIPAddress & 0x0000FF00) >>  8); /* between mask   */
        g_ip_address[0] = (uint8_t)((g_xnd.ulDefaultIPAddress & 0x000000FF));       /* and assignment */
    }
#endif
}
/**********************************************************************************************************************
 End of function update_ipconfig
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: dns_querry_func
 * Description  : DNS Query for the requested Domain name.  Uses the FreeRTOS Client API  FreeRTOS_gethostbyname
 *                to get the IP address for the domain name
 * Argument     : domain
 * Return Value : .
 *********************************************************************************************************************/
void dns_querry_func(char_t * domain)
{
    uint32_t ulIPAddress = RESET_VALUE;
    int8_t cBuffer[16]   = {RESET_VALUE};

    /* Lookup the IP address of the FreeRTOS.org website. */
    ulIPAddress = FreeRTOS_gethostbyname((char_t *)domain);

    if (0 != ulIPAddress)
    {
        /* Convert the IP address to a string. */
        FreeRTOS_inet_ntoa( ulIPAddress, (char_t *) cBuffer);

        memcpy(g_remote_ip_address, &cBuffer, strlen(g_remote_ip_address));
    }

    return;
}
/**********************************************************************************************************************
 End of function dns_querry_func
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: get_resolved_ip_address
 * Description  : .
 * Arguments    : ipv4a
 *              : ipv4b
 *              : ipv4c
 *              : ipv4d
 * Return Value : .
 *********************************************************************************************************************/
bool_t get_resolved_ip_address(uint8_t *ipv4a, uint8_t *ipv4b, uint8_t *ipv4c, uint8_t *ipv4d)
{
    bool_t result = true;


    *ipv4a =  g_ip_address[0];
    *ipv4b =  g_ip_address[1];
    *ipv4c =  g_ip_address[2];
    *ipv4d =  g_ip_address[3];

    return (result);

}
/**********************************************************************************************************************
 End of function get_resolved_ip_address
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: is_network_up
 * Description  : This Function checks the Network status (Both Ethernet and IP Layer). If the Network is down
 *                the Application will not send any data on the network.
 * Return Value : .
 *********************************************************************************************************************/
static uint32_t is_network_up(void)
{
    fsp_err_t  eth_link_status = FSP_ERR_NOT_OPEN;
    BaseType_t networkUp = pdFALSE;
    volatile uint32_t network_status = (IP_LINK_UP | ETHERNET_LINK_UP);

#if (ipconfigUSE_DHCP != 0)
    if(!s_dhcp_in_use)
    {
        return IP_LINK_DOWN;
    }
#endif

    networkUp = FreeRTOS_IsNetworkUp();
    eth_link_status = R_ETHER_LinkProcess(g_ether0.p_ctrl);

    if ((FSP_SUCCESS == eth_link_status) && (pdTRUE == networkUp))
    {
        return network_status;
    }
    else
    {
        if (FSP_SUCCESS != eth_link_status)
        {
            network_status |= ETHERNET_LINK_DOWN;
        }
        else
        {
            if (FSP_SUCCESS == eth_link_status)
            {
                network_status |= ETHERNET_LINK_UP;
            }
        }

        if (pdTRUE != networkUp)
        {
            network_status |= IP_LINK_DOWN;
        }
        else
        {
            if (pdTRUE == networkUp)
            {
                network_status |= IP_LINK_UP;
            }
        }

        return network_status;
    }
}
/**********************************************************************************************************************
 End of function is_network_up
 *********************************************************************************************************************/

#if( ipconfigUSE_DHCP != 0 )
/**********************************************************************************************************************
 * Function Name: update_dhcp_response_to_usr
 * Description  : Update the DHCP info to the User data structure.
 * Return Value : .
 *********************************************************************************************************************/
void update_dhcp_response_to_usr(void)
{
    if (s_dhcp_in_use)
    {
        memcpy(&g_xnd, &xNetworkAddressing, sizeof(g_xnd));
    }
}
/**********************************************************************************************************************
 End of function update_dhcp_response_to_usr
 *********************************************************************************************************************/#endif


#if( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
/**********************************************************************************************************************
 * Function Name: pcApplicationHostnameHook
 * Description  : DHCP Hook function to populate the user defined Host name for the Kit
 * Return Value : Hostname
 *********************************************************************************************************************/
const char *pcApplicationHostnameHook(void)
{
    return (KIT_NAME);
}
/**********************************************************************************************************************
 End of function pcApplicationHostnameHook
 *********************************************************************************************************************/#endif






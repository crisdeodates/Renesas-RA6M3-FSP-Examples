/***********************************************************************************************************************
 * File Name    : ftp_server_ep.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FTP_SERVER_EP_H_
#define FTP_SERVER_EP_H_
#include "common_utils.h"
#include "board_qspi.h"


#define USER_DISP_MSG               "Please input file name to be viewed !! \r\n"

#define HANDLE_API_FAILURE_IF_FAIL(api_ret, err, msg)   { if((api_ret==err)){FSP_PARAMETER_NOT_USED (api_ret);}\
                                    else {PRINT_ERR_STR(msg);ERROR_TRAP(api_ret);}}


/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), str);
#define PRINT_ERR_STR(str)   app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), str);
#define ERROR_TRAP(err)      app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &err);

#define EP_INFO        "\r\nThis project demonstrates the basic functionality of Netx Duo ftp server\r\n"\
                       "with ethernet driver on Renesas RA MCUs based on Renesas FSP using AzureRTOS.\r\n"\
                       "ftp Server is created, user can communicate through server through valid FTP client \r\n"\
                       "Error and info messages will be printed on JlinkRTTViewer.\r\n\n\n"
#define MIN_FILE_NAME_LENGTH    2U

#define LOGIN_EVENT_FLAG        (UINT)(1<<2)
#define DEFAULT_SIZE            (ULONG)40
#define SAMPLE_DATA_BYTES       {82,101,110,101,115,97,115,95,102,116,112,95,115,101,114,118,101,114,69,112}
#define SAMPLE_DATA_SIZE        20U
#define WAIT_OPTION_NETWORK_RETRY      50U
/* ***************** Azure FileX API documentation ********************
 * fx_media_format API requires memory region long enough to
 * hold several directory entries, a data structure to stack the current
 * directory entry position before entering into sub directories
 * and finally the logical FAT bit map. The scratch memory should be at
 * least 512-1024 bytes plus memory for the logical FAT bit map, which requires
 * as many bits as there are clusters in the media. For example,
 * a device with 8000 clusters would require 1000 bytes to represent and thus
 * require a total scratch area on the order of 2048 bytes.
 **************************************************************************
 * In current case number of cluster is 8192
 * number of cluster = (number of sectors/ number of sector per cluster)
 * number of sectors is 8192 and number of sector per cluster is 1
 * On following above logic 8192 cluster accounts for
 * 2100 bytes of memory
 * (2048/8000)*8192 = 2097.15 rounded to 2100
 */
#define SCRATCH_MEM_SIZE        2100U
#endif /* FTP_SERVER_EP_H_ */

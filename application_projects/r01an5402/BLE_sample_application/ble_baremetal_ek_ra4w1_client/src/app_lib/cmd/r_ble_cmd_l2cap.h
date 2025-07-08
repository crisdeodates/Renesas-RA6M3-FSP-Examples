/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019-2020 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup cmd_l2cap L2CAP Command Library
 * @{
 * @ingroup app_lib
 * @brief L2CAP Commands
 * @details Provides CLI for L2CAP. And exposes APIs used in the commands.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 15.12.2020 1.00    First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "cli/r_ble_cli.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifndef R_BLE_CMD_L2CAP_H
#define R_BLE_CMD_L2CAP_H

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/

/** @brief L2CAP command definition. */
extern const st_ble_cli_cmd_t g_l2cap_cmd;


/*******************************************************************************************************************//**
 * @brief Register L2CAP Credit-based service PSM
 * @details
 * This function register/deregister psm.
 * @param[in] psm Credit-based service psm.
 * @param[in] lwm Low water mark
***********************************************************************************************************************/
void R_BLE_CMD_RegisterL2capPSM(uint16_t psm, uint16_t lwm);

/*******************************************************************************************************************//**
 * @brief Connect/Disconnect operation .
 * @details
 * This function connects/disconnect l2cap credit-based service.
 * @param[in] conn_hdl Connection handle.
 * @param[in] psm   Credit-based service psm.
 * @param[in] mtu   MTU
 * @param[in] credit Credit
***********************************************************************************************************************/
void R_BLE_CMD_RequestL2capConn(uint16_t conn_hdl, uint16_t psm, uint16_t mtu, uint16_t credit);

/*******************************************************************************************************************//**
 * @brief Reply connect request.
 * @details
 * This function reply connection request from remote device.
 * @param[in] lcid Local channel ID.
 * @param[in] response Response for connection request.
 * @param[in] mtu   MTU
 * @param[in] credit Credit
***********************************************************************************************************************/
void R_BLE_CMD_ReplyL2capConn(uint16_t lcid, uint16_t response, uint16_t mtu, uint16_t credit);


#endif /* R_BLE_CMD_L2CAP_H */

/** @} */

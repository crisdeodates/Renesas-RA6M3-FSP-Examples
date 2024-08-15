/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name: r_ble_lls.h
 * Description : The header file for Link Loss Service Server.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup lls Link Loss Service Server
 * @{
 * @ingroup profile
 * @brief   This service uses the Alert Level characteristic to cause an alert in the device when the link is lost.
 **********************************************************************************************************************/
#include "profile_cmn/r_ble_servs_if.h"
#include "gatt_db.h"

#ifndef R_BLE_LLS_H
#define R_BLE_LLS_H

/* Start user code for file includes and macro definitions. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/*----------------------------------------------------------------------------------------------------------------------
    Alert Level Characteristic : The level of an alert a device is to sound. If this level is changed while the alert is being sounded,the new level should take effect.
----------------------------------------------------------------------------------------------------------------------*/
/***************************************************************************//**
 * @brief Alert Level Level enumeration.
*******************************************************************************/
typedef enum {
    BLE_LLS_ALERT_LEVEL_LEVEL_NO_ALERT = 0, /**< No Alert */
    BLE_LLS_ALERT_LEVEL_LEVEL_MILD_ALERT = 1, /**< Mild Alert */
    BLE_LLS_ALERT_LEVEL_LEVEL_HIGH_ALERT = 2, /**< High Alert */
} e_ble_lls_alert_level_level_t;

/***************************************************************************//**
 * @brief     Set Alert Level characteristic value to the local GATT database.
 * @param[in] conn_hdl Connection handle.
 * @param[in] p_value  Characteristic value to set.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LLS_SetAlertLevel(uint16_t conn_hdl, const uint8_t *p_value);

/***************************************************************************//**
 * @brief     Get Alert Level characteristic value from the local GATT database.
 * @param[in] conn_hdl Connection handle.
 * @param[out] p_value  Output location for the acquired descriptor value.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LLS_GetAlertLevel(uint16_t conn_hdl, uint8_t *p_value);

/*----------------------------------------------------------------------------------------------------------------------
    Link Loss Service Server
----------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief Link Loss Service characteristic Index.
*******************************************************************************/
typedef enum {
    BLE_LLS_ALERT_LEVEL_IDX,
} e_ble_lls_char_idx_t;

/***************************************************************************//**
 * @brief Link Loss Service event type.
*******************************************************************************/
typedef enum {
    /* Alert Level */
    BLE_LLS_EVENT_ALERT_LEVEL_WRITE_REQ = BLE_SERVS_ATTR_EVENT(BLE_LLS_ALERT_LEVEL_IDX, BLE_SERVS_WRITE_REQ),
    BLE_LLS_EVENT_ALERT_LEVEL_WRITE_COMP = BLE_SERVS_ATTR_EVENT(BLE_LLS_ALERT_LEVEL_IDX, BLE_SERVS_WRITE_COMP),
    BLE_LLS_EVENT_ALERT_LEVEL_READ_REQ = BLE_SERVS_ATTR_EVENT(BLE_LLS_ALERT_LEVEL_IDX, BLE_SERVS_READ_REQ),
} e_ble_lls_event_t;

/***************************************************************************//**
 * @brief     Initialize Link Loss Service server.
 * @param[in] cb Service callback.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LLS_Init(ble_servs_app_cb_t cb);

/* Start user code for function declarations. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

#endif /* R_BLE_LLS_H */

/** @} */

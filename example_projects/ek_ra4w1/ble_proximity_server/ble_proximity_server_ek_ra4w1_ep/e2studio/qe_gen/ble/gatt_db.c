/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**
 *  GATT DATABASE QUICK REFERENCE TABLE:
 *  Abbreviations used for PROPERTIES:
 *      BC = Broadcast
 *      RD = Read
 *      WW = Write Without Response
 *      WR = Write
 *      NT = Notification
 *      IN = Indication
 *      RW = Reliable Write
 * 
 *  HANDLE | ATT_TYPE          | PROPERTIES  | ATT_VALUE                        | DEFINITION
 *  ============================================================================================
 *  GAP Service
 *  ============================================================================================
 *  0x0001 | 0x28,0x00         | RD          | 0x7e,0x76,0x10,0x96,0xd6,0x87... | GAP Service Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0002 | 0x28,0x02         | RD          | 0x17,0x00,0x1C,0x00,0x02,0x18    | Immediate Alert Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0003 | 0x28,0x02         | RD          | 0x1D,0x00,0x23,0x00,0x03,0x18    | Link Loss Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0004 | 0x28,0x02         | RD          | 0x24,0x00,0x2A,0x00,0x04,0x18    | Tx Power Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0005 | 0x28,0x03         | RD          | 0x0A,0x06,0x00,0x00,0x2A         | Device Name characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0006 | 0x00,0x2A         | RD,WR       | 0x00,0x00,0x00,0x00,0x00,0x00... | Device Name characteristic value
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0007 | 0x28,0x03         | RD          | 0x02,0x08,0x00,0x01,0x2A         | Appearance characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0008 | 0x01,0x2A         | RD          | 0x00,0x00                        | Appearance characteristic value
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0009 | 0x28,0x03         | RD          | 0x02,0x0A,0x00,0x04,0x2A         | Peripheral Preferred Connection Parameters characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x000A | 0x04,0x2A         | RD          | 0x00,0x00,0x00,0x00,0x00,0x00... | Peripheral Preferred Connection Parameters characteristic value
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x000B | 0x28,0x03         | RD          | 0x02,0x0C,0x00,0xA6,0x2A         | Central Address Resolution characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x000C | 0xA6,0x2A         | RD          | 0x00                             | Central Address Resolution characteristic value
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x000D | 0x28,0x03         | RD          | 0x02,0x0E,0x00,0xC9,0x2A         | Resolvable Private Address Only characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x000E | 0xC9,0x2A         | RD          | 0x00                             | Resolvable Private Address Only characteristic value
 *  ============================================================================================
 *  GATT Service
 *  ============================================================================================
 *  0x000F | 0x28,0x00         | RD          | 0x01,0x18                        | GATT Service Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0010 | 0x28,0x02         | RD          | 0x01,0x00,0x0E,0x00              | GAP Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0011 | 0x28,0x02         | RD          | 0x17,0x00,0x1C,0x00,0x02,0x18    | Immediate Alert Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0012 | 0x28,0x02         | RD          | 0x1D,0x00,0x23,0x00,0x03,0x18    | Link Loss Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0013 | 0x28,0x02         | RD          | 0x24,0x00,0x2A,0x00,0x04,0x18    | Tx Power Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0014 | 0x28,0x03         | RD          | 0x20,0x15,0x00,0x05,0x2A         | Service Changed characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0015 | 0x05,0x2A         | IN          | 0x00,0x00,0x00,0x00              | Service Changed characteristic value
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0016 | 0x02,0x29         | RD,WR       | 0x00,0x00                        | Client Characteristic Configuration descriptor
 *  ============================================================================================
 *  Immediate Alert Service
 *  ============================================================================================
 *  0x0017 | 0x28,0x00         | RD          | 0x02,0x18                        | Immediate Alert Service Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0018 | 0x28,0x02         | RD          | 0x24,0x00,0x2A,0x00,0x04,0x18    | Tx Power Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0019 | 0x28,0x02         | RD          | 0x1D,0x00,0x23,0x00,0x03,0x18    | Link Loss Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x001A | 0x28,0x02         | RD          | 0x0F,0x00,0x16,0x00,0x01,0x18    | GATT Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x001B | 0x28,0x03         | RD          | 0x04,0x1C,0x00,0x06,0x2A         | Alert Level characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x001C | 0x06,0x2A         | WW          | 0x00                             | Alert Level characteristic value
 *  ============================================================================================
 *  Link Loss Service
 *  ============================================================================================
 *  0x001D | 0x28,0x00         | RD          | 0x03,0x18                        | Link Loss Service Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x001E | 0x28,0x02         | RD          | 0x01,0x00,0x0E,0x00              | GAP Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x001F | 0x28,0x02         | RD          | 0x17,0x00,0x1C,0x00,0x02,0x18    | Immediate Alert Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0020 | 0x28,0x02         | RD          | 0x24,0x00,0x2A,0x00,0x04,0x18    | Tx Power Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0021 | 0x28,0x02         | RD          | 0x0F,0x00,0x16,0x00,0x01,0x18    | GATT Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0022 | 0x28,0x03         | RD          | 0x0A,0x23,0x00,0x06,0x2A         | Alert Level characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0023 | 0x06,0x2A         | RD,WR       | 0x00                             | Alert Level characteristic value
 *  ============================================================================================
 *  Tx Power Service
 *  ============================================================================================
 *  0x0024 | 0x28,0x00         | RD          | 0x04,0x18                        | Tx Power Service Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0025 | 0x28,0x02         | RD          | 0x01,0x00,0x0E,0x00              | GAP Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0026 | 0x28,0x02         | RD          | 0x17,0x00,0x1C,0x00,0x02,0x18    | Immediate Alert Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0027 | 0x28,0x02         | RD          | 0x1D,0x00,0x23,0x00,0x03,0x18    | Link Loss Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0028 | 0x28,0x02         | RD          | 0x0F,0x00,0x16,0x00,0x01,0x18    | GATT Service Included Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x0029 | 0x28,0x03         | RD          | 0x02,0x2A,0x00,0x07,0x2A         | Tx Power Level characteristic Declaration
 *  -------+-------------------+-------------+----------------------------------+---------------
 *  0x002A | 0x07,0x2A         | RD          | 0x00                             | Tx Power Level characteristic value
 *  ============================================================================================
 
 */

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdio.h>
#include "gatt_db.h"

/*****************************************************************************
* Global definition
******************************************************************************/
static const uint8_t gs_gatt_const_uuid_arr[] =
{
    /* Primary Service Declaration : 0 */
    0x00, 0x28,

    /* Secondary Service Declaration : 2 */
    0x01, 0x28,

    /* Included Service Declaration : 4 */
    0x02, 0x28,

    /* Characteristic Declaration : 6 */
    0x03, 0x28,

    /* GAP Service : 8 */
    0x7e, 0x76, 0x10, 0x96, 0xd6, 0x87, 0x39, 0x99, 0xe1, 0x4e, 0x11, 0x13, 0x62, 0x34, 0xf0, 0x27,

    /* Device Name : 24 */
    0x00, 0x2A,

    /* Appearance : 26 */
    0x01, 0x2A,

    /* Peripheral Preferred Connection Parameters : 28 */
    0x04, 0x2A,

    /* Central Address Resolution : 30 */
    0xA6, 0x2A,

    /* Resolvable Private Address Only : 32 */
    0xC9, 0x2A,

    /* GATT Service : 34 */
    0x01, 0x18,

    /* Service Changed : 36 */
    0x05, 0x2A,

    /* Client Characteristic Configuration : 38 */
    0x02, 0x29,

    /* Immediate Alert Service : 40 */
    0x02, 0x18,

    /* Alert Level : 42 */
    0x06, 0x2A,

    /* Link Loss Service : 44 */
    0x03, 0x18,

    /* Tx Power Service : 46 */
    0x04, 0x18,

    /* Tx Power Level : 48 */
    0x07, 0x2A,

};

static uint8_t gs_gatt_value_arr[] =
{
    /* Device Name */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* Appearance */
    0x00, 0x00,

    /* Peripheral Preferred Connection Parameters */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* Central Address Resolution */
    0x00,

    /* Resolvable Private Address Only */
    0x00,

};

static const uint8_t gs_gatt_const_value_arr[] =
{
    /* Included Service : Immediate Alert Service */
    0x17, 0x00, // Start Handle
    0x1C, 0x00, // End Handle
    0x02, 0x18, // UUID

    /* Included Service : Link Loss Service */
    0x1D, 0x00, // Start Handle
    0x23, 0x00, // End Handle
    0x03, 0x18, // UUID

    /* Included Service : Tx Power Service */
    0x24, 0x00, // Start Handle
    0x2A, 0x00, // End Handle
    0x04, 0x18, // UUID

    /* Device Name */
    0x0A,       // Properties
    0x06, 0x00, // Attr Handle
    0x00, 0x2A, // UUID

    /* Appearance */
    0x02,       // Properties
    0x08, 0x00, // Attr Handle
    0x01, 0x2A, // UUID

    /* Peripheral Preferred Connection Parameters */
    0x02,       // Properties
    0x0A, 0x00, // Attr Handle
    0x04, 0x2A, // UUID

    /* Central Address Resolution */
    0x02,       // Properties
    0x0C, 0x00, // Attr Handle
    0xA6, 0x2A, // UUID

    /* Resolvable Private Address Only */
    0x02,       // Properties
    0x0E, 0x00, // Attr Handle
    0xC9, 0x2A, // UUID

    /* Included Service : GAP Service */
    0x01, 0x00, // Start Handle
    0x0E, 0x00, // End Handle

    /* Included Service : Immediate Alert Service */
    0x17, 0x00, // Start Handle
    0x1C, 0x00, // End Handle
    0x02, 0x18, // UUID

    /* Included Service : Link Loss Service */
    0x1D, 0x00, // Start Handle
    0x23, 0x00, // End Handle
    0x03, 0x18, // UUID

    /* Included Service : Tx Power Service */
    0x24, 0x00, // Start Handle
    0x2A, 0x00, // End Handle
    0x04, 0x18, // UUID

    /* Service Changed */
    0x20,       // Properties
    0x15, 0x00, // Attr Handle
    0x05, 0x2A, // UUID

    /* Included Service : Tx Power Service */
    0x24, 0x00, // Start Handle
    0x2A, 0x00, // End Handle
    0x04, 0x18, // UUID

    /* Included Service : Link Loss Service */
    0x1D, 0x00, // Start Handle
    0x23, 0x00, // End Handle
    0x03, 0x18, // UUID

    /* Included Service : GATT Service */
    0x0F, 0x00, // Start Handle
    0x16, 0x00, // End Handle
    0x01, 0x18, // UUID

    /* Alert Level */
    0x04,       // Properties
    0x1C, 0x00, // Attr Handle
    0x06, 0x2A, // UUID

    /* Included Service : GAP Service */
    0x01, 0x00, // Start Handle
    0x0E, 0x00, // End Handle

    /* Included Service : Immediate Alert Service */
    0x17, 0x00, // Start Handle
    0x1C, 0x00, // End Handle
    0x02, 0x18, // UUID

    /* Included Service : Tx Power Service */
    0x24, 0x00, // Start Handle
    0x2A, 0x00, // End Handle
    0x04, 0x18, // UUID

    /* Included Service : GATT Service */
    0x0F, 0x00, // Start Handle
    0x16, 0x00, // End Handle
    0x01, 0x18, // UUID

    /* Alert Level */
    0x0A,       // Properties
    0x23, 0x00, // Attr Handle
    0x06, 0x2A, // UUID

    /* Included Service : GAP Service */
    0x01, 0x00, // Start Handle
    0x0E, 0x00, // End Handle

    /* Included Service : Immediate Alert Service */
    0x17, 0x00, // Start Handle
    0x1C, 0x00, // End Handle
    0x02, 0x18, // UUID

    /* Included Service : Link Loss Service */
    0x1D, 0x00, // Start Handle
    0x23, 0x00, // End Handle
    0x03, 0x18, // UUID

    /* Included Service : GATT Service */
    0x0F, 0x00, // Start Handle
    0x16, 0x00, // End Handle
    0x01, 0x18, // UUID

    /* Tx Power Level */
    0x02,       // Properties
    0x2A, 0x00, // Attr Handle
    0x07, 0x2A, // UUID

};

static const uint8_t gs_gatt_db_const_peer_specific_val_arr[] =
{
    /* Service Changed : Client Characteristic Configuration */
    0x00, 0x00,

    /* Alert Level */
    0x00,

    /* Alert Level */
    0x00,

    /* Tx Power Level */
    0,

};

#ifdef BSP_MCU_GROUP_RA4W1 /* RA4W1 */
static uint8_t gs_gatt_db_peer_specific_val_arr[sizeof(gs_gatt_db_const_peer_specific_val_arr)*(BLE_ABS_CFG_RF_CONNECTION_MAXIMUM+1)];
#else /* RX23W or RE01B */
static uint8_t gs_gatt_db_peer_specific_val_arr[sizeof(gs_gatt_db_const_peer_specific_val_arr)*(BLE_CFG_RF_CONN_MAX+1)];
#endif
static const st_ble_gatts_db_uuid_cfg_t gs_gatt_type_table[] =
{
    /* 0 : Primary Service Declaration */
    {
        /* UUID Offset */
        0,
        /* First Occurrence for Type */
        0x0001,
        /* Last Occurrence for Type */
        0x0024,
    },

    /* 1 : GAP Service */
    {
        /* UUID Offset */
        8,
        /* First Occurrence for Type */
        0x0001,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 2 : Included Service Declaration */
    {
        /* UUID Offset */
        4,
        /* First Occurrence for Type */
        0x0002,
        /* Last Occurrence for Type */
        0x0028,
    },

    /* 3 : Characteristic Declaration */
    {
        /* UUID Offset */
        6,
        /* First Occurrence for Type */
        0x0005,
        /* Last Occurrence for Type */
        0x0029,
    },

    /* 4 : Device Name */
    {
        /* UUID Offset */
        24,
        /* First Occurrence for Type */
        0x0006,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 5 : Appearance */
    {
        /* UUID Offset */
        26,
        /* First Occurrence for Type */
        0x0008,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 6 : Peripheral Preferred Connection Parameters */
    {
        /* UUID Offset */
        28,
        /* First Occurrence for Type */
        0x000A,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 7 : Central Address Resolution */
    {
        /* UUID Offset */
        30,
        /* First Occurrence for Type */
        0x000C,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 8 : Resolvable Private Address Only */
    {
        /* UUID Offset */
        32,
        /* First Occurrence for Type */
        0x000E,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 9 : GATT Service */
    {
        /* UUID Offset */
        34,
        /* First Occurrence for Type */
        0x000F,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 10 : Service Changed */
    {
        /* UUID Offset */
        36,
        /* First Occurrence for Type */
        0x0015,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 11 : Client Characteristic Configuration */
    {
        /* UUID Offset */
        38,
        /* First Occurrence for Type */
        0x0016,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 12 : Immediate Alert Service */
    {
        /* UUID Offset */
        40,
        /* First Occurrence for Type */
        0x0017,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 13 : Alert Level */
    {
        /* UUID Offset */
        42,
        /* First Occurrence for Type */
        0x001C,
        /* Last Occurrence for Type */
        0x0023,
    },

    /* 14 : Link Loss Service */
    {
        /* UUID Offset */
        44,
        /* First Occurrence for Type */
        0x001D,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 15 : Tx Power Service */
    {
        /* UUID Offset */
        46,
        /* First Occurrence for Type */
        0x0024,
        /* Last Occurrence for Type */
        0x0000,
    },

    /* 16 : Tx Power Level */
    {
        /* UUID Offset */
        48,
        /* First Occurrence for Type */
        0x002A,
        /* Last Occurrence for Type */
        0x0000,
    },

};

static const st_ble_gatts_db_attr_cfg_t gs_gatt_db_attr_table[] =
{
    /* Handle : 0x0000 */
    /* Blank */
    {
        /* Properties */
        0,
        /* Auxiliary Properties */
        BLE_GATT_DB_NO_AUXILIARY_PROPERTY,
        /* Value Size */
        1,
        /* Next Attribute Type Index */
        0x0001,
        /* UUID Offset */
        0,
        /* Value */
        NULL,
    },

    /* Handle : 0x0001 */
    /* GAP Service : Primary Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY | BLE_GATT_DB_128_BIT_UUID_FORMAT,
        /* Value Size */
        16,
        /* Next Attribute Type Index */
        0x000F,
        /* UUID Offset */
        0,
        /* Value */
        (uint8_t *)(gs_gatt_const_uuid_arr + 8),
    },

    /* Handle : 0x0002 */
    /* Immediate Alert Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0003,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 0),
    },

    /* Handle : 0x0003 */
    /* Link Loss Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0004,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 6),
    },

    /* Handle : 0x0004 */
    /* Tx Power Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0010,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 12),
    },

    /* Handle : 0x0005 */
    /* Device Name : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x0007,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 18),
    },

    /* Handle : 0x0006 */
    /* Device Name */
    {
        /* Properties */
        BLE_GATT_DB_READ | BLE_GATT_DB_WRITE,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        128,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        24,
        /* Value */
        (uint8_t *)(gs_gatt_value_arr + 0),
    },

    /* Handle : 0x0007 */
    /* Appearance : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x0009,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 23),
    },

    /* Handle : 0x0008 */
    /* Appearance */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        2,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        26,
        /* Value */
        (uint8_t *)(gs_gatt_value_arr + 128),
    },

    /* Handle : 0x0009 */
    /* Peripheral Preferred Connection Parameters : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x000B,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 28),
    },

    /* Handle : 0x000A */
    /* Peripheral Preferred Connection Parameters */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        8,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        28,
        /* Value */
        (uint8_t *)(gs_gatt_value_arr + 130),
    },

    /* Handle : 0x000B */
    /* Central Address Resolution : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x000D,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 33),
    },

    /* Handle : 0x000C */
    /* Central Address Resolution */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        1,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        30,
        /* Value */
        (uint8_t *)(gs_gatt_value_arr + 138),
    },

    /* Handle : 0x000D */
    /* Resolvable Private Address Only : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x0014,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 38),
    },

    /* Handle : 0x000E */
    /* Resolvable Private Address Only */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        1,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        32,
        /* Value */
        (uint8_t *)(gs_gatt_value_arr + 139),
    },

    /* Handle : 0x000F */
    /* GATT Service : Primary Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        2,
        /* Next Attribute Type Index */
        0x0017,
        /* UUID Offset */
        0,
        /* Value */
        (uint8_t *)(gs_gatt_const_uuid_arr + 34),
    },

    /* Handle : 0x0010 */
    /* GAP Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        4,
        /* Next Attribute Type Index */
        0x0011,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 43),
    },

    /* Handle : 0x0011 */
    /* Immediate Alert Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0012,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 47),
    },

    /* Handle : 0x0012 */
    /* Link Loss Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0013,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 53),
    },

    /* Handle : 0x0013 */
    /* Tx Power Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0018,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 59),
    },

    /* Handle : 0x0014 */
    /* Service Changed : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x001B,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 65),
    },

    /* Handle : 0x0015 */
    /* Service Changed */
    {
        /* Properties */
        0,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        4,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        36,
        /* Value */
        (NULL),
    },

    /* Handle : 0x0016 */
    /* Service Changed : Client Characteristic Configuration */
    {
        /* Properties */
        BLE_GATT_DB_READ | BLE_GATT_DB_WRITE,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY | BLE_GATT_DB_PEER_SPECIFIC_VAL_PROPERTY,
        /* Value Size */
        2,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        38,
        /* Value */
        (uint8_t *)(gs_gatt_db_peer_specific_val_arr + 0),
    },

    /* Handle : 0x0017 */
    /* Immediate Alert Service : Primary Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        2,
        /* Next Attribute Type Index */
        0x001D,
        /* UUID Offset */
        0,
        /* Value */
        (uint8_t *)(gs_gatt_const_uuid_arr + 40),
    },

    /* Handle : 0x0018 */
    /* Tx Power Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0019,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 70),
    },

    /* Handle : 0x0019 */
    /* Link Loss Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x001A,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 76),
    },

    /* Handle : 0x001A */
    /* GATT Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x001E,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 82),
    },

    /* Handle : 0x001B */
    /* Alert Level : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x0022,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 88),
    },

    /* Handle : 0x001C */
    /* Alert Level */
    {
        /* Properties */
        BLE_GATT_DB_WRITE_WITHOUT_RSP,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY | BLE_GATT_DB_PEER_SPECIFIC_VAL_PROPERTY,
        /* Value Size */
        1,
        /* Next Attribute Type Index */
        0x0023,
        /* UUID Offset */
        42,
        /* Value */
        (uint8_t *)(gs_gatt_db_peer_specific_val_arr + 2),
    },

    /* Handle : 0x001D */
    /* Link Loss Service : Primary Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        2,
        /* Next Attribute Type Index */
        0x0024,
        /* UUID Offset */
        0,
        /* Value */
        (uint8_t *)(gs_gatt_const_uuid_arr + 44),
    },

    /* Handle : 0x001E */
    /* GAP Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        4,
        /* Next Attribute Type Index */
        0x001F,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 93),
    },

    /* Handle : 0x001F */
    /* Immediate Alert Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0020,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 97),
    },

    /* Handle : 0x0020 */
    /* Tx Power Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0021,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 103),
    },

    /* Handle : 0x0021 */
    /* GATT Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0025,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 109),
    },

    /* Handle : 0x0022 */
    /* Alert Level : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x0029,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 115),
    },

    /* Handle : 0x0023 */
    /* Alert Level */
    {
        /* Properties */
        BLE_GATT_DB_READ | BLE_GATT_DB_WRITE,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY | BLE_GATT_DB_PEER_SPECIFIC_VAL_PROPERTY,
        /* Value Size */
        1,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        42,
        /* Value */
        (uint8_t *)(gs_gatt_db_peer_specific_val_arr + 3),
    },

    /* Handle : 0x0024 */
    /* Tx Power Service : Primary Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        2,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        0,
        /* Value */
        (uint8_t *)(gs_gatt_const_uuid_arr + 46),
    },

    /* Handle : 0x0025 */
    /* GAP Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        4,
        /* Next Attribute Type Index */
        0x0026,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 120),
    },

    /* Handle : 0x0026 */
    /* Immediate Alert Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0027,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 124),
    },

    /* Handle : 0x0027 */
    /* Link Loss Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0028,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 130),
    },

    /* Handle : 0x0028 */
    /* GATT Service : Included Service Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        6,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        4,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 136),
    },

    /* Handle : 0x0029 */
    /* Tx Power Level : Characteristic Declaration */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY,
        /* Value Size */
        5,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        6,
        /* Value */
        (uint8_t *)(gs_gatt_const_value_arr + 142),
    },

    /* Handle : 0x002A */
    /* Tx Power Level */
    {
        /* Properties */
        BLE_GATT_DB_READ,
        /* Auxiliary Properties */
        BLE_GATT_DB_FIXED_LENGTH_PROPERTY | BLE_GATT_DB_PEER_SPECIFIC_VAL_PROPERTY,
        /* Value Size */
        1,
        /* Next Attribute Type Index */
        0x0000,
        /* UUID Offset */
        48,
        /* Value */
        (uint8_t *)(gs_gatt_db_peer_specific_val_arr + 4),
    },

};

static const st_ble_gatts_db_char_cfg_t gs_gatt_characteristic[] =
{
    /* 0 : Device Name */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x0005,
        /* Service Index */
        0,
    },

    /* 1 : Appearance */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x0007,
        /* Service Index */
        0,
    },

    /* 2 : Peripheral Preferred Connection Parameters */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x0009,
        /* Service Index */
        0,
    },

    /* 3 : Central Address Resolution */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x000B,
        /* Service Index */
        0,
    },

    /* 4 : Resolvable Private Address Only */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x000D,
        /* Service Index */
        0,
    },

    /* 5 : Service Changed */
    {
        /* Number of Attributes */
        {
            3,
        },
        /* Start Handle */
        0x0014,
        /* Service Index */
        1,
    },

    /* 6 : Alert Level */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x001B,
        /* Service Index */
        2,
    },

    /* 7 : Alert Level */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x0022,
        /* Service Index */
        3,
    },

    /* 8 : Tx Power Level */
    {
        /* Number of Attributes */
        {
            2,
        },
        /* Start Handle */
        0x0029,
        /* Service Index */
        4,
    },

};

static const st_ble_gatts_db_serv_cfg_t gs_gatt_service[] =
{
    /* GAP Service */
    {
        /* Num of Services */
        {
            1,
        },
        /* Description */
        0,
        /* Service Start Handle */
        0x0001,
        /* Service End Handle */
        0x000E,
        /* Characteristic Start Index */
        0,
        /* Characteristic End Index */
        4,
    },

    /* GATT Service */
    {
        /* Num of Services */
        {
            1,
        },
        /* Description */
        0,
        /* Service Start Handle */
        0x000F,
        /* Service End Handle */
        0x0016,
        /* Characteristic Start Index */
        5,
        /* Characteristic End Index */
        5,
    },

    /* Immediate Alert Service */
    {
        /* Num of Services */
        {
            1,
        },
        /* Description */
        0,
        /* Service Start Handle */
        0x0017,
        /* Service End Handle */
        0x001C,
        /* Characteristic Start Index */
        6,
        /* Characteristic End Index */
        6,
    },

    /* Link Loss Service */
    {
        /* Num of Services */
        {
            1,
        },
        /* Description */
        0,
        /* Service Start Handle */
        0x001D,
        /* Service End Handle */
        0x0023,
        /* Characteristic Start Index */
        7,
        /* Characteristic End Index */
        7,
    },

    /* Tx Power Service */
    {
        /* Num of Services */
        {
            1,
        },
        /* Description */
        0,
        /* Service Start Handle */
        0x0024,
        /* Service End Handle */
        0x002A,
        /* Characteristic Start Index */
        8,
        /* Characteristic End Index */
        8,
    },

};

st_ble_gatts_db_cfg_t g_gatt_db_table =
{
    gs_gatt_const_uuid_arr,
    gs_gatt_value_arr,
    gs_gatt_const_value_arr,
    gs_gatt_db_peer_specific_val_arr,
    gs_gatt_db_const_peer_specific_val_arr,
    gs_gatt_type_table,
    gs_gatt_db_attr_table,
    gs_gatt_characteristic,
    gs_gatt_service,
    ARRAY_SIZE(gs_gatt_service),
    ARRAY_SIZE(gs_gatt_characteristic),
    ARRAY_SIZE(gs_gatt_type_table),
    ARRAY_SIZE(gs_gatt_db_const_peer_specific_val_arr),
};
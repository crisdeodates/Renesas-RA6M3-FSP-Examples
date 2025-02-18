/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : qe_ble_profile.h
* Description  : This file includes definitions.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History      : MM/DD/YYYY Version Description
*              : 08/31/2021 1.00    First Release
***********************************************************************************************************************/

#ifndef QE_BLE_PROFILE_H
#define QE_BLE_PROFILE_H

#include <stdint.h>

/***********************************************************************************************************************
* Type definitions
***********************************************************************************************************************/
typedef enum qe_attribute_handle
{
	QE_ATTRIBUTE_HANDLE_PROFILE_START = 0,
	QE_ATTRIBUTE_HANDLE_SERVICE_DECL_GAP,
	QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_DECL_GAP_DEV_NAME,
	QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_VALUE_GAP_DEV_NAME,
	QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_DECL_GAP_APPEARANCE,
	QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_VALUE_GAP_APPEARANCE,
	QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_DECL_GAP_PER_PREF_CONN_PARAM,
	QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_VALUE_GAP_PER_PREF_CONN_PARAM,
	QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_DECL_GAP_CENT_ADDR_RSLV,
	QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_VALUE_GAP_CENT_ADDR_RSLV,
	QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_DECL_GAP_RSLV_PRIV_ADDR_ONLY,
	QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_VALUE_GAP_RSLV_PRIV_ADDR_ONLY,
	QE_ATTRIBUTE_HANDLE_SERVICE_DECL_GAT,
	QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_DECL_GAT_SERV_CHGED,
	QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_VALUE_GAT_SERV_CHGED,
	QE_ATTRIBUTE_HANDLE_DESCRIPTOR_GAT_SERV_CHGED_CLI_CNFG,
	QE_ATTRIBUTE_HANDLE_PROFILE_END
} qe_attribute_handle_t;

typedef struct attribute
{												//These comments reflect the gatt_db.c GATT DATABASE QUICK REFERENCE TABLE
	uint16_t		handle;						//HANDLE: 		The handle for the attribute
	uint16_t		encapsulated_attributes;	//This signifies the number of attributes inside this service. If none exist place 0, if the service includes 4 attributes place 5 (4+itself)
	uint8_t			permissions;				//PROPERTIES: 	EX = RD,WR
	uint8_t			uuid_length;				//				Either 2 byes for adopted or 16 bytes for custom
	uint16_t		value_length;				//				Length of the value for this attribute
	uint8_t 		*uuid;						//ATT_TYPE: 	Pointer to the UUID data container (size of uuid_length)
	uint8_t			*value;						//ATT_VALUE:	Pointer to the value for our attribute
	uint8_t			notify_write;				//New Boolean value, does the host need to be notified if this attribute is written
	uint8_t			notify_read;				//New Boolean value, does the host need to be notified if this attribute is read
} attribute_t;

typedef enum advertising_types
{
	ADVERTISING_TYPE_DATA,
	ADVERTISING_TYPE_SCAN_RESPONSE,
	ADVERTISING_TYPE_DATA_EXTENDED,
	ADVERTISING_TYPE_PERIODIC
}advertising_types_t;

typedef struct advertising_pdu
{
	advertising_types_t pdu_type;
	uint16_t			size;
	uint8_t				data[];
} advertising_pdu_t;

extern const attribute_t qe_ble_profile[QE_ATTRIBUTE_HANDLE_PROFILE_END];

/***********************************************************************************************************************
* Macros
***********************************************************************************************************************/
#define		QE_BLE_PROFILE_UUID_SIZE_ADOPTED	(0x02)
#define		QE_BLE_PROFILE_UUID_SIZE_CUSTOM		(0x10)

#define		QE_BLE_PROFILE_NUMBER_OF_ATTRIBUTES	(0x0010)

#endif /* QE_BLE_PROFILE_H */

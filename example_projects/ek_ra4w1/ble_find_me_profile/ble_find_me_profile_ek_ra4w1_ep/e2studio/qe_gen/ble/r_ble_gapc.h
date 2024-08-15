/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name: r_ble_gapc.h
 * Description : The header file for GAP Service client.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup gapc GAP Service Client
 * @{
 * @ingroup profile
 * @brief   The generic_access service contains generic information about the device.
 **********************************************************************************************************************/
#include "profile_cmn/r_ble_servc_if.h"

#ifndef R_BLE_GAPC_H
#define R_BLE_GAPC_H

/* Start user code for file includes and macro definitions. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/*----------------------------------------------------------------------------------------------------------------------
    Device Name Characteristic : The Device Name characteristic shall contain the name of the device.
----------------------------------------------------------------------------------------------------------------------*/
#define BLE_GAPC_DEV_NAME_UUID (0x2A00)
#define BLE_GAPC_DEV_NAME_LEN (128)
/***************************************************************************//**
 * @brief Device Name value structure.
*******************************************************************************/
typedef struct {
    uint8_t name[128]; /**< Name */
    uint8_t length; /**< Length */
} st_ble_gapc_dev_name_t;

/***************************************************************************//**
 * @brief Device Name attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
} st_ble_gapc_dev_name_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read Device Name characteristic value from the remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_ReadDevName(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief     Write Device Name characteristic value to remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @param[in] p_value Device Name characteristic value to write.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_WriteDevName(uint16_t conn_hdl, const st_ble_gapc_dev_name_t *p_value);

/***************************************************************************//**
 * @brief      Get Device Name attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GAPC_GetDevNameAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_dev_name_attr_hdl_t *p_hdl);

/*----------------------------------------------------------------------------------------------------------------------
    Appearance Characteristic : The Appearance characteristic defines the representation of the external appearance of the device.
----------------------------------------------------------------------------------------------------------------------*/
#define BLE_GAPC_APPEARANCE_UUID (0x2A01)
#define BLE_GAPC_APPEARANCE_LEN (2)
/***************************************************************************//**
 * @brief Appearance Category enumeration.
*******************************************************************************/
typedef enum {
    BLE_GAPC_APPEARANCE_CATEGORY_UNKNOWN = 0, /**< Unknown */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_PHONE = 64, /**< Generic Phone */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_COMPUTER = 128, /**< Generic Computer */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_WATCH = 192, /**< Generic Watch */
    BLE_GAPC_APPEARANCE_CATEGORY_WATCH_SPORTS_WATCH = 193, /**< Watch: Sports Watch */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_CLOCK = 256, /**< Generic Clock */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_DISPLAY = 320, /**< Generic Display */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_REMOTE_CONTROL = 384, /**< Generic Remote Control */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_EYE_GLASSES = 448, /**< Generic Eye-glasses */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_TAG = 512, /**< Generic Tag */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_KEYRING = 576, /**< Generic Keyring */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_MEDIA_PLAYER = 640, /**< Generic Media Player */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_BARCODE_SCANNER = 704, /**< Generic Barcode Scanner */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_THERMOMETER = 768, /**< Generic Thermometer */
    BLE_GAPC_APPEARANCE_CATEGORY_THERMOMETER_EAR = 769, /**< Thermometer Ear */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_HEART_RATE_SENSOR = 832, /**< Generic Heart rate Sensor */
    BLE_GAPC_APPEARANCE_CATEGORY_HEART_RATE_SENSOR_HEART_RATE_BELT = 833, /**< Heart Rate Sensor Heart Rate Belt */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_BLOOD_PRESSURE = 896, /**< Generic Blood Pressure */
    BLE_GAPC_APPEARANCE_CATEGORY_BLOOD_PRESSURE_ARM = 897, /**< Blood Pressure: Arm */
    BLE_GAPC_APPEARANCE_CATEGORY_BLOOD_PRESSURE_WRIST = 898, /**< Blood Pressure: Wrist */
    BLE_GAPC_APPEARANCE_CATEGORY_HUMAN_INTERFACE_DEVICE = 960, /**< Human Interface Device (HID) */
    BLE_GAPC_APPEARANCE_CATEGORY_KEYBOARD = 961, /**< Keyboard */
    BLE_GAPC_APPEARANCE_CATEGORY_MOUSE = 962, /**< Mouse */
    BLE_GAPC_APPEARANCE_CATEGORY_JOYSTICK = 963, /**< Joystick */
    BLE_GAPC_APPEARANCE_CATEGORY_GAMEPAD = 964, /**< Gamepad */
    BLE_GAPC_APPEARANCE_CATEGORY_DIGITIZER_TABLET = 965, /**< Digitizer Tablet */
    BLE_GAPC_APPEARANCE_CATEGORY_CARD_READER = 966, /**< Card Reader */
    BLE_GAPC_APPEARANCE_CATEGORY_DIGITAL_PEN = 967, /**< Digital Pen */
    BLE_GAPC_APPEARANCE_CATEGORY_BARCODE_SCANNER = 968, /**< Barcode Scanner */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_GLUCOSE_METER = 1024, /**< Generic Glucose Meter */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_RUNNING_WALKING_SENSOR = 1088, /**< Generic: Running Walking Sensor */
    BLE_GAPC_APPEARANCE_CATEGORY_RUNNING_WALKING_SENSOR_IN_SHOE = 1089, /**< Running Walking Sensor: In-Shoe */
    BLE_GAPC_APPEARANCE_CATEGORY_RUNNING_WALKING_SENSOR_ON_SHOE = 1090, /**< Running Walking Sensor: On-Shoe */
    BLE_GAPC_APPEARANCE_CATEGORY_RUNNING_WALKING_SENSOR_ON_HIP = 1091, /**< Running Walking Sensor: On-Hip */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC__CYCLING = 1152, /**< Generic Cycling */
    BLE_GAPC_APPEARANCE_CATEGORY_CYCLING_CYCLING_COMPUTER = 1153, /**< Cycling Cycling Computer */
    BLE_GAPC_APPEARANCE_CATEGORY_CYCLING_SPEED_SENSOR = 1154, /**< Cycling Speed Sensor */
    BLE_GAPC_APPEARANCE_CATEGORY_CYCLING_CADENCE_SENSOR = 1155, /**< Cycling Cadence Sensor */
    BLE_GAPC_APPEARANCE_CATEGORY_CYCLING_POWER_SENSOR = 1156, /**< Cycling: Power Sensor */
    BLE_GAPC_APPEARANCE_CATEGORY_CYCLING_SPEED_AND_CADENCE_SENSOR = 1157, /**< Cycling Speed and Cadence Sensor */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_PULSE_OXIMETER = 3136, /**< Generic Pulse Oximeter */
    BLE_GAPC_APPEARANCE_CATEGORY_FINGERTIP = 3137, /**< Fingertip */
    BLE_GAPC_APPEARANCE_CATEGORY_WRIST_WORN = 3138, /**< Wrist Worn */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC__WEIGHT_SCALE = 3200, /**< Generic Weight Scale */
    BLE_GAPC_APPEARANCE_CATEGORY_GENERIC_OUTDOOR_SPORTS_ACTIVITY = 5184, /**< Generic Outdoor Sports Activity */
    BLE_GAPC_APPEARANCE_CATEGORY_LOCATION_DISPLAY_DEVICE = 5185, /**< Location Display Device */
    BLE_GAPC_APPEARANCE_CATEGORY_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE = 5186, /**< Location and Navigation Display Device */
    BLE_GAPC_APPEARANCE_CATEGORY_LOCATION_POD = 5187, /**< Location Pod */
    BLE_GAPC_APPEARANCE_CATEGORY_LOCATION_AND_NAVIGATION_POD = 5188, /**< Location and Navigation Pod */
} e_ble_gapc_appearance_category_t;

/***************************************************************************//**
 * @brief Appearance attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
} st_ble_gapc_appearance_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read Appearance characteristic value from the remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_ReadAppearance(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief      Get Appearance attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GAPC_GetAppearanceAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_appearance_attr_hdl_t *p_hdl);

/*----------------------------------------------------------------------------------------------------------------------
    Peripheral Preferred Connection Parameters Characteristic : The Peripheral Preferred Connection Parameters (PPCP) characteristic contains the preferred connection parameters of the Peripheral
----------------------------------------------------------------------------------------------------------------------*/
#define BLE_GAPC_PER_PREF_CONN_PARAM_UUID (0x2A04)
#define BLE_GAPC_PER_PREF_CONN_PARAM_LEN (8)
/***************************************************************************//**
 * @brief Peripheral Preferred Connection Parameters value structure.
*******************************************************************************/
typedef struct {
    uint16_t minimum_connection_interval; /**< Minimum Connection Interval */
    uint16_t maximum_connection_interval; /**< Maximum Connection Interval */
    uint16_t slave_latency; /**< Slave Latency */
    uint16_t connection_supervision_timeout_multiplier; /**< Connection Supervision Timeout Multiplier */
} st_ble_gapc_per_pref_conn_param_t;

/***************************************************************************//**
 * @brief Peripheral Preferred Connection Parameters attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
} st_ble_gapc_per_pref_conn_param_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read Peripheral Preferred Connection Parameters characteristic value from the remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_ReadPerPrefConnParam(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief      Get Peripheral Preferred Connection Parameters attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GAPC_GetPerPrefConnParamAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_per_pref_conn_param_attr_hdl_t *p_hdl);

/*----------------------------------------------------------------------------------------------------------------------
    Central Address Resolution Characteristic : The Peripheral should check if the peer device supports address resolution by reading the Central Address Resolution characteristic.
----------------------------------------------------------------------------------------------------------------------*/
#define BLE_GAPC_CENT_ADDR_RSLV_UUID (0x2AA6)
#define BLE_GAPC_CENT_ADDR_RSLV_LEN (1)
/***************************************************************************//**
 * @brief Central Address Resolution attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
} st_ble_gapc_cent_addr_rslv_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read Central Address Resolution characteristic value from the remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_ReadCentAddrRslv(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief      Get Central Address Resolution attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GAPC_GetCentAddrRslvAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_cent_addr_rslv_attr_hdl_t *p_hdl);

/*----------------------------------------------------------------------------------------------------------------------
    Resolvable Private Address Only Characteristic : The device should check if the peer will only use Resolvable Private Addresses (RPAs) after bonding by reading the Resolvable Private Address Only characteristic.
----------------------------------------------------------------------------------------------------------------------*/
#define BLE_GAPC_RSLV_PRIV_ADDR_ONLY_UUID (0x2AC9)
#define BLE_GAPC_RSLV_PRIV_ADDR_ONLY_LEN (1)
/***************************************************************************//**
 * @brief Resolvable Private Address Only attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
} st_ble_gapc_rslv_priv_addr_only_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read Resolvable Private Address Only characteristic value from the remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_ReadRslvPrivAddrOnly(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief      Get Resolvable Private Address Only attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GAPC_GetRslvPrivAddrOnlyAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_rslv_priv_addr_only_attr_hdl_t *p_hdl);


/*----------------------------------------------------------------------------------------------------------------------
    GAP Service Client
----------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief GAP Service client event data.
*******************************************************************************/
typedef struct {
    uint16_t    conn_hdl;  /**< Connection handle */
    uint16_t    param_len; /**< Event parameter length */
    const void *p_param;   /**< Event parameter */
} st_ble_gapc_evt_data_t;

/***************************************************************************//**
 * @brief GAP Service characteristic ID.
*******************************************************************************/
typedef enum {
    BLE_GAPC_DEV_NAME_IDX,
    BLE_GAPC_APPEARANCE_IDX,
    BLE_GAPC_PER_PREF_CONN_PARAM_IDX,
    BLE_GAPC_CENT_ADDR_RSLV_IDX,
    BLE_GAPC_RSLV_PRIV_ADDR_ONLY_IDX,
} e_ble_gapc_char_idx_t;

/***************************************************************************//**
 * @brief GAP Service client event type.
*******************************************************************************/
typedef enum {
    /* Device Name */
    BLE_GAPC_EVENT_DEV_NAME_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_GAPC_DEV_NAME_IDX, BLE_SERVC_READ_RSP),
    BLE_GAPC_EVENT_DEV_NAME_WRITE_RSP = BLE_SERVC_ATTR_EVENT(BLE_GAPC_DEV_NAME_IDX, BLE_SERVC_WRITE_RSP),
    /* Appearance */
    BLE_GAPC_EVENT_APPEARANCE_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_GAPC_APPEARANCE_IDX, BLE_SERVC_READ_RSP),
    /* Peripheral Preferred Connection Parameters */
    BLE_GAPC_EVENT_PER_PREF_CONN_PARAM_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_GAPC_PER_PREF_CONN_PARAM_IDX, BLE_SERVC_READ_RSP),
    /* Central Address Resolution */
    BLE_GAPC_EVENT_CENT_ADDR_RSLV_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_GAPC_CENT_ADDR_RSLV_IDX, BLE_SERVC_READ_RSP),
    /* Resolvable Private Address Only */
    BLE_GAPC_EVENT_RSLV_PRIV_ADDR_ONLY_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_GAPC_RSLV_PRIV_ADDR_ONLY_IDX, BLE_SERVC_READ_RSP),
} e_ble_gapc_event_t;

/***************************************************************************//**
 * @brief     Initialize GAP Service client.
 * @param[in] cb Client callback.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GAPC_Init(ble_servc_app_cb_t cb);

/***************************************************************************//**
 * @brief     GAP Service client discovery callback.
 * @param[in] conn_hdl Connection handle
 * @param[in] serv_idx Service instance index.
 * @param[in] type     Service discovery event type.
 * @param[in] p_param  Service discovery event parameter.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GAPC_ServDiscCb(uint16_t conn_hdl, uint8_t serv_idx, uint16_t type, void *p_param);

/***************************************************************************//**
 * @brief     Get GAP Service client attribute handle.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
*******************************************************************************/
void R_BLE_GAPC_GetServAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gatt_hdl_range_t *p_hdl);

/* Start user code for function declarations. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

#endif /* R_BLE_GAPC_H */

/** @} */

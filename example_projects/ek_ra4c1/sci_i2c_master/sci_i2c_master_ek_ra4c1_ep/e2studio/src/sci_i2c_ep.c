/***********************************************************************************************************************
 * File Name    : sci_i2c_ep.c
 * Description  : Contains data structures and functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "common_utils.h"
#include "sci_i2c_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup r_sci_i2c_master_ep
 * @{
 **********************************************************************************************************************/

/* Global Variables */
static volatile i2c_master_event_t g_sci_i2c_event = I2C_MASTER_EVENT_ABORTED;
static volatile bool g_sensor_ready_flag = false;

/* Private function declarations */
static fsp_err_t sensor_init(void);
static fsp_err_t sensor_wait_data_ready(void);
static fsp_err_t sensor_read_data(uint8_t * p_data);
static fsp_err_t sensor_convert_to_g_unit(uint8_t * p_data, float * p_x_axis, float * p_y_axis, float * p_z_axis);
static void sensor_deinit(void);
static fsp_err_t sci_i2c_wait_event(const i2c_master_event_t event);
static fsp_err_t sensor_init_hardware(void);
static fsp_err_t sensor_init_software(void);
static fsp_err_t sensor_write_reg(uint8_t addr, uint8_t value);
static fsp_err_t sensor_read_reg(uint8_t addr, uint8_t * p_value);
static fsp_err_t sensor_set_reg(uint8_t addr, uint8_t value);
static void handle_error (fsp_err_t err,  const char * err_str);

/* Callback function implementations */

/*******************************************************************************************************************//**
 *  @brief      SCI I2C Master callback function.
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void sci_i2c_master_callback(i2c_master_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        g_sci_i2c_event = p_args->event;
    }
}

/*******************************************************************************************************************//**
 *  @brief      External IRQ callback function.
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void external_irq_callback(external_irq_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        if (g_external_irq_cfg.channel == p_args->channel)
        {
            g_sensor_ready_flag = true;
        }
    }
}

/* Private function implementations */

/*******************************************************************************************************************//**
 *  @brief      This function waits for an I2C event from the SCI I2C Master callback.
 *  @param[in]  event             I2C Master event to wait for.
 *  @retval     FSP_SUCCESS       Upon successful operation.
 *  @retval     FSP_ERR_TIMEOUT   Timeout occurred.
 **********************************************************************************************************************/
static fsp_err_t sci_i2c_wait_event(const i2c_master_event_t event)
{
    fsp_err_t   err     = FSP_SUCCESS;
    uint8_t     timeout = UINT8_MAX;

    /* Wait until I2C Master event equals the expected event */
    while (event != g_sci_i2c_event)
    {
        /* Check time elapsed to avoid infinite loop */
        if(RESET_VALUE == timeout)
        {
            APP_ERR_RETURN(FSP_ERR_TIMEOUT, "No SCI I2C callback events received\r\n");
        }

        /* Decrease timeout value, then apply software delay */
        timeout --;
        R_BSP_SoftwareDelay(I2C_TIMEOUT_DIV, I2C_TIMEOUT_UNIT);
    }

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function initializes necessary hardware to communicate with the sensor.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code Initialization failed.
 **********************************************************************************************************************/
static fsp_err_t sensor_init_hardware(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ICU External IRQ */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);
    APP_ERR_RETURN(err, "R_ICU_ExternalIrqOpen API failed\r\n");

    /* Enable ICU External IRQ to detect new data from sensor */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);
    APP_ERR_RETURN(err, "R_ICU_ExternalIrqEnable API failed\r\n");

    /* Open SCI I2C Master module to communicate with sensor */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
    err = R_SCI_B_I2C_Open(&g_sci_i2c_master_ctrl, &g_sci_i2c_master_cfg);
#else
    err = R_SCI_I2C_Open(&g_sci_i2c_master_ctrl, &g_sci_i2c_master_cfg);
#endif
    APP_ERR_RETURN(err, "R_"SCI_TYPE"_I2C_Open API failed\r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function configures sensor registers so that sensor behaves as expected.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code  Indicates failure during sensor initialization.
 **********************************************************************************************************************/
static fsp_err_t sensor_init_software(void)
{
    fsp_err_t   err         = FSP_SUCCESS;
    uint8_t     sensor_id   = RESET_VALUE;

    /* Read sensor ID */
    err =  sensor_read_reg(SENSOR_ID_REG, &sensor_id);
    APP_ERR_RETURN(err, "sensor_read_reg failed\r\n");

    /* Verify sensor ID */
    if(SENSOR_ID_DATA != sensor_id)
    {
        APP_ERR_RETURN(FSP_ERR_INVALID_DATA, "sensor_read_reg failed\r\n");
    }

    /* Setup BW_RATE register to select output data rate of 1.56 Hz */
    err = sensor_set_reg(SENSOR_BW_RATE_REG, SENSOR_BW_RATE_DATA);
    APP_ERR_RETURN(err, "sensor_set_reg failed\r\n");

    /* Setup INT_ENABLE register to enable data ready interrupts */
    err = sensor_set_reg(SENSOR_INT_ENABLE_REG, SENSOR_INT_ENABLE_DATA);
    APP_ERR_RETURN(err, "sensor_set_reg failed\r\n");

    /* Setup POWER_CTL register to select measurement mode */
    err = sensor_set_reg(SENSOR_POWER_CTL_REG, SENSOR_POWER_CTL_DATA);
    APP_ERR_RETURN(err, "sensor_set_reg failed\r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function writes data to a sensor register via I2C interface.
 *  @param[in]   addr           Register address.
 *  @param[in]   value          Value want to write to register.
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t sensor_write_reg(uint8_t addr, uint8_t value)
{
    fsp_err_t   err                     = FSP_SUCCESS;
    uint8_t     i2c_buf[I2C_TWO_BYTE]   = {addr, value};

    /* Reset SCI I2C callback event */
    g_sci_i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Perform SCI I2C write operation */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
    err = R_SCI_B_I2C_Write(&g_sci_i2c_master_ctrl, i2c_buf, I2C_TWO_BYTE, false);
#else
    err = R_SCI_I2C_Write(&g_sci_i2c_master_ctrl, i2c_buf, I2C_TWO_BYTE, false);
#endif
    APP_ERR_RETURN(err, "R_"SCI_TYPE"_I2C_Write API failed\r\n");

    /* Wait I2C event TX COMPLETE */
    err = sci_i2c_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);
    APP_ERR_RETURN(err, "sci_i2c_wait_event failed\r\n");
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function reads data from a sensor register via I2C interface.
 *  @param[in]   addr           Register address.
 *  @param[in]   p_value        Pointer to store register value.
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t sensor_read_reg(uint8_t addr, uint8_t * p_value)
{
    fsp_err_t   err         = FSP_SUCCESS;
    uint8_t     reg_addr    = addr;

    /* Reset SCI I2C callback event */
    g_sci_i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Performs write SCI I2C */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
    err = R_SCI_B_I2C_Write(&g_sci_i2c_master_ctrl, &reg_addr, I2C_ONE_BYTE, true);
#else
    err = R_SCI_I2C_Write(&g_sci_i2c_master_ctrl, &reg_addr, I2C_ONE_BYTE, true);
#endif
    APP_ERR_RETURN(err, "R_"SCI_TYPE"_I2C_Write API failed\r\n");

    /* Wait I2C event TX COMPLETE */
    err = sci_i2c_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);
    APP_ERR_RETURN(err, "sci_i2c_wait_event failed\r\n");

    /* Reset SCI I2C callback event */
    g_sci_i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Performs read operation */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
    err = R_SCI_B_I2C_Read(&g_sci_i2c_master_ctrl, p_value, I2C_ONE_BYTE, false);
#else
    err = R_SCI_I2C_Read(&g_sci_i2c_master_ctrl, p_value, I2C_ONE_BYTE, false);
#endif
    APP_ERR_RETURN(err, "R_"SCI_TYPE"_I2C_Read API failed\r\n");

    /* Wait I2C event RX COMPLETE */
    err = sci_i2c_wait_event(I2C_MASTER_EVENT_RX_COMPLETE);
    APP_ERR_RETURN(err, "sci_i2c_wait_event failed\r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function set data to a sensor register via I2C interface.
 *  @param[in]   addr           Register address.
 *  @param[in]   value          Value want to set register.
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t sensor_set_reg(uint8_t addr, uint8_t value)
{
    fsp_err_t   err         = FSP_SUCCESS;
    uint8_t     read_value  = RESET_VALUE;

    /* Write data to register */
    err = sensor_write_reg(addr, value);
    APP_ERR_RETURN(err, "sensor_write_reg failed\r\n");

    /* Read back data from register */
    err = sensor_read_reg(addr, &read_value);
    APP_ERR_RETURN(err, "sensor_read_reg failed\r\n");

    /* Verify data */
    if(value != read_value)
    {
        APP_ERR_RETURN(FSP_ERR_WRITE_FAILED, "Data read back mismatch data written\r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function initializes necessary hardware and configures sensor register.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t sensor_init(void)
{
    fsp_err_t   err                         = FSP_SUCCESS;
    uint8_t     value                       = RESET_VALUE;

    /* Initialize the necessary hardware to control the sensor */
    err = sensor_init_hardware();
    APP_ERR_RETURN(err, "sensor_init_hardware failed\r\n");

    /* Configure sensor registers */
    err = sensor_init_software();
    APP_ERR_RETURN(err, "sensor_init_software failed\r\n");

    /* Read INT_SOURCE REG to detect data ready */
    err = sensor_read_reg(SENSOR_INT_SOURCE_REG, &value);
    APP_ERR_RETURN(err, "sensor_read_reg failed\r\n");

    /* If data is ready, trigger data ready flag by software */
    if(SENSOR_INT_SOURCE_DATA == (value & SENSOR_INT_SOURCE_DATA))
    {
        g_sensor_ready_flag = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function waits until sensor data is ready.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t sensor_wait_data_ready(void)
{
    fsp_err_t   err     = FSP_SUCCESS;
    uint8_t     timeout = UINT8_MAX;

    /* Wait until new data is ready
     * The timeout value depends on the output data rate configured by the BW_RATE register */
    while (true != g_sensor_ready_flag)
    {
        /* Check time elapsed to avoid infinite loop */
        if(RESET_VALUE == timeout)
        {
            APP_ERR_RETURN(FSP_ERR_TIMEOUT, "No data received\r\n");
        }

        /* Decrease timeout value then software delay */
        timeout --;
        R_BSP_SoftwareDelay(DATA_TIMEOUT_DIV, DATA_TIMEOUT_UNIT);
    }

    /* Clear sensor data ready flag */
    g_sensor_ready_flag = false;

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function reads 3-axis raw data from a sensor via I2C interface.
 *  @param[in]   p_data         Pointer to store 3-axis raw data.
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t sensor_read_data(uint8_t * p_data)
{
    fsp_err_t   err         = FSP_SUCCESS;
    uint8_t     reg_addr    = SENSOR_AXIS_DATA_REG;

    /* Reset SCI I2C callback event */
    g_sci_i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Performs write SCI I2C */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
    err = R_SCI_B_I2C_Write(&g_sci_i2c_master_ctrl, &reg_addr, I2C_ONE_BYTE, true);
#else
    err = R_SCI_I2C_Write(&g_sci_i2c_master_ctrl, &reg_addr, I2C_ONE_BYTE, true);
#endif
    APP_ERR_RETURN(err, "R_"SCI_TYPE"_I2C_Write API failed\r\n");

    /* Wait SCI I2C event TX COMPLETE */
    err = sci_i2c_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);
    APP_ERR_RETURN(err, "sci_i2c_wait_event failed\r\n");

    /* Reset SCI I2C callback event */
    g_sci_i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Performs read operation */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
    err = R_SCI_B_I2C_Read(&g_sci_i2c_master_ctrl, p_data, I2C_DATA_BYTE, false);
#else
    err = R_SCI_I2C_Read(&g_sci_i2c_master_ctrl, p_data, I2C_DATA_BYTE, false);
#endif
    APP_ERR_RETURN(err, "R_"SCI_TYPE"_Read API failed\r\n");

    /* Wait I2C event RX COMPLETE */
    err = sci_i2c_wait_event(I2C_MASTER_EVENT_RX_COMPLETE);
    APP_ERR_RETURN(err, "sci_i2c_wait_event failed\r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function converts raw sensor data to gravitational units (g units).
 *  @param[in]   p_data         Pointer to 3-axis raw data.
 *  @param[out]  p_x_axis       Pointer to store x-axis value at g unit.
 *  @param[out]  p_y_axis       Pointer to store y-axis value at g unit.
 *  @param[out]  p_z_axis       Pointer to store z-axis value at g unit.
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t sensor_convert_to_g_unit(uint8_t * p_data, float * p_x_axis, float * p_y_axis, float * p_z_axis)
{
    fsp_err_t err = FSP_SUCCESS;
    int16_t * p_axis_data = (int16_t *)p_data;

    /* Check NULL pointer */
    if(NULL == p_data || NULL == p_x_axis || NULL == p_y_axis || NULL == p_z_axis)
    {
        APP_ERR_RETURN(FSP_ERR_INVALID_POINTER, "Null pointer detected\r\n");
    }

    /* Convert to g unit */
    *p_x_axis = (float)(*(p_axis_data + X_AXIS_ADDR_OFFSET)) * SENSOR_SENSITIVITY;
    *p_y_axis = (float)(*(p_axis_data + Y_AXIS_ADDR_OFFSET)) * SENSOR_SENSITIVITY;
    *p_z_axis = (float)(*(p_axis_data + Z_AXIS_ADDR_OFFSET)) * SENSOR_SENSITIVITY;

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function de-initializes hardware used by sensor.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
static void sensor_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close the ICU module if already open */
    if(g_external_irq_ctrl.open)
    {
        err = R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("R_ICU_ExternalIrqClose API failed\r\n");
        }
    }

    /* Close the SCI I2C module if already open */
    if(g_sci_i2c_master_ctrl.open)
    {
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
        err = R_SCI_B_I2C_Close(&g_sci_i2c_master_ctrl);
#else
        err = R_SCI_I2C_Close(&g_sci_i2c_master_ctrl);
#endif
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("R_"SCI_TYPE"_I2C_Close API failed\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to initialize the related modules and start the SCI I2C example operation.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void sci_i2c_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };
    float x_axis = SENSOR_RESET_VALUE;
    float y_axis = SENSOR_RESET_VALUE;
    float z_axis = SENSOR_RESET_VALUE;
    char float_str[SENSOR_STR_SIZE] = { RESET_VALUE };
    uint8_t xyz_axis[SENSOR_DATA_SIZE] = { RESET_VALUE };

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    /* Example Project information printed on the RTT Viewer */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3, EP_VERSION);
    APP_PRINT(BANNER_4, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);
    APP_PRINT(EP_INFO);
    APP_PRINT_WAIT;

    /* Initializes Pmod ACL sensor */
    err = sensor_init ();
    handle_error (err, "sensor_init failed\r\n");

    /* Main loop */
    while (true)
    {
        /* Wait until sensor have new data */
        err = sensor_wait_data_ready ();
        handle_error (err, "sensor_wait_data_ready failed\r\n");

        /* Read raw data from sensor */
        err = sensor_read_data (xyz_axis);
        handle_error (err, "sensor_read_data failed\r\n");

        /* Convert raw data to g unit */
        err = sensor_convert_to_g_unit (xyz_axis, &x_axis, &y_axis, &z_axis);
        handle_error (err, "sensor_convert_to_g_unit failed\r\n");

        /* Convert float value to string */
        snprintf (float_str, SENSOR_STR_SIZE, "X-axis = %5.2f g, Y-axis = %5.2f g, Z-axis = %5.2f g",\
                  x_axis, y_axis,z_axis);

        /* Display value on RTT Viewer */
        APP_PRINT("%s \r\n", float_str);
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function handles error if error occurred, closes all opened modules, prints and traps error.
 *  @param[in]   err        Error code value.
 *  @param[in]   err_str    Error message string.
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error (fsp_err_t err,  const char * err_str)
{
    if(FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* De-initializes hardware used by sensor */
        sensor_deinit();

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_sci_i2c_master_ep)
 **********************************************************************************************************************/

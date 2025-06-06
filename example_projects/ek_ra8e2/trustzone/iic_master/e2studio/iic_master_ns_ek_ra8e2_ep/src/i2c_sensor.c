/***********************************************************************************************************************
 * File Name    : i2c_sensor.c
 * Description  : Contains data structures and functions used in i2c_sensor.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "i2c_sensor.h"

/*******************************************************************************************************************//**
 * @addtogroup r_iic_master_ep
 * @{
 **********************************************************************************************************************/

/* Global Variables */
i2c_master_callback_args_t g_i2c_master_callback_args;
/* Reading I2C call back event through i2c_master callback */
static volatile i2c_master_event_t i2c_event = I2C_MASTER_EVENT_ABORTED;

/* Private function declarations */
static fsp_err_t get_device_id(uint8_t *dev_id);
static fsp_err_t validate_i2c_event(void);

/*******************************************************************************************************************//**
 *  @brief       initialize IIC master module and set up PMOD ACL sensor
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS                  Upon successful open and start of timer
 *  @retval      Any Other Error code apart from FSP_SUCCESS is  Unsuccessful open or start
 **********************************************************************************************************************/
fsp_err_t init_sensor(void)
{
    uint8_t device_id = RESET_VALUE;
    fsp_err_t err     = FSP_SUCCESS;
    uint8_t measure_enable_payload[MEASURE_PAYLOAD_SIZE] = { POWER_CTL_REG, ENABLE_BIT };

    /* Opening IIC master module */
    err = g_i2c_master_open_guard(NULL,NULL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_Open API failed ** \r\n");
        return err;
    }

    err = g_i2c_master_callback_set_guard(NULL, i2c_master_callback, NULL, &g_i2c_master_callback_args);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_CallbackSet API failed ** \r\n");
        return err;
    }

    /* Get device ID as a start of communication */
    err =  get_device_id(&device_id);

    /* Verify I2C transaction is successful with PMOD ACL and correct device ID is received */
    if ( (FSP_SUCCESS == err) && (DEVICE_SIGNATURE == device_id) )
    {
        /* Access to POWER_CTL Register (0x2D) and enable measurement */
        err = g_i2c_master_write_guard(NULL,measure_enable_payload,TWO_BYTE,false);
        if (FSP_SUCCESS == err)
        {
            err = validate_i2c_event();
            /* handle error */
            if(FSP_ERR_TRANSFER_ABORTED == err)
            {
                APP_ERR_PRINT("** POWER_CTL reg I2C write failed!!! ** \r\n");
            }
        }
        /* Handle error */
        else
        {
            /* Write API returns itself is not successful */
            APP_ERR_PRINT("** R_IIC_MASTER_Write API failed ** \r\n");
        }
    }
    /* Handle error */
    else
    {
        /* Failed to get Device ID */
        APP_ERR_PRINT("Error in reading Device ID ** \r\n");
    }
    return err;
}


/*******************************************************************************************************************//**
 *  @brief       DeInitialize IIC master module
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void deinit_sensor(void)
{
    fsp_err_t err     = FSP_SUCCESS;

    /* Close open modules */
    err =  g_i2c_master_close_guard (NULL);

    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_Close API failed ** \r\n");
    }
}
/*******************************************************************************************************************//**
 *  @brief       Read The ACL Sensor data for X,Y and Z axis. And prints the raw data on the RTT console
 *  @param[IN]   Array pointer to get the sensor data
 *  @retval      FSP_SUCCESS                  Upon successful open and start of timer
 *  @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful open or start
 **********************************************************************************************************************/
fsp_err_t read_sensor_data(uint8_t *xyz_data)
{
    fsp_err_t err           = FSP_SUCCESS;
    uint8_t adxl_data[7]    = { RESET_VALUE };
    uint8_t internal_reg    = AXIS_DATA;


    if(NULL == xyz_data)
    {
        err = FSP_ERR_INVALID_POINTER;
        APP_ERR_PRINT("** NULL Pointer check fail ** \r\n");
        return err;
    }
    /* Write Accelerometer Data register */
    err = g_i2c_master_write_guard(NULL, &internal_reg, ONE_BYTE, true);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_Write API failed ** \r\n");
        return err;
    }
    else
    {
        err = validate_i2c_event();

        /* Performs read operation only when write event is successful, failure part is handled at next stage */
        if (FSP_SUCCESS == err)
        {
            err = g_i2c_master_read_guard(NULL, &adxl_data[0], DATA_REGISTERS, false);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("** R_IIC_MASTER_Read API failed ** \r\n");
                return err;
            }
            else
            {
                err = validate_i2c_event();
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("** Data register, I2C read failed ** \r\n");
                    return err;
                }
            }
        }
        else
        {
            APP_ERR_PRINT("** Accel Data register, I2C write failed ** \r\n");
            return err;
        }

        /* Read is successful update the x,y,z values and return success */
        for (uint8_t itr = RESET_VALUE; itr < SENSOR_DATA_SIZE; itr++)
        {
            *(xyz_data+itr) = adxl_data[itr];
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief     Read Device signature/ID from sensor
 *
 *  @param[in] dev_id                       retrieve PMOD ACL device ID
 *
 *  @retval    FSP_SUCCESS                  on successful I2C transaction
 *             FSP_ERR_INVALID_POINTER      if invalid parameters passed
 *             err                          whichever occurs on either API or on I2C event failure
 **********************************************************************************************************************/
static fsp_err_t get_device_id(uint8_t *dev_id)
{
    fsp_err_t err         = FSP_SUCCESS;
    uint8_t reg_address   = DEVICE_ID_REG;

    /* Parameter checking */
    if (NULL == dev_id)
    {
        err = FSP_ERR_INVALID_POINTER;
        APP_ERR_PRINT("** NULL Pointer check fail ** \r\n");
        return err;
    }

    /* Send register address to sensor */
    err = g_i2c_master_write_guard(NULL, &reg_address, ONE_BYTE, true);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_Write API failed ** \r\n");
    }
    else
    {
        err = validate_i2c_event();
        /* Handle error */
        if(FSP_ERR_TRANSFER_ABORTED == err)
        {
            APP_ERR_PRINT("** Device ID reg, I2C write failed ** \r\n");
        }
        else
        {
            /* Read only when I2C write and its callback event is successful */
            err  = g_i2c_master_read_guard(NULL, dev_id, ONE_BYTE, false);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("** R_IIC_MASTER_Read API failed ** \r\n");
                /* Do nothing, the error is returned in the end */
            }
            else
            {
                err = validate_i2c_event();
                /* Handle error */
                if(FSP_ERR_TRANSFER_ABORTED == err)
                {
                    APP_ERR_PRINT("** Device ID read,  I2C read failed ** \r\n");
                }
            }
        }
    }
    /* On successful I2C transaction with PMOD-ACL return from here */
    return err;
}

/*******************************************************************************************************************//**
 *  @brief      User callback function
 *  @param[in]  p_args
 *  @retval None
 **********************************************************************************************************************/
void i2c_master_callback(i2c_master_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        /* Capture callback event for validating the I2C transfer event */
        i2c_event = p_args->event;
    }
}

/*******************************************************************************************************************//**
 *  @brief       Validate i2c receive/transmit  based on required write read operation
 *
 *  @param[in]   None
 *
 *  @retval      FSP_SUCCESS                       successful event receiving returns FSP_SUCCESS
 *               FSP_ERR_TRANSFER_ABORTED          Either on timeout elapsed or received callback event is
 *                                                 I2C_MASTER_EVENT_ABORTED
 **********************************************************************************************************************/
static fsp_err_t validate_i2c_event(void)
{
    uint16_t local_time_out = UINT16_MAX;

    /* Resetting call back event capture variable */
    i2c_event = (i2c_master_event_t)RESET_VALUE;

    do
    {
        /* This is to avoid infinite loop */
        --local_time_out;

        if(RESET_VALUE == local_time_out)
        {
            return FSP_ERR_TRANSFER_ABORTED;
        }

    }while(i2c_event == RESET_VALUE);

    if(i2c_event != I2C_MASTER_EVENT_ABORTED)
    {
        i2c_event = (i2c_master_event_t)RESET_VALUE;  /* Make sure this is always Reset before return */
        return FSP_SUCCESS;
    }

    i2c_event = (i2c_master_event_t)RESET_VALUE; /* Make sure this is always Reset before return */
    return FSP_ERR_TRANSFER_ABORTED;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_iic_master_ep)
 **********************************************************************************************************************/

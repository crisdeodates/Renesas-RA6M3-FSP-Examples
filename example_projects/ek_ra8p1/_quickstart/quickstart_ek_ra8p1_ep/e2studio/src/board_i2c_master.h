/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : board_i2c_master.h
 * Version      : 1.0
 * Description  : I2C Bus master driver, I/F .h
  *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
 /**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#ifndef BOARD_I2C_MASTER_CFG_H_
#define BOARD_I2C_MASTER_CFG_H_
/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern fsp_err_t board_i2c_master_wait_complete(void);

extern fsp_err_t wr_sensor_reg8_8(int regID, int regDat);
extern fsp_err_t rd_sensor_reg8_8(uint16_t regID, uint8_t* regDat);

extern fsp_err_t wr_sensor_reg16_8(i2c_master_ctrl_t * p_api_ctrl, uint16_t regID, uint8_t regDat);
extern fsp_err_t rd_sensor_reg16_8(i2c_master_ctrl_t * p_api_ctrl, uint16_t regID, uint8_t* regDat);

#endif /* BOARD_I2C_MASTER_CFG_H_ */

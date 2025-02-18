/***********************************************************************************************************************
 * File Name    : flash.c
 * Description  : flash handling subroutines.
 **********************************************************************************************************************/
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "flash.h"
#include "common_util.h"
#include "framedProtocolTarget.h"

/*****************************************************************************************************************
 *  @brief      roundup: round up data
 *  @param[in]  numToRound: the number to be rounded
 *  @param[in]  multiple: round up to multiples of this integer
 *  @retval     the number after rounded up
 *  ****************************************************************************************************************/
static int roundup(int numToRound, int multiple)
{
    if (0 == multiple)
        return numToRound;

    int remainder = numToRound % multiple;
    if (0 == remainder )
        return numToRound;

    return (numToRound + multiple - remainder);
}

/*****************************************************************************************************************
 *  @brief      flash_init: flash initialization
 *  @param[in]  none
 *  @retval     flash open API return status
 *  ****************************************************************************************************************/
fsp_err_t flash_init (void)
{
    fsp_err_t status = FSP_SUCCESS;

    status = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);

    if (FSP_SUCCESS != status )
    {

        APP_ERR_TRAP(status);
    }

    return status;
}

/*****************************************************************************************************************
 *  @brief      flash_read: read and return the flash data. This function is not used in this application.
 *  @param[in]  data_buffer: stores the flash data read
 *  @param[in]  data_size: size of the flash data to read
 *  @param[in]  info_type: type of the flash data
 *  @retval     status of the data copy to the destination buffer
 *  ****************************************************************************************************************/
fsp_err_t flash_read(uint8_t *data_buffer, unsigned int data_size, unsigned int info_type)
{
    unsigned int _dataflash_addr = 0;

    switch(info_type)
    {
        case DEV_CERT_CFG:
            _dataflash_addr = DEVICE_CERT_BLOCK_ADDR;
            break;

        default:
            return FSP_ERR_INVALID_ARGUMENT;
    }

    /*Read code flash data */
    memcpy(data_buffer, (uint8_t *) _dataflash_addr, data_size);

    return ((fsp_err_t)(memcpy(data_buffer, (uint8_t *) _dataflash_addr, data_size)));

}

/*****************************************************************************************************************
 *  @brief      flash_write: write the data_buffer of size data_size
 *  @param[in]  data_buffer: data to be written to flash
 *  @param[in]  data_size: size of the data to write
 *  @param[in]  info_type: type of the flash data
 *  @retval     flash write result
 *  ****************************************************************************************************************/

fsp_err_t flash_write (uint8_t *data_buffer , unsigned int data_size, unsigned int info_type)
{
    fsp_err_t status = FSP_SUCCESS;
    unsigned int flash_addr = 0;
    unsigned int _num_of_blocks = 1;
    uint32_t copy_size = 0;
    int tx_size = 0;
    flash_result_t blank_check_result = FLASH_RESULT_NOT_BLANK;

    switch(info_type)
    {
        case DEV_CERT_CFG:
            flash_addr = DEVICE_CERT_BLOCK_ADDR;
            _num_of_blocks = (unsigned int)NUM_OF_BLOCKS(DEVICE_CERT_BLOCK_LEN);
            break;

        default:
            return FSP_ERR_INVALID_ARGUMENT;
    }

    if( data_size > (_num_of_blocks * FLASH_BLOCK_SIZE))
    {

        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Erase flash memory location first before write */
    status = R_FLASH_HP_Erase(&g_flash0_ctrl, flash_addr, _num_of_blocks);
    if(FSP_SUCCESS == status)
    {
        status = R_FLASH_HP_BlankCheck(&g_flash0_ctrl, flash_addr, FLASH_HP_CF_BLOCK_SIZE_32KB, &blank_check_result);
        /* Error Handle */
        if (FSP_SUCCESS != status)
        {

            return status;
        }
        while (data_size > 0)
        {
            if (data_size > FLASH_BLOCK_SIZE)
            {
                copy_size = FLASH_BLOCK_SIZE;
            }
            else
            {
                copy_size = data_size;
            }

            /* Check if the size is multiple of 128 bytes in case of internal code flash.*/
            tx_size = roundup((int)copy_size, CODE_FLASH_WRITE_SIZE);

            /* Write code flash data*/
            status = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t)data_buffer, flash_addr, (uint32_t)tx_size);

            if ( FSP_SUCCESS == status )
            {

                data_size -= (copy_size);
                data_buffer += (copy_size);
                flash_addr += FLASH_BLOCK_SIZE;
            }
            else
            {
                status = FSP_ERR_WRITE_FAILED;
                break;
            }
        }
    }
    else
    {

        status = FSP_ERR_ERASE_FAILED;
    }

    return status;
}


/*****************************************************************************************************************
 *  @brief      flash_deinit: close the flash driver
 *  @param[in]  none
 *  @retval     close the flash driver
 *  ****************************************************************************************************************/

fsp_err_t flash_deinit(void)
{
    fsp_err_t status = FSP_SUCCESS;
    status = R_FLASH_HP_Close(&g_flash0_ctrl);
    if( FSP_SUCCESS != status )
    {

    }

    return status;
}

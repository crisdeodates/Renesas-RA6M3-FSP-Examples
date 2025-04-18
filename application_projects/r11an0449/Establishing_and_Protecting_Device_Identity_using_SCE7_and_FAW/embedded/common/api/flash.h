/***********************************************************************************************************************
 * File Name    : flash.h
 * Description  : common flash
 * flash routine related definitions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#ifndef FLASH_H_
#define FLASH_H_

#include "bsp_api.h"
#include "hal_data.h"
/* Provision info storage address ( internal flash ) */
#define CODE_FLASH_WRITE_SIZE           (128)
#define FLASH_BLOCK_SIZE                (8 * 1024)
#define FLASH_HP_CF_BLOCK_SIZE_32KB       (32*1024)    /* Block Size 32 KB */
#define DEVIC_CERT_OFFSET               (0x00106000)

#define DEVICE_CERT_BLOCK_ADDR                      (DEVIC_CERT_OFFSET)
#define DEVICE_CERT_BLOCK_LEN                       (FLASH_BLOCK_SIZE * 1)

#define NUM_OF_BLOCKS(x)                ((x) / FLASH_BLOCK_SIZE)

/* Info type IDs */
typedef enum e_int_storage_type {
    DEV_CERT_CFG,           /*  Device Certificate */
}int_storage_type_t;

fsp_err_t flash_init(void);
fsp_err_t flash_deinit(void);
fsp_err_t flash_read(uint8_t *data_buffer, unsigned int data_size,unsigned int info_type);
fsp_err_t flash_write(uint8_t *data_buffer , unsigned int data_size, unsigned int info_type);
fsp_err_t flash_FAW_Set(uint32_t start_addr, uint32_t end_addr);
fsp_err_t flash_FAW_Clear(void);
#endif /* FLASH_H_ */

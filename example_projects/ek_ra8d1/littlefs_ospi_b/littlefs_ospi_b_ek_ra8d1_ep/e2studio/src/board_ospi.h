/***********************************************************************************************************************
 * File Name    : board_ospi.h
 * Description  : Contains macros data structures and functions used in board_ospi.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef BOARD_OSPI_H_
#define BOARD_OSPI_H_

#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macro for OSPI reset pin */
#define OM_RESET                                    (BSP_IO_PORT_01_PIN_06)

/* Macro for OSPI command code */
#define OSPI_B_COMMAND_ENTER_4_ADDR_SPI             (0xB7)
#define OSPI_B_COMMAND_EXIT_4_ADDR_SPI              (0xB8)
#define OSPI_B_COMMAND_ERASE_CHIP_SPI               (0x60)

#define OSPI_B_COMMAND_WRITE_ENABLE_SPI             (0x06)
#define OSPI_B_COMMAND_WRITE_ENABLE_OPI             (0x0606)

#define OSPI_B_COMMAND_WRITE_REGISTER_SPI           (0x71)
#define OSPI_B_COMMAND_WRITE_REGISTER_OPI           (0x7171)

#define OSPI_B_COMMAND_READ_STATUS_SPI              (0x05)
#define OSPI_B_COMMAND_READ_STATUS_OPI              (0x0505)

#define OSPI_B_COMMAND_READ_REGISTER_SPI            (0x65)
#define OSPI_B_COMMAND_READ_REGISTER_OPI            (0x6565)

#define OSPI_B_COMMAND_READ_DEVICE_ID_SPI           (0x9F)
#define OSPI_B_COMMAND_READ_DEVICE_ID_OPI           (0x9F9F)

/* Macro for OSPI command length */
#define OSPI_B_COMMAND_LENGTH_SPI                   (1U)
#define OSPI_B_COMMAND_LENGTH_OPI                   (2U)

/* Macro for OSPI transfer address */
#define OSPI_B_ADDRESS_DUMMY                        (0U)
#define OSPI_B_ADDRESS_LENGTH_ZERO                  (0U)
#define OSPI_B_ADDRESS_LENGTH_THREE                 (3U)
#define OSPI_B_ADDRESS_LENGTH_FOUR                  (4U)

/* Macro for OSPI transfer data */
#define OSPI_B_DATA_DUMMY                           (0U)
#define OSPI_B_DATA_LENGTH_ZERO                     (0U)
#define OSPI_B_DATA_LENGTH_ONE                      (1U)
#define OSPI_B_DATA_LENGTH_TWO                      (2U)
#define OSPI_B_DATA_LENGTH_FOUR                     (4U)

/* Macro for OSPI transfer dummy cycles */
#define OSPI_B_DUMMY_CYCLE_WRITE_SPI                (0U)
#define OSPI_B_DUMMY_CYCLE_WRITE_OPI                (0U)
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI          (0U)
#define OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI          (3U)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI        (0U)
#define OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI        (3U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI          (8U)
#define OSPI_B_DUMMY_CYCLE_READ_MEMORY_OPI          (20U)

/* Macro for flash device register address */
#define OSPI_B_ADDRESS_STR1V_REGISTER               (0x00800000)
#define OSPI_B_ADDRESS_STR2V_REGISTER               (0x00800001)
#define OSPI_B_ADDRESS_CFR1V_REGISTER               (0x00800002)
#define OSPI_B_ADDRESS_CFR2V_REGISTER               (0x00800003)
#define OSPI_B_ADDRESS_CFR3V_REGISTER               (0x00800004)
#define OSPI_B_ADDRESS_CFR4V_REGISTER               (0x00800005)
#define OSPI_B_ADDRESS_CFR5V_REGISTER               (0x00800006)

#define OSPI_B_ADDRESS_CFR1N_REGISTER               (0x00000002)
#define OSPI_B_ADDRESS_CFR2N_REGISTER               (0x00000003)
#define OSPI_B_ADDRESS_CFR3N_REGISTER               (0x00000004)
#define OSPI_B_ADDRESS_CFR4N_REGISTER               (0x00000005)
#define OSPI_B_ADDRESS_CFR5N_REGISTER               (0x00000006)

/* Macros for configure flash device */
#define OSPI_B_DATA_CFR2V_REGISTER                  (0x88)
#define OSPI_B_DATA_DEFAULT_CFR3V_REGISTER          (0x00)
#define OSPI_B_DATA_CFR4V_REGISTER                  (0xA0)
#define OSPI_B_DATA_DEFAULT_CFR4V_REGISTER          (0xA8)
#define OSPI_B_DATA_SET_SPI_CFR5V_REGISTER          (0x40)
#define OSPI_B_DATA_SET_OPI_CFR5V_REGISTER          (0x43)

/* Flash device status bit */
#define OSPI_B_WEN_BIT_MASK                         (0x00000002)
#define OSPI_B_BUSY_BIT_MASK                        (0x00000001)

/* Flash device sector size */
#define OSPI_B_SECTOR_4K_SIZE                       (0x1000)
#define OSPI_B_SECTOR_4K_NUM                        (32U)
#define OSPI_B_SECTOR_4K_END_ADDRESS                (0x9001FFFF)

#define OSPI_B_32K_SIZE                             (0x8000)

/* Macros for periodic delay and periodic line feed */
#define PERIODIC_PRINT_OUT                          (0x10000U)
#define PERIODIC_LINE_FEED                          (0x400000U)

/* Flash device address space mapping */
#define OSPI_B_CS0_START_ADDRESS                    (0x80000000)
#define OSPI_B_CS1_START_ADDRESS                    (0x90000000)
#define OSPI_B_AUTO_CALIB_ADDRESS                   (0x9001F000)

/* Flash device timing */
#define OSPI_B_TIME_UNIT                            (BSP_DELAY_UNITS_MICROSECONDS)
#define OSPI_B_TIME_RESET_SETUP                     (2U)        //  Type 50ns
#define OSPI_B_TIME_RESET_PULSE                     (1000U)     //  Type 500us
#define OSPI_B_TIME_ERASE_4K                        (50000U)    //  Type 4KB sector is 95 KBps
#define OSPI_B_TIME_WRITE                           (10000U)    //  Type 256B page (4KB/256KB sector) is 595/533 KBps

typedef enum e_ospi_b_transfer
{
    OSPI_B_TRANSFER_ENTER_4_ADDR_SPI = 0,
    OSPI_B_TRANSFER_EXIT_4_ADDR_SPI,
    OSPI_B_TRANSFER_WRITE_ENABLE_SPI,
    OSPI_B_TRANSFER_WRITE_REG_SPI,
    OSPI_B_TRANSFER_WRITE_VOL_REG_SPI,
    OSPI_B_TRANSFER_WRITE_CFR5V_SPI,
    OSPI_B_TRANSFER_READ_STATUS_SPI,
    OSPI_B_TRANSFER_READ_VOL_REG_SPI,
    OSPI_B_TRANSFER_READ_NON_VOL_REG_SPI,
    OSPI_B_TRANSFER_READ_DEVICE_ID_SPI,
    OSPI_B_TRANSFER_WRITE_ENABLE_OPI,
    OSPI_B_TRANSFER_WRITE_REG_OPI,
    OSPI_B_TRANSFER_WRITE_VOL_REG_OPI,
    OSPI_B_TRANSFER_READ_STATUS_OPI,
    OSPI_B_TRANSFER_READ_VOL_REG_OPI,
    OSPI_B_TRANSFER_READ_CFR5V_OPI,
    OSPI_B_TRANSFER_READ_DEVICE_ID_OPI,
    OSPI_B_TRANSFER_MAX
} ospi_b_transfer_t;

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern spi_flash_direct_transfer_t g_ospi_b_direct_transfer [OSPI_B_TRANSFER_MAX];

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  Function Name: ospi_b_init
 *  Description  : This function initializes OSPI module and Flash device.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful initialization of OSPI module and Flash device
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t ospi_b_init (void);

/***********************************************************************************************************************
 *  Function Name: erase_ospi_flash
 *  Description  : This function erases the flash memory using lower level flash APIs.
 *  Arguments    : erase_size      The size of flash to be erased
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t erase_ospi_flash (uint32_t erase_size);

#endif /* BOARD_OSPI_H_ */

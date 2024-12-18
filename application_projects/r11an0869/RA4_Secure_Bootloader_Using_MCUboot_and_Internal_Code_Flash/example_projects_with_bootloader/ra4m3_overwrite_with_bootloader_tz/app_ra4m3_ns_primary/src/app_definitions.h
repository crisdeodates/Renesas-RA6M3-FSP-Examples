/***********************************************************************************************************************
 * File Name    : app_definitions.h
 * Description  : Contains macros specific to this application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020-2021] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/
#ifndef APP_DEFINITIONS_H_
#define APP_DEFINITIONS_H_

#include <stdint.h>
/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"
#include "common_utils.h"

#define _unused(x) ((void)(x))

#define NUM_OF_FLASH_SECTOR             (1)
#define FLASH_SECTOR_SIZE_8KB           (8*1024)    // 8KB sector
#define FLASH_SECTOR_SIZE_256_BYTES     (256)
#define FLASH_SECTOR_SIZE_32KB          (32*1024)   // 32kB sector

/*
 * non-secure flash test block
 */

#define FLASH_WRITE_TEST_BLOCK          (0xF0000)       // test block located in non-secure flash not locked


#define FLASH_WRITE_LENGTH              (128)
#define FLASH_WRITE_TEST_DATA           (0x44)


#define SYSTEM_CAME_OUT_OF_RESET                "\r\n system came out of reset, up running"
#define ACCESS_SUCCESSFUL                       "\r\n flash write successful!\r\n"
#define ACCESS_UN_SUCCESSFUL                    "\r\n flash write not successful!\r\n"
#define FLASH_ERASE_FAILED                      "\r\n flash erase failed! \r\n"
#define FLASH_WRITE_SOURCE_ADDR_CHECK_FAILED    "\r\n invalid secure flash region access! \r\n"

/* Macros for menu options to be displayed */


#define MENUSTATUS1       " Running the Primary non-secure application with overwrite update mode. \r\n"
#define MENUSTATUS2       " Flash Operation is successful. The Red, Blue and Green LEDs should be blinking.\r\n\r\n"


#endif /* APP_DEFINITIONS_H_ */

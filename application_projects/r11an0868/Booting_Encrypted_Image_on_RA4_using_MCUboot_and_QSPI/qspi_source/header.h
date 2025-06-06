/***********************************************************************************************************************
 * File Name    : header.h
 * Description  : Contains application image header information and related function prototypes
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Copyright [2020-2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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

#ifndef HEADER_H_
#define HEADER_H_

#include "downloader_thread.h"

#if 0
 #define PRIMARY_IMAGE_START_ADDRESS      0x00010000
 #define PRIMARY_IMAGE_END_ADDRESS        0x0007FFFF
 #define SECONDARY_IMAGE_START_ADDRESS    0x00080000
 #define SECONDARY_IMAGE_END_ADDRESS      0x000EFFFF
#else
 #define PRIMARY_IMAGE_START_ADDRESS      0x00018000
 #define PRIMARY_IMAGE_END_ADDRESS        0x000F7FFF
 #define SECONDARY_IMAGE_START_ADDRESS    0x60000000
 #define SECONDARY_IMAGE_END_ADDRESS      0x600DFFFF

#endif
#define FLASH_BLOCK_SIZE                  (32 * 1024)
#define SECONDARY_IMAGE_NUM_BLOCKS        ((SECONDARY_IMAGE_END_ADDRESS - SECONDARY_IMAGE_START_ADDRESS + 1) / \
                                           FLASH_BLOCK_SIZE)

typedef enum e_enable_disable
{
    DISABLE,
    RE_ENABLE
} enable_disable_t;

void ThreadsAndInterrupts(enable_disable_t EnableDisable);
void display_image_slot_info(void);

#endif                                 /* HEADER_H_ */

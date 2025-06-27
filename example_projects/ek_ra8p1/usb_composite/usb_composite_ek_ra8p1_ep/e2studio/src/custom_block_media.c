/***********************************************************************************************************************
 * File Name    : custom_block_media.c
 * Description  : Contains block media related function descriptions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "rm_block_media_api.h"
#include "block_media_ram_disk.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_composite_ep
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
/* These API signatures (input parameters for all these API's) are standard signatures for block media driver
 * and used in this driver (block media custom driver) to maintain compatibility with the original block media APIs */
static fsp_err_t RM_BLOCK_MEDIA_RAM_Open (rm_block_media_ctrl_t * const p_ctrl,\
                                          rm_block_media_cfg_t const * const p_cfg);
static fsp_err_t RM_BLOCK_MEDIA_RAM_MediaInit (rm_block_media_ctrl_t * const p_ctrl);
static fsp_err_t RM_BLOCK_MEDIA_RAM_Read (rm_block_media_ctrl_t * const p_ctrl,
                                          uint8_t * const               p_dest_address,
                                          uint32_t const                block_address,
                                          uint32_t const                num_blocks);
static fsp_err_t RM_BLOCK_MEDIA_RAM_Write (rm_block_media_ctrl_t * const p_ctrl,
                                           uint8_t const * const         p_src_address,
                                           uint32_t const                block_address,
                                           uint32_t const                num_blocks);
static fsp_err_t RM_BLOCK_MEDIA_RAM_Erase (rm_block_media_ctrl_t * const p_ctrl,
                                           uint32_t const                block_address,
                                           uint32_t const                num_blocks);
static fsp_err_t RM_BLOCK_MEDIA_RAM_StatusGet (rm_block_media_ctrl_t * const   p_api_ctrl,
                                               rm_block_media_status_t * const p_status);
static fsp_err_t RM_BLOCK_MEDIA_RAM_InfoGet (rm_block_media_ctrl_t * const p_ctrl,\
                                             rm_block_media_info_t * const p_info);
static fsp_err_t RM_BLOCK_MEDIA_RAM_Close (rm_block_media_ctrl_t * const p_ctrl);

/* Global variables */
extern volatile bool g_blockmedia_complete_event;
extern const uint8_t g_ram_disk_boot_sector[];
extern const uint8_t g_usb_pmsc_table1[];
extern const uint8_t g_usb_pmsc_tablefat1[];
extern const uint8_t g_usb_pmsc_tablefat2[];
extern const uint8_t g_usb_pmsc_rootdir[];
uint32_t g_ram_disk_base_address;

const rm_block_media_api_t g_rm_block_media_on_user_media =
{
    .open       = RM_BLOCK_MEDIA_RAM_Open,
    .mediaInit  = RM_BLOCK_MEDIA_RAM_MediaInit,
    .read       = RM_BLOCK_MEDIA_RAM_Read,
    .write      = RM_BLOCK_MEDIA_RAM_Write,
    .erase      = RM_BLOCK_MEDIA_RAM_Erase,
    .infoGet    = RM_BLOCK_MEDIA_RAM_InfoGet,
    .statusGet  = RM_BLOCK_MEDIA_RAM_StatusGet,
    .close      = RM_BLOCK_MEDIA_RAM_Close,
};
extern uint32_t __ddsc_RAM_END;
#define ALIGN_4K (4096)

/*******************************************************************************************************************//**
 * Opens the module.
 *
 * Implements @ref rm_block_media_api_t::open().
 *
 * @retval     FSP_SUCCESS                     Module is available and is now open.
 * @retval     Any other error code apart from FSP_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_Open (rm_block_media_ctrl_t * const p_ctrl, rm_block_media_cfg_t const * const p_cfg)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_cfg);
    uint32_t adr = RESET_VALUE;

    /* Update the SRAM media address and copy the boot sector data to it */

    g_ram_disk_base_address = (uint32_t)&(__ddsc_RAM_END);
    g_ram_disk_base_address = (g_ram_disk_base_address + (ALIGN_4K - 1)) & (uint32_t)~(ALIGN_4K - 1);
    adr = g_ram_disk_base_address;
    memcpy((void *)adr, (void *)&g_ram_disk_boot_sector, STRG_SECTSIZE);

    /* Update the SRAM media address and copy the usb_pmsc table data to it */
    adr += (STRG_SECTSIZE);
    memcpy((void *)adr, (void *)&g_usb_pmsc_table1, STRG_SECTSIZE);

    /* Update the SRAM media address and copy the usb_pmsc FAT1 table data to it */
    adr += (STRG_SECTSIZE);
    memcpy((void *)adr, (void *)&g_usb_pmsc_tablefat1, (STRG_SECTSIZE * RAMDISK_FATSIZE));

    /* Update the SRAM media address and copy the usb_pmsc FAT2 table data to it */
    adr += (STRG_SECTSIZE * RAMDISK_FATSIZE);
    memcpy((void *)adr, (void *)&g_usb_pmsc_tablefat2, (STRG_SECTSIZE * RAMDISK_FATSIZE));

    /* Update the SRAM media address and copy the usb_pmsc root directory data to it */
    adr += (STRG_SECTSIZE * RAMDISK_FATSIZE);
    memcpy((void *)adr, (void *)&g_usb_pmsc_rootdir, (STRG_SECTSIZE * RAMDISK_DIRSIZE));

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Initializes the media. This function blocks until all identification and configuration commands are complete.
 *
 * Implements @ref rm_block_media_api_t::mediaInit().
 *
 * @retval     FSP_SUCCESS               Module is initialized and ready to access the memory device.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_MediaInit (rm_block_media_ctrl_t * const p_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    /* Set the block media complete event flag */
    g_blockmedia_complete_event = true;
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Reads data from a media.
 * Implements @ref rm_block_media_api_t::read().
 *
 * @retval     FSP_SUCCESS                   Data read successfully.
 * @retval     Any other error code apart from FSP_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_Read (rm_block_media_ctrl_t * const p_ctrl,
                                     uint8_t * const               p_dest_address,
                                     uint32_t const                block_address,
                                     uint32_t const                num_blocks)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    /* Copy block from specified RAM disk block address to read_buffer */
    memcpy(p_dest_address, (void *)(g_ram_disk_base_address + (block_address * STRG_SECTSIZE)),\
           (STRG_SECTSIZE * num_blocks));

    /* Set the block media complete event flag */
    g_blockmedia_complete_event = true;
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Writes data to a media.
 * Implements @ref rm_block_media_api_t::write().
 *
 * @retval     FSP_SUCCESS                   Write finished successfully.
 * @retval     Any other error code apart from FSP_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_Write (rm_block_media_ctrl_t * const p_ctrl,
                                    uint8_t const * const         p_src_address,
                                    uint32_t const                block_address,
                                    uint32_t const                num_blocks)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    /* Copy block from write_buffer to appropriate block address in RAM disk */
    memcpy((void *)(g_ram_disk_base_address + (block_address * STRG_SECTSIZE)), p_src_address,\
           (STRG_SECTSIZE * num_blocks));

    /* Set the block media complete event flag */
    g_blockmedia_complete_event = true;
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Erases sectors of a media.
 * Implements @ref rm_block_media_api_t::erase().
 *
 * @retval     FSP_SUCCESS                   Erase operation requested.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_Erase (rm_block_media_ctrl_t * const p_ctrl,
                                    uint32_t const                block_address,
                                    uint32_t const                num_blocks)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(block_address);
    FSP_PARAMETER_NOT_USED(num_blocks);

    /* Set the block media complete event flag */
    g_blockmedia_complete_event = true;
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Provides driver status. Implements @ref rm_block_media_api_t::statusGet().
 *
 * @retval     FSP_SUCCESS                   Status stored in p_status.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_StatusGet (rm_block_media_ctrl_t * const   p_api_ctrl,
                                        rm_block_media_status_t * const p_status)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    /* Update the block media driver current status */
    p_status->media_inserted = true;
    p_status->initialized    = true;
    p_status->busy           = false;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Retrieves module information. Implements @ref rm_block_media_api_t::infoGet().
 *
 * @retval     FSP_SUCCESS                   Erase operation requested.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_InfoGet (rm_block_media_ctrl_t * const p_ctrl, rm_block_media_info_t * const p_info)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    /* Update the block media driver info */
    p_info->sector_size_bytes = STRG_SECTSIZE;
    p_info->num_sectors       = STRG_TOTALSECT;
    p_info->reentrant         = false;
    p_info->write_protected   = false;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Closes an open media. Implements @ref rm_block_media_api_t::close().
 *
 * @retval     FSP_SUCCESS                   Successful close.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_Close (rm_block_media_ctrl_t * const p_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_composite_ep)
 **********************************************************************************************************************/

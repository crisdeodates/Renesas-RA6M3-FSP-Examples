/**********************************************************************************************************************
 * File Name    : filex.c
 * Description  : Contains data structures and functions
 *********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <app_thread.h>
#include "filex.h"

/**********************************************************************************************************************
 * Public global variable
 *********************************************************************************************************************/
FX_MEDIA g_fx_media;

/**********************************************************************************************************************
 * Private global variable
 *********************************************************************************************************************/
static uint8_t g_fx_media_media_memory[G_FX_MEDIA_MEDIA_MEMORY_SIZE];
static CHAR g_dir_name[] = DIR_NAME_ONE;
static CHAR g_file_name[] = FILE_NAME_ONE;
static CHAR g_write_data[WRITE_BUFFER_SIZE] = {RESET_VALUE};
volatile _Bool g_media_opened = false;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static UINT media_format(void);
static UINT media_open(void);
static UINT media_get_property(void);
static UINT dir_create(void);
static UINT dir_get_property(void);
static UINT dir_delete(void);
static UINT file_create(void);
static UINT file_write(void);
static UINT file_read(void);
static UINT file_delete(void);
static void create_fixed_buffer(void);
static UINT get_month(CHAR * p_month);

/**********************************************************************************************************************
 *  Function Name: g_rm_filex_levelx_nor_callback
 *  Description  : FileX LevelX NOR callback function.
 *  Arguments    : p_args   Callback
 *  Return Value : None
 *********************************************************************************************************************/
void g_rm_filex_levelx_nor_callback(rm_filex_levelx_nor_callback_args_t *p_args)
{
    if (p_args->event == RM_FILEX_LEVELX_NOR_EVENT_BUSY)
    {
        /* Put the thread to sleep while waiting for operation to complete. */
        tx_thread_sleep(APP_SLEEP_TICK);
    }
}
/**********************************************************************************************************************
* End of function g_rm_filex_levelx_nor_callback
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: file_system_init
 *  Description  : This function initializes the FileX file system.
 *  Arguments    : None
 *  Return Value : FX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 *********************************************************************************************************************/
UINT file_system_init(void)
{
    UINT status = FX_SUCCESS;
    time_t time = {RESET_VALUE};

    /* Extract current Date from compiler MACROS */
    time.month = get_month(__DATE__ + MONTH_INDEX);
    time.date = atoi(__DATE__ + DATE_INDEX);
    time.year = atoi(__DATE__ + YEAR_INDEX);

    /* Extract current Time from compiler MACROS */
    time.hour = atoi(__TIME__ + HOUR_INDEX);
    time.min  = atoi(__TIME__ + MIN_INDEX);
    time.sec  = atoi(__TIME__ + SEC_INDEX);

    /* Initialize the FileX system */
    fx_system_initialize();

    /* Set date for FileX system */
    status = fx_system_date_set (time.year, time.month, time.date);
    RETURN_ERR_STR(status, "fx_system_date_set failed\r\n");

    /* Set time for FileX system */
    status = fx_system_time_set (time.hour, time.min, time.sec);
    RETURN_ERR_STR(status, "fx_system_time_set failed\r\n");

    return status;
}
/**********************************************************************************************************************
* End of function file_system_init
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: filex_ospi_levelx_operation
 *  Description  : This function performs file system operations based on user requests.
 *  Arguments[in]: request      User requests for file system operations
 *  Return Value : FX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 *********************************************************************************************************************/
UINT filex_ospi_levelx_operation(UINT request)
{
    UINT status = FX_SUCCESS;

    switch (request)
    {
        case REQUEST_ERASE_FLASH:
            status = (UINT) erase_ospi_flash();
            RETURN_ERR_STR (status, "erase_ospi_flash failed\r\n");
            break;

        case REQUEST_MEDIA_FORMAT:
            status = media_format();
            RETURN_ERR_STR (status, "media_format failed\r\n");
            break;

        case REQUEST_MEDIA_OPEN:
            status = media_open();
            RETURN_ERR_STR (status, "media_open failed\r\n");
            break;

        case REQUEST_DEFRAGMENT_FLASH:
            status = defragment_flash();
            RETURN_ERR_STR (status, "defragment_flash failed\r\n");
            break;

        case REQUEST_DIR_CREATE:
            status = dir_create();
            RETURN_ERR_STR (status, "dir_create failed\r\n");
            break;

        case REQUEST_DIR_PROPERTY:
            status = dir_get_property();
            RETURN_ERR_STR (status, "dir_get_property failed\r\n");
            break;

        case REQUEST_DIR_DELETE:
            status = dir_delete();
            RETURN_ERR_STR (status, "dir_delete failed\r\n");
            break;

        case REQUEST_FILE_CREATE:
            status = file_create();
            RETURN_ERR_STR (status, "file_create failed\r\n");
            break;

        case REQUEST_FILE_WRITE:
            status = file_write();
            RETURN_ERR_STR (status, "file_write failed\r\n");
            break;

        case REQUEST_FILE_READ:
            status = file_read();
            RETURN_ERR_STR (status, "file_read failed\r\n");
            break;

        case REQUEST_FILE_DELETE:
            status = file_delete();
            RETURN_ERR_STR (status, "file_delete failed\r\n");
            break;

        case REQUEST_SECTOR_WRITE:
            status = (UINT) sector_write();
            RETURN_ERR_STR (status, "sector_write failed\r\n");
            break;

        case REQUEST_SECTOR_READ:
            status = (UINT) sector_read();
            RETURN_ERR_STR (status, "sector_read failed\r\n");
            break;

        case REQUEST_SECTOR_ERASE:
            status = (UINT) sector_erase();
            RETURN_ERR_STR (status, "sector_read failed\r\n");
            break;

        default:
            PRINT_INFO_STR("\r\nInvalid input\r\n\r\n");
            break;
    }

    return status;
}
/**********************************************************************************************************************
* End of function filex_ospi_levelx_operation
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: media_format
 *  Description  : This function formats the media.
 *  Arguments    : None
 *  Return Value : FX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 *********************************************************************************************************************/
static UINT media_format(void)
{
    UINT status = FX_SUCCESS;

    PRINT_INFO_STR("\r\n\r\nErasing the sectors before use for the file system ...\r\n");
    PRINT_INFO_STR("This operation will take several minutes\r\n");

    /* Close media before format */
    status = fx_media_close(&g_fx_media);
    if (FX_SUCCESS != status && FX_MEDIA_NOT_OPEN != status)
    {
        RETURN_ERR_STR(status, "fx_media_close failed\r\n");
    }

    /* Clear the media opened flag after close media */
    g_media_opened = false;

	/* Open OSPI module */
	status = (UINT) R_OSPI_B_Open(&g_ospi_b_ctrl, &g_ospi_b_cfg);
	RETURN_ERR_STR(status, "R_OSPI_B_Open API FAILED \r\n");

	/* Erase the flash sectors prior to usage for file system operations. */
    for (uint32_t i = RESET_VALUE; i <= g_rm_levelx_nor_spi_cfg.size ; i+= OSPI_B_SECTOR_256K_SIZE)
    {
		status = (UINT) R_OSPI_B_Erase(&g_ospi_b_ctrl, (uint8_t *)FILEX_START_ADDRESS + i, OSPI_B_SECTOR_256K_SIZE);
		RETURN_ERR_STR(status, "R_OSPI_B_Erase API FAILED \r\n");

		/* Wait until erase operation complete */
		status = (UINT) ospi_b_wait_operation(OSPI_B_TIME_ERASE_256K);
		RETURN_ERR_STR(status, "ospi_b_wait_operation FAILED \r\n");
    }

    /* Close OSPI flash */
    status = (UINT) R_OSPI_B_Close(&g_ospi_b_ctrl);
    RETURN_ERR_STR(status, "R_OSPI_B_Close API FAILED \r\n");

    /* Erase OSPI flash successful */
    PRINT_INFO_STR("\r\nErased the sectors use for the file system successfully\r\n");

    PRINT_INFO_STR("\r\nFormatting media ...\r\n");
    PRINT_INFO_STR("This operation will take several minutes\r\n");

    /* Format the media */
    status = fx_media_format(&g_fx_media,                               // Pointer to FileX media control block.
                             RM_FILEX_LEVELX_NOR_DeviceDriver,          // Driver entry
                             (void *) &g_rm_filex_levelx_nor_instance,  // Pointer to LevelX NOR Driver
                             g_fx_media_media_memory,                   // Media buffer pointer
                             G_FX_MEDIA_MEDIA_MEMORY_SIZE,              // Media buffer size
                             (char *) G_FX_MEDIA_VOLUME_NAME,           // Volume Name
                             G_FX_MEDIA_NUMBER_OF_FATS,                 // Number of FATs
                             G_FX_MEDIA_DIRECTORY_ENTRIES,              // Directory Entries
                             G_FX_MEDIA_HIDDEN_SECTORS,                 // Hidden sectors
                             G_FX_MEDIA_TOTAL_SECTORS,                  // Total sectors
                             G_FX_MEDIA_BYTES_PER_SECTOR,               // Sector size
                             G_FX_MEDIA_SECTORS_PER_CLUSTER,            // Sectors per cluster
                             G_FX_MEDIA_HEADS,                          // Heads (disk media)
                             G_FX_MEDIA_SECTORS_PER_TRACK);

    RETURN_ERR_STR(status, "fx_media_format FileX failed\r\n");

    /* Format the media successfully */
    PRINT_INFO_STR("\r\nMedia has been formatted");

    return status;
}
/**********************************************************************************************************************
* End of function media_format
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: media_open
 *  Description  : This function opens the media.
 *  Arguments    : None
 *  Return Value : FX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 *********************************************************************************************************************/
static UINT media_open(void)
{
    UINT status = FX_SUCCESS;

    PRINT_INFO_STR("\r\n\r\nOpening media ...\r\n");
    PRINT_INFO_STR("This operation will take several minutes\r\n");

    /* Check media opened flag */
    if (true == g_media_opened)
    {
        PRINT_INFO_STR ("Media has already been opened\r\n");
        return FX_SUCCESS;
    }

    /* Open media using the Azure FileX API */
    status = fx_media_open(&g_fx_media,
                           "g_fx_media",
                           RM_FILEX_LEVELX_NOR_DeviceDriver,
                           (void *) &g_rm_filex_levelx_nor_instance,
                           g_fx_media_media_memory,
                           G_FX_MEDIA_MEDIA_MEMORY_SIZE);
    if (FX_SUCCESS != status)
    {
        PRINT_INFO_STR ("Media open failed, please format media\r\n");
        return FX_SUCCESS;
    }

    /* Set the media opened status */
    g_media_opened = true;

    /* Open media successfully */
    PRINT_INFO_STR("\r\nMedia has been opened successfully\r\n");

    /* Get the media property */
    status = media_get_property();
    RETURN_ERR_STR(status, "media_get_property failed\r\n");

    return FX_SUCCESS;
}
/**********************************************************************************************************************
* End of function media_open
**********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: media_get_property
 *  Description  : This function retrieves media properties.
 *  Arguments    : None
 *  Return Value : FX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 **********************************************************************************************************************/
static UINT media_get_property(void)
{
    UINT status = FX_SUCCESS;
    media_property_t media = {RESET_VALUE};

    /* Get the free space size of the media */
    status = fx_media_extended_space_available(&g_fx_media, &media.free_size);
    RETURN_ERR_STR(status, "fx_media_extended_space_available failed\r\n");

    /* Calculate the total size of the media */
    media.total_size = (ULONG64)g_fx_media.fx_media_total_clusters *
                       (ULONG64)g_fx_media.fx_media_sectors_per_cluster *
                       (ULONG64)g_fx_media.fx_media_bytes_per_sector;

    /* Get the format type of the media */
    if(g_fx_media.fx_media_12_bit_FAT)
    {
        media.format_type = "FAT 12";
    }
    else if (g_fx_media.fx_media_32_bit_FAT)
    {
        media.format_type = "FAT 32";
    }
    else
    {
        media.format_type = "FAT 16";
    }

    /* Display media information */
    PRINT_MEDIA_PROPERTY(media);

    return FX_SUCCESS;
}
/**********************************************************************************************************************
* End of function media_get_property
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: dir_create
 *  Description  : This function creates a new directory.
 *  Arguments    : None
 *  Return Value : FX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 *********************************************************************************************************************/
static UINT dir_create(void)
{
    UINT status = FX_SUCCESS;
    entry_info_t entry = {RESET_VALUE};

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR ("Media has not been opened. Please open media first!!!\r\n");
        return FX_SUCCESS;
    }

    PRINT_INFO_STR("\r\n\r\nCreating a new directory ...\r\n\r\n");

    /* Create directory using Azure FileX API */
    status = fx_directory_create (&g_fx_media, g_dir_name);
    if (FX_ALREADY_CREATED == status)
    {
        PRINT_INFO_STR("Directory already exists\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_directory_create failed\r\n");
    }

    /* Flush data into the physical media */
    status = fx_media_flush(&g_fx_media);
    RETURN_ERR_STR(status, "fx_media_flush failed\r\n");

    /* Retrieve a directory name */
    strcpy(entry.name, g_dir_name);

    /* Retrieve a directory full information */
    status = fx_directory_information_get(&g_fx_media,
                                          entry.name, &entry.attr, &entry.size,
                                          &entry.time.year, &entry.time.month, &entry.time.date,
                                          &entry.time.hour, &entry.time.min, &entry.time.sec);
    RETURN_ERR_STR(status, "fx_directory_information_get failed\r\n");

    /* Display directory information */
    PRINT_ENTRY_INFO(entry);

    /* Retrieve and display sector information */
    status = sector_info_get();
    RETURN_ERR_STR(status, "sector_info_get failed\r\n");

    /* Create a directory successfully */
    PRINT_INFO_STR("\r\nDirectory created successfully");

    return status;
}
/**********************************************************************************************************************
* End of function dir_create
**********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: dir_get_property
 *  Description  : This function gets the properties of a directory.
 *  Arguments    : None
 *  Return Value : FX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 **********************************************************************************************************************/
static UINT dir_get_property(void)
{
    UINT status = FX_SUCCESS;
    dir_property_t dir = {RESET_VALUE};
    entry_info_t entry = {RESET_VALUE};

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR ("Media has not been opened. Please open media first!!!\r\n");
        return FX_SUCCESS;
    }

    PRINT_INFO_STR("\r\n\r\nGetting directory properties ...\r\n");

    /* Extract first entry details in the current directory */
    status = fx_directory_first_full_entry_find(&g_fx_media,
                                               entry.name, &entry.attr, &entry.size,
                                               &entry.time.year, &entry.time.month, &entry.time.date,
                                               &entry.time.hour, &entry.time.min, &entry.time.sec);
    /* Return in the case of a local directory that is empty. */
    if (FX_NO_MORE_ENTRIES == status)
    {
        PRINT_INFO_STR("\r\nDirectory empty\r\n");
        return FX_SUCCESS;
    }

    /* Return fails in other failed cases */
    if (FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_directory_first_full_entry_find failed\r\n");
    }

    /* Print root directory name */
    PRINT_INFO_STR("\r\nDirectory of :\\ \r\n\r\n");

    /* Check all entries in root directory */
    do
    {
        /* In case the entry is a directory */
        if(FX_DIRECTORY == (entry.attr & FX_DIRECTORY))
        {
            /* Print entry info */
            PRINT_ENTRY_INFO(entry);
            /* Count number of directories */
            dir.subdir ++;
        }

        /* In case the entry is a file */
        else if (FX_ARCHIVE == (entry.attr & FX_ARCHIVE))
        {
            /* Print entry info */
            PRINT_ENTRY_INFO(entry);
            /* Calculate total files size */
            dir.size += entry.size;
            /* Count number of files */
            dir.file ++;
        }

        /* In other cases */
        else
        {
           /* do not thing */
        }

        /* Extract details of next entry in the root directory */
        status = fx_directory_next_full_entry_find(&g_fx_media,
                                                  entry.name, &entry.attr, &entry.size,
                                                  &entry.time.year, &entry.time.month, &entry.time.date,
                                                  &entry.time.hour, &entry.time.min, &entry.time.sec);
        if ((FX_SUCCESS != status) && (FX_NO_MORE_ENTRIES != status))
        {
            RETURN_ERR_STR(status, "fx_directory_next_full_entry_find failed\r\n");
        }
    }
    while (FX_NO_MORE_ENTRIES != status);

    /* Print number of file and total file size */
    PRINT_DIR_PROPERTY(dir);

    /* Get directory properties successfully */
    PRINT_INFO_STR("Got directory properties successfully");

    return FX_SUCCESS;
}
/**********************************************************************************************************************
* End of function dir_get_property
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: dir_delete
 *  Description  : This function deletes a directory.
 *  Arguments    : None
 *  Return Value : FX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 *********************************************************************************************************************/
static UINT dir_delete(void)
{
    UINT status = FX_SUCCESS;

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR ("Media has not been opened. Please open media first!!!\r\n");
        return FX_SUCCESS;
    }

    PRINT_INFO_STR("\r\n\r\nDeleting a directory ...\r\n");

    /* Delete a directory using Azure FileX API */
    status = fx_directory_delete(&g_fx_media, g_dir_name);

    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("Directory not exists\r\n");
        return FX_SUCCESS;
    }

    if (FX_DIR_NOT_EMPTY == status)
    {
        PRINT_INFO_STR("Directory not empty\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_directory_delete failed\r\n");
    }

    /* Flush data to physical media */
    status = (ULONG)fx_media_flush(&g_fx_media);
    RETURN_ERR_STR(status, "fx_media_flush failed\r\n");

    /* Delete a directory successfully */
    PRINT_INFO_STR("\r\nDirectory has been deleted");

    return status;
}
/**********************************************************************************************************************
* End of function dir_delete
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: file_create
 *  Description  : This function creates a new file.
 *  Arguments    : None
 *  Return Value : FX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 *********************************************************************************************************************/
static UINT file_create(void)
{
    UINT status = FX_SUCCESS;
    entry_info_t entry = {RESET_VALUE};

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR ("Media has not been opened. Please open media first!!!\r\n");
        return FX_SUCCESS;
    }

    PRINT_INFO_STR("\r\n\r\nCreating a new file ...\r\n\r\n");

    /* Create a new file using the Azure FileX API */
    status = fx_file_create(&g_fx_media, g_file_name);

    if (FX_ALREADY_CREATED == status)
    {
        PRINT_INFO_STR("File already exists\r\n");
        return FX_SUCCESS;
    }

    else if (FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_file_create failed\r\n");
    }

    /* Flush data into the physical media */
    status = fx_media_flush(&g_fx_media);
    RETURN_ERR_STR(status, "fx_media_flush failed\r\n");

    /* Retrieve a file name */
    strcpy(entry.name, g_file_name);

    /* Retrieve a file full information */
    status = fx_directory_information_get(&g_fx_media,
                                          entry.name, &entry.attr, &entry.size,
                                          &entry.time.year, &entry.time.month, &entry.time.date,
                                          &entry.time.hour, &entry.time.min, &entry.time.sec);
    RETURN_ERR_STR(status, "fx_directory_information_get failed\r\n");

    /* Display file information */
    PRINT_ENTRY_INFO(entry);

    /* Retrieve and display sector information */
    status = sector_info_get();
    RETURN_ERR_STR(status, "sector_info_get failed\r\n");

    /* Create a new file successfully */
    PRINT_INFO_STR("\r\nFile created successfully");

    return FX_SUCCESS;
}
/**********************************************************************************************************************
* End of function file_create
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: file_write
 *  Description  : This function writes fixed data to a file.
 *  Arguments    : None
 *  Return Value : FX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 *********************************************************************************************************************/
static UINT file_write(void)
{
    UINT status = FX_SUCCESS;
    UINT status_temp = FX_SUCCESS;
    FX_FILE file = {RESET_VALUE};
    time_t time = {RESET_VALUE};
    entry_info_t entry = {RESET_VALUE};

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR ("Media has not been opened. Please open media first!!!\r\n");
        return FX_SUCCESS;
    }

    PRINT_INFO_STR("\r\n\r\nWriting to a file ...\r\n\r\n");

    /* Open the file for writing by using the Azure FileX API */
    status = fx_file_open(&g_fx_media, &file, g_file_name, FX_OPEN_FOR_WRITE);

    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("File does not exist\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_file_open failed\r\n");
    }

    /* Clean the file contents */
    status = fx_file_truncate(&file, TRUNCATE_VALUE);
    if (FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

        /* Return fx_file_extended_truncate failed status */
        RETURN_ERR_STR(status, "fx_file_truncate failed\r\n");
    }

    /* Create fixed buffer */
    create_fixed_buffer();

    /* Write content to the opened file */
    status = fx_file_write(&file, (VOID *)g_write_data, WRITE_BUFFER_SIZE);
    if (FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

        /* Return fx_file_write failed status */
        RETURN_ERR_STR(status, "fx_file_write failed\r\n");
    }

    /* Get system time */
    status = fx_system_time_get(&time.hour, & time.min, &time.sec);
    if (FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

        /* Return fx_system_time_get failed status */
        RETURN_ERR_STR(status, "fx_system_time_get failed\r\n");
    }

    /* Get system date */
    status = fx_system_date_get(&time.year, & time.month, &time.date);
    if (FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

        /* Return fx_system_date_get failed status */
        RETURN_ERR_STR(status, "fx_system_date_get failed\r\n");
    }

    /* Set the date and time information for the opened file */
    status = fx_file_date_time_set(&g_fx_media, g_file_name,
                                   time.year, time.month, time.date,
                                   time.hour, time.min, time.sec);
    if (FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

        /* Return fx_file_date_time_set failed status */
        RETURN_ERR_STR(status, "fx_file_date_time_set failed\r\n");
    }

    /* Close the file using the Azure FileX API */
    status = fx_file_close(&file);
    RETURN_ERR_STR(status, "fx_file_close failed\r\n");

    /* Flush data into the physical media */
    status = fx_media_flush(&g_fx_media);
    RETURN_ERR_STR(status, "fx_media_flush failed\r\n");

    /* Get file name */
    strcpy(entry.name, g_file_name);

    /* Get file full information */
    status = fx_directory_information_get(&g_fx_media,
                                          entry.name, &entry.attr, &entry.size,
                                          &entry.time.year, &entry.time.month, &entry.time.date,
                                          &entry.time.hour, &entry.time.min, &entry.time.sec);
    RETURN_ERR_STR(status, "fx_directory_information_get failed\r\n");

    /* Display file information */
    PRINT_ENTRY_INFO(entry);

    /* Write to the file successfully. */
    PRINT_INFO_STR("\r\nWrote to a file successfully");

    return FX_SUCCESS;
}
/**********************************************************************************************************************
* End of function file_write
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: file_read
 *  Description  : This function reads and verifies data from a file.
 *  Arguments    : None
 *  Return Value : FX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 *********************************************************************************************************************/
static UINT file_read(void)
{
    UINT status = FX_SUCCESS;
    UINT status_temp = FX_SUCCESS;
    FX_FILE file = {RESET_VALUE};
    ULONG len = RESET_VALUE;
    entry_info_t entry = {RESET_VALUE};
    CHAR read_data[READ_BUFFER_SIZE + ONE_BYTE] = {RESET_VALUE};

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR ("Media has not been opened. Please open media first!!!\r\n");
        return FX_SUCCESS;
    }

    PRINT_INFO_STR("\r\n\r\nReading from a file ...\r\n\r\n");

    /* Open the file for reading by using the Azure FileX API */
    status = fx_file_open(&g_fx_media, &file, g_file_name, FX_OPEN_FOR_READ);

    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("File does not exist\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_file_open failed\r\n");
    }

    /* Seek to the beginning of the file  */
    status = fx_file_seek(&file, SEEK_VALUE);
    if (FX_SUCCESS != status)
    {
       /* Close the file using the Azure FileX API */
       status_temp = fx_file_close(&file);
       RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

       /* Return fx_file_seek failed status */
       RETURN_ERR_STR(status, "fx_file_extended_seek failed\r\n");
    }

    /* Read data from the file */
    status = fx_file_read(&file, read_data, READ_BUFFER_SIZE, &len);

    /* In case reading the file failed */
    if (FX_END_OF_FILE != status && FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

        /* Return fx_file_read failed status */
        RETURN_ERR_STR(status, "fx_file_read failed\r\n");
    }

    /* Close the file using the Azure FileX API */
    status = fx_file_close(&file);
    RETURN_ERR_STR(status, "fx_file_close failed\r\n");

    /* Retrieve file name */
    strcpy(entry.name, g_file_name);

    /* Retrieve file full information */
    status = fx_directory_information_get(&g_fx_media,
                                          entry.name, &entry.attr, &entry.size,
                                          &entry.time.year, &entry.time.month, &entry.time.date,
                                          &entry.time.hour, &entry.time.min, &entry.time.sec);
    RETURN_ERR_STR(status, "fx_directory_information_get failed\r\n");

    /* Display file information */
    PRINT_ENTRY_INFO(entry);

    if (READ_BUFFER_SIZE > len)
    {
        /* Display content of the file */
        PRINT_INFO_STR("\r\nContent of the file\r\n\r\n");
        terminal_send_output_queue(TERMINAL_OUTPUT_APP_INFO_STR, len + ONE_BYTE, read_data);
    }
    else
    {
        /* Display content of the first 1 kB of the file */
        PRINT_INFO_STR("\r\nContent of the first 1 kB of the file\r\n\r\n");
        terminal_send_output_queue(TERMINAL_OUTPUT_APP_INFO_STR, READ_BUFFER_SIZE + ONE_BYTE, read_data);
    }

    /* Read a file successfully */
    PRINT_INFO_STR("\r\nRead a file successfully");

    return FX_SUCCESS;
}
/**********************************************************************************************************************
* End of function file_read
**********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: file_delete
 *  Description  : This function deletes a file.
 *  Arguments    : None
 *  Return Value : FX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 **********************************************************************************************************************/
static UINT file_delete(void)
{
    UINT status = FX_SUCCESS;

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR ("Media has not been opened. Please open media first!!!\r\n");
        return FX_SUCCESS;
    }

    PRINT_INFO_STR("\r\n\r\nDeleting a file ...\r\n");

    /* Delete the file using the Azure FileX API */
    status = fx_file_delete(&g_fx_media, g_file_name);

    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("File does not exist\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_file_delete failed\r\n");
    }

    /* Flush data into the physical media */
    status = fx_media_flush(&g_fx_media);
    RETURN_ERR_STR(status, "fx_media_flush failed\r\n");

    /* Delete the file successfully */
    PRINT_INFO_STR("\r\nDeleted a file successfully");

    return FX_SUCCESS;
}
/**********************************************************************************************************************
* End of function file_delete
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: create_fixed_buffer
 *  Description  : This function creates a fixed data buffer.
 *  Arguments    : None
 *  Return Value : None
 *********************************************************************************************************************/
static void create_fixed_buffer(void)
{
    CHAR * p_data = g_write_data;

    /* Clean write buffer */
    memset(p_data, NULL_CHAR, WRITE_BUFFER_SIZE);

    /* Create fixed buffer */
    for (uint16_t i = 0; i < WRITE_BUFFER_SIZE / WRITE_LINE_SIZE ; i ++)
    {
        strncpy(p_data, WRITE_LINE_TEXT, WRITE_LINE_SIZE);
        p_data += WRITE_LINE_SIZE;
    }
}
/**********************************************************************************************************************
* End of function create_fixed_buffer
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: get_month
 *  Description  : This function converts a string month to an integer month.
 *  Arguments[in]: p_month  pointer to a string month
 *  Return Value : integer month
 *********************************************************************************************************************/
static UINT get_month(CHAR * p_month)
{
    const CHAR * p_string[] = {"Nul","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

    /* Check and convert month string to integer */
    for (UINT value = (UINT)JANUARY; value <= (UINT)DECEMBER ; value ++)
    {
        if(RESET_VALUE == strncmp (p_string[value], p_month, MONTH_STR_LEN))
        {
            return (UINT)value;
        }
    }
    /* Set the default month to JANUARY if month is not detected */
    return (UINT)JANUARY;
}
/**********************************************************************************************************************
* End of function get_month
**********************************************************************************************************************/
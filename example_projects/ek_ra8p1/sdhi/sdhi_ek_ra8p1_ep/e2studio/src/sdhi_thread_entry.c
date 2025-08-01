/***********************************************************************************************************************
 * File Name    : sdhi_thread_entry.c
 * Description  : Contains data structures and functions used in sdhi_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "sdhi_thread.h"
#include "common_utils.h"
#include "sdhi_ep.h"

extern EventGroupHandle_t g_SDHI_EventGroupHandle;

/* Global variables */
static EventBits_t xEventGroupValue = RESET_VALUE;          /* Event group to capture the event */
static rm_freertos_plus_fat_device_t device;                /* Device structure contains device details */
FF_Disk_t my_disk;                                          /* Pointer to store FreeRTOS+FAT disk structure */

uint8_t g_write_data[BUFFER_SIZE] = {RESET_VALUE};          /* Data(10k) to write to file */
uint8_t g_read_data[BUFFER_SIZE] = {RESET_VALUE};           /* Variable to store the data read from file */
static volatile bool mount_failed = false;                  /* variable to capture mount status */

/* Function declarations */
static void process_sd_operation(uint8_t p_input_buffer);   /* Process SD operation */
static void sd_write_operation(void);                       /* Perform SD write operation */
static void sd_read_operation(void);                        /* Perform SD read operation */
static void format_sd_device(void);                         /* Perform SD format operation */
static void sd_safely_eject(void);                          /* Perform SD card safely disconnect operation */
static void update_buffer(void);                            /* Update write buffer with data and clear read buffer */
static fsp_err_t sd_init(void);                             /* Initialize SD card device */
static void fat_clean_up(void);                             /* Close FreeRTOS+FAT */
static void sd_display_cwd(void);                           /* Display content in a current working directory */

/* SDHI Thread entry function */
/* pvParameters contains TaskHandle_t */
void sdhi_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    memset(&my_disk, RESET_VALUE, sizeof(my_disk));
    fsp_pack_version_t version = { RESET_VALUE };

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);
    APP_PRINT(SDHI_EP_NOTE);

    g_SDHI_EventGroupHandle = NULL;

    fsp_err_t freertos_fat_error = FSP_SUCCESS;
    freertos_fat_error = sd_init();
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nError in initializing FreeRTOS+FAT with SDHI\r\n");
        APP_ERR_TRAP(freertos_fat_error);
    }

    /* Print SDHI menu */
    APP_PRINT(SDHI_MENU);

    while (true)
    {
        uint8_t rtt_input_buf[BUFFER_SIZE_DOWN] = {RESET_VALUE};
        uint8_t converted_rtt_input = RESET_VALUE;
        /* Read RTT input from the user */
        if (APP_CHECK_DATA)
        {
            APP_READ(rtt_input_buf);
            converted_rtt_input = (uint8_t)atoi((char *)rtt_input_buf);
            process_sd_operation(converted_rtt_input);
        }

        if((RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED == xEventGroupValue))
        {
            APP_ERR_PRINT("\r\n\r\nSD Card disconnected without eject option.\r\n");
            APP_ERR_PRINT("\r\nConnect the SD Card and execute Safely Eject option to make sure file operations works"\
                          " correctly\r\n");
            /* Wait until SD Card Device is connected */
            xEventGroupValue = xEventGroupWaitBits(g_SDHI_EventGroupHandle,
                                                   RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_INSERTED,
                                                   pdTRUE,
                                                   pdFALSE,
                                                   portMAX_DELAY);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function initiates the SD Card operation by calling respective functions.
 * @param[IN]   input_buffer    Converted RTT input.
 * @retval      None
 **********************************************************************************************************************/
static void process_sd_operation(uint8_t input_buffer)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;
    switch(input_buffer)
    {
        case SD_WRITE:
        {
            if(false == mount_failed)
            {
                sd_write_operation();
                sd_read_operation();
            }
            else
            {
                APP_ERR_PRINT("\r\nDisk is not formatted, first format the disk\r\n");
            }
            APP_PRINT(SDHI_MENU);
        }
        break;

        case SD_FORMAT:
        {
            format_sd_device();
            APP_PRINT(SDHI_MENU);
        }
        break;

        case SAFELY_EJECT_SD_CARD:
        {
            sd_safely_eject();
            APP_PRINT(SDHI_MENU);
        }
        break;

        case SD_DISPLAY_CWD:
        {
            if(false == mount_failed)
            {
                sd_display_cwd();
            }
            else
            {
                APP_ERR_PRINT("\r\nDisk is not formatted, first format the disk\r\n");
            }
            APP_PRINT(SDHI_MENU);
        }
        break;

        case SD_INIT:
        {
            if((RESET_VALUE == xEventGroupValue))
            {
                APP_PRINT(SDHI_EP_NOTE);
                freertos_fat_error = sd_init();
                if(FSP_SUCCESS != freertos_fat_error)
                {
                    APP_ERR_PRINT("\r\nError in initializing FreeRTOS+FAT with SDHI\r\n");
                    APP_ERR_TRAP(freertos_fat_error);
                }
            }
            else
            {
                APP_PRINT("\r\nFreeRTOS+FAT with SDHI driver is already initialized. Provide any other command\r\n");
            }
            APP_PRINT(SDHI_MENU);
        }
        break;

        default:
        {
            APP_PRINT("\r\nInvalid input. Provide a valid input\r\n");
            APP_PRINT(SDHI_MENU);
        }
        break;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function performs SD Card write operation.
 * @param[IN]   None
 * @retval      None
 ***********************************************************************************************************************/
static void sd_write_operation(void)
{
    FF_FILE *file_pointer = NULL;
    FF_Stat_t file_details;
    int32_t file_error = FF_ERR_NONE;
    /* Capture the number of bytes written in the variable to check write status */
    size_t bytes_written = RESET_VALUE;

    memset(&file_details, RESET_VALUE, sizeof(file_details));

    /* Double check the connection again to ensure the SD Card is still mounted */
    if (RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED != xEventGroupValue)
    {
        /* Once connection is detected open file is write mode */
        file_pointer = ff_fopen((const char *) FILE_NAME, WRITE_MODE);
        if (NULL != file_pointer)
        {
            /* Fill write buffer with data */
            update_buffer();

            APP_PRINT("\r\nSD Card write operation will be initiated\r\n");

            /* Write API writes */
            bytes_written = ff_fwrite(g_write_data, sizeof(g_write_data[RESET_VALUE]), BUFFER_SIZE, file_pointer);
            if (BUFFER_SIZE != bytes_written)
            {
                APP_ERR_PRINT("ff_fwrite API failed. Closing opened file.\r\n");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT("%d\r\n", stdioGET_ERRNO());
                file_error = ff_fclose(file_pointer);
                if (FF_ERR_NONE != file_error)
                {
                    APP_ERR_PRINT("ff_fclose API failed\r\n");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT("%d\r\n", stdioGET_ERRNO());
                }
                return;
            }

            /* Close the file after write operation and open again in read mode */
            file_error = ff_fclose(file_pointer);
            if (FF_ERR_NONE != file_error)
            {
                APP_ERR_PRINT("ff_fclose API failed\r\n");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT("%d\r\n", stdioGET_ERRNO());
                return;
            }

            file_error = ff_stat((const char*)FILE_NAME, &file_details);
            /* ff_stat returns 0 on success and -1 on error */
            if (FF_ERR_NONE == file_error)
            {
                /* Compare the actual bytes written and file size after write operation */
                if (bytes_written == file_details.st_size)
                {
                    APP_PRINT("%d bytes data successfully written to file %s\r\n", bytes_written, FILE_NAME);
                    APP_PRINT("Write operation is successful\r\n");
                }
                else
                {
                    APP_ERR_PRINT("ff_fwrite API failed\r\n");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT("%d\r\n", stdioGET_ERRNO());
                }
            }
            else
            {
                APP_ERR_PRINT("ff_stat API failed\r\n");
                /* Adding extra %d as parses string and prints %d as-is. */
                APP_PRINT("%d\r\n", stdioGET_ERRNO());
            }
        }
        else
        {
            APP_ERR_PRINT("ff_fopen API failed\r\n");
            /* Adding extra %d as parses string and prints %d as-is */
            APP_PRINT("%d\r\n", stdioGET_ERRNO());
        }
    }
    else
    {
        APP_ERR_PRINT("SD Card Device disconnected or not initialized after eject command\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function performs SD Card read operation.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void sd_read_operation(void)
{
    FF_FILE *file_pointer = NULL;
    FF_Stat_t file_details;
    int32_t file_error = FF_ERR_NONE;

    memset(&file_details, RESET_VALUE, sizeof(file_details));

    /* Double check the connection again to ensure the SD Card is still mounted */
    if (RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED != xEventGroupValue)
    {
        /* Open the file read mode to read data written previously */
        file_pointer = ff_fopen((const char *) FILE_NAME, READ_MODE);
        if (NULL != file_pointer)
        {
            APP_PRINT("\r\nSD Card read operation will be initiated\r\n");

            /* Capture the number of bytes read in the variable to check read status */
            size_t bytes_read = RESET_VALUE;
            bytes_read = ff_fread(g_read_data, sizeof(g_read_data[RESET_VALUE]), BUFFER_SIZE, file_pointer);
            if (BUFFER_SIZE != bytes_read)
            {
                APP_ERR_PRINT("ff_fread API failed. Closing opened file.\r\n", stdioGET_ERRNO());
                file_error = ff_fclose(file_pointer);
                if(FF_ERR_NONE != file_error)
                {
                    APP_ERR_PRINT("ff_fclose API failed\r\n");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT("%d\r\n", stdioGET_ERRNO());
                }
                return;
            }

            file_error = ff_fclose(file_pointer);
            if(FF_ERR_NONE != file_error)
            {
                APP_ERR_PRINT("ff_fclose API failed\r\n");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT("%d\r\n", stdioGET_ERRNO());
                return;
            }

            file_error = ff_stat((const char*)FILE_NAME, &file_details);
            /* ff_stat returns 0 on success and -1 on error */
            if (FF_ERR_NONE == file_error)
            {
                /* Compare the actual bytes written and file size after write operation */
                if (bytes_read == file_details.st_size)
                {
                    APP_PRINT("%d bytes data successfully read from file %s\r\n", bytes_read, FILE_NAME);
                    APP_PRINT("Read operation is successful\r\n");
                }
                else
                {
                    APP_ERR_PRINT("ff_fwrite API failed\r\n");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT("%d\r\n", stdioGET_ERRNO());
                    return;
                }

                /* Compare write and read data */
                if(FF_ERR_NONE == memcmp(g_write_data, g_read_data, BUFFER_SIZE))
                {
                    APP_PRINT("\r\nWrite and read data are same\r\n");
                }
                else
                {
                    APP_ERR_PRINT("\r\nWrite and read data are not match\r\n");
                }
            }
            else
            {
                APP_ERR_PRINT("ff_stat API failed\r\n");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT("%d\r\n", stdioGET_ERRNO());
            }
        }
        else
        {
            APP_ERR_PRINT("ff_fopen API failed\r\n");
            /* Adding extra %d as parses string and prints %d as-is */
            APP_PRINT("%d\r\n", stdioGET_ERRNO());
        }
    }
    else
    {
        APP_ERR_PRINT("SD Card device disconnected or not initialized after eject command\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function performs SD Card format operation.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void format_sd_device(void)
{
    FF_Error_t ff_err = FF_ERR_NONE;
    /* The disk is likely not formatted. Partition and format the disk, then mount again. */
    FF_PartitionParameters_t partition_params;
    partition_params.ulSectorCount = device.sector_count;
    partition_params.ulHiddenSectors = 1;
    partition_params.ulInterSpace = 0;
    memset(partition_params.xSizes, 0, sizeof(partition_params.xSizes));
    partition_params.xSizes[0] = (BaseType_t) (partition_params.ulSectorCount - 1);
    partition_params.xPrimaryCount = 1;
    partition_params.eSizeType = eSizeIsSectors;

    if (RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED != xEventGroupValue)
    {
        APP_PRINT("\r\nSD Card formatting will be initiated. Formatting will take time "\
                  "depending on SD Card capacity.\r\n");
        APP_PRINT("Do not disconnect the SD Card while formatting is in progress. Please wait...\r\n");

        /* Set the number of partition before formatting SD Card */
        ff_err = FF_Partition(&my_disk, &partition_params);
        if (FSP_SUCCESS != ff_err)
        {
            APP_ERR_PRINT("\r\nFF_Partition API failed %d. Check the SD Card device.\r\n", FF_GetErrMessage(ff_err));
            APP_PRINT("%d\r\n", FF_GetErrMessage(ff_err));
        }
        /* Formatting time varies with SD Card capacity. Might take longer time for higher capacity SD Card. */
        ff_err = FF_Format(&my_disk, my_disk.xStatus.bPartitionNumber, pdFALSE, pdFALSE);
        if (FF_ERR_NONE != ff_err)
        {
            APP_ERR_PRINT("\r\nFF_Format API failed %d. Check the SD Card.\r\n", FF_GetErrMessage(ff_err));
            APP_PRINT("%d\r\n", FF_GetErrMessage(ff_err));
        }
        else
        {
            APP_PRINT("\r\nSD Card formatted successfully\r\n");
            /* Check if mount failed previously */
            if(true == mount_failed)
            {
                /* Try mounting the disk again after partition */
                ff_err = FF_Mount(&my_disk, my_disk.xStatus.bPartitionNumber);
                if (FSP_SUCCESS != ff_err)
                {
                    APP_ERR_PRINT("\r\nFF_Mount API failed\r\n");
                    /* Close the FREERTOS_PLUS_FAT_Close instance */
                    fat_clean_up();
                    /* Trap the error */
                    APP_ERR_TRAP(ff_err);
                }
                /* Add file system to the disk */
                ff_err = FF_FS_Add("/", &my_disk);
                if (FSP_SUCCESS == ff_err)
                {
                    APP_ERR_PRINT("\r\nFF_FS_Add API failed\r\n");
                    /* Close the FREERTOS_PLUS_FAT_Close instance */
                    fat_clean_up();
                    /* Trap the error */
                    APP_ERR_TRAP(ff_err);
                }
                mount_failed = false;
            }
        }
    }
    else
    {
        APP_PRINT("SD Card disconnected or not initialized after eject command\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes FreeRTOS+FAT on SDHI and safely ejects.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void sd_safely_eject(void)
{
    /* Check the SD Card connection before formating */
    if (RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED != xEventGroupValue)
    {
        fsp_err_t freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskDeinit(&g_rm_freertos_plus_fat_ctrl, &my_disk);
        if (FSP_SUCCESS != freertos_fat_error)
        {
            APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_DiskDeinit API failed\r\n");
            APP_ERR_TRAP(freertos_fat_error);
        }

        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        freertos_fat_error = RM_FREERTOS_PLUS_FAT_Close(&g_rm_freertos_plus_fat_ctrl);
        if (FSP_SUCCESS != freertos_fat_error)
        {
            APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_Close API failed\r\n");
            APP_ERR_TRAP(freertos_fat_error);
        }

        /* Clear the EventGroupBit value */
        xEventGroupValue = xEventGroupClearBits(g_SDHI_EventGroupHandle, RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_INSERTED);

        APP_PRINT("\r\nSD Card can be safely removed now\r\n");
    }
    else
    {
        /* SD Card disconnected */
        APP_PRINT("SD Card disconnected or not initialized after eject command\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function updates write buffer with data and clears read buffer.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void update_buffer(void)
{
    for (uint16_t buf_index = RESET_VALUE ; buf_index < BUFFER_SIZE ; buf_index++)
    {
        g_write_data[buf_index] = ASCII_CHAR_A;
        g_read_data[buf_index] = RESET_VALUE;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is callback for FreeRTOS+FAT and triggered when SD Card is removed or inserted.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void free_rtos_fat_callback(rm_freertos_plus_fat_callback_args_t *p_args)
{
    /* Post an event if FreeRTOS is available */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(g_SDHI_EventGroupHandle, p_args->event, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*******************************************************************************************************************//**
 * @brief       This function initializes the FreeRTOS+FAT instance.
 * @param[IN]   None
 * @retval      Any other error code apart from FSP_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t sd_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int32_t ff_err = FF_ERR_NONE;

    /* Create event flags */
    if (NULL == g_SDHI_EventGroupHandle)
    {
        g_SDHI_EventGroupHandle = xEventGroupCreate();
        if(NULL == g_SDHI_EventGroupHandle)
        {
            /* The event group was not created because there was insufficient FreeRTOS heap available */
            APP_ERR_PRINT("\r\nError in creating eventGroup\r\n");
            APP_ERR_TRAP(FSP_ERR_OUT_OF_MEMORY);
        }
    }

    memset(&device, RESET_VALUE, sizeof(device));

    /* Open FreeRTOS PLUS FAT */
    err = RM_FREERTOS_PLUS_FAT_Open(&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_Open API failed\r\n");
        return err;
    }

    APP_PRINT("\r\n\nFreeRTOS+FAT Open successful\r\n");
    /* Wait for SD Card connection */
    APP_PRINT("Connect SD Card...\r\n");

    /* Wait until SD Card is connected */
    xEventGroupValue = xEventGroupWaitBits(g_SDHI_EventGroupHandle,
                                           RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_INSERTED,
                                           pdTRUE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* Initialize the mass storage device. This should not be done until the device is plugged in and initialized. */
    err = RM_FREERTOS_PLUS_FAT_MediaInit(&g_rm_freertos_plus_fat_ctrl, &device);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_MediaInit API failed\r\n");
        fat_clean_up();
        return err;
    }

    /* Initialize one disk for each partition used in the application */
    err = RM_FREERTOS_PLUS_FAT_DiskInit(&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_disk_cfg, &my_disk);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_DiskInit API failed\r\n");
        fat_clean_up();
        return err;
    }

    /* Mount each disk. This assumes the disk is already partitioned and formatted. */
    ff_err = FF_Mount(&my_disk, my_disk.xStatus.bPartitionNumber);
    if (FF_ERR_NONE != ff_err)
    {
        mount_failed = true;
        APP_ERR_PRINT("\r\nFF_Mount API failed indicating disk is not formatted\r\n");
        APP_ERR_PRINT("First format the disk\r\n");
    }
    else
    {
        /* Add the disk to the file system */
        ff_err = FF_FS_Add("/", &my_disk);
        if (FF_FS_ADD_FAILURE == ff_err)
        {
            APP_ERR_PRINT("\r\nFF_FS_Add API failed\r\n");
            /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
            fat_clean_up();
            /* Returning FSP error in case of failure */
            return ((fsp_err_t )FF_ERR_DEVICE_DRIVER_FAILED);
        }
    }

    /* Set this flag to let application know that SD Card is initialized */
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function displays the content in current working directory.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void sd_display_cwd(void)
{
    /* Buffer to store CWD content */
    char pcBuffer[50];
    int32_t ff_err = FF_ERR_NONE;
    FF_FindData_t * pxFindStruct = NULL;
    const char    * pcAttrib = NULL;
    const char    * pcWritableFile = "writable file";
    const char    * pcReadOnlyFile = "read only file";
    const char    * pcDirectory    = "directory";

    memset(&pcBuffer, RESET_VALUE, sizeof(pcBuffer));

    /* Check the SD Card connection before formating */
    if (RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED != xEventGroupValue)
    {
        /* Allocate the memory */
        pxFindStruct = (FF_FindData_t *) pvPortMalloc(sizeof(FF_FindData_t));

        /* Create a directory in current working directory */
        ff_err = ff_mkdir("/ra_dir");
        if(FSP_SUCCESS != ff_err)
        {
            APP_ERR_PRINT("ff_mkdir API failed\r\n");
            /* Adding extra %d as parses string and prints %d as-is */
            APP_PRINT("%d\r\n", stdioGET_ERRNO());
            APP_ERR_TRAP(ff_err);
        }
        else
        {
            APP_PRINT("\r\nDirectory ra_dir is created in root directory\r\n\r\n", pcBuffer);
        }

        /* pcBuffer is returned, hence return value is not captured */
        ff_getcwd(pcBuffer, sizeof(pcBuffer));
        APP_PRINT("CWD is: %s\r\n", pcBuffer);
        APP_PRINT("File Name    Attribute     File Size\r\n\r\n");

        /* The first parameter to ff_findfist() is the directory being searched.
         * Do not add wild cards to the end of the directory name. */
        if(RESET_VALUE == ff_findfirst("/", pxFindStruct))
        {
            do
            {
                /* Point pcAttrib to a string that describes the file */
                if(RESET_VALUE != (pxFindStruct->ucAttributes & FF_FAT_ATTR_DIR))
                {
                    pcAttrib = pcDirectory;
                }
                else if(pxFindStruct->ucAttributes & FF_FAT_ATTR_READONLY)
                {
                    pcAttrib = pcReadOnlyFile;
                }
                else
                {
                    pcAttrib = pcWritableFile;
                }

                /* Print the files name, size, and attribute string */
                APP_PRINT("%s    %s    %d bytes\r\n", pxFindStruct->pcFileName, pcAttrib, pxFindStruct->ulFileSize);

            } while(RESET_VALUE == ff_findnext(pxFindStruct));
        }
        /* Release the allocated memory */
        vPortFree((void *) pxFindStruct);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes the FreeRTOS+FAT instance.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void fat_clean_up(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;

    /* Close the RM_FREERTOS_PLUS_FAT instance on any failure */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Close(&g_rm_freertos_plus_fat_ctrl);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_Close API failed\r\n");
    }
    else
    {
        APP_PRINT("\r\nRM FREERTOS PLUS FAT instance closed successfully\r\n");
    }
}

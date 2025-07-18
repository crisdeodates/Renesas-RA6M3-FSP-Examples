/***********************************************************************************************************************
 * File Name    : usb_hmsc_thread_entry.c
 * Description  : Contains data structures and functions used in usb_hmsc_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usb_hmsc_thread.h"
#include "common_utils.h"
#include "usb_hmsc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_hmsc_ep
 * @{
 **********************************************************************************************************************/
/* Global variables */
uint8_t g_write_data[WRITE_ITEM_SIZE] = {RESET_VALUE};   /* Data (10k) to write to file */
uint8_t g_read_data[WRITE_ITEM_SIZE] = {RESET_VALUE};    /* Variable to store the data read from file */
FF_Disk_t g_my_disk;                                     /* Pointer to store FreeRTOS+FAT disk structure */

/* Variables to check the Safely Eject option */
static bool b_usb_hmsc_close = false;
static volatile bool g_rm_freertos_plus_fat_insertion_events = false;

/* USB HMSC Thread entry function */
/* pvParameters contains TaskHandle_t */
void usb_hmsc_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    memset(&g_my_disk, RESET_VALUE, sizeof(g_my_disk));
    fsp_pack_version_t version = {RESET_VALUE};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    fsp_err_t freertos_fat_error = FSP_SUCCESS;

    freertos_fat_error = usb_init();
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT ("\r\nError in initializing FreeRTOS+FAT with USB_HMSC\r\n");
        APP_ERR_TRAP (freertos_fat_error);
    }

    /* Print USB HMSC menu */
    APP_PRINT (USB_HMSC_MENU);

    while (true)
    {
        static uint8_t rtt_input_buf[BUFFER_SIZE_DOWN] = {RESET_VALUE};
        static uint8_t converted_rtt_input = RESET_VALUE;
        /* Read RTT Viewer input from the user */
        if (APP_CHECK_DATA)
        {
            APP_READ (rtt_input_buf);
            converted_rtt_input = (uint8_t)atoi((char *)rtt_input_buf);
            process_usb_operation(converted_rtt_input);
        }

        if((false == b_usb_hmsc_close) && (false == g_rm_freertos_plus_fat_insertion_events))
        {
            APP_PRINT("\r\n\nUSB device disconnected without Eject option.\r\n");
            APP_PRINT("\r\nConnect the USB and execute Safely Eject option to make sure file operations work "
                      "correctly\r\n");
            /* Wait until USB device is connected */
            while (true != check_usb_connection());
        }
        vTaskDelay (1);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function initiates the USB operation by calling respective functions.
 * @param[IN]   input_buffer     Converted RTT input.
 * @retval      None
 **********************************************************************************************************************/
void process_usb_operation(uint8_t input_buffer)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;
    switch (input_buffer)
    {
        case USB_WRITE:
        {
            usb_write_operation();
            usb_read_operation();
            APP_PRINT(USB_HMSC_MENU);
        }
        break;
        case USB_FORMAT:
        {
            format_usb_device();
            APP_PRINT(USB_HMSC_MENU);
        }
        break;
        case USB_SAFELY_EJECT:
        {
            usb_safely_eject();
            APP_PRINT(USB_HMSC_MENU);
        }
        break;
        case USB_INIT:
        {
            if(true == b_usb_hmsc_close)
            {
                freertos_fat_error = usb_init();
                if(FSP_SUCCESS != freertos_fat_error)
                {
                    APP_ERR_PRINT("\r\nError in initializing FreeRTOS+FAT with USB_HMSC\r\n");
                    APP_ERR_TRAP(freertos_fat_error);
                }
            }
            else
            {
                APP_PRINT("\r\nFreeRTOS+FAT USB_HMSC driver is already initialized. Provide any other command.\r\n");
            }
            APP_PRINT(USB_HMSC_MENU);
        }
        break;
        default:
        {
            APP_PRINT("\r\nInvalid input. Provide a valid input.\r\n");
        }
        break;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function performs USB HMSC write operation.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void usb_write_operation(void)
{
    FF_FILE *file_pointer = NULL;
    FF_Stat_t file_details;
    int32_t file_error = SUCCESS;
    /* Capture the number of bytes written in the variable to check write status */
    size_t bytes_written = RESET_VALUE;

    memset(&file_details, RESET_VALUE, sizeof(file_details));

    /* Double check the connection again to ensure the USB device is still mounted */
    if ((true == check_usb_connection()) && (true != b_usb_hmsc_close))
    {
        /* Once connection is detected open file is write mode */
        file_pointer = ff_fopen((const char *) FILE_NAME, WRITE_MODE);
        if (NULL != file_pointer)
        {
            /* Fill write buffer with data */
            update_buffer();

            APP_PRINT("USB write operation will be initiated\n");

            /* Write API writes */
            bytes_written = ff_fwrite(g_write_data, sizeof(g_write_data[RESET_VALUE]),\
                                      WRITE_ITEM_SIZE, file_pointer);
            if (WRITE_ITEM_SIZE != bytes_written)
            {
                APP_ERR_PRINT("ff_fwrite API failed. Closing opened file.\r\n");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT("%d\r\n", stdioGET_ERRNO());
                file_error = ff_fclose(file_pointer);
                if (SUCCESS != file_error)
                {
                    APP_ERR_PRINT("ff_fclose API failed");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT("%d\r\n", stdioGET_ERRNO());
                }
                return;
            }

            /* Close the file after write operation and open again in read mode */
            file_error = ff_fclose(file_pointer);
            if (SUCCESS != file_error)
            {
                APP_ERR_PRINT("ff_fclose API failed");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT("%d\r\n", stdioGET_ERRNO());
                return;
            }

            file_error = ff_stat((const char*)FILE_NAME, &file_details);
            /* ff_stat returns 0 on success and -1 on error */
            if (SUCCESS == file_error)
            {
                /* Compare the actual bytes written and file size after write operation */
                if (bytes_written == file_details.st_size)
                {
                    APP_PRINT("%d Bytes data successfully written to file %s\n", bytes_written, FILE_NAME);
                    APP_PRINT("Write operation is successful\n");
                }
                else
                {
                    APP_ERR_PRINT("ff_fwrite API failed");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT("%d\r\n", stdioGET_ERRNO());
                    return;
                }
            }
            else
            {
                APP_ERR_PRINT("ff_stat API failed");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT("%d\r\n", stdioGET_ERRNO());
                return;
            }
        }
        else
        {
            APP_ERR_PRINT("ff_fopen API failed");
            /* Adding extra %d as parses string and prints %d as-is. */
            APP_PRINT("%d\r\n", stdioGET_ERRNO());
            return;
        }
    }
    else
    {
        APP_PRINT ("USB device disconnected or not initialized after Eject command\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function performs USB HMSC read operation.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void usb_read_operation(void)
{
    FF_FILE *file_pointer = NULL;
    FF_Stat_t file_details;
    int32_t file_error = SUCCESS;

    memset(&file_details, RESET_VALUE, sizeof(file_details));

    /* Double check the connection again to ensure the USB device is still mounted */
    if ((true == check_usb_connection()) && (true != b_usb_hmsc_close))
    {
        /* Open the file read mode to read data written previously */
        file_pointer = ff_fopen((const char *) FILE_NAME, READ_MODE);
        if (file_pointer != NULL)
        {
            APP_PRINT("USB read operation will be initiated\n");

            /* Capture the number of bytes read in the variable to check read status */
            size_t bytes_read = RESET_VALUE;
            bytes_read = ff_fread(g_read_data, sizeof(g_read_data[RESET_VALUE]), WRITE_ITEM_SIZE, file_pointer);
            if (READ_WRITE_FAILURE == bytes_read)
            {
                APP_ERR_PRINT("ff_fread API failed. Closing opened file.\r\n", stdioGET_ERRNO());
                file_error = ff_fclose(file_pointer);
                if(SUCCESS != file_error)
                {
                    APP_ERR_PRINT("ff_fclose API failed");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT("%d\r\n", stdioGET_ERRNO());
                    return;
                }
                return;
            }

            file_error = ff_fclose(file_pointer);
            if(SUCCESS != file_error)
            {
                APP_ERR_PRINT("ff_fclose API failed");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT("%d\r\n", stdioGET_ERRNO());
                return;
            }

            file_error = ff_stat((const char*)FILE_NAME, &file_details);
            /* ff_stat returns 0 on success and -1 on error */
            if (SUCCESS == file_error)
            {
                /* Compare the actual bytes written and file size after write operation */
                if (bytes_read == file_details.st_size)
                {
                    APP_PRINT("%d Bytes data successfully read from file %s\n", bytes_read, FILE_NAME);
                    APP_PRINT("Read operation is successful\n");
                }
                else
                {
                    APP_ERR_PRINT("ff_fwrite API failed");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT("%d\r\n", stdioGET_ERRNO());
                    return;
                }

                /* Compare write and read data */
                if(SUCCESS == memcmp(g_write_data, g_read_data, WRITE_ITEM_SIZE))
                {
                    APP_PRINT("\r\nWrite and read data is same\r\n");
                }
                else
                {
                    APP_ERR_PRINT("\r\nWrite and read data did not match\r\n");
                }
            }
            else
            {
                APP_ERR_PRINT ("ff_stat API failed");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT("%d\r\n", stdioGET_ERRNO());
                return;
            }
        }
        else
        {
            APP_ERR_PRINT ("ff_fopen API failed");
            /* Adding extra %d as parses string and prints %d as-is */
            APP_PRINT("%d\r\n", stdioGET_ERRNO());
            return;
        }
    }
    else
    {
        APP_PRINT ("USB device disconnected or not initialized after Eject command\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function performs USB HMSC format operation.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void format_usb_device(void)
{
    if ((true == check_usb_connection()) && (true != b_usb_hmsc_close))
    {
        APP_PRINT("\r\nUSB device formatting will be initiated. Formatting will take time "
                  "depending on USB device capacity.\r\n");
        APP_PRINT("Do not disconnect the USB device while formatting is in progress. Please Wait ...\r\n");

        /* Formatting time varies with USB device capacity. Might take longer time for higher capacity USB device */
        FF_Error_t ff_error = FF_Format(&g_my_disk, g_my_disk.xStatus.bPartitionNumber, pdFALSE, pdFALSE);
        if (FF_ERR_NONE != ff_error)
        {
            APP_ERR_PRINT("\r\nFF_Format API failed %d. Check the USB device.\r\n", FF_GetErrMessage(ff_error));
            APP_PRINT("%d\r\n", FF_GetErrMessage(ff_error));
        }
        else
        {
            APP_PRINT("\r\nUSB device formatted successfully\r\n");
        }
    }
    else
    {
        APP_PRINT ("USB device disconnected or not initialized after Eject command\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes USB HMSC on FreeRTOS+FAT and safely ejects.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void usb_safely_eject(void)
{
    /* Check the USB device connection before formatting */
    if ((true == check_usb_connection()) && (true != b_usb_hmsc_close))
    {
        fsp_err_t freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskDeinit(&g_rm_freertos_plus_fat_ctrl, &g_my_disk);
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

        APP_PRINT("\r\nUSB device can be safely removed now\r\n");
        /* Update the flag */
        b_usb_hmsc_close = true;
        g_rm_freertos_plus_fat_insertion_events = false;
    }
    else
    {
        /* USB device disconnected */
        APP_PRINT("USB device disconnected or not initialized after Eject command\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function checks the USB HMSC connection status.
 * @param[IN]   None
 * @retval      true if connection detected; false if connection is not detected.
 **********************************************************************************************************************/
bool check_usb_connection(void)
{
    if (true != g_rm_freertos_plus_fat_insertion_events)
    {
        return false;
    }
    else
    {
        APP_PRINT("\r\nUSB device is connected\r\n");
        return true;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function updates write buffer with data and clears read buffer.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void update_buffer(void)
{
    for (uint16_t i = RESET_VALUE; i < WRITE_ITEM_SIZE; i++)
    {
        g_write_data[i] = ASCII_CHAR_A;
        g_read_data[i] = RESET_VALUE;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is callback for FreeRTOS+FAT and triggered when USB Pen drive is removed or inserted.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void free_rtos_fat_callback(rm_freertos_plus_fat_callback_args_t *p_args)
{
    if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_INSERTED)
    {
        g_rm_freertos_plus_fat_insertion_events = true;
    }
    if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED)
    {
        g_rm_freertos_plus_fat_insertion_events = false;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function initializes the FreeRTOS+FAT instance.
 * @param[IN]   None
 * @retval      Any other error code apart from FSP_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t usb_init(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;
    int32_t file_error = SUCCESS;
    rm_freertos_plus_fat_device_t device;

    memset(&device, RESET_VALUE, sizeof(device));

    /* Open FreeRTOS plus FAT */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Open(&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_cfg);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_Open API failed\r\n");
        return freertos_fat_error;
    }

    APP_PRINT("\r\n\nRM_FREERTOS_PLUS_FAT_Open successful\r\n");
    /* Wait for USB device connection */
    APP_PRINT("Connect USB device...\r\n");

    /* Wait until USB device is connected */
    while (true != check_usb_connection());

    /* Initialize the mass storage device. This should not be done until the device is plugged in and initialized */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_MediaInit(&g_rm_freertos_plus_fat_ctrl, &device);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_MediaInit API failed\r\n");
        fat_clean_up();
        return freertos_fat_error;
    }

    /* Initialize one disk for each partition used in the application */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskInit(&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_disk_cfg,\
                                                       &g_my_disk);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_DiskInit API failed\r\n");
        fat_clean_up();
        return freertos_fat_error;
    }

    /* Mount each disk. This assumes the disk is already partitioned and formatted */
    FF_Error_t ff_err = FF_Mount(&g_my_disk, g_my_disk.xStatus.bPartitionNumber);
    if (FSP_SUCCESS != ff_err)
    {
        APP_ERR_PRINT("\r\nFF_Mount API failed\r\n");
        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        fat_clean_up();
        /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
        APP_ERR_TRAP(ff_err);
    }

    /* Add the disk to the file system */
    file_error = FF_FS_Add("/", &g_my_disk);
    if (SUCCESS == file_error)
    {
        APP_ERR_PRINT("\r\nFF_FS_Add API failed\r\n");
        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        fat_clean_up();
        /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
        APP_ERR_TRAP(file_error);
    }

    /* Set this flag to let application know that USB is initialized */
    b_usb_hmsc_close = false;
    return freertos_fat_error;
}

/*******************************************************************************************************************//**
 * @brief       This function closes the FreeRTOS+FAT instance.
 * @param[IN]   None
 * @retval      Any other error code apart from FSP_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
void fat_clean_up(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;

    /* Close the FREERTOS_PLUS_FAT_Close instance on any failure */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Close(&g_rm_freertos_plus_fat_ctrl);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_Close API failed\r\n");
    }
    else
    {
        APP_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_Close API successfully\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_hmsc_ep)
 **********************************************************************************************************************/

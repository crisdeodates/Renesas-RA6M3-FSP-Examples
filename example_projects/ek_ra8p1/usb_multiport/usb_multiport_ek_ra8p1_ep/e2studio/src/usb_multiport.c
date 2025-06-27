/***********************************************************************************************************************
 * File Name    : usb_multiport.c
 * Description  : Contains multiple functions definitions used in the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "usb_multiport.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_multiport_ep
 * @{
 **********************************************************************************************************************/

/* Global variables */
uint8_t g_write_data[WRITE_ITEM_SIZE] = {RESET_VALUE};   /* Data to write to file */
uint8_t g_read_data[WRITE_ITEM_SIZE]  = {RESET_VALUE};   /* Variable to store the data read from file */
bool g_write_to_usb = false;                             /* Flag to check write status */

extern uint16_t g_bytes_to_write;
FF_Disk_t my_disk;

/*******************************************************************************************************************//**
 * @brief       This function initializes the FreeRTOS+FAT instance.
 * @param[IN]   None
 * @retval      Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t file_system_and_usb_init(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;
    int32_t file_error = SUCCESS;
    rm_freertos_plus_fat_device_t device;
    memset (&device, RESET_VALUE, sizeof(device));

    /* Open FreeRTOS PLUS FAT */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Open (&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_cfg);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nFREERTOS PLUS FAT OPEN API failed.\r\n");
        return freertos_fat_error;
    }

    APP_PRINT("\r\nFreeRTOS+FAT Open successful.\r\n");
    /* Wait for USB Device connection */
    APP_PRINT("Connect USB Device...\r\n");

    /* Wait until USB Device is connected. This is the blocking call */
    while (true != wait_till_usb_detected ());

    APP_PRINT("USB Device Detected...\r\n");

    /* Initialize the mass storage device. This should not be done until the device is plugged in and initialized */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_MediaInit (&g_rm_freertos_plus_fat_ctrl, &device);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_MediaInit API failed.\r\n");
        clean_up ();
        return freertos_fat_error;
    }

    /* Initialize one disk for each partition used in the application */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskInit (&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_disk_cfg,
                                                        &my_disk);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_DiskInit API failed.\r\n");
        clean_up ();
        return freertos_fat_error;
    }

    /* Mount each disk. This assumes the disk is already partitioned and formatted */
    FF_Error_t ff_err = FF_Mount (&my_disk, my_disk.xStatus.bPartitionNumber);
    if (FSP_SUCCESS != ff_err)
    {
        APP_ERR_PRINT("\r\nFF_Mount API failed.\r\n");
        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        clean_up ();
        /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
        APP_ERR_TRAP(ff_err);
    }

    /* Add the disk to the file system */
    file_error = FF_FS_Add ("/", &my_disk);
    if (SUCCESS == file_error)
    {
        APP_ERR_PRINT("\r\nFF_Mount API failed.\r\n");
        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        clean_up ();
        /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
        APP_ERR_TRAP(file_error);
    }
    return freertos_fat_error;
}

/*******************************************************************************************************************//**
 * @brief       This function initiates the USB operation by calling respective functions.
 * @param[IN]   Converted RTT input
 * @retval      None
 **********************************************************************************************************************/
void process_usb_operation(uint8_t input_buffer)
{
    switch (input_buffer)
    {
        case USB_READ:
        {
            /* Read operation from predefined file */
            usb_read_operation ();
            APP_PRINT (MENU);
            break;
        }

        case USB_WRITE:
        {
            /* Set the flag for write operation */
            g_write_to_usb = true;
            APP_PRINT("Goto Tera Term and write data on file. Press Enter to complete input (up to 512 bytes)\r\n"
                      "and initiate writing the file to the Mass Storage Device titled %s\r\n", FILE_NAME);
            break;
        }

        default:
        {
            APP_PRINT ("\r\nInvalid input. Provide a valid input.\r\n");
            APP_PRINT (MENU);
            break;
        }
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

    /* Resetting the variables */
    memset(&file_details, RESET_VALUE, sizeof(file_details));
    memset(g_read_data, RESET_VALUE, sizeof(g_read_data));

    /* Double check the connection again to ensure the USB device is still mounted */
    if (true == check_usb_connected())
    {
        /* Open the file read mode to read data written previously */
        file_pointer = ff_fopen ((const char *) FILE_NAME, READ_MODE);
        if (NULL != file_pointer)
        {
            APP_PRINT("USB read operation will be initiated.\n");

            /* Capture the number of bytes read in the variable to check read status */
            size_t bytes_read = RESET_VALUE;
            size_t bytes_read_total = RESET_VALUE;

            while(!ff_feof(file_pointer))
            {
                /* Read data from file */
                bytes_read = ff_fread (g_read_data , sizeof(g_read_data[RESET_VALUE]) , WRITE_ITEM_SIZE , file_pointer);
                if (READ_WRITE_FAILURE == bytes_read)
                {
                    APP_ERR_PRINT ("ff_fread API failed. Closing opened file\r\n", stdioGET_ERRNO());
                    file_error = ff_fclose (file_pointer);
                    if(SUCCESS != file_error )
                    {
                        APP_ERR_PRINT("ff_fclose API failed.");
                        /* Adding extra %d as parses string and prints %d as-is */
                        APP_PRINT(" %d\r\n",  stdioGET_ERRNO());
                    }
                    return;
                }

                /* Send event received to queue */
                if (pdTRUE == (xQueueSend(g_queue_data_from_hmsc, g_read_data, (TickType_t)(RESET_VALUE))))
                {
                    bytes_read_total = bytes_read_total+bytes_read;

                }
            }
            /* Close the file after read */
            file_error = ff_fclose (file_pointer);
            if(SUCCESS != file_error)
            {
                APP_ERR_PRINT("ff_fclose API failed.");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT(" %d\r\n",  stdioGET_ERRNO());
                return;
            }

            file_error = ff_stat ((const char*)FILE_NAME , &file_details);
            /* ff_stat returns 0 on success and -1 on error */
            if (SUCCESS == file_error)
            {
                /* Compare the actual bytes written and file size after write operation */
                if (bytes_read_total == file_details.st_size)
                {
                    APP_PRINT("%d bytes Data successfully read from file  %s\n", bytes_read_total, FILE_NAME);
                    APP_PRINT("Read operation is Successful. \n");
                }
                else
                {
                    APP_ERR_PRINT("ff_write API failed.");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT(" %d\r\n",  stdioGET_ERRNO());
                    return;
                }
            }
            else
            {
                APP_ERR_PRINT ("ff_stat API failed.");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT(" %d\r\n",  stdioGET_ERRNO());
                return;
            }
        }
        else
        {
            APP_ERR_PRINT ("ff_fopen API failed.");
            /* Adding extra %d as parses string and prints %d as-is */
            APP_PRINT(" %d\r\n",  stdioGET_ERRNO());
            return;
        }
    }
    else
    {
        APP_PRINT ("USB not detected.\n");
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
    if (true == check_usb_connected())
    {
        /* Once connection is detected open file is write mode */
        file_pointer = ff_fopen ((const char *) FILE_NAME, WRITE_MODE);
        if (NULL != file_pointer)
        {
            APP_PRINT("USB write operation will be initiated.\n");

            /* Write data to file  */
            bytes_written = ff_fwrite (g_write_data , sizeof(g_write_data[RESET_VALUE]) ,
                                       g_bytes_to_write , file_pointer);
            if (RESET_VALUE != bytes_written)
            {
                g_bytes_to_write = RESET_VALUE;
                APP_PRINT("Data is successfully written.\r\n");
            }
            else
            {
                APP_ERR_PRINT ("ff_fwrite API failed. Closing opened file.\r\n");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT("%d\r\n", stdioGET_ERRNO());

                file_error = ff_fclose (file_pointer);
                if (SUCCESS != file_error)
                {
                    APP_ERR_PRINT("ff_fclose API failed.");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT("%d\r\n", stdioGET_ERRNO());
                }
                /* Clear the flag */
                g_write_to_usb = false;
                return;
            }
            /* Close the file after write operation and open again in read mode */
            file_error = ff_fclose (file_pointer);
            if (SUCCESS != file_error)
            {
                APP_ERR_PRINT("ff_fclose API failed.");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT("%d\r\n", stdioGET_ERRNO());
                return;
            }
        }
        else
        {
            APP_ERR_PRINT ("ff_fopen API failed.");
            /* Adding extra %d as parses string and prints %d as-is */
            APP_PRINT("%d\r\n", stdioGET_ERRNO());
            return;
        }
    }
    else
    {
        APP_PRINT ("USB not detected.\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function checks status and waits till USB is detected.
 * @param[IN]   None
 * @retval      true if connection detected; false if connection is not detected.
 **********************************************************************************************************************/
bool wait_till_usb_detected(void)
{
    EventBits_t uxBits;
    bool status = false;
    uxBits = xEventGroupWaitBits (g_event_group, MEDIA_INSERTED | MEDIA_REMOVED, pdFALSE, pdTRUE,
                                  (TickType_t ) WAIT_TIME / portTICK_PERIOD_MS);
    if (uxBits & MEDIA_INSERTED)
    {
        status = true;
    }
    else if (uxBits & MEDIA_REMOVED)
    {
        status = false;
    }
    else
    {
        /* None */
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief       This function checks the USB HMSC connection status.
 * @param[IN]   None
 * @retval      true if connection detected; false if connection is not detected.
 **********************************************************************************************************************/
bool check_usb_connected(void)
{
    EventBits_t uxBits;
    bool status = false;
    uxBits = xEventGroupGetBits(g_event_group);
    if(uxBits & MEDIA_INSERTED)
    {
        status = true;
    }
    else if(uxBits & MEDIA_REMOVED)
    {
        status = false;
    }
    else
    {
        /* None */
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief       This function is callback for FreeRTOS+FAT and triggered when USB Pen drive is removed or inserted.
 * @param[IN]   pointer to p_args
 * @retval      None.
 **********************************************************************************************************************/
void free_rtos_fat_callback(rm_freertos_plus_fat_callback_args_t *p_args)
{
    xEventGroupSetBits (g_event_group, p_args->event);
}

/*******************************************************************************************************************//**
 * @brief       This function closes the FreeRTOS+FAT instance.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void clean_up(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;

    /* Close the FREERTOS_PLUS_FAT_Close instance on any failure */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Close (&g_rm_freertos_plus_fat_ctrl);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_PRINT ("\r\nRM_FREERTOS_PLUS_FAT_Close API failed.\r\n");
    }
    else
    {
        APP_PRINT ("\r\nFREERTOS PLUS FAT instance closed successfully.\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_multiport_ep)
 **********************************************************************************************************************/

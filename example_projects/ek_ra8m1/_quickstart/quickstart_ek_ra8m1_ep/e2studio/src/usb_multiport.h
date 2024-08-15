/***********************************************************************************************************************
 * File Name    : usb_multiport.h
 * Description  : Contains macros, data structures and function declaration used in EP
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_MULTIPORT_H_
#define USB_MULTIPORT_H_

#define MEDIA_INSERTED          (1U << 1)         /* USB media inserted state */
#define MEDIA_REMOVED           (1U << 0)         /* USB media removed state */
#define SUCCESS                 (0)               /* Success case */
#define USB_READ                (1u)              /* RTT Input selection for read operation */
#define USB_WRITE               (2u)              /* RTT Input selection for Re-initialization of USB */
#define WRITE_MODE              "w"               /* File Write mode */
#define READ_MODE               "r"               /* File Read mode */
#define READ_FILE_NAME          "_source.txt"     /* Filename for read ('_' at start of filename important)    */
#define WRITE_FILE_NAME         "_copy1.txt"      /* Filename for write ('_' at start of filename important)   */
#define ENCRYPT_FILE_NAME       "_encrypt.txt"    /* Filename for encrypt ('_' at start of filename important) */
#define DECRYPT_FILE_NAME       "_decrypt.txt"    /* Filename for decrypt ('_' at start of filename important) */
#define FILE_NAME               "_test.txt"        /* File name for write/read */
#define WRITE_ITEM_SIZE         (1023u)           /* The number of elements to read */
#define READ_WRITE_FAILURE      (0)               /* Fail case of read write */
#define WAIT_TIME               (4000)            /* Wait time for USB detection */
#define CARRIAGE_RETURN         ('\r')            /* Carriage return */
#define SIZE                    (1U)              /* Size to read byte by byte */

#define LINE_CODING_LENGTH      (0x07U)          /* Line coding length */
#define NUM_STRING_DESCRIPTOR   (7U)             /* String descriptor */

#define USB_IO_BUFFER           (WRITE_ITEM_SIZE + 18)

extern FF_Disk_t g_my_disk;

/* function declaration */

/* Function Name: file_system_and_usb_init */
/******************************************************************************************************************//**
 * @brief       This function Initializes the FreeRTOS+FAT instance.
 * @param[IN]   None
 * @retval      Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 *********************************************************************************************************************/
fsp_err_t file_system_and_usb_init (void);        /* Initialize USB HMSC device */

/* Function Name: wait_till_usb_detected */
/******************************************************************************************************************//**
 * @brief       This function checks status and waits till USB is detected.
 * @param[IN]   None
 * @retval      true if connection detected; false if connection is not detected.
 *********************************************************************************************************************/
bool wait_till_usb_detected (void);               /* Check and wait for usb detection */

/* Function Name: check_usb_connected */
/******************************************************************************************************************//**
 * @brief       This function checks the USB HMSC connection status.
 * @param[IN]   None
 * @retval      true if connection detected; false if connection is not detected.
 *********************************************************************************************************************/
bool check_usb_connected (void);                  /* Check USB connection status */

/* Function Name: usb_read_operation */
/******************************************************************************************************************//**
 * @brief       This function performs USB HMSC read operation.
 * @param[IN]   None
 * @retval      Error code passed up from lower layer
 *********************************************************************************************************************/
int32_t usb_read_operation (void);                /* Performs USB read operation */

/* Function Name: usb_write_operation */
/******************************************************************************************************************//**
 * @brief       This function performs USB HMSC write operation.
 * @param[IN]   None
 * @retval      None
 *********************************************************************************************************************/
void usb_write_operation (void);                  /* Performs USB write operation */

/* Function Name: clean_up */
/******************************************************************************************************************//**
 * @brief       This function closes the FreeRTOS+FAT instance..
 * @param[IN]   None
 * @retval      None
 *********************************************************************************************************************/
void clean_up (void);                             /* Clean up freertos plus fat */

#endif /* USB_MULTIPORT_H_ */

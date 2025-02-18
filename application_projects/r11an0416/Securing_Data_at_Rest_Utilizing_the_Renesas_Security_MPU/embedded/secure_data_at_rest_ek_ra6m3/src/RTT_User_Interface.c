/***********************************************************************************************************************
 * File Name    : RTT_User_Interface.c
 * Description  : Contains RTT Viewer input and output control
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "RTT_User_Interface.h"
#include "common_utils.h"
#include "menu_callbacks.h"

/* global definition */
void start_RTT_user_interface();

/*******************************************************************************************************************//**
 * @brief start_RTT_user_interface function
 * This function takes user input from the RTT Viewer and print the test result to the RTT Viewer
 *
 * return: void
 **********************************************************************************************************************/

void  start_RTT_user_interface()
{
	APP_PRINT("**************************\n\r");
	APP_PRINT("Choose an option from the below menu");
	APP_PRINT("\r\n Press 1 to view current security settings \r\n Press 2 for secure flash operation \r\n Press 3 for secure sram operation \r\n Press 4 for non-secure flash operation \r\n Press ~ for EXIT\r\n");
	APP_PRINT("**************************\n\r");

	unsigned char rByte[BUFF_SIZE] = { RESET_VALUE };
	while (EXIT != rByte[BUFF_INDEX])
	{
		if (APP_CHECK_DATA)
		{
			APP_READ(rByte);

			switch (rByte[BUFF_INDEX])
			{
			 case VIEW_SECURITY_SETTING:
				 read_secure_settings();
				 break;

			 case SECURE_FLASH_OPERATION:
				 APP_PRINT("**************************\n\r");
				 APP_PRINT("Choose an option from the below menu for secure flash operation");
				 APP_PRINT("\r\n Press a to read \r\n Press b to write \r\n Press c to setup faw \r\n Press d to reset faw \r\n");
				 APP_PRINT("**************************\n\r");
				 break;

			 case SECURE_FLASH_READ:
				 secure_code_read();
				 break;

			 case SECURE_FLASH_WRITE:
				 secure_code_write();
				 break;

			 case SET_UP_FAW:
				 setup_faw();
				 break;

			 case RESET_FAW:
				 reset_faw();
				 break;

			 case SECURE_SRAM_OPERATION:
				 APP_PRINT("**************************\n\r");
				 APP_PRINT("Choose an option from the below menu for secure sram operation");
				 APP_PRINT("\r\n Press g to read \r\n Press h to write\r\n");
				 APP_PRINT("**************************\n\r");
				 break;

			 case SECURE_SRAM_READ:
				 secure_sram_code_read();
				 break;

			 case SECURE_SRAM_WRITE:
				 secure_sram_code_write();
				 break;

			 case NON_SECURE_FLASH_OPERATION:
				 APP_PRINT("**************************\n\r");
				 APP_PRINT("Choose an option from the below menu for non_secure flash operation");
				 APP_PRINT("\r\n Press m to read \r\n Press n to write\r\n");
				 APP_PRINT("**************************\n\r");
				 break;

			 case NON_SECURE_FLASH_READ:
				 non_secure_code_read();
				 break;

			 case NON_SECURE_FLASH_WRITE:
				 non_secure_code_write();
				 break;

			 case EXIT:
				 APP_PRINT("\n\rExiting the secure data at rest.");
				 break;

			 default:
				 APP_PRINT("**************************\n\r");
				 APP_PRINT("Choose an option from the below menu");
				 APP_PRINT("\r\n Press 1 to view current security settings \r\n Press 2 for secure flash operation \r\n Press 3 for secure sram operation \r\n Press 4 for non-secure flash operation \r\n Press ~ for EXIT\r\n");
				 APP_PRINT("**************************\n\r");
				 break;
			}
			if ((VIEW_SECURITY_SETTING <= rByte[BUFF_INDEX]) && (EXIT > rByte[BUFF_INDEX]))
			{
			 APP_PRINT("Enter r to go back to the main menu\r\n\r\n");
			}
		}
	}

}

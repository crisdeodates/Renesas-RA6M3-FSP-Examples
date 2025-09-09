/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "hal_data.h"
#include "display_thread.h"
#include "common_util.h"
#include "r_display_api.h"
#include "display_layer.h"
#include <stdio.h>
#include "time_counter.h"
#include "console_output.h"
#include "camera_control.h"


#if 0
#define X1_CO_ORDINATE      0U
#define Y1_CO_ORDINATE      0U
#define COLOR_BAND_COUNT    8U
#define INC_DEC_VALUE       1
#endif
/******************************************************************************
 * Static global variables
 *****************************************************************************/



void glcdc_vsync_isr(display_callback_args_t *p_args);

extern void  do_image_classification_screen(bool ai_result_new);


void console_output_processing_time(void)
{
    sprintf (sprintf_buffer, "\r\nProcessing time:\r\n");
    print_to_console(sprintf_buffer);

    sprintf (sprintf_buffer, "  Camera image capture vsync period : %4d ms, %4d fps\r\n",
            application_processing_time.camera_image_capture_time_ms, TimeCounter_ConvertFromMsToFps(application_processing_time.camera_image_capture_time_ms));
    print_to_console(sprintf_buffer);

    sprintf (sprintf_buffer, "  AI inference pre processing time  : %4d ms, %4d fps\r\n",
            application_processing_time.ai_inference_pre_processing_time_ms, TimeCounter_ConvertFromMsToFps(application_processing_time.ai_inference_pre_processing_time_ms));
    print_to_console(sprintf_buffer);
    sprintf (sprintf_buffer, "  AI inference time                 : %4d ms, %4d fps\r\n",
            application_processing_time.ai_inference_time_ms, TimeCounter_ConvertFromMsToFps(application_processing_time.ai_inference_time_ms));
    print_to_console(sprintf_buffer);
    sprintf (sprintf_buffer, "  LCD display vsync period          : %4d ms, %4d fps\r\n",
            application_processing_time.lcd_display_update_refresh_ms, TimeCounter_ConvertFromMsToFps(application_processing_time.lcd_display_update_refresh_ms));
    print_to_console(sprintf_buffer);
}

/* Display Thread entry function */
/* pvParameters contains TaskHandle_t */
void display_thread_entry(void *pvParameters)
{

    FSP_PARAMETER_NOT_USED (pvParameters);

    console_output_init();
    bool ai_result_updated = false;
    drw_init();

    memset(vin_image_buffer_1, 0x55, sizeof(vin_image_buffer_1));
    memset(vin_image_buffer_2, 0x55, sizeof(vin_image_buffer_2));
    memset(vin_image_buffer_3, 0x55, sizeof(vin_image_buffer_3));

    initialise_display();


  
    // Set display initialization complete flag
    xEventGroupSetBits(g_ai_app_event, HARDWARE_DISPLAY_INIT_DONE);

    while(true)
    {

        /* Wait for vertical blanking period */
        xEventGroupWaitBits(g_ai_app_event, GLCDC_VSYNC, pdFALSE, pdTRUE, 1);
        {
            static bool wait_for_first_time = true;
           
            if (ai_result_updated)
            {
                wait_for_first_time = false;
            }
            if(wait_for_first_time == false)
            {
                
                R_GLCDC_BufferChange(&g_plcd_display_ctrl, (uint8_t * const) gp_next_buffer, DISPLAY_FRAME_LAYER_1);
                do_image_classification_screen(ai_result_updated);
                console_output_processing_time();
            }
            ai_result_updated = ((xEventGroupGetBits(g_ai_app_event) & AI_INFERENCE_RESULT_UPDATED) != 0) ? true : false;
            xEventGroupClearBits(g_ai_app_event, AI_INFERENCE_RESULT_UPDATED);
        }
        vTaskDelay(150);
    }


}




/**********************************************************************************************************************
 End of function glcdc_vsync_isr
 *********************************************************************************************************************/

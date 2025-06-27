/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : common_init.h
 * Version      : 1.0
 * Description  : Common initialization functions.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include "event_groups.h"
#include "portable.h"

#include "hal_data.h"
#include "r_typedefs.h"
#include "board_cfg.h"
#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#ifndef COMMON_INIT_H_
#define COMMON_INIT_H_

/* Use following macro to enable additional feedback or logging information in serial console */
#define R_RELEASE_MODE  (1)

#define SHORT_NAME                      ("RA8P1")
#define FULL_NAME                       ("EK-RA8P1")
#define PART_NUMBER                     ("RTK7EKA8P1S01001BE")
#define DEVICE_NUMBER                   ("R7KA8P1KFLCAC")

#define KIT_NAME                        (FULL_NAME)
#define EP_VERSION                      ("1.00")

#define LED_INTENSITY_10                (90)            /* 90 percent off */
#define LED_INTENSITY_50                (50)            /* 50 percent off */
#define LED_INTENSITY_90                (10)            /* 10 percent off */

#define PMOD_DISPLY_DELAY_SETTING_CFG   (1000)

#define BLINK_FREQ_1HZ                  (60000000)              /* 1Hz */
#define BLINK_FREQ_5HZ                  (BLINK_FREQ_1HZ / 5)    /* 5Hz */
#define BLINK_FREQ_10HZ                 (BLINK_FREQ_1HZ / 10)   /* 10Hz */

#define ADC_CALIBRATION_ERROR           (300)
#define ADC_CALIBRATION_NA              (65535)

#define NUM_STRING_DESCRIPTOR           (7U)

/* Available option for LCD screens in the demo  */
#define LCD_FULL_BG_DEFAULT             (99)
#define LCD_FULL_BG_POPUP_MENU          (42)
#define LCD_FULL_BG_GETTING_STARTED     (9)
#define LCD_FULL_BG_KIT_INFORMATION     (1)
#define LCD_FULL_BG_USER_LED            (2)
#define LCD_FULL_BG_EXTERNAL_MEMORY     (3)
#define LCD_FULL_BG_CAMERA_BACKGROUND   (4)
#define LCD_FULL_BG_NEXT_STEPS          (5)
#define LCD_FULL_BG_WELCOME             (127)

/* OSPI Data Transfer Mode */
#define DTR_MODE                (1)


/* g_update_console_event */
#define STATUS_DISPLAY_MENU_KIS     (1 << 0)    /* Update Kit Information Screen EVENT */
#define STATUS_UPDATE_KIS_INFO      (1 << 1)    /* Update Kit Information Screen data EVENT */
#define STATUS_UPDATE_TEMP_INFO     (1 << 2)    /* Update Kit Temperature EVENT */
#define STATUS_UPDATE_FREQ_INFO     (1 << 3)    /* Update Kit Blue LED Frequency EVENT */
#define STATUS_UPDATE_INTENSE_INFO  (1 << 4)    /* Update Kit Blue LED Intensity EVENT */
#define STATUS_WRITE_COMPLETE       (1 << 5)    /* Update USB Write EVENT */
#define STATUS_ENABLE_USB_MSC       (1 << 6)    /* USB MSC Detection is UP / Down */
#define STATUS_USB_READY            (1 << 7)    /* USB MSC Disk is Ready for Read/Write */
#define STATUS_USB_MSC_ERROR        (1 << 8)    /* USB MSC Generic Error ABORT */
#define STATUS_USB_REQUEST_READ     (1 << 9)    /* USB MSC Read request */
#define STATUS_USB_REQUEST_WRITE    (1 << 10)   /* USB MSC Write request */
#define STATUS_USB_REQUEST_RESET    (1 << 11)   /* USB MSC reset the driver */

#define MENU_RETURN_INFO  "\r\n\r\n> Press space bar to return to MENU.\r\n"

#define TMR_DEF_QSEP_BLINKY_BLUE    (0)
#define TMR_DEF_LED_UX_BLINK_RED    (1)
#define TMR_DEF_LED_UX_BLINK_GREEN  (2)
#define TMR_DEF_LED_UX_BLINK_BLUE   (3)

/* Unsigned equiv of -1 */
#define INVALID_CHARACTER        (0xFFFFFFFF)

/* Unsigned equiv of -2 */
#define INVALID_BLOCK_SIZE       (0xFFFFFFFE)

/* Unsigned equiv of -3 */
#define INVALID_BLOCK_BOUNDARY   (0xFFFFFFFD)

#define INVALID_MARKERS          (0xFFFFFFF0)

#define MAX_MENU_STR_LEN   (256)

#define MAX_OPTION_STR_LEN (32)

#ifndef APP_ERR_TRAP
#define APP_ERR_TRAP(a)             {if (a) {__asm("BKPT #0\n"); }} /* Trap the error location */
#endif

/* The ADC slope, for 12bit at a full scale of 3.3v
 * Cal is given for +127degC. So slope*127 (rounded to integer) is the zero calibration offset
 * Deg F conversion used: (C * (9/5)) +32
 * (Don't use the R_ADC_InfoGet .slope_microvolts, as this is inaccurate according to data supplied.
 *  Use slope=4.1mV/DegC)
 */
#define TSN_ADC_COVERSION_SLOPE_COUNTS_PER_DEG_C            (5.08896f)
#define TSN_CAL_OFFEST_COUNTS_AT_127DEG_TO_0DEG_C           (646)

#define TSN_ADC_COVERSION_SLOPE_COUNTS_PER_DEG_F            (2.8272f)
#define TSN_CAL_OFFEST_COUNTS_AT_260_6DEG_TO_0DEG_F         (737)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
typedef struct menu_fn_tbl
{
    char_t * p_name;                      /*<! Name of Test */
    test_fn ( * p_func)(void);            /*<! Pointer to Test Function */
} st_menu_fn_tbl_t;

typedef struct ai_detection_point_t
{
    signed short      m_x;
    signed short      m_y;
    signed short      m_w;
    signed short      m_h;
} st_ai_detection_point_t;

typedef struct ai_classification_point_t
{
    unsigned short    category;
    float             prob;
} st_ai_classification_point_t;

typedef struct
{
    uint16_t whole_number;                      // integer part of temperature
    uint16_t mantissa;                          // decimal part of temperature
} st_temp_expression_t;

typedef struct
{
    uint16_t             adc_temperature_data;  // temperature (un-calibrated data)
    st_temp_expression_t temperature_f;         // temperature (fahrenheit)
    st_temp_expression_t temperature_c;         // temperature (celsius)
    float32_t            temperature_f_as_f;    // temperature (fahrenheit) as float
    float32_t            temperature_c_as_f;    // temperature (celsius) as float
    uint16_t             led_intensity;         // PWM pulse width
    uint16_t             led_frequency;         // PWM pulse frequency
} st_board_status_t;

typedef enum e_display_status
{
    DISP_CONNECTION_NONE = 0,
    DISP_CONNECTION_PLCD,
    DISP_CONNECTION_MIPI,
} e_display_status_t;

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/
extern const char_t * const gp_cursor_store;
extern const char_t * const gp_cursor_restore;
extern const char_t * const gp_cursor_temp;
extern const char_t * const gp_cursor_frequency;
extern const char_t * const gp_cursor_intensity;

extern const char_t * const gp_red_fg;
extern const char_t * const gp_orange_fg;
extern const char_t * const gp_green_fg;
extern const char_t * const gp_white_fg;

extern const char_t * const gp_clear_screen;
extern const char_t * const gp_cursor_home;

extern const char_t * const gp_hide_cursor;
extern const char_t * const gp_show_cursor;

extern uint8_t g_pwm_dcs_data[];
extern uint8_t g_pwm_rates_data[];

extern uint32_t g_pwm_dcs[3];
extern uint32_t g_pwm_rates[3];


extern bool_t g_sw1_updated;
extern bool_t g_sw2_updated;

extern adc_info_t g_adc_info_rtn;
extern bool_t g_overlay_selected;
extern int8_t g_selected_menu;
extern int8_t g_next_menu_selection;

extern st_board_status_t g_board_status;

extern fsp_err_t common_init (void);

extern void led_duty_cycle_update ();

extern int8_t g_selected_menu;
extern bool_t g_show_getting_started_page;

extern fsp_err_t print_to_console (char_t * p_data);

extern int8_t input_from_console (void);
extern void input_from_any_console (int8_t *c);

extern int8_t* input_from_console_multi (void);

extern bool_t system_up(void);

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/

/* Static backgrounds  in ospi memory byte reversed */
extern const st_full_image_rgb565_t g_ref_000_welcome_screen;
extern const st_full_image_rgb565_t g_ref_001_user_led_background;

/* In main read only memory */
extern const st_overlay_image_rgb565_t g_menu_overlay_image;

#endif /* COMMON_INIT_H_ */

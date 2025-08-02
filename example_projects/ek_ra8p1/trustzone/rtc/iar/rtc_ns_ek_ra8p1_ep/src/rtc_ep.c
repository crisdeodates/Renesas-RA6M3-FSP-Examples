/***********************************************************************************************************************
 * File Name    : rtc_ep.c
 * Description  : Contains data structures and functions used in rtc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "rtc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup rtc_ep
 * @{
 **********************************************************************************************************************/

/* Variable to store time */
static rtc_time_t g_set_time =
{
 .tm_hour    =  RESET_VALUE,
 .tm_isdst   =  RESET_VALUE,
 .tm_mday    =  RESET_VALUE,
 .tm_min     =  RESET_VALUE,
 .tm_mon     =  RESET_VALUE,
 .tm_sec     =  RESET_VALUE,
 .tm_wday    =  RESET_VALUE,
 .tm_yday    =  RESET_VALUE,
 .tm_year    =  RESET_VALUE,
};
static rtc_time_t g_present_time =
{
 .tm_hour    =  RESET_VALUE,
 .tm_isdst   =  RESET_VALUE,
 .tm_mday    =  RESET_VALUE,
 .tm_min     =  RESET_VALUE,
 .tm_mon     =  RESET_VALUE,
 .tm_sec     =  RESET_VALUE,
 .tm_wday    =  RESET_VALUE,
 .tm_yday    =  RESET_VALUE,
 .tm_year    =  RESET_VALUE,
};
/* Flag to be set on successful RTC Calendar setting */
static uint32_t set_time_flag = RESET_FLAG;

static void rtc_date_time_conversion(rtc_time_t * time, unsigned char read_buffer[]);

extern volatile uint32_t g_alarm_irq_flag;
extern volatile uint32_t g_periodic_irq_flag;
extern volatile uint32_t g_invalid_sequence;

/*******************************************************************************************************************//**
 * @brief       This function initializes RTC module.
 * @param[IN]   None.
 * @retval      FSP_SUCCESS     Upon successful open of RTC module.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful open.
 **********************************************************************************************************************/
fsp_err_t rtc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    /* Open RTC module */
    err = g_rtc_open_guard(NULL, NULL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nRTC module open failed.\r\nRestart the application\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions sets the Calendar time provided by the user.
 * @param[IN]   None.
 * @retval      FSP_SUCCESS     Upon successful Calendar time set.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful calendar time set.
 **********************************************************************************************************************/
fsp_err_t set_rtc_calendar_time(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */
    unsigned char read_time[BUFFER_SIZE_DOWN] = {NULL_CHAR};

    APP_PRINT("\r\nSetting RTC Date and Time\r\n");

    APP_PRINT("\r\nEnter Date and Time in DD:MM:YYYY HH:MM:SS format.\nSample Input: '04:11:2019 15:23:05'\r\n");

    /* Wait until data is available in RTT */
    while(BYTES_RECEIVED_ZERO == APP_CHECK_DATA);

    /* Reading Calendar time provided by the user */
    APP_READ(read_time);

    /* Modify the user provided date and time to standard format */
    rtc_date_time_conversion(&g_set_time, &read_time[0]);

    /* Set the time provided by the user */
    err = g_rtc_calendar_time_set_guard(NULL, &g_set_time);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCalendar time set failed.\r\n");
        return err;
    }

    /* Get the current Calendar time */
    err = g_rtc_calendar_time_get_guard(NULL, &g_present_time);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCalendar time get failed.\r\n");
        return err;
    }
    /* Modify the date in standard format to the user readable format */
    rtc_date_readability_update(&g_present_time);

    APP_PRINT("\r\nRTC calendar set to Date: %d/%d/%d\r\nTime: %d:%d:%d\r\n\n", g_present_time.tm_mday,
              g_present_time.tm_mon, g_present_time.tm_year, g_present_time.tm_hour,
              g_present_time.tm_min, g_present_time.tm_sec);
    set_time_flag = SET_FLAG;
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function sets the alarm time provided by the user.
 * @param[IN]   None.
 * @retval      FSP_SUCCESS     Upon successful Calendar alarm set.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful calendar alarm set.
 **********************************************************************************************************************/
fsp_err_t set_rtc_calendar_alarm(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */
    rtc_alarm_time_t alarm_time_set =
    {
     .sec_match        =  RESET_VALUE,
     .min_match        =  RESET_VALUE,
     .hour_match       =  RESET_VALUE,
     .mday_match       =  RESET_VALUE,
     .mon_match        =  RESET_VALUE,
     .year_match       =  RESET_VALUE,
     .dayofweek_match  =  RESET_VALUE,
    };
    rtc_alarm_time_t alarm_time_get =
    {
     .sec_match        =  RESET_VALUE,
     .min_match        =  RESET_VALUE,
     .hour_match       =  RESET_VALUE,
     .mday_match       =  RESET_VALUE,
     .mon_match        =  RESET_VALUE,
     .year_match       =  RESET_VALUE,
     .dayofweek_match  =  RESET_VALUE,
    };
    unsigned char read_alarm[BUFFER_SIZE_DOWN] = {NULL_CHAR};

    /* Set alarm only if proper RTC Calendar date and Time is set */
    if (SET_FLAG == set_time_flag)
    {
        APP_PRINT("\r\nSetting Calendar Alarm for RTC\r\n");

        /* Set the flags for which the alarm has to be generated */
        alarm_time_set.hour_match  = false;
        alarm_time_set.min_match   = false;
        alarm_time_set.sec_match   = true;
        alarm_time_set.mday_match  = false;
        alarm_time_set.mon_match   = false;
        alarm_time_set.year_match  = false;

        APP_PRINT("\r\nEnter seconds value (Format: SS) for which recursive alarm has to be set."\
                  "\r\nPlease input seconds value in the range between 00 and 59."\
                  "\r\nSample input: '35'. At HH:MM:35, alarm occurred, and LED will be turned ON.\r\n");

        /* Wait until data is available in RTT */
        while(BYTES_RECEIVED_ZERO == APP_CHECK_DATA);

        /* Reading time value for which alarm has to be generated */
        APP_READ(read_alarm);

        /* Modify the user provided time value to standard format */
        alarm_time_set.time.tm_sec = (((read_alarm[0] - ASCII_ZERO) * PLACE_VALUE_TEN )+ (read_alarm[1] - ASCII_ZERO));

        /* Set the alarm time provided by the user */
        err = g_rtc_calendar_alarm_set_guard(NULL, &alarm_time_set);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nCalendar alarm set failed.\r\n");
            return err;
        }

        /* Get the alarm time */
        err = g_rtc_calendar_alarm_get_guard(NULL, &alarm_time_get);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nCalendar alarm get failed.\r\n");
            return err;
        }

        APP_PRINT("\r\nSeconds value for which alarm is set: %d", alarm_time_get.time.tm_sec);
    }
    else
    {
        /* Set this flag to notify the user to provide required options */
        g_invalid_sequence = SET_FLAG;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function sets the periodic interrupt rate provided by the user.
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful periodic interrupt rate set.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful periodic interrupt rate set.
 **********************************************************************************************************************/
fsp_err_t set_rtc_periodic_rate(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */
    APP_PRINT("\r\nSetting Periodic IRQ rate for RTC\r\n");

    /* Enable periodic interrupt rate */
    err = g_rtc_periodic_irq_rate_set_guard(NULL, RTC_PERIODIC_IRQ_SELECT_1_SECOND);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nPeriodic interrupt rate setting failed.\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function gets the current RTC time.
 * @param[IN]   None.
 * @retval      FSP_SUCCESS     Upon successful current RTC time get.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful current RTC time get.
 **********************************************************************************************************************/
fsp_err_t get_rtc_calendar_time(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    /* Get the current RTC Calendar time */
    err = g_rtc_calendar_time_get_guard(NULL, &g_present_time);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nGetting RTC Calendar time failed.\r\n");
        return err;
    }
    /* Modify the date in standard format to the user readable format */
    rtc_date_readability_update(&g_present_time);

    APP_PRINT("\r\nRTC Date: %d/%d/%d\r\nTime: %d:%d:%d\r\n\n", g_present_time.tm_mday,
              g_present_time.tm_mon , g_present_time.tm_year, g_present_time.tm_hour,
              g_present_time.tm_min, g_present_time.tm_sec);

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions modifies the date in readable format to the user.
 * @param[IN]   time        Date to be modified.
 * @retval      None.
 **********************************************************************************************************************/
void rtc_date_readability_update(rtc_time_t * time)
{
    time->tm_mon  +=  MON_ADJUST_VALUE;
    time->tm_year +=  YEAR_ADJUST_VALUE;
}

/*******************************************************************************************************************//**
 * @brief       This function modifies the user input to RTC date and time.
 * @param[IN]   time        Date and time to be modified.
 * @param[IN]   read_buffer Buffer that contains the user input time values.
 * @retval      None.
 **********************************************************************************************************************/
static void rtc_date_time_conversion(rtc_time_t * time, unsigned char read_buffer[])
{
    /* The user is expected to enter Date and Time in DD:MM:YYYY HH:MM:SS format. This the user input in the buffer is
     * processed using the following conversion, where ASCII value is converted to integer value and stored in the
     * RTC time structure. According to DD:MM:YYYY HH:MM:SS format, different array index values are used. */
    time->tm_mday = (((read_buffer[0] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[1] - ASCII_ZERO));
    time->tm_mon = ((((read_buffer[3] - ASCII_ZERO) * PLACE_VALUE_TEN)
                     + (read_buffer[4] - ASCII_ZERO))) - MON_ADJUST_VALUE;
    time->tm_year = (((read_buffer[6] - ASCII_ZERO) * PLACE_VALUE_THOUSAND) +
                     ((read_buffer[7] - ASCII_ZERO )* PLACE_VALUE_HUNDRED) +
                     ((read_buffer[8] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[9] - ASCII_ZERO))
                    - YEAR_ADJUST_VALUE;
    time->tm_hour = (((read_buffer[11] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[12] - ASCII_ZERO));
    time->tm_min = (((read_buffer[14] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[15] - ASCII_ZERO));
    time->tm_sec = (((read_buffer[17] - ASCII_ZERO) * PLACE_VALUE_TEN )+ (read_buffer[18] - ASCII_ZERO));
}

/*******************************************************************************************************************//**
 * @brief       RTC callback function.
 * @param[in]   p_args.
 * @retval      None.
 **********************************************************************************************************************/
void rtc_callback(rtc_callback_args_t *p_args)
{
    if(RTC_EVENT_ALARM_IRQ == p_args->event)
    {
        g_alarm_irq_flag = SET_FLAG;
    }
    else
    {
        g_periodic_irq_flag = SET_FLAG;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened RTC module before the project ends up in an error trap.
 * @param[IN]   None.
 * @retval      None.
 **********************************************************************************************************************/
void rtc_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close RTC module */
    err = g_rtc_close_guard(NULL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* RTC Close failure message */
        APP_ERR_PRINT("** RTC module close failed **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup rtc_ep)
 **********************************************************************************************************************/

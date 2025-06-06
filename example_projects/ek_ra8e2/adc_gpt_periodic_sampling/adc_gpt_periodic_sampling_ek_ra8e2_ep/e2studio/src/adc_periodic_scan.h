/***********************************************************************************************************************
 * File Name    : adc_periodic_scan.h
 * Description  : Contains data structures and functions used in adc_periodic_scan.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADC_PERIODIC_SCAN_H_
#define ADC_PERIODIC_SCAN_H_

typedef enum e_module_name
{
    ELC_MODULE,
    ELC_DTC1,
    ELC_DTC12,
    ELC_DTC_123,
    ELC_DTC_MODULE_ALL,
    ELC_DTC_ADC0_MODULE,
    ELC_DTC_ADC_MODULE_ALL,
    ELC_DTC_ADC_GPT0_MODULE,
    ELC_DTC_ADC_GPT1_MODULE,
    ALL
}module_name_t;

#define EP_INFO "\r\nThis Example Project demonstrates the basic functionality of ADC GPT Periodic Sampling \r\n"\
                "on Renesas RA MCUs based on FSP. On successful initialization of ADC, GPT, ELC and DTC modules,\r\n"\
                "GPT triggers an ADC group scan at periodic intervals. When each group scan completes,\r\n"\
                "DTC triggers data transfer and copies data to user buffer. The sample ADC data\r\n"\
                "can be viewed through waveform rendering of memory using memory viewer in e2studio.\r\n\n"\

#endif /* ADC_PERIODIC_SCAN_H_ */

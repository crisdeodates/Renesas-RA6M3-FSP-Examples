/***********************************************************************************************************************
 * File Name    : adc_ep.h
 * Description  : Contains data structures and functions used in adc_ep.c/.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADC_EP_H_
#define ADC_EP_H_

/*******************************************************************************************************************//**
 * @ingroup adc_ep
 * @{
 **********************************************************************************************************************/

/* Macros for commands to be received through RTT Viewer inputs */
#define SCAN_START                      (0x01)
#define SCAN_STOP                       (0x02)

/* Macro for default buff size for reading through RTT Viewer */
#define BUFF_SIZE                       (0x0F)

/* Macro for checking the deviation in ADC values */
#define TEMPERATURE_DEVIATION_LIMIT     (0x04)

/* Macro for indexing buffer used to read the bytes received from RTT Viewer input */
#define BUFF_INDEX                      (0x00)

/* Macro to provide delay in read ADC data */
#define ADC_READ_DELAY                  (0x01)

/* Macro to set value to output voltage control */
#define VREFADCG_VALUE                  (0x03)

/* Macro to set value to enable VREFADC output */
#define VREFADCG_ENABLE                 (0x03)

#define SHIFT_BY_ONE                    (0x01)
#define SHIFT_BY_THREE                  (0x03)

/* Macros for menu options to be displayed */
#define MENUOPTION1       "\r\nMENU to Select\r\n"
#define MENUOPTION2       "Press 1 to Start ADC Scan\r\n"

#if (BSP_PERIPHERAL_ADC_D_PRESENT)
#define MENUOPTION3       "Press 2 to Stop ADC Scan (Only for Sequential mode)\r\n"
#else
#define MENUOPTION3       "Press 2 to Stop ADC Scan (Only for Continuous mode)\r\n"
#endif

#define MENUOPTION4       "User Input :"

#if (BSP_PERIPHERAL_ADC_D_PRESENT)
#define BANNER_7          "\r\nThe project initializes the ADC in One-shot or Sequential mode based on the user"\
                          "\r\nselection in RA Configurator. Once initialized, the user can start the ADC scan"\
                          "\r\nand also stop the scan (in the case of Sequential mode) using J-Link RTT Viewer"\
                          "\r\nby sending commands. Results are displayed on J-Link RTT Viewer.\r\n"
#else
#define BANNER_7          "\r\nThe project initializes the ADC in Single Scan or Continuous Scan mode based on"\
                          "\r\nthe user selection in RA Configurator. Once initialized, the user can start the"\
						  "\r\nADC scan and also stop the scan (in the case of Continuous Scan mode) using J-Link"\
						  "\r\nRTT Viewer by sending commands. Results are displayed on J-Link RTT Viewer.\r\n"
#endif
/* Read the commands from RTT input and process it */
fsp_err_t read_process_input_from_RTT(void);

/* Read the ADC data available */
fsp_err_t adc_read_data(void);
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
/* Extern configure to check scan mode */
    extern const adc_d_extended_cfg_t g_adc_cfg_extend;
#endif

/* Macro for ADC version */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
#define ADC_TYPE                   "ADC_D"
#else
#define ADC_TYPE                   "ADC"
#endif

/* Close the ADC module */
void deinit_adc_module(void);

/** @} */
#endif /* ADC_EP_H_ */

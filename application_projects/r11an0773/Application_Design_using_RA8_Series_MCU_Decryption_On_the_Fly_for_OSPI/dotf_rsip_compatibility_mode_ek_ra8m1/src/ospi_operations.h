
/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef OSPI_OPERATIONS_H_
#define OSPI_OPERATIONS_H_

/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/
#include "hal_data.h"

/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/

extern uint32_t __ospi_device_1_start__;                /*defined in fsp_app.lld*/
extern uint32_t __ospi_device_1_plaintext_start__;      /*defined in fsp_app.lld*/

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/


/**********************************************************************************************************************
 * Global Function Prototypes
 **********************************************************************************************************************/
extern void ospi_init(void);
extern void write_encrypted_data_to_ospi(void);


#endif /* OSPI_OPERATIONS_H_ */

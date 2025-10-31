/***********************************************************************************************************************
 * File Name    : filex_media_operation.h
 * Description  : Contains macros, data structures and functions used in FileX thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FILEX_MEDIA_OPERATION_H_
#define FILEX_MEDIA_OPERATION_H_

#include "filex.h"

#define PRINT_MEDIA_PROPERTY(property)      (send_data_to_rtt(RTT_OUTPUT_APP_MEDIA_PROPERTY, sizeof(media_property_t),\
                                                              &(property)))
/* Macro for used card */
#if defined (BOARD_RA8T2_MCK)
#define CARD_TYPE     "MicroSD"
#else
#define CARD_TYPE     "SD"
#endif

/* Function declarations */
UINT media_verify(void);
UINT media_open(void);
UINT media_get_property(void);
UINT media_format(void);
UINT media_close(void);

#endif /* FILEX_MEDIA_OPERATION_H_ */

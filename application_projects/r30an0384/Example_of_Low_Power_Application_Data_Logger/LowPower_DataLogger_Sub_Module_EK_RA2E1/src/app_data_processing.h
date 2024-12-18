/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "hal_data.h"

typedef struct _unit_unconvert_processed_data_t
{
	uint16_t light_ave;
	uint16_t light_min;
	uint16_t light_max;
	uint16_t temp_ave;
	uint16_t temp_min;
	uint16_t temp_max;
} unit_unconvert_processed_data_t;

extern void data_processing(void);

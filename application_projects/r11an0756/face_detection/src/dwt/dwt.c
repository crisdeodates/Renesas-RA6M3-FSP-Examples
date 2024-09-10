
/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : dwt.c
 * Description  : Contains function implementations for using the DWT.c
 **********************************************************************************************************************/

#include "dwt.h"

/*********************************************************************************************************************
 *  Enable the DWT
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void InitCycleCounter(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk |CoreDebug_DEMCR_MON_EN_Msk;
}

/*********************************************************************************************************************
 *  Reset the DWT Cycle Counter
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void ResetCycleCounter(void)
{
    DWT->CYCCNT = 0;
}

/*********************************************************************************************************************
 *  Enable the DWT Cycle Counter
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void EnableCycleCounter(void)
{
    DWT->CTRL |= (DWT_CTRL_CYCCNTENA_Msk << DWT_CTRL_CYCCNTENA_Pos);
}
/*********************************************************************************************************************
 *  @brief       Read the DWT Cycle Counter
 *  @param[IN]   None
 *  @retval      the cycle count
***********************************************************************************************************************/
uint32_t GetCycleCounter(void)
{
    return DWT->CYCCNT;
}
/*********************************************************************************************************************
 *  Disable the DWT
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void DisableCycleCounter(void)
{
    CoreDebug->DEMCR &= ~(CoreDebug_DEMCR_TRCENA_Msk |CoreDebug_DEMCR_MON_EN_Msk);
}

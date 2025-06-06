/***********************************************************************************************************************
 * Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
 * display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
 * purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
 * SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 * NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
 * INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
 * be subject to different terms.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name    : system_cfg.h
 * Description  : Header file for system configurations.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *           06.03.2015 1.00    Initial Release.
 **********************************************************************************************************************/

#ifndef SYSTEM_CFG_H
#define SYSTEM_CFG_H

#include "system_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define TEMP_MAX_C                      (30.0f)
#define TEMP_MIN_C                      (15.0f)
#define TEMP_INC_C                      (0.6f)

#define PWM_BRIGHTNESS_CONTROL          (1)
#define BRIGHTNESS_DIMMING              (1)
#define BRIGHTNESS_MAX                  (100)
#define BRIGHTNESS_MIN                  (10)
#define BRIGHTNESS_INC                  (5)

#define SPLASH_TIMEOUT                  (160)


#endif /* SYSTEM_CFG_H */

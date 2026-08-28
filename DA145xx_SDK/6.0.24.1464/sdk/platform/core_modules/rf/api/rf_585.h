/**
 ****************************************************************************************
 * @addtogroup Core_Modules
 * @{
 * @addtogroup RF
 * @brief RF driver
 * @{
 *
 * @file rf_585.h
 *
 * @brief DA14585 RF control Header File.
 *
 * Copyright (C) 2012-2025 Renesas Electronics Corporation and/or its affiliates.
 * All rights reserved. Confidential Information.
 *
 * This software ("Software") is supplied by Renesas Electronics Corporation and/or its
 * affiliates ("Renesas"). Renesas grants you a personal, non-exclusive, non-transferable,
 * revocable, non-sub-licensable right and license to use the Software, solely if used in
 * or together with Renesas products. You may make copies of this Software, provided this
 * copyright notice and disclaimer ("Notice") is included in all such copies. Renesas
 * reserves the right to change or discontinue the Software at any time without notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS". RENESAS DISCLAIMS ALL WARRANTIES OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. TO THE
 * MAXIMUM EXTENT PERMITTED UNDER LAW, IN NO EVENT SHALL RENESAS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE, EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES. USE OF THIS SOFTWARE MAY BE SUBJECT TO TERMS AND CONDITIONS CONTAINED IN
 * AN ADDITIONAL AGREEMENT BETWEEN YOU AND RENESAS. IN CASE OF CONFLICT BETWEEN THE TERMS
 * OF THIS NOTICE AND ANY SUCH ADDITIONAL LICENSE AGREEMENT, THE TERMS OF THE AGREEMENT
 * SHALL TAKE PRECEDENCE. BY CONTINUING TO USE THIS SOFTWARE, YOU AGREE TO THE TERMS OF
 * THIS NOTICE.IF YOU DO NOT AGREE TO THESE TERMS, YOU ARE NOT PERMITTED TO USE THIS
 * SOFTWARE.
 *
 ****************************************************************************************
 */

#ifndef _RF_585_H_
#define _RF_585_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip.h"               // for RF API structure definition

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 *****************************************************************************************
 * @brief Initialize Radio module.
 * @param[in] api  Stucture of API functions of the RF driver that are used by the BLE
 *                 software.
 *****************************************************************************************
 */
void rf_init(struct rwip_rf_api *api);

/**
 *****************************************************************************************
 * @brief Re-initialize Radio module. Called when system exits sleep mode.
 *****************************************************************************************
 */
void rf_reinit(void);

/**
 *****************************************************************************************
 * @brief Initialize RF module registers.
 *****************************************************************************************
 */
void rf_regs(void);

/*
 * NEAR FIELD MODE API
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Checks if Near Field mode is enabled.
 * @return true if Near Field mode is enabled, otherwise false.
 ****************************************************************************************
 */
bool rf_nfm_is_enabled(void);

/**
 ****************************************************************************************
 * @brief Enables Near Field mode for all connections.
 ****************************************************************************************
 */
void rf_nfm_enable(void);

/**
 ****************************************************************************************
 * @brief Disables Near Field mode for all connections.
 ****************************************************************************************
 */
void rf_nfm_disable(void);

#endif // _RF_585_H_

/// @}
/// @}

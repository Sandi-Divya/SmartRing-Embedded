/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup PLXS
 * @brief Pulse Oximeter Service Server Application API
 * @{
 *
 * @file app_plxs.h
 *
 * @brief Header file - Pulse Oximeter Service Server Application Layer.
 *
 * Copyright (C) 2017-2024 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _APP_PLXS_H_
#define _APP_PLXS_H_

/*
 * DEFINITIONS
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_PLX_SERVER)
#include "plx_common.h"
/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// PLXS APP callbacks
struct app_plxs_cb
{
    void (*on_plxs_enable_rsp)(uint8_t conidx, uint8_t status);

    void (*on_plxs_cfg_indntf_ind)(uint8_t conidx, uint8_t  ntf_ind_cfg);
    
    void (*on_plxs_spot_meas_send_rsp)(uint8_t  status);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize PLX Application
 ****************************************************************************************
 */
void app_plxs_init(void);

/**
 ****************************************************************************************
 * @brief Add a Pulse Oximeter Service application DB
 ****************************************************************************************
 */
void app_plxs_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable PLXS.
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_plxs_enable(uint8_t conidx);

#endif // (BLE_PLX_SERVER)

#endif // _APP_PLXS_H_

///@}
///@}
///@}

/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup CGMS
 * @brief Continuous Glucose Monitoring Profile Server Application API
 * @{
 *
 * @file app_cgms.h
 *
 * @brief Continuous Glucose Monitoring Server Application entry point.
 *
 * Copyright (C) 2022-2024 Renesas Electronics Corporation and/or its affiliates.
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
 *
 ****************************************************************************************
 */

#ifndef _APP_CGMS_H_
#define _APP_CGMS_H_

/*
 * DEFINITIONS
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_CGM_SERVER)
#include "cgm_common.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// CGMS APP callbacks
struct app_cgms_cb
{
    void (*on_cgms_enable_rsp)(uint8_t conidx, uint8_t status);

    void (*on_cgms_cfg_indntf_ind)(uint8_t conidx, uint8_t  ntf_ind_cfg);
    
    void (*on_cgms_status_send_req)(uint8_t conidx);
    
    void (*on_cgms_racp_req_rcv_ind)(uint8_t conidx, struct cgm_racp_req const *ind);
    
    void (*on_cgms_spec_ops_cp_req_rcv_ind)(uint8_t conidx, struct cgm_spec_ops_cp_req const *ind);
    
    void (*on_cgms_cmp_evt)(uint8_t request, uint8_t status);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize CGM Application
 ****************************************************************************************
 */
void app_cgms_init(void);

/**
 ****************************************************************************************
 * @brief Add a Continuous Gluocose Monitoring Service application DB
 ****************************************************************************************
 */
void app_cgms_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable CGMS.
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_cgms_enable(uint8_t conidx);

#endif // (BLE_CGM_SERVER)

#endif // _APP_CGMS_H_

///@}
///@}
///@}

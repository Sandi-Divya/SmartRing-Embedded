/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles BLE Profiles
 * @{
 * @addtogroup ANCC
 * @brief Apple Notification Center Service Client Application API
 * @{
 *
 * @file app_ancc.h
 *
 * @brief Apple Notification Center Service Client Application entry point.
 *
 * Copyright (C) 2017-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef APP_ANCC_H_
#define APP_ANCC_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_ANC_CLIENT)

#include "anc_common.h"
#include "ancc.h"

/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/// ANCC APP callbacks
struct app_ancc_cb
{
    /// Callback upon 'ancc_enable'
    void (*on_ancc_enable)(uint8_t conidx, uint8_t status, const struct ancc_content *anc);

    /// Callback upon 'wr_cfg_ntf_src_rsp'
    void (*on_wr_cfg_ntf_src_rsp)(uint8_t condix, uint8_t status);

    /// Callback upon 'wr_cfg_data_src_rsp'
    void (*on_wr_cfg_data_src_rsp)(uint8_t condix, uint8_t status);

    /// Callback upon 'rd_cfg_ntf_src_rsp'
    void (*on_rd_cfg_ntf_src_rsp)(uint8_t condix, uint8_t status, bool enabled);

    /// Callback upon 'rd_cfg_data_src_rsp'
    void (*on_rd_cfg_data_src_rsp)(uint8_t condix, uint8_t status, bool enabled);

    /// Callback upon 'ntf_src_ind'
    void (*on_ntf_src_ind)(uint8_t conidx, const struct anc_ntf_src *ntf);

    /// Callback upon 'ntf_att_ind'
    void (*on_ntf_att_ind)(uint8_t conidx, uint32_t uid, uint8_t att_id, uint8_t *val);

    /// Callback upon 'app_att_ind'
    void (*on_app_att_ind)(uint8_t conidx, uint8_t att_id, uint8_t *app_id, uint8_t *att_val);

    /// Callback upon 'get_ntf_atts_cmd_cmp'
    void (*on_get_ntf_atts_cmd_cmp)(uint8_t condix, uint8_t status);

    /// Callback upon 'get_app_atts_cmd_cmp'
    void (*on_get_app_atts_cmd_cmp)(uint8_t condix, uint8_t status);

    /// Callback upon 'perf_ntf_act_cmd_cmp'
    void (*on_perf_ntf_act_cmd_cmp)(uint8_t condix, uint8_t status);
};

/**
 ****************************************************************************************
 * @brief Initialize ANC Service Application
 ****************************************************************************************
 */
void app_ancc_init(void);

/**
 ****************************************************************************************
 * @brief Add an ANC Service instance to the DB
 ****************************************************************************************
 */
void app_ancc_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable ANC Service client
 * @param[in] conidx            The connection id number
 ****************************************************************************************
 */
void app_ancc_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Write Notification Source configuration
 * @param[in] conidx            The connection id number
 * @param[in] enable            Notification enable or disable
 ****************************************************************************************
 */
void app_ancc_wr_cfg_ntf_src(uint8_t conidx, bool enable);

/**
 ****************************************************************************************
 * @brief Write Data Source configuration
 * @param[in] conidx            The connection id number
 * @param[in] enable            Notification enable or disable
 ****************************************************************************************
 */
void app_ancc_wr_cfg_data_src(uint8_t conidx, bool enable);

/**
 ****************************************************************************************
 * @brief Read Notification Source configuration
 * @param[in] conidx            The connection id number
 ****************************************************************************************
 */
void app_ancc_rd_cfg_ntf_src(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Read Data Source configuration
 * @param[in] conidx            The connection id number
 ****************************************************************************************
 */
void app_ancc_rd_cfg_data_src(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Execute Get notification attributes command
 * @param[in] conidx            The connection id number
 * @param[in] uid               Notification UID
 * @param[in] atts              Requested attributes mask
 * @param[in] title_len         Length of Title attribute
 * @param[in] sub_len           Length of Subtitle attribute
 * @param[in] msg_len           Length of Message attribute
 ****************************************************************************************
 */
void app_ancc_get_ntf_atts(uint8_t conidx, uint32_t uid, uint8_t atts, uint16_t title_len,
                               uint16_t sub_len, uint16_t msg_len);

/**
 ****************************************************************************************
 * @brief Execute Get application attributes command
 * @param[in] conidx            The connection id number
 * @param[in] atts              Requested attributes mask
 * @param[in] app_id            Application ID - must be null terminated
 ****************************************************************************************
 */
void app_ancc_get_app_atts(uint8_t conidx, uint8_t atts, uint8_t *app_id);

/**
 ****************************************************************************************
 * @brief Execute Get application attributes command
 * @param[in] conidx            The connection id number
 * @param[in] uid               Notification UID
 * @param[in] act_positive      True if action shall be positive
 ****************************************************************************************
 */
void app_ancc_ntf_action(uint8_t conidx, uint32_t uid, bool act_positive);


#endif // (BLE_ANC_CLIENT)

#endif // APP_ANCC_H_

///@}
///@}
///@}

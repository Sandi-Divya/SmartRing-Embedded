/**
 ****************************************************************************************
 *
 * @file ancc.h
 *
 * @brief Header file - Apple Notification Center Service Client Declaration.
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
 ****************************************************************************************
 */

#ifndef ANCC_H_
#define ANCC_H_

/**
 ****************************************************************************************
 * @addtogroup ANCC Apple Notification Center Client
 * @ingroup ANCC
 * @brief Apple Notification Center Client
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_ANC_CLIENT)

#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of ANCC task instances
#define ANCC_IDX_MAX                    (BLE_CONNECTION_MAX)
/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// ANCS Characteristics
enum ancc_anc_chars
{
    /// Notification Source
    ANCC_CHAR_NTF_SRC,
    /// Control Point
    ANCC_CHAR_CTRL_PT,
    /// Data Source
    ANCC_CHAR_DATA_SRC,

    ANCC_CHAR_MAX
};

/// ANCS Characteristic Descriptors
enum ancc_anc_descs
{
    /// Notification Source - Client Characteristic Configuration
    ANCC_DESC_NTF_SRC_CL_CFG,
    /// Data Source - Client Characteristic Configuration
    ANCC_DESC_DATA_SRC_CL_CFG,

    ANCC_DESC_MAX,

    ANCC_DESC_MASK = 0x10
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
///Structure containing the characteristics handles, value handles and descriptors

struct ancc_content
{
    /// Service info
    struct prf_svc svc;
    /// Characteristics
    struct prf_char_inf chars[ANCC_CHAR_MAX];
    /// Descriptors
    struct prf_char_desc_inf descs[ANCC_DESC_MAX];
};

/// Structure for ANCS Notification reception variables
struct ancc_data_src_ntf_tag
{
    /// Notification Reception state
    uint8_t state;
    /// Number of requested attributes
    uint8_t nb_req_atts;
    /// Current attribute id
    uint8_t curr_att_id;
    /// Current attribute id
    uint8_t curr_app_id_length;
    /// Current attribute id
    uint8_t curr_att_length;
    /// Remaining bytes for current value
    uint16_t curr_value_idx;
    /// Last allocated notification
    void *alloc_rsp_ind;
};

struct ancc_cnx_env
{
    /// Last requested operation code
    uint8_t last_op_code;
    /// Provide an indication about the last operation
    uint16_t last_req;
    /// Last requested UUID(to keep track of the two services and char)
    uint16_t last_uuid_req;
    /// Last characteristic code used in a read or a write request
    uint16_t last_char_code;
    /// Counter used to check service uniqueness
    uint8_t nb_svc;
    /// Data Source Notification reception variables structure
    struct ancc_data_src_ntf_tag data_src;
    /// ANC characteristics
    struct ancc_content anc;
    /// Current operation code
    void *operation;
};

/// ANCC environment variable
struct ancc_env_tag
{
    /// Profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct ancc_cnx_env* env[ANCC_IDX_MAX];
    /// State of different task instances
    ke_state_t state[ANCC_IDX_MAX];
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve ANC client profile interface
 * @return ANC client profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs *ancc_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Send ANC ATT DB discovery results to ANCC host.
 ****************************************************************************************
 */
void ancc_enable_rsp_send(struct ancc_env_tag *ancc_env, uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send Notification Configuration Response
 ****************************************************************************************
 */
void ancc_wr_cfg_ntf_rsp_send(struct ancc_env_tag *ancc_env, uint8_t conidx,
                              uint8_t op_code, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send Read Notification Configuration Response
 ****************************************************************************************
 */
void ancc_rd_cfg_ntf_rsp_send(struct ancc_env_tag *ancc_env, uint8_t conidx,
                              uint8_t op_code, uint16_t value, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send Operation Completion Event
 ****************************************************************************************
 */
void ancc_cmp_evt_send(struct ancc_env_tag *ancc_env, uint8_t conidx, uint8_t op_code,
                       uint8_t status);

/**
 ****************************************************************************************
 * @brief Handles reception of response to a Get Notification Attributes command.
 * @param[in] ancc_env Pointer to the profile environment.
 * @param[in] conidx Connection index
 * @param[in] value Pointer to the response value.
 * @param[in] length Length of the response value.
 ****************************************************************************************
 */
void ancc_get_ntf_atts_rsp(struct ancc_env_tag *ancc_env, uint8_t conidx,
                           const uint8_t *value, uint16_t length);

/**
 ****************************************************************************************
 * @brief Handles reception of response to a Get App Attributes command.
 * @param[in] ancc_env Pointer to the profile environment.
 * @param[in] value Pointer to the response value.
 * @param[in] length Length of the response value.
 ****************************************************************************************
 */
void ancc_get_app_atts_rsp(struct ancc_env_tag *ancc_env, uint8_t conidx,
                           const uint8_t *value, uint16_t length);

#endif // (BLE_ANC_CLIENT)

/// @} ANCC

#endif // ANCC_H_

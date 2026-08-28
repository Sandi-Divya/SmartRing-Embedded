/**
 ****************************************************************************************
 *
 * @file bmss.h
 *
 * @brief Header file - Bond Management Service Server Declaration.
 *
 * Copyright (C) 2015-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef BMSS_H_
#define BMSS_H_

/**
 ****************************************************************************************
 * @addtogroup BMSS Bond Management 'Profile' Server
 * @ingroup BMSS
 * @brief Bond Management 'Profile' Server
 * @{
 ****************************************************************************************
 */

#if (BLE_BMS_SERVER)

#include "prf_types.h"
#include "prf_utils.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define BMS_CNTL_PT_MAX_LEN (512)

/// Maximum number of Bond Management Server task instances
#define BMSS_IDX_MAX        (1)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Bond Management Service Attribute Table Indices
enum
{
    /// Bond Management Service
    BMS_IDX_SVC,
    /// Bond Management Control Point
    BMS_IDX_BM_CNTL_PT_CHAR,
    BMS_IDX_BM_CNTL_PT_VAL,
    BMS_IDX_BM_CNTL_PT_EXT_PROP,
    /// Bond Management Feature
    BMS_IDX_BM_FEAT_CHAR,
    BMS_IDX_BM_FEAT_VAL,
    /// Number of attributes
    BMS_IDX_NB,
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Bond Management Service environment variable
struct bmss_env_tag
{
    /// Profile environment
    prf_env_t prf_env;
    /// BMS Start Handles
    uint16_t shdl;
    /// Database configuration
    uint32_t features;
    uint8_t reliable_writes;
    /// State of different task instances
    ke_state_t state[BMSS_IDX_MAX];
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/// Full BMS Database Description
extern const struct attm_desc bms_att_db[BMS_IDX_NB];

/// Bond Management Service
extern const att_svc_desc_t bms_svc;

/// Bond Management Service Control Point Characteristic
extern const struct att_char_desc bms_ctrl_pt_char;

/// Bond Management Service Feature Characteristic
extern const struct att_char_desc bms_feat_char;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve Bond Management service profile interface
 *
 * @return Bond Management service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* bmss_prf_itf_get(void);

#endif // (BLE_BMS_SERVER)

/// @} BMSS

#endif // BMSS_H_

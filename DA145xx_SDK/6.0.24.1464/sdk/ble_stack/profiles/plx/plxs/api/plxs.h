/**
 ****************************************************************************************
 *
 * @file plxs.h
 *
 * @brief Header file - Pulse Oximeter Service Server Declaration.
 *
 * Copyright (C) 2015-2024 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef PLXS_H_
#define PLXS_H_

/**
 ****************************************************************************************
 * @addtogroup PLXS Pulse Oximeter Profile Server
 * @ingroup PLX
 * @brief Pulse Oximeter Profile Server
 *
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#if (BLE_PLX_SERVER)
#include <stdint.h>
#include <stdbool.h>
#include "atts.h"
#include "plx_common.h"
#include "prf_types.h"
#include "prf_utils.h"

#include "gap.h"

/*
 * MACROS
 ****************************************************************************************
 */
#define PLXS_IS_FEATURE_SUPPORTED(feat, bit_mask) (((feat & bit_mask) == bit_mask))

#define PLXS_HANDLE(idx) (plxs_att_hdl_get(plxs_env, (idx)))

#define PLXS_IDX(hdl)    (plxs_att_idx_get(plxs_env, (hdl)))


/*
 * DEFINES
 ****************************************************************************************
 */
///Maximum number of Pulse Oximetere Servers task instances
#define PLXS_IDX_MAX    (1)

#define PLXS_SERV_DECL_MASK               (0x0001)
#define PLXS_SPOT_MEAS_MASK               (0x000E)
#define PLXS_CONT_MEAS_MASK               (0x0070)
#define PLXS_FEAT_CHAR_MASK               (0x0180)

#define PLXS_SPOT_MEAS_ATT_NB             (3)
#define PLXS_CONT_MEAS_ATT_NB             (3)
#define PLXS_FEAT_ATT_NB                  (2)

#define PLXS_SPOT_MEAS_MAX_LEN            (19)
#define PLXS_CONT_MEAS_MAX_LEN            (21)
#define PLXS_IND_NTF_CFG_MAX_LEN          (2)

#define PLXS_FEAT_VALUE         (PLXS_FEAT_MEASUREMENT_STAT_SUP | PLXS_FEAT_DEVICE_SENSOR_STAT_SUP | \
                                PLXS_FEAT_SPO2_FAST_METRIC_SUP | PLXS_FEAT_SPO2_SLOW_METRIC_SUP | \
                                PLXS_FEAT_PULSE_AMPL_INDEX_SUP | PLXS_FEAT_MULTI_BOND_SUP )

#if (PLXS_FEAT_VALUE & PLXS_FEAT_MEASUREMENT_STAT_SUP) == PLXS_FEAT_MEASUREMENT_STAT_SUP
#define PLXS_MEAS_STAT_VALUE    0x0000
#endif

#if (PLXS_FEAT_VALUE & PLXS_FEAT_DEVICE_SENSOR_STAT_SUP) == PLXS_FEAT_DEVICE_SENSOR_STAT_SUP
#define PLXS_DEVICE_SENSOR_STAT_VALUE   0x000000
#endif
/*
 * ENUMS
 ****************************************************************************************
 */


 ///Attributes database elements
enum plxs_att_db_list
{
    PLX_IDX_SVC,

    PLX_IDX_SPOT_MEAS_CHAR,
    PLX_IDX_SPOT_MEAS_VAL,
    PLX_IDX_SPOT_MEAS_IND_CFG,

    PLX_IDX_CONT_MEAS_CHAR,
    PLX_IDX_CONT_MEAS_VAL,
    PLX_IDX_CONT_MEAS_NTF_CFG,

    PLX_IDX_FEAT_CHAR,
    PLX_IDX_FEAT_VAL,

    PLX_IDX_NB,
};


/// Ongoing operation information
struct plxs_op
{
     /// Operation
     uint8_t op;
     /// Cursor on connection
     uint8_t cursor;
     /// Handle of the attribute to indicate/notify
     uint16_t handle;
     /// Task that request the operation that should receive completed message response
     uint16_t dest_id;
     /// Packed notification/indication data size
     uint8_t length;
     /// used to know on which device interval update has been requested, and to prevent
     /// indication to be triggered on this connection index
     uint8_t conidx;
     /// Packed notification/indication data
     uint8_t data[__ARRAY_EMPTY];
};


///Pulse Oximeter Profile Server Environment Variable
struct plxs_env_tag
{
    /// Profile environment
    prf_env_t prf_env;
    /// On-going operation
    struct plxs_op *operation;
    /// Service Start Handle
    uint16_t shdl;
    /// Database configuration
    uint16_t features;
    
    /// Notification and indication configuration of peer devices.
    uint8_t ntf_ind_cfg[BLE_CONNECTION_MAX];
    /// State of different task instances
    ke_state_t state[PLXS_IDX_MAX];
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve PLX service profile interface
 *
 * @return PLX service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* plxs_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Retrieve attribute handle from attribute index
 *
 * @param[in] plxs_env   Environment variable
 * @param[in] att_idx    Attribute Index
 *
 * @return attribute Handle
 ****************************************************************************************
 */
uint16_t plxs_att_hdl_get(struct plxs_env_tag* plxs_env, uint8_t att_idx);

/**
 ****************************************************************************************
 * @brief Retrieve attribute index from attribute handle
 *
 * @param[in] plxs_env   Environment variable
 * @param[in] handle     Attribute Handle
 *
 * @return attribute Index
 ****************************************************************************************
 */
uint8_t plxs_att_idx_get(struct plxs_env_tag* plxs_env, uint16_t handle);

/**
 ****************************************************************************************
 * @brief Pack spot-check measurement value from several components
 *
 * @return size of packed value
 ****************************************************************************************
 */
uint8_t plxs_pack_spot_meas_value(uint8_t *packed_spot_meas, struct plx_spot_check_meas spot_meas);

/**
 ****************************************************************************************
 * @brief Pack continuous measurement value from several components
 *
 * @return size of packed value
 ****************************************************************************************
 */
uint8_t plxs_pack_cont_meas_value(uint8_t *packed_cont_meas, struct plx_cont_meas cont_meas);

/**
 ****************************************************************************************
 * @brief  This function fully manage notification and indication of pulse oximeter
 *         to peer(s) device(s) according to on-going operation requested by application:
 *            - Indicate to a known device that a Spot-Check Measurement has been acquired
 *            - Notify a known device that a Continuous Measurement has been acquired
 ****************************************************************************************
 */
void plxs_exe_operation(void);

/**
 ****************************************************************************************
 * @brief Update Notification, Indication configuration
 *
 * @param[in] conidx    Connection index
 * @param[in] cfg       Indication configuration flag
 * @param[in] valid_val Valid value if NTF/IND enable
 * @param[in] value     Value set by peer device.
 *
 * @return status of configuration update
 ****************************************************************************************
 */
uint8_t plxs_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value);

#endif //BLE_PLX_SERVER

/// @} PLXS

#endif // PLXS_H_

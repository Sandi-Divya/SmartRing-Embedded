/**
 ****************************************************************************************
 *
 * @file gatt_client.h
 *
 * @brief Header file - Generic Attribute Profile Service Client Declaration.
 *
 * Copyright (C) 2018-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef GATT_CLIENT_H_
#define GATT_CLIENT_H_

/**
 ****************************************************************************************
 * @addtogroup Generic Attribute Profile Service Client
 * @ingroup GATT_CLIENT
 * @brief Generic Attribute Profile Service Client
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_GATT_CLIENT)

#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of GATT client task instances
#define GATT_CLIENT_IDX_MAX           (BLE_CONNECTION_MAX)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// GATT Characteristics
enum gattc_gatt_chars
{
    /// Service Changed
    GATTC_CHAR_SVC_CHANGED,

    GATTC_CHAR_MAX
};

/// GATT Characteristic Descriptors
enum gattc_gatt_descs
{
    /// Service Changed - Client Characteristic Configuration
    GATTC_DESC_SVC_CHANGED_CCC,

    GATTC_DESC_MAX,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
/// Structure containing the characteristic handles, value handles and descriptors

struct gatt_client_content
{
    /// Service info
    struct prf_svc svc;
    /// Characteristics
    struct prf_char_inf chars[GATTC_CHAR_MAX];
    /// Descriptors
    struct prf_char_desc_inf descs[GATTC_DESC_MAX];
};

struct gatt_client_cnx_env
{
    /// Counter used to check service uniqueness
    uint8_t nb_svc;
    /// GATTC characteristics
    struct gatt_client_content gatt;
};

/// GATTC environment variable
struct gatt_client_env_tag
{
    /// Profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct gatt_client_cnx_env* env[GATT_CLIENT_IDX_MAX];
    /// State of different task instances
    ke_state_t state[GATT_CLIENT_IDX_MAX];
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve GATT client profile interface
 * @return GATT client profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs *gatt_client_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Send GATT ATT DB discovery results to GATTC host.
 *
 * @param[in] gatt_env  Service environment
 * @param[in] conidx    Connection index
 * @param[in] status    Operation status
 ****************************************************************************************
 */
void gatt_client_enable_rsp_send(struct gatt_client_env_tag *gattc_env,
                                 uint8_t conidx,
                                 uint8_t status);


/**
 ****************************************************************************************
 * @brief Send response for read request to APP layer.
 *
 * @param[in] gatt_env  Service environment
 * @param[in] conidx    Connection index
 * @param[in] cfg_val   Configuration value
 * @param[in] status    Operation status
 ****************************************************************************************
 */
void gatt_client_rd_cfg_ind_rsp_send(struct gatt_client_env_tag *gatt_env,
                                     uint8_t conidx,
                                     uint16_t cfg_val,
                                     uint8_t status);

/**
 ****************************************************************************************
 * @brief Send response for write request to APP layer.
 *
 * @param[in] gatt_env  Service environment
 * @param[in] conidx    Connection index
 * @param[in] status    Operation status
 ****************************************************************************************
 */
void gatt_client_wr_cfg_ind_rsp_send(struct gatt_client_env_tag *gatt_env,
                                     uint8_t conidx,
                                     uint8_t status);
#endif // BLE_GATT_CLIENT

/// @} GATT_CLIENT

#endif // GATT_CLIENT_H_

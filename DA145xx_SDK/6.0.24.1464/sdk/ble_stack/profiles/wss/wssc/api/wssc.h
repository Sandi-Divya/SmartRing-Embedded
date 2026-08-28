/**
 ****************************************************************************************
 *
 * @file wssc.h
 *
 * @brief Header file - Weight Scale Service Client Declaration.
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

#ifndef WSSC_H_
#define WSSC_H_

/**
 ****************************************************************************************
 * @addtogroup WSSC Weight Scale 'Profile' Client
 * @ingroup WSSC
 * @brief Weight Scale 'Profile' Client
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_WSS_COLLECTOR)

#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of WSSC task instances
#define WSSC_IDX_MAX       (BLE_CONNECTION_MAX)

/// Pointer to the connection clean-up function
#define WSSC_CLEANUP_FNCT        (NULL)

#define WSSC_PACKED_MEAS_MIN_LEN        (3)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Weight Scale Service Characteristics
enum
{
    // Weight Scale Feature
    WSSC_CHAR_WSS_WS_FEAT,
    // Weight Scale Measurment
    WSSC_CHAR_WSS_WEIGHT_MEAS,

    WSSC_CHAR_MAX
};

/// Weight Scale Service Characteristic Descriptor
enum
{
    // Weight Meas Client Config
    WSSC_DESC_WSS_WM_CLI_CFG,

    WSSC_DESC_MAX
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

///Structure containing the characteristics handles, value handles and descriptors
struct wss_content
{
    /// service info
    struct prf_svc svc;

    /// characteristic info:
    /// - Weight Scale Feature
    /// - Weight Scale Measurement
    struct prf_char_inf chars[WSSC_CHAR_MAX];

    /// descriptors:
    struct prf_char_desc_inf descs[WSSC_DESC_MAX];
};

/// Environment variable for each Connections
struct wssc_cnx_env
{
    /// Last requested UUID(to keep track of the two services and char)
    uint16_t last_uuid_req;
    /// Last Char. for which a read request has been sent
    uint8_t last_char_code;

    /// counter used to check service uniqueness
    uint8_t nb_svc;

    /// WSS characteristics
    struct wss_content wss;
};

/// Weight Scale Client environment variable
struct wssc_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct wssc_cnx_env* env[WSSC_IDX_MAX];
    /// State of different task instances
    ke_state_t state[WSSC_IDX_MAX];
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
 * @brief Retrieve WSS client profile interface
 *
 * @return WSS client profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* wssc_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Send Weight Scale ATT DB discovery results to WSSC host.
 ****************************************************************************************
 */
void wssc_enable_rsp_send(struct wssc_env_tag *wssc_env, uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Unpack the received weight measurement value
 ****************************************************************************************
 */
void wssc_unpack_meas_value(struct wssc_env_tag *wssc_env, uint8_t conidx, uint8_t *packed_data, uint8_t lenght);

/**
 ****************************************************************************************
 * @brief Send error indication from service to Host, with proprietary status codes.
 * @param status Status code of error.
 ****************************************************************************************
 */
void wssc_err_ind_send(struct wssc_env_tag *wssc_env, uint8_t conidx, uint8_t status);

#endif // (BLE_WSS_COLLECTOR)

/// @} WSSC

#endif // WSSC_H_

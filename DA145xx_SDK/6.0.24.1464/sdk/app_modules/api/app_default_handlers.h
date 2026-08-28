/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Default_Handlers Default Handlers
 * @brief Application Default Handlers API
 * @{
 *
 * @file app_default_handlers.h
 *
 * @brief Application default handlers header file.
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

#ifndef _APP_DEFAULT_HANDLERS_H_
#define _APP_DEFAULT_HANDLERS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_APP_PRESENT)

#include <stdio.h>
#include "gapc_task.h"          // GAP Controller Task API
#include "gapm_task.h"          // GAP Manager Task API
#include "co_bt.h"
#include "app_utils.h"
#include "app.h"
#if (BLE_APP_SEC)
#include "app_security.h"
#endif // (BLE_APP_SEC)

#if (BLE_SUOTA_RECEIVER)
#include "app_suotar.h"
#endif

/// Possible advertising scenarios
enum default_advertise_scenario
{
    /// Advertising forever
    DEF_ADV_FOREVER,

    /// Advertising periodically
    DEF_ADV_WITH_TIMEOUT
};

/// Possible security request scenarios
enum default_security_request_scenario
{
    /// No security request
    DEF_SEC_REQ_NEVER,

    /// Security request upon connection
    DEF_SEC_REQ_ON_CONNECT
};

/// Configuration options for the default_handlers
struct default_handlers_configuration
{
    /// Advertising operation used by the default handlers
    enum default_advertise_scenario adv_scenario;

    /// Advertising period in the DEF_ADV_WITH_TIMEOUT case; measured in timer
    /// units (10ms) - use MS_TO_TIMERUNITS to convert from milliseconds (ms) to
    /// timer units
    int32_t advertise_period;

    /// Security start operation of the default handlers if security is enabled
    /// (CFG_APP_SECURITY)
    enum default_security_request_scenario security_request_scenario;
};

/// Operations used by the default handlers
struct default_app_operations
{
    /// Advertising operation
    void (*default_operation_adv)(void);
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Default function called on initialization event.
 ****************************************************************************************
 */
void default_app_on_init(void);

/**
 ****************************************************************************************
 * @brief Default function called on connection event.
 * @param[in] conidx Connection Id number
 * @param[in] param          Pointer to GAPC_CONNECTION_REQ_IND message
 ****************************************************************************************
 */
void default_app_on_connection(uint8_t conidx, struct gapc_connection_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on disconnection event.
 * @param[in] param          Pointer to GAPC_DISCONNECT_IND message
 ****************************************************************************************
 */
void default_app_on_disconnect(struct gapc_disconnect_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on device configuration completion event.
 ****************************************************************************************
 */
void default_app_on_set_dev_config_complete(void);

/**
 ****************************************************************************************
 * @brief Default function called on database initialization completion event.
 ****************************************************************************************
 */
void default_app_on_db_init_complete(void);

/**
 ****************************************************************************************
 * @brief Default function called on advertising operation.
 ****************************************************************************************
 */
void default_advertise_operation(void);

/**
 ****************************************************************************************
 * @brief Default function called on device name read request from peer.
 * @param[in,out] device_name       The device_name value returned.
 ****************************************************************************************
 */
void default_app_on_get_dev_name(struct app_device_name* device_name);

/**
 ****************************************************************************************
 * @brief Default function called on device appearance read request from peer.
 * @param[in,out] appearance         The appearance value returned.
 ****************************************************************************************
 */
void default_app_on_get_dev_appearance(uint16_t* appearance);

/**
 ****************************************************************************************
 * @brief Default function called on slave preferred connection parameters read request
 *        from peer.
 * @param[in,out] slv_params         The slave preferred connection parameters.
 ****************************************************************************************
 */
void default_app_on_get_dev_slv_pref_params(struct gap_slv_pref* slv_params);

/**
 ****************************************************************************************
 * @brief Default function called on device info write request from peer.
 * @param[in] param          Pointer to GAPC_SET_DEV_INFO_REQ_IND message
 * @param[in,out] status     Status code used to know if requested has been accepted
 *                           or not.
 ****************************************************************************************
 */
void default_app_on_set_dev_info(struct gapc_set_dev_info_req_ind const * param, uint8_t* status);

/**
 ****************************************************************************************
 * @brief Default function called on parameter update request indication.
 * @param[in] param             Pointer to a gapc_param_update_req_ind message struct
 * @param[in,out] cfm           Pointer to a gapc_param_update_cfm message struct
 * @note The application may accept or reject the received parameter update request,
 *       depending on the received param values.
 ****************************************************************************************
 */
void default_app_update_params_request(struct gapc_param_update_req_ind const *param,
                                       struct gapc_param_update_cfm *cfm);

/**
 ****************************************************************************************
 * @brief Generate a 48-bit static random address. The static random address is generated
 *        only once in a device power cycle and it is stored in the retention RAM.
 *        The two MSB shall be equal to '1'.
 * @param[in,out] addr      The generated static random address
 ****************************************************************************************
 */
void default_app_generate_static_random_addr(struct bd_addr *addr);

/**
 ****************************************************************************************
 * @brief Generate a 48-bit unique static random address. The static random address is
 *        generated only once in a device power cycle and it is stored in the retention
 *        RAM. It is based on the OTP header data (device time stamp, site ID, test site,
 *        bandgap and LNA trim value) thus is guaranteed to be constant over every power
 *        cycle. The two MSB shall be equal to '1'.
 * @param[in,out] addr      The generated static random address
 ****************************************************************************************
 */
void default_app_generate_unique_static_random_addr(struct bd_addr *addr);

/**
 ****************************************************************************************
 * @brief Generate a 16-byte key for storage data encryption. This key is generated once
          during during start up, and should be the same on every device start up.
 * @param[in,out] key   The generated key
 * @param[out] size     The size of the key that should be generated
 ****************************************************************************************
 */
uint8_t default_app_generate_unique_key(uint8_t *key);

/**
 ****************************************************************************************
 * @brief Generate a 16-byte irk key. The irk key is generated only once in 
 *        a device power cycle and it is stored in the retention RAM. 
 *        It is based on the OTP header data thus is guaranteed to be constant over 
 *        every power cycle.
 * @param[in, out] Pointer to the array that will hold the generated irk key
 ****************************************************************************************
 */
void default_app_on_provide_local_irk(uint8_t* key);

#if (BLE_APP_SEC)

/**
 ****************************************************************************************
 * @brief Default function called on pairing request event.
 * @param[in] conidx         Connection Id number
 * @param[in] param          Pointer to GAPC_BOND_REQ_IND message
 ****************************************************************************************
 */
void default_app_on_pairing_request(uint8_t conidx, struct gapc_bond_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on no-man-in-the-middle TK exchange event.
 * @param[in] conidx         Connection Id number
 * @param[in] param          Pointer to GAPC_BOND_REQ_IND message
 ****************************************************************************************
 */
void default_app_on_tk_exch(uint8_t conidx, struct gapc_bond_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on CSRK exchange event.
 * @param[in] conidx         Connection Id number
 * @param[in] param          Pointer to GAPC_BOND_REQ_IND message
 ****************************************************************************************
 */
void default_app_on_csrk_exch(uint8_t conidx, struct gapc_bond_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on long term key exchange event.
 * @param[in] conidx         Connection Id number
 * @param[in] param          Pointer to GAPC_BOND_REQ_IND message
 ****************************************************************************************
 */
void default_app_on_ltk_exch(uint8_t conidx, struct gapc_bond_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on encryption request event.
 * @param[in] conidx         Connection Id number
 * @param[in] param          Pointer to GAPC_ENCRYPT_REQ_IND message
 ****************************************************************************************
 */
void default_app_on_encrypt_req_ind(uint8_t conidx, struct gapc_encrypt_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on pairing succeeded.
 * @param[in] conidx         Connection Id number
 ****************************************************************************************
 */
void default_app_on_pairing_succeeded(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Default function called on address resolved indication.
 * @param[in] conidx         Connection Id number
 * @param[in] param          Pointer to GAPM_ADDR_SOLVED_IND message
 ****************************************************************************************
 */
void default_app_on_addr_solved_ind(uint8_t conidx, struct gapm_addr_solved_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on GAPM_CMP_EVT (GAPM_RESOLV_ADDR) event with
 *        status = GAP_ERR_NOT_FOUND.
 * @param[in] conidx         Connection Id number
 ****************************************************************************************
 */
void default_app_on_addr_resolve_failed(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Default function called on GAPM_CMP_EVT for any RAL operation
 * @param[in] param         Pointer to GAPM_CMP_EVT message
 * @note Not applicable to DA14531-01
 ****************************************************************************************
 */
void default_app_on_ral_cmp_evt(const struct gapm_cmp_evt *param);

#endif // (BLE_APP_SEC)

#if (BLE_SUOTA_RECEIVER)
/**
 **************************************************************************************************
 * @brief SUOTA callback function that performs checks for the validity of the 
 *        incoming image's string version 
 * @param[in]   cur_version:    Input, pointer to the current image header
 *              new_version:    Input, pointer to the updated image header
 * @return      true if version string has correct syntax and it is not older than the current one
 *              false otherwise
 ***************************************************************************************************
 */
SUOTAR_IMG_VERSION_CHECK default_suotar_on_version_check(const uint8_t *cur_version, const uint8_t *new_version);
#endif // (BLE_SUOTA_RECEIVER)

#endif // (BLE_APP_PRESENT)

#endif // _APP_DEFAULT_HANDLERS_H_

///@}
///@}

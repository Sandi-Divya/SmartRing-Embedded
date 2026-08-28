/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Callbacks
 * @brief Application Callbacks API
 * @{
 *
 * @file app_callback.h
 *
 * @brief Application callbacks definitions.
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

#ifndef _APP_CALLBACK_H_
#define _APP_CALLBACK_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "co_bt.h"
#include "gapc_task.h"
#include "gapm_task.h"
#include "l2cc_task.h"
#include "gap.h"
#include "app.h"

#if (BLE_APP_SEC)
#include "app_security.h"
#endif // (BLE_APP_SEC)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// APP callbacks
struct app_callbacks
{
    /// Callback upon 'connection'
    void (*app_on_connection)(const uint8_t, struct gapc_connection_req_ind const *);

    /// Callback upon 'disconnect'
    void (*app_on_disconnect)(struct gapc_disconnect_ind const *);

    /// Callback upon 'update_params_rejected'
    void (*app_on_update_params_rejected)(const uint8_t);

    /// Callback upon 'update_params_complete'
    void (*app_on_update_params_complete)(void);

    /// Callback upon 'set_dev_config_complete'
    void (*app_on_set_dev_config_complete)(void);

    /// Callback upon 'adv_nonconn_complete'
    void (*app_on_adv_nonconn_complete)(const uint8_t);

    /// Callback upon 'adv_undirect_complete'
    void (*app_on_adv_undirect_complete)(const uint8_t);

    /// Callback upon 'adv_direct_complete'
    void (*app_on_adv_direct_complete)(const uint8_t);

    /// Callback upon 'db_init_complete'
    void (*app_on_db_init_complete)(void);

    /// Callback upon 'scanning_completed'
    void (*app_on_scanning_completed)(const uint8_t);

    /// Callback upon 'adv_report_ind'
    void (*app_on_adv_report_ind)(struct gapm_adv_report_ind const *);

    /// Callback upon 'connect_failed'
    void (*app_on_connect_failed)(void);

    /// Callback upon 'get_dev_name'
    void (*app_on_get_dev_name)(struct app_device_name *);

    /// Callback upon 'get_dev_appearance'
    void (*app_on_get_dev_appearance)(uint16_t*);

    /// Callback upon 'get_dev_slv_pref_params'
    void (*app_on_get_dev_slv_pref_params)(struct gap_slv_pref*);

    /// Callback upon 'set_dev_info'
    void (*app_on_set_dev_info)(struct gapc_set_dev_info_req_ind const *, uint8_t*);

    /// Callback upon 'data_length_change'
    void (*app_on_data_length_change)(const uint8_t, struct gapc_le_pkt_size_ind *);

    /// Callback upon 'update_params_request'
    void (*app_on_update_params_request)(struct gapc_param_update_req_ind const *, struct gapc_param_update_cfm *);

    /// Callback upon 'generate_static_random_addr'
    void (*app_on_generate_static_random_addr)(struct bd_addr *);

    /// Callback upon 'svc_changed_cfg_ind'
    void (*app_on_svc_changed_cfg_ind)(uint8_t, uint16_t);

    /// Callback upon 'get_peer_features'
    void (*app_on_get_peer_features)(const uint8_t, struct gapc_peer_features_ind const *);
    
    /// Callback upon irk generation
    void (*app_on_provide_local_irk)(uint8_t*);

#if (BLE_APP_SEC)
    /// Callback upon 'pairing_request'
    void (*app_on_pairing_request)(const uint8_t, struct gapc_bond_req_ind const *);

    /// Callback upon 'tk_exch'
    void (*app_on_tk_exch)(const uint8_t, struct gapc_bond_req_ind const *);

    /// Callback upon 'irk_exch'
    void (*app_on_irk_exch)(struct gapc_bond_req_ind const *);

    /// Callback upon 'csrk_exch'
    void (*app_on_csrk_exch)(const uint8_t, struct gapc_bond_req_ind const *);

    /// Callback upon 'ltk_exch'
    void (*app_on_ltk_exch)(const uint8_t, struct gapc_bond_req_ind const *);

    /// Callback upon 'pairing_succeeded'
    void (*app_on_pairing_succeeded)(const uint8_t);

    /// Callback upon 'encrypt_ind'
    void (*app_on_encrypt_ind)(const uint8_t, const uint8_t);

    /// Callback upon 'encrypt_req_ind'
    void (*app_on_encrypt_req_ind)(const uint8_t, struct gapc_encrypt_req_ind const *);

    /// Callback upon 'security_req_ind'
    void (*app_on_security_req_ind)(const uint8_t);

    /// Callback upon 'addr_solved_ind'
    void (*app_on_addr_solved_ind)(const uint8_t, struct gapm_addr_solved_ind const *);

    /// Callback upon 'addr_resolve_failed'
    void (*app_on_addr_resolve_failed)(const uint8_t);
#if !defined (__DA14531_01__) && !defined (__DA14535__)
    /// Callback upon 'ral_cmp_evt'
    void (*app_on_ral_cmp_evt)(struct gapm_cmp_evt const *);

    /// Callback upon 'ral_size_ind'
    void (*app_on_ral_size_ind)(const uint8_t);

    /// Callback upon 'ral_addr_ind'
    void (*app_on_ral_addr_ind)(const uint8_t, const uint8_t *);
#endif // not for DA14531-01, DA14535
#endif // (BLE_APP_SEC)
};

#if (BLE_APP_SEC)
/// APP Bond Database callbacks
struct app_bond_db_callbacks
{
    /// Callback upon 'bdb_init'
    void (*app_bdb_init)(void);

    /// Callback upon 'bdb_get_size'
    uint8_t (*app_bdb_get_size)(void);

    /// Callback upon 'bdb_add_entry'
    void (*app_bdb_add_entry)(struct app_sec_bond_data_env_tag *);

    /// Callback upon 'bdb_remove_entry'
    void (*app_bdb_remove_entry)(enum bdb_search_by_type, enum bdb_remove_type, void *, uint8_t);

    /// Callback upon 'bdb_search_entry'
    const struct app_sec_bond_data_env_tag * (*app_bdb_search_entry)(enum bdb_search_by_type, void *, uint8_t);

    /// Callback upon 'bdb_get_number_of_stored_irks'
    uint8_t (*app_bdb_get_number_of_stored_irks)(void);

    /// Callback upon 'bdb_get_stored_irks'
    uint8_t (*app_bdb_get_stored_irks)(struct gap_sec_key *);

    /// Callback upon 'bdb_get_device_info_from_slot'
    bool (*app_bdb_get_device_info_from_slot)(uint8_t, struct gap_ral_dev_info *);
    
    bool (*app_bdb_erase)(void);
};
#endif // (BLE_APP_SEC)

/// LECB APP callbacks
struct app_lecb_callbacks
{
    /// Callback upon 'gapc_lecb_disconnect_ind'
    void (*app_lecb_disconnect_ind) (const uint8_t, struct gapc_lecb_disconnect_ind const *);

    /// Callback upon 'gapc_lecb_connect_req_ind'
    void (*app_lecb_connect_req_ind) (const uint8_t, struct gapc_lecb_connect_req_ind const *, uint16_t *);

    /// Callback upon 'gapc_lecb_connect_ind'
    void (*app_lecb_connect_ind) (const uint8_t, struct gapc_lecb_connect_ind const *);

    /// Callback upon 'gapc_lecb_add_ind'
    void (*app_lecb_add_ind) (const uint8_t, struct gapc_lecb_add_ind const *);

    /// Callback upon 'l2cc_lecnx_data_recv_ind'
    void (*app_lecb_data_recv_ind) (const uint8_t, struct l2cc_lecnx_data_recv_ind const *);

    /// Callback upon 'l2cc_data_send_rsp'
    void (*app_lecb_data_send_rsp) (const uint8_t, struct l2cc_data_send_rsp const *);
};

/*
 * DEFINES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Helper macro calling a callback function taking no arguments.
 * @note Has no effect if the provided callback function is not defined.
 * @param[in] cb The callback function.
 ****************************************************************************************
 */
#define CALLBACK_ARGS_0(cb)                         {if (cb != NULL) cb();}

/**
 ****************************************************************************************
 * @brief Helper macro calling a callback function taking one argument.
 * @note Has no effect if the provided callback function is not defined.
 * @param[in] cb The callback function.
 * @param[in] arg1 The argument the callback function takes.
 ****************************************************************************************
 */
#define CALLBACK_ARGS_1(cb, arg1)                   {if (cb != NULL) cb(arg1);}

/**
 ****************************************************************************************
 * @brief Helper macro calling a callback function taking two arguments.
 * @note Has no effect if the provided callback function is not defined.
 * @param[in] cb The callback function.
 * @param[in] arg1 The first argument the callback function takes.
 * @param[in] arg2 The second argument the callback function takes.
 ****************************************************************************************
 */
#define CALLBACK_ARGS_2(cb, arg1, arg2)             {if (cb != NULL) cb(arg1, arg2);}

/**
 ****************************************************************************************
 * @brief Helper macro calling a callback function taking three arguments.
 * @note Has no effect if the provided callback function is not defined.
 * @param[in] cb The callback function.
 * @param[in] arg1 The first argument the callback function takes.
 * @param[in] arg2 The second argument the callback function takes.
 * @param[in] arg3 The third argument the callback function takes.
 ****************************************************************************************
 */
#define CALLBACK_ARGS_3(cb, arg1, arg2, arg3)       {if (cb != NULL) cb(arg1, arg2, arg3);}

/**
 ****************************************************************************************
 * @brief Helper macro calling a callback function taking four arguments.
 * @note Has no effect if the provided callback function is not defined.
 * @param[in] cb The callback function.
 * @param[in] arg1 The first argument the callback function takes.
 * @param[in] arg2 The second argument the callback function takes.
 * @param[in] arg3 The third argument the callback function takes.
 * @param[in] arg4 The fourth argument the callback function takes.
 ****************************************************************************************
 */
#define CALLBACK_ARGS_4(cb, arg1, arg2, arg3, arg4) {if (cb != NULL) cb(arg1, arg2, arg3, arg4);}

#endif // _APP_CALLBACK_H_

///@}
///@}

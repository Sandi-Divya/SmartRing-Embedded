/**
 ****************************************************************************************
 *
 * @file app_task.h
 *
 * @brief Header file for application handlers for ble events and responses.
 *
 * Copyright (C) 2012-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef APP_TASK_H_
#define APP_TASK_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "ke_task.h"
#include "ke_msg.h"
#include "gapm_task.h"
#include "gapc_task.h"
#include "proxr_task.h"
#include "diss_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// application states
enum
{
    /// Idle state
    APP_IDLE,
    /// Scanning state
    APP_CONNECTABLE,
    /// Connected state
    APP_CONNECTED,
    /// Number of defined states.
    APP_STATE_MAX,
    /// Scanning state
    APP_SCAN,
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

struct gap_ready_evt {

};

extern struct app_env_tag app_env;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Extracts device name from adv data if present and copy it to app_env.
 *
 * @param[in] adv_data      Pointer to advertise data.
 * @param[in] adv_data_len  Advertise data length.
 * @param[in] dev_indx      Devices index in device list.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
void app_find_device_name(unsigned char * adv_data, unsigned char adv_data_len, unsigned char dev_indx);

/**
 ****************************************************************************************
 * @brief Handles GAPM_CMP_EVT event for GAPM_SET_DEV_CONFIG_CMD.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapm_set_dev_config_completion_handler(ke_msg_id_t msgid,
                                           struct gapm_cmp_evt *param,
                                           ke_task_id_t dest_id,
                                           ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Handles ready indication from the GAP.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapm_device_ready_ind_handler(ke_msg_id_t msgid,
                                  struct gap_ready_evt *param,
                                  ke_task_id_t dest_id,
                                  ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Handles GAPM_ADV_REPORT_IND event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapm_adv_report_ind_handler(ke_msg_id_t msgid,
                                struct gapm_adv_report_ind *param,
                                ke_task_id_t dest_id,
                                ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Handles Connection request indication event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapc_connection_req_ind_handler(ke_msg_id_t msgid,
                                    struct gapc_connection_req_ind *param,
                                    ke_task_id_t dest_id,
                                    ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Handles Disconnection indication event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapc_disconnect_ind_handler(ke_msg_id_t msgid,
                                struct gapc_disconnect_ind *param,
                              ke_task_id_t dest_id,
                              ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Handles RSSI indication event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapc_con_rssi_ind_handler(ke_msg_id_t msgid,
                              struct gapc_con_rssi_ind *param,
                              ke_task_id_t dest_id,
                              ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Get dev info req indication event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapc_get_dev_info_req_ind_handler(ke_msg_id_t msgid,
                                      struct gapc_get_dev_info_req_ind *param,
                                      ke_task_id_t dest_id,
                                      ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Set dev info req indication event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapc_set_dev_info_req_ind_handler(ke_msg_id_t msgid,
                                      struct gapc_set_dev_info_req_ind *param,
                                      ke_task_id_t dest_id,
                                      ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Param update req indication event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapc_param_update_req_ind_handler(ke_msg_id_t msgid,
                                      struct gapc_param_update_req_ind *param,
                                      ke_task_id_t dest_id,
                                      ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Handle Bond indication.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapc_bond_ind_handler(ke_msg_id_t msgid,
                          struct gapc_bond_ind *param,
                          ke_task_id_t dest_id,
                          ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Handle reset GAP request completion event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapm_reset_completion_handler(ke_msg_id_t msgid,
                                           struct gapm_cmp_evt *param,
                                           ke_task_id_t  dest_id,
                                           ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Handles GAPM_PROFILE_ADDED_IND event.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
*/
int gapm_profile_added_ind_handler(ke_msg_id_t msgid,
                                  struct gapm_profile_added_ind *param,
                                  ke_task_id_t dest_id,
                                  ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Handles the GAPC_BOND_REQ_IND event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapc_bond_req_ind_handler(ke_msg_id_t msgid,
                              struct gapc_bond_req_ind *param,
                                  ke_task_id_t dest_id,
                                  ke_task_id_t src_id);

/**
 ****************************************************************************************
 * @brief Handles the GAPC_ENCRYPT_REQ_IND event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapc_encrypt_req_ind_handler(ke_msg_id_t const msgid,
                                 struct gapc_encrypt_req_ind * param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Handles the GAPC_ENCRYPT_IND event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int gapc_encrypt_ind_handler(ke_msg_id_t const msgid,
                             struct gapc_encrypt_ind *param,
                             ke_task_id_t const dest_id,
                             ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Handles the DISS_VALUE_REQ_IND indication.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   Id of the receiving task instance (TASK_GAP).
 * @param[in] src_id    Id of the sending task instance.
*****************************************************************************************
*/
void diss_value_req_ind_handler(ke_msg_id_t const msgid,
                                struct diss_value_req_ind *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Handles the PROXR_ALERT_IND indication.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   Id of the receiving task instance (TASK_GAP).
 * @param[in] src_id    Id of the sending task instance.
*****************************************************************************************
*/
void proxr_alert_ind_handler(ke_msg_id_t msgid,
                                 struct proxr_alert_ind  *param,
                                 ke_task_id_t dest_id,
                                 ke_task_id_t src_id);

#endif // APP_TASK_H_

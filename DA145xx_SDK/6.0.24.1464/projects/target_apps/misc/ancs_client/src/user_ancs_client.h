/**
 ****************************************************************************************
 *
 * @file user_ancs_client.h
 *
 * @brief ANCS client demo application header file.
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

#ifndef _USER_ANCC_H_
#define _USER_ANCC_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup
 *
 * @brief
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gapc_task.h"                 // gap functions and messages
#include "app_task.h"                  // application task
#include "app.h"                       // application definitions
#include "app_callback.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/* Duration of timer for connection parameter update request */
#define APP_PARAM_UPDATE_REQUEST_TO         (1000)   // 1000*10ms = 10sec, The maximum allowed value is 30000 = 300 sec

/* Advertising data update timer */
#define APP_ADV_DATA_UPDATE_TO              (3000)   // 3000*10ms = 30sec

/* Manufacturer specific data constants */
#define APP_AD_MSD_COMPANY_ID               (0xABCD)
#define APP_AD_MSD_COMPANY_ID_LEN           (2)
#define APP_AD_MSD_DATA_LEN                 (sizeof(uint16_t))

/// Passkey that is presented to the user and is entered on the peer device (MITM)
#define APP_SECURITY_MITM_PASSKEY_VAL    (123456)

/// OOB data
#define APP_SECURITY_OOB_TK_VAL          {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,\
                                          0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Application initialization function.
 ****************************************************************************************
*/
void user_app_init(void);

/**
 ****************************************************************************************
 * @brief Connection function.
 * @param[in] conidx        Connection Id index
 * @param[in] param         Pointer to GAPC_CONNECTION_REQ_IND message
 ****************************************************************************************
*/
void user_app_connection(const uint8_t conidx, struct gapc_connection_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Disconnection function.
 * @param[in] param         Pointer to GAPC_DISCONNECT_IND message
 ****************************************************************************************
*/
void user_app_disconnect(struct gapc_disconnect_ind const *param);

/**
 ****************************************************************************************
 * @brief ANC enable complete callback
 * @param[in] conidx         Connection index
 * @param[in] status         Operation status
 * @param[in] anc            Service content
 ****************************************************************************************
*/
void user_on_annc_enable_cb(uint8_t conidx, uint8_t status, const struct ancc_content *anc);

/**
 ****************************************************************************************
 * @brief Wrtite notification source configuration callback
 * @param[in] conidx         Connection index
 * @param[in] status         Operation status
 ****************************************************************************************
*/
void user_on_ancc_wr_cfg_ntf_src_cb(uint8_t condix, uint8_t status);

/**
 ****************************************************************************************
 * @brief Wrtite data source configuration callback
 * @param[in] conidx         Connection index
 * @param[in] status         Operation status
 ****************************************************************************************
*/
void user_on_ancc_wr_cfg_data_src_cb(uint8_t condix, uint8_t status);

/**
 ****************************************************************************************
 * @brief Notification source indication callback
 * @param[in] conidx         Connection index
 * @param[in] ntf            Notification parametrs
 ****************************************************************************************
*/
void user_on_ancc_ntf_src_ind_cb(uint8_t conidx, const struct anc_ntf_src *ntf);

/**
 ****************************************************************************************
 * @brief Notification Attribute indication callback
 * @param[in] conidx         Connection index
 * @param[in] uid            Notification uid
 * @param[in] att_id         Attribute ID
 * @param[in] val            Attribute value NULL terminated, NULL pointer if not present
 ****************************************************************************************
*/
void user_on_ancc_ntf_att_ind_cb(uint8_t conidx, uint32_t uid, uint8_t att_id, uint8_t *val);

/**
 ****************************************************************************************
 * @brief Notification Attribute indication callback
 * @param[in] conidx         Connection index
 * @param[in] uid            Notification uid
 * @param[in] app_id         Application ID NULL terminated
 * @param[in] val            Attribute value NULL terminated
 ****************************************************************************************
*/
void user_on_ancc_app_att_ind_cb(uint8_t conidx, uint8_t att_id, uint8_t *app_id, uint8_t *att);

/**
 ****************************************************************************************
 * @brief Get notification attribute command complete callback
 * @param[in] conidx         Connection index
 * @param[in] status         Command status
 ****************************************************************************************
*/
void user_on_ancc_get_ntf_att_cmp_cb(uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Get application attribute command complete callback
 * @param[in] conidx         Connection index
 * @param[in] status         Command status
 ****************************************************************************************
*/
void user_on_ancc_get_app_att_cmp_cb(uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Perform notification action command complete callback
 * @param[in] conidx         Connection index
 * @param[in] status         Command status
 ****************************************************************************************
*/
void user_on_ancc_perf_ntf_act_cmp_cb(uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Function called on term key exchange event.
 * @param[in] conidx    Connection Id number
 * @param[in] param     Pointer to GAPC_BOND_REQ_IND message
 ****************************************************************************************
 */
void user_app_on_tk_exch(uint8_t conidx,
                         struct gapc_bond_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Pairing complete function
 * @param[in] conidx         Connection index
 ****************************************************************************************
*/
void user_app_on_pairing_succeeded(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief GATT client enable callback
 * @param[in] conidx         Connection index
 * @param[in] status         Operation status
 * @param[in] anc            Service content
 ****************************************************************************************
*/
void user_on_gattc_enable(uint8_t conidx, uint8_t status, const struct gatt_client_content *gatt);

/**
 ****************************************************************************************
 * @brief GATT client service changed indication callback
 * @param[in] conidx      Connection index
 * @param[in] val         Indication value
 ****************************************************************************************
*/
void user_on_gattc_svc_change_indication(uint8_t conidx, struct prf_svc *val);

/**
 ****************************************************************************************
 * @brief Handles the messages that are not handled by the SDK internal mechanisms.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 ****************************************************************************************
*/
void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id);

/// @} APP

#endif //_USER_ANCC_H_

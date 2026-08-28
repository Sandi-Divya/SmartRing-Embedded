/**
 ****************************************************************************************
 *
 * @file user_lecb.h
 *
 * @brief LE Credit-Based Channels project header file.
 *
 * Copyright (C) 2023-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _USER_LECB_H_
#define _USER_LECB_H_

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

#include "gapc_task.h"
#include "l2cc_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/* Duration of timer for connection parameter update request */
#define APP_PARAM_UPDATE_REQUEST_TO         (1000)   // 1000*10ms = 10sec, The maximum allowed value is 41943sec (4194300 * 10ms)

#define USER_L2CAP_LEPSM                    (L2C_LEPSM_DYN_MIN)
#define USER_L2CAP_CID                      (L2C_CID_DYN_MIN)
#define USER_LECB_CREDITS                   (8)

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
 * @brief Undirect advertising completion function.
 * @param[in] status Command complete event message status
 ****************************************************************************************
*/
void user_app_adv_undirect_complete(uint8_t status);

/**
 ****************************************************************************************
 * @brief Disconnection function.
 * @param[in] param         Pointer to GAPC_DISCONNECT_IND message
 ****************************************************************************************
*/
void user_app_disconnect(struct gapc_disconnect_ind const *param);

/**
 ****************************************************************************************
 * @brief Function called on lecb_disconnect indication.
 * @param[in] conidx    Connection Id number
 * @param[in] param     Pointer to GAPC_LECB_DISCONNECT_IND message
 ****************************************************************************************
 */
void user_app_lecb_disconnect_ind(uint8_t conidx,
                                  struct gapc_lecb_disconnect_ind const *param);

/**
 ****************************************************************************************
 * @brief Function called on lecb_connect_req indication.
 * @param[in] conidx      Connection Id number
 * @param[in] param       Pointer to GAPC_LECB_CONNECT_REQ_IND message
 * @param[in,out] status
 ****************************************************************************************
 */
void user_app_lecb_connect_req_ind(uint8_t conidx,
                                   struct gapc_lecb_connect_req_ind const *param, uint16_t *status);

/**
 ****************************************************************************************
 * @brief Function called on lecb_connect indication.
 * @param[in] conidx      Connection Id number
 * @param[in] param       Pointer to GAPC_LECB_CONNECT_IND message
 ****************************************************************************************
 */
void user_app_lecb_connect_ind(uint8_t conidx,
                               struct gapc_lecb_connect_ind const *param);

/**
 ****************************************************************************************
 * @brief Function called on lecb_add indication.
 * @param[in] conidx      Connection Id number
 * @param[in] param       Pointer to GAPC_LECB_ADD_IND message
 ****************************************************************************************
 */
void user_app_lecb_add_ind(uint8_t conidx,
                           struct gapc_lecb_add_ind const *param);

/**
 ****************************************************************************************
 * @brief Function called on packet receive indication.
 * @param[in] conidx      Connection Id number
 * @param[in] param       Pointer to L2CC_LECNX_DATA_RECV_IND message
 ****************************************************************************************
 */
void user_app_lecb_data_recv_ind(uint8_t conidx,
                                 struct l2cc_lecnx_data_recv_ind const *param);

/**
 ****************************************************************************************
 * @brief Function called on pdu send response.
 * @param[in] conidx      Connection Id number
 * @param[in] param       Pointer to L2CC_PDU_SEND_RSP message
 ****************************************************************************************
 */
void user_app_lecb_data_send_rsp(uint8_t conidx,
                                 struct l2cc_data_send_rsp const *param);
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

#endif //_USER_LECB_H_

/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup LE_CREDIT_BASED
 * @{
 * @addtogroup APP_LE_CREDIT_BASED_TASK  APP LE Credit-Based channels Task
 * @brief Application LE Credit-Based channels Task API
 * @{
 *
 * @file app_lecb_task.h
 *
 * @brief Application LE Credit-Based Channels Task Header file
 *
 * Copyright (C) 2023 Renesas Electronics Corporation and/or its affiliates.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gapc_task.h"
#include "l2cc_task.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of disconnect indication.
 * @param[in] msgid     Id of the message received
 * @param[in] param     Pointer to the parameters of the message
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance
 * @return If the message was consumed or not
 ****************************************************************************************
 */
int app_lecb_disconnect_ind_handler(ke_msg_id_t msgid,
                                    struct gapc_lecb_disconnect_ind *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Handles reception of connect indication.
 * @param[in] msgid     Id of the message received
 * @param[in] param     Pointer to the parameters of the message
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance
 * @return If the message was consumed or not
 ****************************************************************************************
 */
int app_lecb_connect_ind_handler(ke_msg_id_t msgid,
                                 struct gapc_lecb_connect_ind *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id);


/**
 ****************************************************************************************
 * @brief Handles reception of connect request indication.
 * @param[in] msgid     Id of the message received
 * @param[in] param     Pointer to the parameters of the message
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance
 * @return If the message was consumed or not
 ****************************************************************************************
 */
int app_lecb_connect_req_ind_handler(ke_msg_id_t msgid,
                                     struct gapc_lecb_connect_req_ind *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Handles reception of add indication.
 * @param[in] msgid     Id of the message received
 * @param[in] param     Pointer to the parameters of the message
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance
 * @return If the message was consumed or not
 ****************************************************************************************
 */
int app_lecb_add_ind_handler(ke_msg_id_t msgid,
                             struct gapc_lecb_add_ind *param,
                             ke_task_id_t const dest_id,
                             ke_task_id_t const src_id);
                                      
/**
 ****************************************************************************************
 * @brief Handles reception of PDU packet receive indication.
 * @param[in] msgid     Id of the message received
 * @param[in] param     Pointer to the parameters of the message
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance
 * @return If the message was consumed or not
 ****************************************************************************************
 */
int app_lecb_recv_data_ind_handler(ke_msg_id_t msgid,
                                   struct l2cc_lecnx_data_recv_ind *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Handles reception of PDU send response.
 * @param[in] msgid     Id of the message received
 * @param[in] param     Pointer to the parameters of the message
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance
 * @return If the message was consumed or not
 ****************************************************************************************
 */
int app_lecb_pdu_send_rsp_handler(ke_msg_id_t msgid,
                                  struct l2cc_data_send_rsp *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id);

/// @}
/// @}
/// @}

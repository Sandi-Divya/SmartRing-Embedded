/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Easy_Msg_Utils Easy Msg Utils
 * @brief Message-related helper functions API
 * @{
 *
 * @file app_easy_msg_utils.h
 *
 * @brief Message related helper functions header file.
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

#ifndef _APP_EASY_MSG_UTILS_H_
#define _APP_EASY_MSG_UTILS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "ke_msg.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Clear the callback function for a given message.
 * @param[in] msg_util_id The Id of the message
 * @return Returns the message id if everything was ok
 ****************************************************************************************
 */
ke_msg_id_t app_easy_msg_free_callback(ke_msg_id_t msg_util_id);

/**
 ****************************************************************************************
 * @brief Hook a callback to a free message from a pool of messages.
 * @param[in] fn Pointer to the callback function
 * @return Returns the message id
 ****************************************************************************************
 */
ke_msg_id_t app_easy_msg_set(void (*fn)(void));

/**
 ****************************************************************************************
 * @brief Change the callback for a given message.
 * @param[in] msg_util_id The Id of the message.
 * @param[in] fn          The new pointer to the callback function.
 * @return Returns the message id, zero if no message is available.
 ****************************************************************************************
 */
ke_msg_id_t app_easy_msg_modify(ke_msg_id_t msg_util_id, void (*fn)(void));

/**
 ****************************************************************************************
 * @brief Set the callback to be invoked when the wake-up message issued from the 
 *        app_easy_wakeup function is scheduled.
 * @param[in] fn Pointer to the callback function
 ****************************************************************************************
 */
void app_easy_wakeup_set(void (*fn)(void));

/**
 ****************************************************************************************
 * @brief Remove the callback from the wake-up message handler.
 ****************************************************************************************
 */
void app_easy_wakeup_free(void);

/**
 ****************************************************************************************
 * @brief This function should be called from within the wake-up handler.
 *        It will put a message in the scheduler queue which will be called when the BLE 
 *        stack is fully awake and operationable.
 ****************************************************************************************
 */
void app_easy_wakeup(void);

/**
 ****************************************************************************************
 * @brief Process handler for the Message Utility messages.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id  ID of the sending task instance.
 * @param[in] msg_ret Result of the message handler.
 * @return Returns if the message is handled by the process handler.
 ****************************************************************************************
 */
enum process_event_response app_msg_utils_api_process_handler(ke_msg_id_t const msgid,
                                                              void const *param,
                                                              ke_task_id_t const dest_id,
                                                              ke_task_id_t const src_id, 
                                                              enum ke_msg_status_tag *msg_ret);

#endif // _APP_EASY_MSG_UTILS_H_

///@}
///@}

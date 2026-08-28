/**
 ****************************************************************************************
 *
 * @file custom_common.h
 *
 * @brief Custom Service profile common header file.
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

#ifndef __CUSTOM_COMMON_H
#define __CUSTOM_COMMON_H

#include "gattc_task.h"

/**
 ****************************************************************************************
 * @brief Validate the value of the Client Characteristic CFG.
 * @param[in] is_notification indicates whether the CFG is Notification (true) or Indication (false)
 * @param[in] param Pointer to the parameters of the message.
 * @return status.
 ****************************************************************************************
 */
int check_client_char_cfg(bool is_notification, const struct gattc_write_req_ind *param);

/**
 ****************************************************************************************
 * @brief Find the handle of the Characteristic Value having as input the Client Characteristic CFG handle
 * @param[in] cfg_handle the Client Characteristic CFG handle
 * @return the corresponding value handle
 ****************************************************************************************
 */
uint16_t get_value_handle(uint16_t cfg_handle);

/**
 ****************************************************************************************
 * @brief Find the handle of Client Characteristic CFG having as input the Characteristic value handle
 * @param[in] value_handle the Characteristic value handle
 * @return the corresponding Client Characteristic CFG handle
 ****************************************************************************************
 */
uint16_t get_cfg_handle(uint16_t value_handle);

#if (BLE_CUSTOM1_SERVER)
/**
 ****************************************************************************************
 * @brief Compute the handle of a given attribute based on its index
 * @details Specific to custom profile / service 1
 * @param[in] att_idx attribute index
 * @return the corresponding handle
 ****************************************************************************************
 */
uint16_t custs1_get_att_handle(uint8_t att_idx);

/**
 ****************************************************************************************
 * @brief Compute the handle of a given attribute based on its index
 * @details Specific to custom profile / service 1
 * @param[in] handle attribute handle
 * @param[out] att_idx attribute index
 * @return high layer error code
 ****************************************************************************************
 */
uint8_t custs1_get_att_idx(uint16_t handle, uint8_t *att_idx);
#endif // (BLE_CUSTOM1_SERVER)

#if (BLE_CUSTOM2_SERVER)
/**
 ****************************************************************************************
 * @brief Compute the handle of a given attribute based on its index
 * @details Specific to custom profile / service 2
 * @param[in] att_idx attribute index
 * @return the corresponding handle
 ****************************************************************************************
 */
uint16_t custs2_get_att_handle(uint8_t att_idx);

/**
 ****************************************************************************************
 * @brief Compute the handle of a given attribute based on its index
 * @details Specific to custom profile / service 2
 * @param[in] handle attribute handle
 * @param[out] att_idx attribute index
 * @return high layer error code
 ****************************************************************************************
 */
uint8_t custs2_get_att_idx(uint16_t handle, uint8_t *att_idx);
#endif // (BLE_CUSTOM2_SERVER)

#endif // __CUSTOM_COMMON_H

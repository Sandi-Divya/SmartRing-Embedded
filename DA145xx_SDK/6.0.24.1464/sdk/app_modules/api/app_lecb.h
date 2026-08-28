/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup LE_CREDIT_BASED LE Credit-Based channels
 * @{
 * @addtogroup APP_LE_CREDIT_BASED  APP LE Credit-Based channels
 * @brief Application LE Credit-Based channels API
 * @{
 *
 * @file app_lecb.h
 *
 * @brief LE Credit-Based channels application API.
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
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Connect to an LE credit based channel
 * @param [in] conidx          Connection index
 * @param [in] le_psm          LE Protocol/Service Multiplexer. The valid range
 *                             for LE PSM is between 0x80 - 0xFF
 * @param [in] cid             Channel identifier. The valid range for cid is
 *                             between 0x0040 - 0x007F
 * @param [in] credit          Credits on channel. The max value is 0xFFFF
 ****************************************************************************************
 */
void app_lecb_connect_channel(uint8_t conidx, uint16_t le_psm, uint16_t cid, uint16_t credit);

/**
 ****************************************************************************************
 * @brief Create an LE credit based channel
 * @param [in] conidx          Connection index
 * @param [in] sec_level       Security level
 * @param [in] le_psm          LE Protocol/Service Multiplexer. The valid range
 *                             for LE PSM is between 0x80 - 0xFF
 * @param [in] cid             Channel identifier. The valid range for cid is
 *                             between 0x0040 - 0x007F
 * @param [in] initial_credit  Initial credits on channel. The max value is 0xFFFF
 ****************************************************************************************
 */
void app_lecb_create_channel(uint8_t conidx, uint16_t sec_level, uint16_t le_psm, uint16_t cid, uint16_t initial_credit);

/**
 ****************************************************************************************
 * @brief Destroy an LE credit based channel
 * @param [in] conidx          Connection index
 * @param [in] le_psm          LE Protocol/Service Multiplexer. The valid range
 *                             for LE PSM is between 0x80 - 0xFF
 ****************************************************************************************
 */
void app_lecb_destroy_channel(uint8_t conidx, uint16_t le_psm);

/**
 ****************************************************************************************
 * @brief Disconnect from an LE credit based channel
 * @param [in] conidx          Connection index
 * @param [in] le_psm          LE Protocol/Service Multiplexer. The valid range
 *                             for LE PSM is between 0x80 - 0xFF
 ****************************************************************************************
 */
void app_lecb_disconnect_channel(uint8_t conidx, uint16_t le_psm);

/**
 ****************************************************************************************
 * @brief Add credits to an LE credit based channel
 * @param [in] conidx          Connection index
 * @param [in] le_psm          LE Protocol/Service Multiplexer. The valid range
                               for LE PSM is between 0x80 - 0xFF
 * @param [in] credit          Number of credits to be added. The max value is 0xFFFF
 ****************************************************************************************
 */
void app_lecb_add_credits(uint8_t conidx, uint16_t le_psm, uint16_t credit);

/**
 ****************************************************************************************
 * @brief Send SDU data over an LE credit based channel
 * @param [in] conidx          Connection index
 * @param [in] cid             Channel identifier. The valid range for cid is
 *                             between 0x0040 - 0x007F
 * @param [in] length          Length of data to be sent
 * @param [in] data            Data to be sent
 ****************************************************************************************
 */
void app_lecb_send_sdu(uint8_t conidx, uint16_t cid, uint16_t length, const void *data);

/// @}
/// @}
/// @}

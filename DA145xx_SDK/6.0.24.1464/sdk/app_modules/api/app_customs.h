/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup CUSTOMS Custom Profile
 * @brief Custom Profile Server Application API
 * @{
 *
 * @file app_customs.h
 *
 * @brief Custom Service Application entry point.
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

#ifndef _APP_CUSTOMS_H_
#define _APP_CUSTOMS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include "user_profiles_config.h"

#if (BLE_CUSTOM_SERVER)

#include <stdint.h>
#include "prf_types.h"
#include "attm_db_128.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Custom Service Application.
 ****************************************************************************************
 */
void app_custs1_init(void);

/**
 ****************************************************************************************
 * @brief Add a Custom Service instance to the DB.
 ****************************************************************************************
 */
void app_custs1_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable the Custom Service.
 * @param[in] conhdl    Connection handle
 ****************************************************************************************
 */
void app_custs1_enable(uint16_t conhdl);

/**
 ****************************************************************************************
 * @brief It is called to validate the characteristic value before it is written
 *        to profile database.
 * @param[in] att_idx Attribute index to be validated before it is written to database.
 * @param[in] offset  Offset at which the data has to be written (used only for values
 *                    that are greater than MTU).
 * @param[in] len     Data length to be written.
 * @param[in] value   Pointer to data to be written to attribute database.
 * @return validation status (high layer error code)
 ****************************************************************************************
 */
uint8_t app_custs1_val_wr_validate(uint16_t att_idx, uint16_t offset, uint16_t len, uint8_t *value);

/**
 ****************************************************************************************
 * @brief Initialize Custom Service Application.
 ****************************************************************************************
 */
void app_custs2_init(void);

/**
 ****************************************************************************************
 * @brief Add a Custom Service instance to the DB.
 ****************************************************************************************
 */
void app_custs2_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable the Custom Service.
 * @param[in] conhdl    Connection handle
 ****************************************************************************************
 */
void app_custs2_enable(uint16_t conhdl);

/**
 ****************************************************************************************
 * @brief It is called to validate the characteristic value before it is written
 *        to profile database.
 * @param[in] att_idx Attribute index to be validated before it is written to database.
 * @param[in] offset  Offset at which the data has to be written (used only for values
 *                    that are greater than MTU).
 * @param[in] len     Data length to be written.
 * @param[in] value   Pointer to data to be written to attribute database.
 * @return validation status (high layer error code)
 ****************************************************************************************
 */
uint8_t app_custs2_val_wr_validate(uint16_t att_idx, uint16_t offset, uint16_t len, uint8_t *value);

/**
 ****************************************************************************************
 * @brief Function returns user callback structure for custom tasks.
 * @param[in] task_id   Task id TASK_ID_CUSTS1 or TASK_ID_CUSTS2
 * @return status       user callback structure or NULL if not present
 ****************************************************************************************
 */
const struct cust_prf_func_callbacks *custs_get_func_callbacks(enum KE_API_ID task_id);

#endif // (BLE_CUSTOM_SERVER)

#endif // _APP_CUSTOMS_H_

///@}
///@}
///@}

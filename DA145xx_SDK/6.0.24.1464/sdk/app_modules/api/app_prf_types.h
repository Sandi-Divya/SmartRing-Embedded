/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Prf_Types Profile Types
 * @brief Application Profile related types
 * @{
 *
 * @file app_prf_types.h
 *
 * @brief app - profiles related header file.
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

#ifndef APP_PRF_TYPES_H_
#define APP_PRF_TYPES_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Format of a profile initialization / db creation function
typedef void (*prf_func_void_t)(void);
/// Format of a profile enable function
typedef void (*prf_func_uint8_t)(uint8_t);
/// Format of a profile validation function
typedef uint8_t (*prf_func_validate_t)(uint16_t, uint16_t, uint16_t, uint8_t*);

/// Structure of profile call back function table.
struct prf_func_callbacks
{
    /// Profile Task ID.
    enum KE_API_ID       task_id;
    /// Pointer to the database create function
    prf_func_void_t         db_create_func;
    /// Pointer to the profile enable function
    prf_func_uint8_t       enable_func;
};

/// Structure of custom profile call back function table.
struct cust_prf_func_callbacks
{
    /// Profile Task ID.
    enum KE_API_ID             task_id;
    /// pointer to the custom database table defined by user
    const struct attm_desc_128 *att_db;
    /// max number of attributes in custom database
    const uint8_t              max_nb_att;
    /// Pointer to the custom database create function defined by user
    prf_func_void_t            db_create_func;
    /// Pointer to the custom profile enable function defined by user
    prf_func_uint8_t           enable_func;
    /// Pointer to the custom profile initialization function
    prf_func_void_t            init_func;
    /// Pointer to the validation function defined by user
    prf_func_validate_t        value_wr_validation_func;
};

#endif //APP_PRF_TYPES_H_

///@}
///@}

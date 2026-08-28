/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Prf_Perm_Types Profile Permission Types
 * @brief Application Profile permissions related API
 * @{
 *
 * @file app_prf_perm_types.h
 *
 * @brief app - Service access permission rights api.
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

#ifndef _APP_PRF_PERM_TYPES_H_
#define _APP_PRF_PERM_TYPES_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "attm.h"                       // Service access permissions

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Service access rights
typedef enum app_prf_srv_perm
{
    /// Disable access
    SRV_PERM_DISABLE  = PERM(SVC_AUTH, DISABLE),
    /// Enable access
    SRV_PERM_ENABLE   = PERM(SVC_AUTH, ENABLE),
    /// Access Requires Unauthenticated link
    SRV_PERM_UNAUTH   = PERM(SVC_AUTH, UNAUTH),
    /// Access Requires Authenticated link
    SRV_PERM_AUTH     = PERM(SVC_AUTH, AUTH),
    /// Access Requires Secure Connection Pairing
    SRV_PERM_SECURE   = PERM(SVC_AUTH, SECURE),
} app_prf_srv_perm_t;

/// Profile access rights
typedef struct app_prf_srv_sec
{
    /// Profile
    enum KE_API_ID        task_id;

    /// Access rights
    app_prf_srv_perm_t    perm;
    
} app_prf_srv_sec_t;

/// Pointer to profile access rights
extern app_prf_srv_sec_t app_prf_srv_perm[];

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of profiles (TASK_IDs) that can be included in a user application
#define PRFS_TASK_ID_MAX    (10)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Returns the Service permission set by user. If user has not set any service
 * permission, the default "ENABLE" is used.
 * @param[in] task_id           Task type definition of the profile.
 * @return                      service access rights
 ****************************************************************************************
 */
app_prf_srv_perm_t get_user_prf_srv_perm(enum KE_API_ID task_id);

/**
 ****************************************************************************************
 * @brief Sets the service permission access rights for a profile.
 * @param[in] task_id           Task type definition of the profile.
 * @param[in] srv_perm          Service permission access rights of the profile.
 ****************************************************************************************
 */
void app_set_prf_srv_perm(enum KE_API_ID task_id, app_prf_srv_perm_t srv_perm);

/**
 ****************************************************************************************
 * @brief Initialises service permissions to "ENABLE". Should be called once upon app
 * initialization.
 ****************************************************************************************
 */
void prf_init_srv_perm(void);

#endif // _APP_PRF_TYPES_H_

///@}
///@}

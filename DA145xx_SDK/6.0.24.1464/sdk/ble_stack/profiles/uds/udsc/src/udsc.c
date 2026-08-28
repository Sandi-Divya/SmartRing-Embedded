/**
 ****************************************************************************************
 *
 * @file udsc.c
 *
 * @brief C file - User Data Service Client Implementation.
 *
 * Copyright (C) 2015-2023 Renesas Electronics Corporation and/or its affiliates.
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

/**
 ****************************************************************************************
 * @addtogroup UDSC
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_UDS_CLIENT)

#include <string.h>

#include "udsc.h"
#include "udsc_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static void udsc_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason);

/**
 ****************************************************************************************
 * @brief Initialization of the UDSC module.
 * This function performs all the initializations of the Profile module.
 *  - Creation of database (if it's a service)
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    env        Client or Service allocated environment data.
 * @param[in|out] start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     app_task   Application task number.
 * @param[in]     sec_lvl    Security level (AUTH, EKS and MI field of @enum attm_svc_perm_mask)
 * @param[in]     param      Configuration parameters of profile collector or service (32 bits aligned)
 *
 * @return status code to know if profile initialization succeed or not.
 ****************************************************************************************
 */
static uint8_t udsc_init(struct prf_task_env* env, uint16_t* start_hdl,
                         uint16_t app_task, uint8_t sec_lvl, void* params)
{
    struct udsc_env_tag* udsc_env =
            (struct udsc_env_tag*) ke_malloc(sizeof(struct udsc_env_tag), KE_MEM_ATT_DB);
    memset(udsc_env, 0, sizeof(struct udsc_env_tag));

    // allocate UDSC required environment variable
    env->env = (prf_env_t*) udsc_env;

    udsc_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
    udsc_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);

    // initialize environment variable
    env->id                     = TASK_ID_UDSC;
    env->desc.idx_max           = UDSC_IDX_MAX;
    env->desc.state             = udsc_env->state;
    env->desc.state_handler     = NULL;
    env->desc.state_max         = UDSC_STATE_MAX;
    env->desc.default_handler   = &udsc_default_handler;

    // initial state for all connection handling instances
    for(uint8_t idx = 0; idx < UDSC_IDX_MAX; idx++)
    {
        udsc_env->env[idx] = NULL;
        // service is ready, go into the Free state
        ke_state_set(KE_BUILD_ID(env->task, idx), UDSC_FREE);
    }

    return GAP_ERR_NO_ERROR;
}

/**
 ****************************************************************************************
 * @brief Destruction of the UDSC module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Client or Service allocated environment data.
 ****************************************************************************************
 */
static void udsc_destroy(struct prf_task_env* env)
{
    struct udsc_env_tag* udsc_env = (struct udsc_env_tag*) env->env;

    // cleanup environment variable for each task instances
    for(uint8_t idx = 0; idx < UDSC_IDX_MAX; idx++)
    {
        if(udsc_env->env[idx] != NULL)
        {
            udsc_cleanup(env, idx, 0);
        }
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(udsc_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void udsc_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Put UDS Client for this particular connection in Idle state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), UDSC_IDLE);
}

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void udsc_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct udsc_env_tag* udsc_env = (struct udsc_env_tag*) env->env;

    // Stop the UCP operation timeout timer
    if (udsc_env->env[conidx]->op_code != UDS_NO_OP)
    {
        KE_TIMER_CLEAR(UDSC_TIMEOUT_TIMER_IND,
                       prf_src_task_get(&(udsc_env->prf_env), conidx));
    }

    // clean-up environment variable allocated for task instance
    if(udsc_env->env[conidx] != NULL)
    {
        ke_free(udsc_env->env[conidx]);
        udsc_env->env[conidx] = NULL;
    }

    /* Put UDS Client in Free state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), UDSC_FREE);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// UDSC Task interface required by profile manager
const struct prf_task_cbs udsc_itf =
{
    (prf_init_fnct) udsc_init,
    udsc_destroy,
    udsc_create,
    udsc_cleanup,
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* udsc_prf_itf_get(void)
{
   return &udsc_itf;
}

void udsc_enable_rsp_send(struct udsc_env_tag *udsc_env, uint8_t conidx, uint8_t status)
{
    //format response to app
    struct udsc_enable_rsp *rsp = KE_MSG_ALLOC(UDSC_ENABLE_RSP,
                                               prf_dst_task_get(&(udsc_env->prf_env), conidx),
                                               prf_src_task_get(&(udsc_env->prf_env), conidx),
                                               udsc_enable_rsp);

    rsp->status = status;

    if (status == GAP_ERR_NO_ERROR)
    {
        rsp->uds = udsc_env->env[conidx]->uds;

        // Register UDSC task in gatt for indication/notifications
        prf_register_atthdl2gatt(&(udsc_env->prf_env), conidx,
                                 &(udsc_env->env[conidx]->uds.svc));

        // Go to IDLE state
        ke_state_set(prf_src_task_get(&(udsc_env->prf_env), conidx), UDSC_IDLE);
    }
    else
    {
        // clean-up environment variable allocated for task instance
        ke_free(udsc_env->env[conidx]);
        udsc_env->env[conidx] = NULL;
    }

    KE_MSG_SEND(rsp);
}

uint8_t udsc_validate_request(struct udsc_env_tag *udsc_env, uint8_t conidx,
                              uint8_t char_code)
{
    uint8_t status = GAP_ERR_NO_ERROR;

    // check if characteristic exists
    if (udsc_env->env[conidx]->uds.chars[char_code].val_hdl == ATT_INVALID_HANDLE)
    {
        status = PRF_ERR_INEXISTENT_HDL;
    }

    return status;
}

void udsc_cmp_evt_ind_send(struct udsc_env_tag *udsc_env, uint8_t conidx, uint8_t status)
{
    struct udsc_cmp_evt_ind *ind = KE_MSG_ALLOC(UDSC_CMP_EVT_IND,
                                                prf_dst_task_get(&(udsc_env->prf_env), conidx),
                                                prf_src_task_get(&(udsc_env->prf_env), conidx),
                                                udsc_cmp_evt_ind);

    // It will be an PRF status code
    ind->status = status;

    // Send the message
    KE_MSG_SEND(ind);
}

void udsc_read_char_val_rsp_send(struct udsc_env_tag *udsc_env, uint8_t conidx,
                                 uint8_t status, uint8_t char_code, uint16_t length,
                                 const uint8_t *value)
{
    struct udsc_read_char_val_cfm *rsp = ke_msg_alloc(UDSC_READ_CHAR_VAL_CFM,
                                                      prf_dst_task_get(&(udsc_env->prf_env), conidx),
                                                      prf_src_task_get(&(udsc_env->prf_env), conidx),
                                                      sizeof(struct udsc_read_char_val_cfm) + length);

    rsp->char_code = char_code;
    rsp->status = status;

    rsp->val_len = length;

    // Fill in the data if success
    if (status == GAP_ERR_NO_ERROR)
    {
        memcpy(rsp->val, value, length);
    }

    KE_MSG_SEND(rsp);
}

void udsc_set_char_val_cfm_send(struct udsc_env_tag *udsc_env, uint8_t conidx,
                                uint8_t status)
{
    struct udsc_set_char_val_cfm *cfm;

    cfm = KE_MSG_ALLOC(UDSC_SET_CHAR_VAL_CFM,
                       prf_dst_task_get(&(udsc_env->prf_env), conidx),
                       prf_src_task_get(&(udsc_env->prf_env), conidx),
                       udsc_set_char_val_cfm);

    cfm->status = status;

    KE_MSG_SEND(cfm);
}

#endif // (BLE_UDS_CLIENT)

/// @} UDSC

/**
 ****************************************************************************************
 *
 * @file plxs.c
 *
 * @brief Pulse Oximeter Service Server implementation.
 *
 * Copyright (C) 2017-2024 Renesas Electronics Corporation and/or its affiliates.
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
 * @addtogroup PLXS
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_PLX_SERVER)
#include <string.h>

#include "attm.h"
#include "plxs.h"
#include "plxs_task.h"
#include "co_utils.h"
#include "prf_utils.h"



/*
 * HTPT PROFILE ATTRIBUTES
 ****************************************************************************************
 */
/// Full PLX Database Description - Used to add attributes into the database
const struct attm_desc plxs_att_db[PLX_IDX_NB] =
{
    // Pulse Oximeter Service Declaration
    [PLX_IDX_SVC]                   =   {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0},

    // PLX Spot-Check Measurement Characteristic Declaration
    [PLX_IDX_SPOT_MEAS_CHAR]        =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    // Spot-Check Measurement Characteristic Value
    [PLX_IDX_SPOT_MEAS_VAL]         =   {ATT_CHAR_PLX_SPOT_MEAS, PERM(IND, ENABLE), PERM(RI, ENABLE)},
    // Spot-Check Measurement Characteristic - Client Characteristic Configuration Descriptor
    [PLX_IDX_SPOT_MEAS_IND_CFG]     =   {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE)|PERM(WR, ENABLE)|PERM(WRITE_REQ, ENABLE), 0},

    // PLX Continuous Measurement Characteristic Declaration
    [PLX_IDX_CONT_MEAS_CHAR]        =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    // Intermediate Measurement Characteristic Value
    [PLX_IDX_CONT_MEAS_VAL]         =   {ATT_CHAR_PLX_CONT_MEAS, PERM(NTF, ENABLE), PERM(RI, ENABLE)},
    // Intermediate Measurement Characteristic - Client Characteristic Configuration Descriptor
    [PLX_IDX_CONT_MEAS_NTF_CFG]     =   {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE)|PERM(WR, ENABLE)|PERM(WRITE_REQ, ENABLE), 0},

    // PLX Features Characteristic Declaration
    [PLX_IDX_FEAT_CHAR]             =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    // PLX Features Characteristic Value
    [PLX_IDX_FEAT_VAL]              =   {ATT_CHAR_PLX_FEAT, PERM(RD, ENABLE), PERM(RI, ENABLE)},
};

static uint16_t plxs_compute_att_table(uint16_t features);

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static uint8_t plxs_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                            uint8_t sec_lvl, struct plxs_db_cfg* params)
{
    // Service content flag
    uint16_t cfg_flag;
    // DB Creation Status
    uint8_t status;

    cfg_flag = plxs_compute_att_table(params->features);

    status = attm_svc_create_db(start_hdl, ATT_SVC_PULSE_OXIMETER, (uint8_t *)&cfg_flag,
               PLX_IDX_NB, NULL, env->task, &plxs_att_db[0],
              (sec_lvl & PERM_MASK_SVC_AUTH) | (sec_lvl & PERM_MASK_SVC_EKS) | PERM(SVC_PRIMARY, ENABLE)
              | PERM(SVC_MI, DISABLE) );

    if( status == ATT_ERR_NO_ERROR )
    {
        //-------------------- allocate memory required for the profile  ---------------------
        struct plxs_env_tag* plxs_env =
                (struct plxs_env_tag* ) ke_malloc(sizeof(struct plxs_env_tag), KE_MEM_ATT_DB);
        memset(plxs_env, 0, sizeof(struct plxs_env_tag));

        // allocate required environment variable
        env->env = (prf_env_t*) plxs_env;

        plxs_env->shdl     = *start_hdl;
        plxs_env->prf_env.app_task = app_task
                        | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        plxs_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id                     = TASK_ID_PLXS;
        env->desc.idx_max           = PLXS_IDX_MAX;
        env->desc.state             = plxs_env->state;
        env->desc.default_handler   = &plxs_default_handler;

        //Save features on the environment
        plxs_env->features      = params->features;
        plxs_env->operation     = NULL;
        memset(plxs_env->ntf_ind_cfg, 0 , sizeof(plxs_env->ntf_ind_cfg));

        // service is ready, go into an Idle state
        ke_state_set(env->task, PLXS_IDLE);
    }
    return status;
}

static void plxs_destroy(struct prf_task_env* env)
{
    struct plxs_env_tag* plxs_env = (struct plxs_env_tag*) env->env;

    // free profile environment variables
    if(plxs_env->operation != NULL)
    {
        ke_free(plxs_env->operation);
    }


    env->env = NULL;
    ke_free(plxs_env);
}

static void plxs_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Clear configuration for this connection */
    struct plxs_env_tag* plxs_env = (struct plxs_env_tag*) env->env;
    plxs_env->ntf_ind_cfg[conidx] = 0;
    
    return;
}

static void plxs_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    return;
}

/**
 ****************************************************************************************
 * @brief Compute a flag allowing to know attributes to add into the database
 *
 * @return a 16-bit flag whose each bit matches an attribute. If the bit is set to 1, the
 * attribute will be added into the database. 
 ****************************************************************************************
 */

static uint16_t plxs_compute_att_table(uint16_t features)
{
    //PLX Features Characteristic is mandatory
    uint16_t att_table = PLXS_FEAT_CHAR_MASK | PLXS_SERV_DECL_MASK;

    //Check if Spot-Check Measurement Characteristic is supported
    if (PLXS_IS_FEATURE_SUPPORTED(features, PLXS_SPOT_MEAS_CHAR_SUP))
    {
        att_table |= PLXS_SPOT_MEAS_MASK;
    }

    //Check if Continuous Measurement Characteristic is supported
    if (PLXS_IS_FEATURE_SUPPORTED(features, PLXS_CONT_MEAS_CHAR_SUP))
    {
        att_table |= PLXS_CONT_MEAS_MASK;
    }

    return att_table;
}


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// PLXS Task interface required by profile manager
const struct prf_task_cbs plxs_itf =
{
    (prf_init_fnct) plxs_init,
    plxs_destroy,
    plxs_create,
    plxs_cleanup,
};


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
uint8_t plxs_pack_spot_meas_value(uint8_t *packed_spot_meas, struct plx_spot_check_meas spot_meas)
{
    volatile uint8_t cursor = 0;

    *(packed_spot_meas + cursor) = spot_meas.flags;
    cursor += 1;

    co_write16p(packed_spot_meas + cursor, spot_meas.sp02);
    cursor += 2;

    co_write16p(packed_spot_meas + cursor, spot_meas.pr);
    cursor += 2;
    
    //Timestamp Flag Set
    if ((spot_meas.flags & PLXS_SPOT_MEAS_FLAGS_TIMESTAMP_PRESENT) == PLXS_SPOT_MEAS_FLAGS_TIMESTAMP_PRESENT)
    {
        cursor += prf_pack_date_time(packed_spot_meas + cursor, &(spot_meas.time_stamp));
    }

    //Measurement Status Flag Set
    if ((spot_meas.flags & PLXS_SPOT_MEAS_FLAGS_MEAS_STAT_PRESENT) == PLXS_SPOT_MEAS_FLAGS_MEAS_STAT_PRESENT)
    {
        co_write16p(packed_spot_meas + cursor, spot_meas.meas_stat);
        cursor += 2;
    }

    //Device Sensor Status Flag Set
    if ((spot_meas.flags & PLXS_SPOT_MEAS_FLAGS_DEV_SENS_STAT_PRESENT) == PLXS_SPOT_MEAS_FLAGS_DEV_SENS_STAT_PRESENT)
    {
        co_write24p(packed_spot_meas + cursor, spot_meas.dev_sens_stat);
        cursor += 3;
    }
    
    //Pulse Amplitude Index Flag Set
    if ((spot_meas.flags & PLXS_SPOT_MEAS_FLAGS_PA_IDX_PRESENT) == PLXS_SPOT_MEAS_FLAGS_PA_IDX_PRESENT)
    {
        co_write16p(packed_spot_meas + cursor, spot_meas.pa_idx);
        cursor += 2;
    }
    
    //Clear unused packet data
    if(cursor < PLXS_SPOT_MEAS_MAX_LEN)
    {
        memset(packed_spot_meas + cursor, 0, (PLXS_SPOT_MEAS_MAX_LEN - cursor));
    }

    return cursor;
}

uint8_t plxs_pack_cont_meas_value(uint8_t *packed_cont_meas, struct plx_cont_meas cont_meas)
{
    uint8_t cursor = 0;

    *(packed_cont_meas + cursor) = cont_meas.flags;
    cursor += 1;

    co_write16p(packed_cont_meas + cursor, cont_meas.sp02pr_normal.sp02);
    cursor += 2;

    co_write16p(packed_cont_meas + cursor, cont_meas.sp02pr_normal.pr);
    cursor += 2;
    
    //SpO2PR-Fast Flag Set
    if ((cont_meas.flags & PLXS_CONT_MEAS_FLAGS_FAST_FIELD_PRESENT) == PLXS_CONT_MEAS_FLAGS_FAST_FIELD_PRESENT)
    {
            co_write16p(packed_cont_meas + cursor, cont_meas.sp02pr_fast.sp02);
            cursor += 2;

            co_write16p(packed_cont_meas + cursor, cont_meas.sp02pr_fast.pr);
            cursor += 2;
    }

    //SpO2PR-Slow Flag Set
    if ((cont_meas.flags & PLXS_CONT_MEAS_FLAGS_SLOW_FIELD_PRESENT) == PLXS_CONT_MEAS_FLAGS_SLOW_FIELD_PRESENT)
    {
            co_write16p(packed_cont_meas + cursor, cont_meas.sp02pr_slow.sp02);
            cursor += 2;

            co_write16p(packed_cont_meas + cursor, cont_meas.sp02pr_slow.pr);
            cursor += 2;
    }
    
    //Measurement Status Flag Set
    if ((cont_meas.flags & PLXS_CONT_MEAS_FLAGS_MEAS_STAT_PRESENT) == PLXS_CONT_MEAS_FLAGS_MEAS_STAT_PRESENT)
    {
        co_write16p(packed_cont_meas + cursor, cont_meas.meas_stat);
        cursor += 2;
    }
    
    //Device Sensor Status Flag Set
    if ((cont_meas.flags & PLXS_CONT_MEAS_FLAGS_DEV_SENS_STAT_PRESENT) == PLXS_CONT_MEAS_FLAGS_DEV_SENS_STAT_PRESENT)
    {
        co_write24p(packed_cont_meas + cursor, cont_meas.dev_sens_stat);
        cursor += 3;
    }
    
    //Pulse Amplitude Index Flag Set
    if ((cont_meas.flags & PLXS_CONT_MEAS_FLAGS_PA_IDX_FIELD_PRESENT) == PLXS_CONT_MEAS_FLAGS_PA_IDX_FIELD_PRESENT)
    {
        co_write16p(packed_cont_meas + cursor, cont_meas.pa_idx);
        cursor += 2;
    }
    
    //Clear unused packet data
    if(cursor < PLXS_CONT_MEAS_MAX_LEN)
    {
        memset(packed_cont_meas + cursor, 0, (PLXS_CONT_MEAS_MAX_LEN - cursor));
    }

    return cursor;
}

void plxs_exe_operation(void)
{
    struct plxs_env_tag* plxs_env = PRF_ENV_GET(PLXS, plxs);

    ASSERT_ERROR(plxs_env->operation != NULL);

    bool finished = true;

    while(plxs_env->operation->cursor < BLE_CONNECTION_MAX)
    {
        // check if this type of event is enabled
        if(((plxs_env->ntf_ind_cfg[plxs_env->operation->cursor] & plxs_env->operation->op) != 0)
            // and event not filtered on current connection
            && (plxs_env->operation->conidx != plxs_env->operation->cursor))
        {
            // trigger the event
            struct gattc_send_evt_cmd * evt = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                    KE_BUILD_ID(TASK_GATTC , plxs_env->operation->cursor), prf_src_task_get(&plxs_env->prf_env, 0),
                    gattc_send_evt_cmd, plxs_env->operation->length);

            evt->operation = (plxs_env->operation->op != PLXS_CFG_SPOT_MEAS_IND) ? GATTC_NOTIFY : GATTC_INDICATE;
            evt->length    = plxs_env->operation->length;
            evt->handle    = plxs_env->operation->handle;
            memcpy(evt->value, plxs_env->operation->data, evt->length);

            ke_msg_send(evt);

            finished = false;
            plxs_env->operation->cursor++;
            break;
        }
        plxs_env->operation->cursor++;
    }


    // check if operation is finished
    if(finished)
    {
        // do not send response if operation has been locally requested
        if(plxs_env->operation->dest_id != prf_src_task_get(&plxs_env->prf_env, 0))
        {
            //Stub for locally requested operations
        }

        // free operation
        ke_free(plxs_env->operation);
        plxs_env->operation = NULL;
        // go back to idle state
        ke_state_set(prf_src_task_get(&(plxs_env->prf_env), 0), PLXS_IDLE);
    }
}

uint8_t plxs_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value)
{
    struct plxs_env_tag* plxs_env = PRF_ENV_GET(PLXS, plxs);
    uint8_t status = GAP_ERR_NO_ERROR;

    if((value != valid_val) && (value != PRF_CLI_STOP_NTFIND))
    {
        status = PRF_APP_ERROR;

    }
    else if (value == valid_val)
    {
        plxs_env->ntf_ind_cfg[conidx] |= cfg;
    }
    else
    {
        plxs_env->ntf_ind_cfg[conidx] &= ~cfg;
    }

    if(status == GAP_ERR_NO_ERROR)
    {
        // inform application that notification/indication configuration has changed
        struct plxs_cfg_indntf_ind * ind = KE_MSG_ALLOC(PLXS_CFG_INDNTF_IND,
                prf_dst_task_get(&plxs_env->prf_env, conidx), prf_src_task_get(&plxs_env->prf_env, conidx),
                plxs_cfg_indntf_ind);
        ind->conidx      = conidx;
        ind->ntf_ind_cfg = plxs_env->ntf_ind_cfg[conidx];
        ke_msg_send(ind);
    }

    return status;
}

const struct prf_task_cbs* plxs_prf_itf_get(void)
{
   return &plxs_itf;
}

uint16_t plxs_att_hdl_get(struct plxs_env_tag* plxs_env, uint8_t att_idx)
{
    volatile uint16_t handle = plxs_env->shdl;

    do
    {
        // Spot-Check Measurement Characteristic supported
        if((PLXS_IS_FEATURE_SUPPORTED(plxs_env->features, PLXS_SPOT_MEAS_CHAR_SUP)) && (att_idx > PLX_IDX_SPOT_MEAS_IND_CFG))
        {
            handle += PLXS_SPOT_MEAS_ATT_NB;
        }
        else if(!PLXS_IS_FEATURE_SUPPORTED(plxs_env->features, PLXS_SPOT_MEAS_CHAR_SUP))
        {
            handle = ATT_INVALID_HANDLE;
            break;
        }
        else
        {
            handle += att_idx - PLX_IDX_SVC;
            break;
        }

        // Continuous Measurement Characteristic supported
        if((PLXS_IS_FEATURE_SUPPORTED(plxs_env->features, PLXS_CONT_MEAS_CHAR_SUP)) && (att_idx > PLX_IDX_CONT_MEAS_NTF_CFG))
        {
            handle += PLXS_CONT_MEAS_ATT_NB;
        }
        else if(!PLXS_IS_FEATURE_SUPPORTED(plxs_env->features, PLXS_CONT_MEAS_CHAR_SUP))
        {
            handle = ATT_INVALID_HANDLE;
            break;
        }
        else
        {
            handle += att_idx - PLX_IDX_SPOT_MEAS_IND_CFG;
            break;
        }
       
        // Measurement Interval
        if (att_idx >= PLX_IDX_NB)
        {
            handle = ATT_INVALID_HANDLE;
            break;
        }
        else
        {
            handle += att_idx - PLX_IDX_FEAT_CHAR;
            break;
        }
    } while (0);

    return handle;
}

uint8_t plxs_att_idx_get(struct plxs_env_tag* plxs_env, uint16_t handle)
{
    uint16_t handle_ref = plxs_env->shdl;
    uint8_t att_idx = ATT_INVALID_IDX;

    do
    {
        // not valid handle
        if(handle < handle_ref)
        {
            break;
        }

        // Spot-Check Measurement
        if(PLXS_IS_FEATURE_SUPPORTED(plxs_env->features, PLXS_SPOT_MEAS_CHAR_SUP))
        {
            handle_ref += PLXS_SPOT_MEAS_ATT_NB;

            if(handle < handle_ref)
            {
                att_idx = PLX_IDX_SPOT_MEAS_IND_CFG - (handle_ref - handle);
                break;
            }
        }

        // Intermediate Temperature Measurement
        if(PLXS_IS_FEATURE_SUPPORTED(plxs_env->features, PLXS_CONT_MEAS_CHAR_SUP))
        {
            handle_ref += PLXS_CONT_MEAS_ATT_NB;

            if(handle < handle_ref)
            {
                att_idx = PLX_IDX_CONT_MEAS_NTF_CFG - (handle_ref - handle);
                break;
            }
        }

        // Mandatory attribute handle
        handle_ref += PLXS_FEAT_ATT_NB;

        if(handle <= handle_ref)
        {
            att_idx = PLX_IDX_FEAT_VAL - (handle_ref - handle);
            break;
        }              
    } while (0);

    return att_idx;
}

#endif //BLE_PLX_SERVER

/// @} PLXS

/**
 ****************************************************************************************
 *
 * @file cgms.c
 *
 * @brief Continuous Glucose Monitoring Profile Server Implementation.
 *
 * Copyright (C) 2022-2024 Renesas Electronics Corporation and/or its affiliates.
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
 * @addtogroup CGMS
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_CGM_SERVER)
#include <string.h>

#include "attm.h"
#include "cgms.h"
#include "cgms_task.h"
#include "co_utils.h"
#include "prf_utils.h"



/*
 * CGMS PROFILE ATTRIBUTES
 ****************************************************************************************
 */
/// Full CGM Database Description - Used to add attributes into the database
const struct attm_desc cgms_att_db[CGM_IDX_NB] =
{
    // Continuous Glucose Monitoring Service Declaration
    [CGM_IDX_SVC]                           =   {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0},

    // CGM Measurement Characteristic Declaration
    [CGM_IDX_MEASUREMENT_CHAR]              =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    // CGM Measurement Characteristic Value
    [CGM_IDX_MEASUREMENT_VAL]               =   {ATT_CHAR_CGM_MEASUREMENT, PERM(NTF, AUTH), PERM(RI, ENABLE)},
    // CGM Measurement Characteristic - Client Characteristic Configuration Descriptor
    [CGM_IDX_MEASUREMENT_NTF_CFG]           =   {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE)|PERM(WR, AUTH)|PERM(WRITE_REQ, ENABLE), 0},

    // CGM Feature Characteristic Declaration
    [CGM_IDX_FEAT_CHAR]                     =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    // CGM Feature Characteristic Value
    [CGM_IDX_FEAT_VAL]                      =   {ATT_CHAR_CGM_FEATURE, PERM(RD, AUTH), PERM(RI, ENABLE)},
    
    // CGM Status Characteristic Declaration
    [CGM_IDX_STATUS_CHAR]                   =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    // CGM Status Characteristic Value
    [CGM_IDX_STATUS_VAL]                    =   {ATT_CHAR_CGM_STATUS, PERM(RD, AUTH), PERM(RI, ENABLE) | CGM_STATUS_VAL_MAX_LEN},
    
    // CGM Session Start Time Characteristic Declaration
    [CGM_IDX_SESSION_START_TIME_CHAR]       =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    // CGM Session Start Time Characteristic Value
    [CGM_IDX_SESSION_START_TIME_VAL]        =   {ATT_CHAR_CGM_SESSION_START_TIME, PERM(RD, AUTH)|PERM(WR, AUTH)|PERM(WRITE_REQ, ENABLE), PERM(RI, ENABLE) | CGMS_SESSION_START_TIME_VAL_LENGTH},
    
    // CGM Session Run Time Characteristic Declaration
    [CGM_IDX_SESSION_RUN_TIME_CHAR]         =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    // CGM Session Run Time Characteristic Value
    [CGM_IDX_SESSION_RUN_TIME_VAL]          =   {ATT_CHAR_CGM_SESSION_RUN_TIME, PERM(RD, AUTH), PERM(RI, ENABLE)},
    
    // CGM Record Access Control Point Characteristic Declaration
    [CGM_IDX_RACP_CHAR]                     =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    // CGM Record Access Control Point Characteristic Value
    [CGM_IDX_RACP_VAL]                      =   {ATT_CHAR_REC_ACCESS_CTRL_PT, PERM(IND, AUTH)|PERM(WR, AUTH)|PERM(WRITE_REQ, AUTH), PERM(RI, ENABLE) | CGM_REC_ACCESS_CTRL_MAX_LEN},
    // CGM Record Access Control Point Characteristic - Client Characteristic Configuration Descriptor
    [CGM_IDX_RACP_IND_CFG]                  =   {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE)|PERM(WR, AUTH)|PERM(WRITE_REQ, AUTH), 0},
    
    // CGM Specific Ops Control Point Characteristic Declaration
    [CGM_IDX_SPEC_OPS_CP_CHAR]              =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    // CGM Specific Ops Control Point Characteristic Value
    [CGM_IDX_SPEC_OPS_CP_VAL]               =   {ATT_CHAR_CGM_SPECIFIC_OPS_CP, PERM(IND, AUTH)|PERM(WR, AUTH)|PERM(WRITE_REQ, AUTH), PERM(RI, ENABLE) | CGM_SPEC_OPS_CTRL_MAX_LEN}, 
    // CGM Specific Ops Control Point Characteristic - Client Characteristic Configuration Descriptor
    [CGM_IDX_SPEC_OPS_CP_IND_CFG]           =   {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE)|PERM(WR, AUTH)|PERM(WRITE_REQ, AUTH), 0},
};

static uint32_t cgms_compute_att_table(uint16_t features);

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static uint8_t cgms_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                            uint8_t sec_lvl, struct cgms_db_cfg* params)
{
    // Service content flag
    uint32_t cfg_flag;
    // DB Creation Status
    uint8_t status;

    cfg_flag = cgms_compute_att_table(params->features);

    status = attm_svc_create_db(start_hdl, ATT_SVC_CONTINUOUS_GLUCOSE_MONITORING, (uint8_t *)&cfg_flag,
               CGM_IDX_NB, NULL, env->task, &cgms_att_db[0],
              (sec_lvl & PERM_MASK_SVC_AUTH) | (sec_lvl & PERM_MASK_SVC_EKS) | PERM(SVC_PRIMARY, ENABLE)
              | PERM(SVC_MI, DISABLE) );

    if( status == ATT_ERR_NO_ERROR )
    {
        //-------------------- allocate memory required for the profile  ---------------------
        struct cgms_env_tag* cgms_env =
                (struct cgms_env_tag* ) ke_malloc(sizeof(struct cgms_env_tag), KE_MEM_ATT_DB);
        memset(cgms_env, 0, sizeof(struct cgms_env_tag));

        // allocate required environment variable
        env->env = (prf_env_t*) cgms_env;

        cgms_env->shdl     = *start_hdl;
        cgms_env->prf_env.app_task = app_task
                        | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        cgms_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id                     = TASK_ID_CGMS;
        env->desc.idx_max           = CGMS_IDX_MAX;
        env->desc.state             = cgms_env->state;
        env->desc.default_handler   = &cgms_default_handler;

        //Save features on the environment
        cgms_env->features      = params->features;
        cgms_env->operation     = NULL;
        memset(cgms_env->ntf_ind_cfg, 0 , sizeof(cgms_env->ntf_ind_cfg));
        memset(cgms_env->session_start_time, 0, sizeof(cgms_env->session_start_time));        
        memset(cgms_env->session_run_time, 0, sizeof(cgms_env->session_run_time));

        memset(cgms_env->cgms_patient_high_alert_lvl, 0, sizeof(cgms_env->cgms_patient_high_alert_lvl));
        memset(cgms_env->cgms_patient_low_alert_lvl, 0, sizeof(cgms_env->cgms_patient_low_alert_lvl));
        memset(cgms_env->cgms_hypo_alert_lvl, 0, sizeof(cgms_env->cgms_hypo_alert_lvl));
        memset(cgms_env->cgms_hyper_alert_lvl, 0, sizeof(cgms_env->cgms_hyper_alert_lvl));
        memset(cgms_env->cgms_rate_of_decr_alert_lvl, 0, sizeof(cgms_env->cgms_rate_of_decr_alert_lvl));
        memset(cgms_env->cgms_rate_of_incr_alert_lvl, 0, sizeof(cgms_env->cgms_rate_of_incr_alert_lvl));

        // service is ready, go into an Idle state
        ke_state_set(env->task, CGMS_IDLE);
    }
    return status;
}

static void cgms_destroy(struct prf_task_env* env)
{
    struct cgms_env_tag* cgms_env = (struct cgms_env_tag*) env->env;

    // free profile environment variables
    if(cgms_env->operation != NULL)
    {
        ke_free(cgms_env->operation);
    }


    env->env = NULL;
    ke_free(cgms_env);
}

static void cgms_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Clear configuration for this connection */
    struct cgms_env_tag* cgms_env = (struct cgms_env_tag*) env->env;
    cgms_env->ntf_ind_cfg[conidx] = 0;
    
    return;
}

static void cgms_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    return;
}

/**
 ****************************************************************************************
 * @brief Compute a flag allowing to know attributes to add into the database
 *
 * @return a 32-bit flag whose each bit matches an attribute. If the bit is set to 1, the
 * attribute will be added into the database. 
 ****************************************************************************************
 */

static uint32_t cgms_compute_att_table(uint16_t features)
{
    //All CGM Characteristics are mandatory
    uint32_t att_table = CGMS_SERV_DECL_MASK | CGMS_MEAS_CHAR_MASK | CGMS_FEAT_CHAR_MASK |
                         CGMS_STAT_CHAR_MASK | CGMS_SES_START_TIME_CHAR_MASK |
                         CGMS_SES_RUN_TIME_CHAR_MASK | CGMS_RACP_CHAR_MASK |
                         CGMS_SPEC_OPS_CHAR_MASK;

    return att_table;
}


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// CGMS Task interface required by profile manager
const struct prf_task_cbs cgms_itf =
{
    (prf_init_fnct) cgms_init,
    cgms_destroy,
    cgms_create,
    cgms_cleanup,
};


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
uint8_t cgms_pack_meas_value(uint8_t *packed_meas, const struct cgm_meas* meas_val)
{
    uint8_t cursor = 0;

    // Size
    packed_meas[cursor] = meas_val->size;
    cursor += 1;
    
    // Flags
    packed_meas[cursor] = meas_val->flags;
    cursor++;

    // Glucose Concentration
    co_write16(packed_meas + cursor, meas_val->gluc_concentr);
    cursor += 2;
    
    //Time Offset
    co_write16p(packed_meas + cursor, meas_val->time_offset);
    cursor += 2;

    // Sensor status annunciation field
    if(((meas_val->flags & (1 << 5)) != 0))
    {
        packed_meas[cursor] = meas_val->sens_stat_annunc.status;
        cursor++;
    }
    if(((meas_val->flags & (1 << 6)) != 0))
    {
        packed_meas[cursor] = meas_val->sens_stat_annunc.cal_temp;
        cursor++;
    }
    if(((meas_val->flags & (1 << 7)) != 0))
    {
        packed_meas[cursor] = meas_val->sens_stat_annunc.warning;
        cursor++;
    }
    
    // Trend Information field
    if((meas_val->flags & (1 << 0)) != 0)
    {
        co_write16p(packed_meas + cursor, meas_val->trend_info);
        cursor += 2;
    }

    // Trend Information field
    if((meas_val->flags & (1 << 1)) != 0)
    {
        co_write16p(packed_meas + cursor, meas_val->quality);
        cursor += 2;
    }
    
    return cursor;
}

uint8_t cgms_unpack_racp_req(const uint8_t *packed_val, uint16_t length, struct cgm_racp_req* racp_req)
{
    uint8_t cursor = 0;

    // verify that enough data present to load operation filter
    if(length < 2)
    {
        return PRF_APP_ERROR;
    }

    // retrieve command op code
    racp_req->op_code = packed_val[cursor];
    cursor++;

    // clear filter structure
    memset(&(racp_req->filter), 0, sizeof(struct cgm_filter));


    // retrieve operator of the function
    racp_req->filter.operator = packed_val[cursor];
    cursor++;

    // check if opcode is supported
    if((racp_req->op_code < CGM_REQ_REP_STRD_RECS) || (racp_req->op_code > CGM_REQ_REP_NUM_OF_STRD_RECS))
    {
        return CGM_RSP_OP_CODE_NOT_SUP;
    }

    // Abort operation don't require any other parameter
    if(racp_req->op_code == CGM_REQ_ABORT_OP)
    {
        return GAP_ERR_NO_ERROR;
    }

    // check if operator is valid
    if(racp_req->filter.operator < CGM_OP_ALL_RECS)
    {
        return CGM_RSP_INVALID_OPERATOR;
    }
    // check if operator is supported
    else if(racp_req->filter.operator > CGM_OP_LAST_REC)
    {
        return CGM_RSP_OPERATOR_NOT_SUP;
    }

    // check if request requires operand (filter)
    if((racp_req->filter.operator >= CGM_OP_LT_OR_EQ)
            && (racp_req->filter.operator <= CGM_OP_WITHIN_RANGE_OF))
    {
        // verify that enough data present to load operand filter
        if(length < cursor)
        {
            return  CGM_RSP_INVALID_OPERAND;
        }

        // retrieve command filter type
        racp_req->filter.filter_type = packed_val[cursor];
        cursor++;

        // filter uses sequence number
        if(racp_req->filter.filter_type == CGM_FILTER_TIME_OFFSET)
        {
            // retrieve minimum value
            if((racp_req->filter.operator == CGM_OP_GT_OR_EQ)
                    || (racp_req->filter.operator == CGM_OP_WITHIN_RANGE_OF))
            {
                // check sufficient data available
                if((length - cursor) < 2)
                {
                    return CGM_RSP_INVALID_OPERAND;
                }

                // retrieve minimum value
                racp_req->filter.val.time_offset.min = co_read16p(packed_val + cursor);
                cursor +=2;
            }

            // retrieve maximum value
            if((racp_req->filter.operator == CGM_OP_LT_OR_EQ)
                    || (racp_req->filter.operator == CGM_OP_WITHIN_RANGE_OF))
            {
                if((length - cursor) < 2)
                {
                    return CGM_RSP_INVALID_OPERAND;
                }

                // retrieve maximum value
                racp_req->filter.val.time_offset.max = co_read16p(packed_val + cursor);
                cursor +=2;
            }


            // check that range value is valid
            if((racp_req->filter.operator == CGM_OP_WITHIN_RANGE_OF)
                    && (racp_req->filter.val.time_offset.min > racp_req->filter.val.time_offset.max))
            {
                return CGM_RSP_INVALID_OPERAND;
            }
        }
        else
        {
            return CGM_RSP_OPERAND_NOT_SUP;
        }
    }
    else if ((length - cursor) != 0)
    {
        return CGM_RSP_INVALID_OPERAND;
    }

    // Check that operand is valid
    return GAP_ERR_NO_ERROR;
}

uint8_t cgms_pack_racp_rsp(uint8_t *packed_val, struct cgm_racp_rsp* racp_rsp)
{
    uint8_t cursor = 0;

    // set response op code
    packed_val[cursor] = racp_rsp->op_code;
    cursor++;

    // set operator (null)
    packed_val[cursor] = 0;
    cursor++;

    // number of record
    if(racp_rsp->op_code == CGM_REQ_NUM_OF_STRD_RECS_RSP)
    {
        co_write16(packed_val+cursor, racp_rsp->operand.num_of_record);
        cursor += 2;
    }
    else
    {
        // requested opcode
        packed_val[cursor] = racp_rsp->operand.rsp.op_code_req;;
        cursor++;
        // command status
        packed_val[cursor] = racp_rsp->operand.rsp.value;
        cursor++;
    }

    return cursor;
}

uint8_t cgms_unpack_spec_ops_cp_req(const uint8_t *packed_val, uint16_t length, struct cgm_spec_ops_cp_req* spec_ops_cp_req)
{
    uint8_t cursor = 0;

    // retrieve command op code
    spec_ops_cp_req->op_code = packed_val[cursor];
    cursor++;

    // check if opcode is supported
    if((spec_ops_cp_req->op_code < CGM_REQ_SET_COMM_INT) || (spec_ops_cp_req->op_code > CGM_REQ_STOP_SESSION))
    {
        return CGM_RSP_OP_CODE_NOT_SUP;
    }

    // If there are no other parameters required
    if(spec_ops_cp_req->op_code == CGM_REQ_GET_COMM_INT || spec_ops_cp_req->op_code == CGM_REQ_GET_PATIENT_HIGH_ALERT_LVL
       || spec_ops_cp_req->op_code == CGM_REQ_GET_PATIENT_LOW_ALERT_LVL || spec_ops_cp_req->op_code == CGM_REQ_GET_HYPO_ALERT_LVL
       || spec_ops_cp_req->op_code == CGM_REQ_GET_HYPER_ALERT_LVL || spec_ops_cp_req->op_code == CGM_REQ_GET_RATE_OF_DECR_ALERT_LVL
       || spec_ops_cp_req->op_code == CGM_REQ_GET_RATE_OF_INCR_ALERT_LVL || spec_ops_cp_req->op_code == CGM_REQ_RESET_DEV_SPEC_ALERT
       || spec_ops_cp_req->op_code == CGM_REQ_START_SESSION || spec_ops_cp_req->op_code == CGM_REQ_STOP_SESSION )
    {
        return GAP_ERR_NO_ERROR;
    }
    else if (spec_ops_cp_req->op_code == CGM_REQ_SET_COMM_INT)
    {
        if((length - cursor) < 1)
        {
            return CGM_RSP_INVALID_OPERAND;
        }
        else
        {
            spec_ops_cp_req->operand.comm_interv = packed_val[cursor];
            return GAP_ERR_NO_ERROR;
        }
    }
    else if (spec_ops_cp_req->op_code == CGM_REQ_SET_GLUC_CAL_VAL)
    {
        if((length - cursor) < 10)
        {
            return CGM_RSP_INVALID_OPERAND;
        }
        else
        {
            spec_ops_cp_req->operand.calib_value.gluc_conc_of_calib = co_read16p(packed_val + cursor);
            cursor += 2;
            spec_ops_cp_req->operand.calib_value.calib_time = co_read16p(packed_val + cursor);
            cursor += 2;
            spec_ops_cp_req->operand.calib_value.calib_type_samp_loc = packed_val[cursor];
            cursor++;
            spec_ops_cp_req->operand.calib_value.next_calib_time = co_read16p(packed_val + cursor);
            cursor += 2;
            spec_ops_cp_req->operand.calib_value.calib_data_rec_num = co_read16p(packed_val + cursor);
            cursor += 2;
            spec_ops_cp_req->operand.calib_value.calib_stat = co_read16p(packed_val + cursor);
            
            return GAP_ERR_NO_ERROR;
        }
    }
    else if (spec_ops_cp_req->op_code == CGM_REQ_GET_GLUC_CAL_VAL)
    {
        if((length - cursor) < 2)
        {
            return CGM_RSP_INVALID_OPERAND;
        }
        else
        {
            spec_ops_cp_req->operand.calib_data_rec_num = co_read16p(packed_val + cursor);    
            return GAP_ERR_NO_ERROR;
        }
    }
    else if( spec_ops_cp_req->op_code == CGM_REQ_SET_PATIENT_HIGH_ALERT_LVL || spec_ops_cp_req->op_code == CGM_REQ_SET_PATIENT_LOW_ALERT_LVL
       || spec_ops_cp_req->op_code == CGM_REQ_SET_HYPO_ALERT_LVL || spec_ops_cp_req->op_code == CGM_REQ_SET_HYPER_ALERT_LVL
       || spec_ops_cp_req->op_code == CGM_REQ_SET_RATE_OF_DECR_ALERT_LVL || spec_ops_cp_req->op_code == CGM_REQ_SET_RATE_OF_INCR_ALERT_LVL )
    {
        if((length - cursor) < 2)
        {
            return CGM_RSP_INVALID_OPERAND;
        }
        else
        {
            spec_ops_cp_req->operand.level = co_read16p(packed_val + cursor);    
            return GAP_ERR_NO_ERROR;
        }
    }    

    // Check that operand is valid
    return GAP_ERR_NO_ERROR;
}

uint8_t cgms_pack_spec_ops_cp_rsp(uint8_t *packed_val, struct cgm_spec_ops_cp_rsp* spec_ops_cp_rsp)
{
    uint8_t cursor = 0;

    // set response op code
    packed_val[cursor] = spec_ops_cp_rsp->op_code;
    cursor++;

    // number of record
    if(spec_ops_cp_rsp->op_code == CGM_REQ_GET_COMM_INT)
    {
        packed_val[cursor] = spec_ops_cp_rsp->operand.comm_interv;
        cursor++;
    }
    else if(spec_ops_cp_rsp->op_code == CGM_REQ_GET_GLUC_CAL_VAL)
    {
        co_write16(packed_val + cursor, spec_ops_cp_rsp->operand.calib_rec.gluc_conc_of_calib);
        cursor += 2;
        co_write16(packed_val + cursor, spec_ops_cp_rsp->operand.calib_rec.calib_time);
        cursor += 2;
        packed_val[cursor] = spec_ops_cp_rsp->operand.calib_rec.calib_type_samp_loc;
        cursor++;
        co_write16(packed_val + cursor, spec_ops_cp_rsp->operand.calib_rec.next_calib_time);
        cursor += 2;
        co_write16(packed_val + cursor, spec_ops_cp_rsp->operand.calib_rec.calib_data_rec_num);
        cursor += 2;
        packed_val[cursor] = spec_ops_cp_rsp->operand.calib_rec.calib_stat;
        cursor++;
    }
    else if(spec_ops_cp_rsp->op_code == CGM_REQ_GET_PATIENT_HIGH_ALERT_LVL ||
            spec_ops_cp_rsp->op_code == CGM_REQ_GET_PATIENT_LOW_ALERT_LVL ||
            spec_ops_cp_rsp->op_code == CGM_REQ_GET_HYPO_ALERT_LVL ||
            spec_ops_cp_rsp->op_code == CGM_REQ_GET_HYPER_ALERT_LVL ||
            spec_ops_cp_rsp->op_code == CGM_REQ_GET_RATE_OF_DECR_ALERT_LVL ||
            spec_ops_cp_rsp->op_code == CGM_REQ_GET_RATE_OF_INCR_ALERT_LVL)
    {
        co_write16(packed_val + cursor, spec_ops_cp_rsp->operand.level);
        cursor += 2;
    }
    else
    {
        // requested opcode
        packed_val[cursor] = spec_ops_cp_rsp->operand.rsp.opcode;
        cursor++;
        // command status
        packed_val[cursor] = spec_ops_cp_rsp->operand.rsp.value;
        cursor++;
    }

    return cursor;
}

uint8_t cgms_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value)
{
    struct cgms_env_tag* cgms_env = PRF_ENV_GET(CGMS, cgms);
    uint8_t status = GAP_ERR_NO_ERROR;

    if((value != valid_val) && (value != PRF_CLI_STOP_NTFIND))
    {
        status = PRF_APP_ERROR;

    }
    else if (value == valid_val)
    {
        cgms_env->ntf_ind_cfg[conidx] |= cfg;
    }
    else
    {
        cgms_env->ntf_ind_cfg[conidx] &= ~cfg;
    }

    if(status == GAP_ERR_NO_ERROR)
    {
        // inform application that notification/indication configuration has changed
        struct cgms_cfg_indntf_ind * ind = KE_MSG_ALLOC(CGMS_CFG_INDNTF_IND,
                prf_dst_task_get(&cgms_env->prf_env, conidx), prf_src_task_get(&cgms_env->prf_env, conidx),
                cgms_cfg_indntf_ind);
        ind->conidx      = conidx;
        ind->ntf_ind_cfg = cgms_env->ntf_ind_cfg[conidx];
        ke_msg_send(ind);
    }

    return status;
}

const struct prf_task_cbs* cgms_prf_itf_get(void)
{
   return &cgms_itf;
}


uint16_t cgms_att_hdl_get(struct cgms_env_tag* cgms_env, uint8_t att_idx)
{
    uint16_t handle = cgms_env->shdl;

    do
    {
        // CGM Measurement Characteristic
        if(att_idx > CGM_IDX_MEASUREMENT_NTF_CFG)
        {
            handle += CGMS_MEASUREMENT_ATT_NB;
        }
        else
        {
            handle += att_idx - CGM_IDX_SVC;
            break;
        }

        // CGM Feature
        if(att_idx > CGM_IDX_FEAT_VAL)
        {
            handle += CGMS_FEAT_ATT_NB;
        }
        else
        {
            handle += att_idx - CGM_IDX_MEASUREMENT_NTF_CFG;
            break;
        }
       
        // CGM Status
        if(att_idx > CGM_IDX_STATUS_VAL)
        {
            handle += CGMS_STATUS_ATT_NB;
        }
        else
        {
            handle += att_idx - CGM_IDX_FEAT_VAL;
            break;
        }
        
        // CGM Session Start Time
        if(att_idx > CGM_IDX_SESSION_START_TIME_VAL)
        {
            handle += CGMS_SESSION_START_TIME_ATT_NB;
        }
        else
        {
            handle += att_idx - CGM_IDX_STATUS_VAL;
            break;
        }
        
        // CGM Session Run Time
        if(att_idx > CGM_IDX_SESSION_RUN_TIME_VAL)
        {
            handle += CGMS_SESSION_RUN_TIME_ATT_NB;
        }
        else
        {
            handle += att_idx - CGM_IDX_SESSION_START_TIME_VAL;
            break;
        }
        
        // CGM RACP
        if(att_idx > CGM_IDX_RACP_IND_CFG)
        {
            handle += CGMS_RACP_ATT_NB;
        }
        else
        {
            handle += att_idx - CGM_IDX_SESSION_RUN_TIME_VAL;
            break;
        }
        
        // CGM Special Ops Control Point
        if(att_idx > CGM_IDX_SPEC_OPS_CP_IND_CFG)
        {
            handle += CGMS_SPEC_OPS_CP_ATT_NB;
        }
        else
        {
            handle += att_idx - CGM_IDX_RACP_IND_CFG;
            break;
        }
        
        // Measurement Interval
        if (att_idx >= CGM_IDX_NB)
        {
            handle = ATT_INVALID_HANDLE;
            break;
        }
    } while (0);

    return handle;
}

uint8_t cgms_att_idx_get(struct cgms_env_tag* cgms_env, uint16_t handle)
{
    uint16_t handle_ref = cgms_env->shdl;
    uint8_t att_idx = ATT_INVALID_IDX;

    do
    {
        // not valid handle
        if(handle < handle_ref)
        {
            break;
        }

        // CGMS Measurement
        handle_ref += CGMS_MEASUREMENT_ATT_NB;

        if(handle < handle_ref)
        {
            att_idx = CGM_IDX_MEASUREMENT_NTF_CFG - (handle_ref - handle);
            break;
        }

        // CGM Feature
        handle_ref += CGMS_FEAT_ATT_NB;

        if(handle < handle_ref)
        {
            att_idx = CGM_IDX_FEAT_VAL - (handle_ref - handle);
            break;
        }

        // CGM Status
        handle_ref += CGMS_STATUS_ATT_NB;

        if(handle < handle_ref)
        {
            att_idx = CGM_IDX_STATUS_VAL - (handle_ref - handle);
            break;
        }        
        
        // CGM Session Start Time
        handle_ref += CGMS_SESSION_START_TIME_ATT_NB;
        
        if(handle < handle_ref)
        {
            att_idx = CGM_IDX_SESSION_START_TIME_VAL - (handle_ref - handle);
            break;
        }  
        
        // CGM Session Run Time
        handle_ref += CGMS_SESSION_RUN_TIME_ATT_NB;

        if(handle < handle_ref)
        {
            att_idx = CGM_IDX_SESSION_RUN_TIME_VAL - (handle_ref - handle);
            break;
        } 
        
        // CGM Record Access Control Point
        handle_ref += CGMS_RACP_ATT_NB;

        if(handle < handle_ref)
        {
            att_idx = CGM_IDX_RACP_IND_CFG - (handle_ref - handle);
            break;
        }        
        
        // CGM Special Ops Control Point
        handle_ref += CGMS_SPEC_OPS_CP_ATT_NB;

        if(handle <= handle_ref)
        {
            att_idx = CGM_IDX_SPEC_OPS_CP_IND_CFG - (handle_ref - handle);
            break;
        }              
    } while (0);

    return att_idx;
}

#endif //BLE_CGM_SERVER

/// @} CGMS

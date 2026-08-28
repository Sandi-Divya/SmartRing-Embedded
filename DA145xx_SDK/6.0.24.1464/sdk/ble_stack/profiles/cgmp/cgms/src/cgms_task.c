/**
 ****************************************************************************************
 *
 * @file cgms_task.c
 *
 * @brief Continuous Glucose Monitoring Profile Task Implementation.
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
 * @addtogroup CGMSTASK
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

#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "atts.h"
#include "cgms.h"
#include "cgms_task.h"
#include "prf_utils.h"


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref CGMS_ENABLE_REQ message.
 * The handler enables the Continuous Glucose Monitoring Profile Server Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int cgms_enable_req_handler(ke_msg_id_t const msgid,
                                   struct cgms_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

    // check state of the task
    if(gapc_get_conhdl(param->conidx) != GAP_INVALID_CONHDL)
    {
        status = GAP_ERR_NO_ERROR;
    }

    // send response
    struct cgms_enable_rsp *rsp = KE_MSG_ALLOC(CGMS_ENABLE_RSP, src_id, dest_id, cgms_enable_rsp);
    rsp->conidx = param->conidx;
    rsp->status = status;
    ke_msg_send(rsp);

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_WRITE_REQ_IND message.
 * The handler compares the new values with current ones and notifies them if they changed.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_write_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    struct cgms_env_tag* cgms_env = PRF_ENV_GET(CGMS, cgms);
    uint8_t conidx = KE_IDX_GET(src_id);
    int msg_status = KE_MSG_CONSUMED;
    
    struct cgm_racp_rsp racp_rsp;
    racp_rsp.op_code = 0;
    
    struct cgm_spec_ops_cp_rsp spec_ops_rsp;
    spec_ops_rsp.op_code = 0;

    // retrieve handle information
    uint8_t status = GAP_ERR_NO_ERROR;

    // If the attribute has been found, status is GAP_ERR_NO_ERROR
    if (cgms_env != NULL)
    {
        // retrieve handle information
        uint8_t att_idx = CGMS_IDX(param->handle);
        
        switch(att_idx)
        {
            case CGM_IDX_MEASUREMENT_NTF_CFG:
                status = cgms_update_ntf_ind_cfg(conidx, CGMS_CFG_MEASUREMENT_NTF, PRF_CLI_START_NTF, co_read16p(param->value));
                break; 
            case CGM_IDX_RACP_IND_CFG:
                status = cgms_update_ntf_ind_cfg(conidx, CGMS_CFG_RACP_IND, PRF_CLI_START_IND, co_read16p(param->value));
                break;
            case CGM_IDX_SPEC_OPS_CP_IND_CFG:
                status = cgms_update_ntf_ind_cfg(conidx, CGMS_CFG_SPEC_OPS_CP_IND, PRF_CLI_START_IND, co_read16p(param->value));
                break;
            case CGM_IDX_SESSION_START_TIME_VAL:
            {
                if (param->length != CGMS_SESSION_START_TIME_VAL_LENGTH)
                {
                    status = ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
                    break;
                }
                else if ((co_read16p(param->value) < 1582 || co_read16p(param->value) > 9999) ||
                        (param->value[2] < 1 || param->value[2] > 12) ||
                        (param->value[3] < 1 || param->value[3] > 31) ||
                        (param->value[4] > 24) ||
                        (param->value[5] > 59) ||
                        (param->value[6] > 59) ||
                        ((int8_t) param->value[7] < -48 || (int8_t) param->value[7] > 56) ||
                        (param->value[8] > 8 || param->value[8] % 2 != 0))
                {
                    status = PRF_OUT_OF_RANGE;
                    break;
                }
                else
                {                    
                    struct cgms_session_start_time_req * req = KE_MSG_ALLOC(CGMS_SESSION_START_TIME_REQ,
                                prf_dst_task_get(&(cgms_env->prf_env), conidx), dest_id, cgms_session_start_time_req);
                    
                    req->conidx = conidx;
                    req->session_start_time.year = co_read16p(param->value);
                    req->session_start_time.month = param->value[2];
                    req->session_start_time.day = param->value[3];
                    req->session_start_time.hours = param->value[4];
                    req->session_start_time.minutes = param->value[5];
                    req->session_start_time.seconds = param->value[6];
                    req->session_start_time.time_zone = param->value[7];
                    req->session_start_time.dst_time_offset = param->value[8];
                                       
                    // Inform application
                    ke_msg_send(req);
                    
                }
            }
                break;
            case CGM_IDX_RACP_VAL:
            {               
                if((cgms_env->ntf_ind_cfg[conidx] & CGMS_CFG_RACP_IND) == 0)
                {
                    // do nothing since indication not enabled for this characteristic
                    status = PRF_CCCD_IMPR_CONFIGURED;
                }
                // If a request is on going
                else if(CGMS_IS(conidx, RACP_ON_GOING))
                {
                    // if it's an abort command, execute it.
                    if((param->offset == 0) && (param->value[0] == CGM_REQ_ABORT_OP))
                    {
                        //forward abort operation to application
                        struct cgms_racp_req_rcv_ind * req = KE_MSG_ALLOC(CGMS_RACP_REQ_RCV_IND,
                                prf_dst_task_get(&(cgms_env->prf_env), conidx), dest_id, cgms_racp_req_rcv_ind);
                        req->conidx = conidx;
                        req->racp_req.op_code = CGM_REQ_ABORT_OP;
                        req->racp_req.filter.operator = 0;
                        ke_msg_send(req);
                    }
                    else
                    {
                        // do nothing since a procedure already in progress
                        status = PRF_PROC_IN_PROGRESS;
                    }
                }
                else
                {
                    struct cgm_racp_req racp_req;
                    // unpack record access control point value
                    uint8_t reqstatus = cgms_unpack_racp_req(param->value, param->length, &racp_req);

                    // check unpacked status
                    switch(reqstatus)
                    {
                        case PRF_APP_ERROR:
                        {
                            /* Request failed */
                            status = ATT_ERR_UNLIKELY_ERR;
                        }
                        break;
                        case GAP_ERR_NO_ERROR:
                        {
                            // check which request shall be send to app task
                            switch(racp_req.op_code)
                            {
                                case CGM_REQ_REP_STRD_RECS:
                                case CGM_REQ_DEL_STRD_RECS:
                                case CGM_REQ_REP_NUM_OF_STRD_RECS:
                                {
                                    //forward request operation to application
                                    struct cgms_racp_req_rcv_ind * req = KE_MSG_ALLOC(CGMS_RACP_REQ_RCV_IND,
                                            prf_dst_task_get(&(cgms_env->prf_env), conidx), dest_id,
                                            cgms_racp_req_rcv_ind);
                                    req->conidx = conidx;
                                    req->racp_req = racp_req;
                                    // RACP on going.
                                    CGMS_SET(conidx, RACP_ON_GOING);
                                    ke_msg_send(req);
                                }
                                break;
                                case CGM_REQ_ABORT_OP:
                                {
                                    // nothing to abort, send an error message.
                                    racp_rsp.op_code = CGM_REQ_RSP_CODE;
                                    racp_rsp.operand.rsp.op_code_req =
                                            racp_req.op_code;
                                    racp_rsp.operand.rsp.value = CGM_RSP_ABORT_UNSUCCESSFUL;
                                }
                                break;
                                default:
                                {
                                    // nothing to do since it's handled during unpack
                                }
                                break;
                            }
                        }
                        break;
                        default:
                        {
                            /* There is an error in record access control request, inform peer
                             * device that request is incorrect. */
                            racp_rsp.op_code = CGM_REQ_RSP_CODE;
                            racp_rsp.operand.rsp.op_code_req = racp_req.op_code;
                            racp_rsp.operand.rsp.value = reqstatus;
                        }
                        break;
                    }
                }
            } break;
            case CGM_IDX_SPEC_OPS_CP_VAL:
            {
                if((cgms_env->ntf_ind_cfg[conidx] & CGMS_CFG_SPEC_OPS_CP_IND) == 0)
                {
                    // do nothing since indication not enabled for this characteristic
                    status = PRF_CCCD_IMPR_CONFIGURED;
                }
                // If a session has started
                else if(CGMS_IS(conidx, SPEC_OPS_CP_SESSION_STARTED))
                {
                    // if it's a stop command, execute it.
                    if((param->offset == 0) && (param->value[0] == CGM_REQ_STOP_SESSION))
                    {
                        //forward abort operation to application
                        struct cgms_spec_ops_cp_req_rcv_ind * req = KE_MSG_ALLOC(CGMS_SPEC_OPS_CP_REQ_RCV_IND,
                                prf_dst_task_get(&(cgms_env->prf_env), conidx), dest_id, cgms_spec_ops_cp_req_rcv_ind);
                        req->conidx = conidx;
                        req->spec_ops_cp_req.op_code = CGM_REQ_STOP_SESSION;
                        ke_msg_send(req);
                    }
                    else
                    {
                        // do nothing since a procedure already in progress
                        status = PRF_PROC_IN_PROGRESS;
                    }
                }
                else
                {
                    struct cgm_spec_ops_cp_req spec_ops_cp_req;
                    // unpack record access control point value
                    uint8_t reqstatus = cgms_unpack_spec_ops_cp_req(param->value, param->length, &spec_ops_cp_req);

                    // check unpacked status
                    switch(reqstatus)
                    {
                        case PRF_APP_ERROR:
                        {
                            /* Request failed */
                            status = ATT_ERR_UNLIKELY_ERR;
                        }
                        break;
                        case GAP_ERR_NO_ERROR:
                        {
                            //forward request operation to application
                            struct cgms_spec_ops_cp_req_rcv_ind * req = KE_MSG_ALLOC(CGMS_SPEC_OPS_CP_REQ_RCV_IND,
                                    prf_dst_task_get(&(cgms_env->prf_env), conidx), dest_id,
                                    cgms_spec_ops_cp_req_rcv_ind);
                            req->conidx = conidx;
                            req->spec_ops_cp_req = spec_ops_cp_req;
                            // check which request shall be send to app task
                            switch(spec_ops_cp_req.op_code)
                            {                            
                                case CGM_REQ_SET_COMM_INT:
                                {
                                    req->spec_ops_cp_req.operand.comm_interv = spec_ops_cp_req.operand.comm_interv;
                                    ke_msg_send(req);
                                } break;
                                case CGM_REQ_SET_GLUC_CAL_VAL:
                                {
                                    req->spec_ops_cp_req.operand.calib_value = spec_ops_cp_req.operand.calib_value;
                                    ke_msg_send(req);
                                } break;
                                case CGM_REQ_GET_GLUC_CAL_VAL:
                                {
                                    req->spec_ops_cp_req.operand.calib_data_rec_num = spec_ops_cp_req.operand.calib_data_rec_num;
                                    ke_msg_send(req);
                                } break;    
                                case CGM_REQ_GET_COMM_INT:
                                case CGM_REQ_GET_PATIENT_HIGH_ALERT_LVL:
                                case CGM_REQ_GET_PATIENT_LOW_ALERT_LVL:
                                case CGM_REQ_GET_HYPO_ALERT_LVL:
                                case CGM_REQ_GET_HYPER_ALERT_LVL:
                                case CGM_REQ_GET_RATE_OF_DECR_ALERT_LVL:
                                case CGM_REQ_GET_RATE_OF_INCR_ALERT_LVL:
                                case CGM_REQ_RESET_DEV_SPEC_ALERT:
                                {
                                    ke_msg_send(req);
                                } break;
                                case CGM_REQ_SET_PATIENT_HIGH_ALERT_LVL:
                                case CGM_REQ_SET_PATIENT_LOW_ALERT_LVL:
                                case CGM_REQ_SET_HYPO_ALERT_LVL:
                                case CGM_REQ_SET_HYPER_ALERT_LVL:
                                case CGM_REQ_SET_RATE_OF_DECR_ALERT_LVL:
                                case CGM_REQ_SET_RATE_OF_INCR_ALERT_LVL:
                                {
                                    req->spec_ops_cp_req.operand.level = spec_ops_cp_req.operand.level;
                                    ke_msg_send(req);
                                } break; 
                                case CGM_REQ_START_SESSION:
                                {
                                    CGMS_SET(conidx, SPEC_OPS_CP_SESSION_STARTED);
                                    ke_msg_send(req);
                                } break;
                                case CGM_REQ_STOP_SESSION:
                                {
                                    // nothing to abort, send an error message.
                                    spec_ops_rsp.op_code = CGM_RSP_RESPONSE;
                                    spec_ops_rsp.operand.rsp.opcode = spec_ops_cp_req.op_code;
                                    spec_ops_rsp.operand.rsp.value = CGM_RSP_ABORT_UNSUCCESSFUL;
                                }
                                break;
                                default:
                                {
                                    // nothing to do since it's handled during unpack
                                }
                                break;
                            }
                        }
                        break;
                        default:
                        {
                            /* There is an error in specific ops control request, inform peer
                             * device that request is incorrect. */
                            spec_ops_rsp.op_code = CGM_RSP_RESPONSE;
                            spec_ops_rsp.operand.rsp.opcode = spec_ops_cp_req.op_code;
                            spec_ops_rsp.operand.rsp.value = reqstatus;
                        }
                        break;
                    }
                }
            } break;
            default:
                status = ATT_ERR_INVALID_HANDLE;
                break;
        }
    }
    
    //Send write response
    struct gattc_write_cfm * cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
    cfm->handle = param->handle;
    cfm->status = status;
    ke_msg_send(cfm);

    
    /* The racp response has to be sent - after write confirm */
    if(racp_rsp.op_code != 0)
    {
        // Allocate the GATT indication message
        struct gattc_send_evt_cmd *racp_rsp_val = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                KE_BUILD_ID(TASK_GATTC, conidx), dest_id,
                gattc_send_evt_cmd, CGM_SPEC_OPS_CTRL_MAX_LEN);

        racp_rsp_val->operation = GATTC_INDICATE;
        racp_rsp_val->handle = param->handle;
        // pack measured value in database
        racp_rsp_val->length = cgms_pack_racp_rsp(racp_rsp_val->value, &racp_rsp);

        // Store that response internally sent by profile
        CGMS_CLEAR(conidx, RACP_RSP_SENT_BY_APP);

        // send RACP Response indication
        ke_msg_send(racp_rsp_val);
    }
    
    /* The specific ops control point response has to be sent - after write confirm */
    if(spec_ops_rsp.op_code != 0)
    {
        // Allocate the GATT indication message
        struct gattc_send_evt_cmd *spec_ops_cp_rsp_val = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                KE_BUILD_ID(TASK_GATTC, conidx), dest_id,
                gattc_send_evt_cmd, CGM_REC_ACCESS_CTRL_MAX_LEN);

        spec_ops_cp_rsp_val->operation = GATTC_INDICATE;
        spec_ops_cp_rsp_val->handle = param->handle;
        // pack measured value in database
        spec_ops_cp_rsp_val->length = cgms_pack_spec_ops_cp_rsp(spec_ops_cp_rsp_val->value, &spec_ops_rsp);
        
        // send Specific Ops CP Response indication
        ke_msg_send(spec_ops_cp_rsp_val);
    }
    
    return (msg_status);    
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_READ_REQ_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_req_ind_handler(ke_msg_id_t const msgid, struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    int msg_status = KE_MSG_CONSUMED;
    uint8_t status = ATT_ERR_NO_ERROR;
    uint8_t conidx  = KE_IDX_GET(src_id);
    ke_state_t state = ke_state_get(dest_id);

    if(state == CGMS_IDLE)
    {
        struct cgms_env_tag* cgms_env = PRF_ENV_GET(CGMS, cgms);
        uint8_t att_idx = CGMS_IDX(param->handle);
        
        switch(att_idx)
        {
            case CGM_IDX_FEAT_VAL:
            {
                size_t val_len = 6;
                
                const uint32_t feat_val = CGMS_FEAT_VALUE;
                const uint16_t feat_type_sample_loc = CGMS_FEAT_TYPE_SAMPLE_LOC;
                
                struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, val_len);
                cfm->handle = param->handle;
                cfm->status = status;
                cfm->length = val_len;
                co_write24p(cfm->value, feat_val);
                cfm->value[3] = feat_type_sample_loc;
                // E2E-CRC always present for Features Characteristic, set to 0xffff if not used
                co_write16p(&cfm->value[4], 0xffff);
                
                if ((CGMS_FEAT_VALUE & CGMS_FEAT_E2E_CRC_SUP) == CGMS_FEAT_E2E_CRC_SUP)
                {
                    //TODO: Change to a CRC-CCITT generator function 
                }
                
                ke_msg_send(cfm);

            } break;
            case CGM_IDX_STATUS_VAL:
            {               
                size_t val_len = 5;
                
                if ((CGMS_FEAT_VALUE & CGMS_FEAT_E2E_CRC_SUP) == CGMS_FEAT_E2E_CRC_SUP)
                {
                   val_len += 2;
                }
                
                struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, val_len);
                cfm->handle = param->handle;
                cfm->status = status;
                cfm->length = val_len;              
                
                co_write16p(&cfm->value[0], cgms_env->status.time_offset);
                co_write24p(&cfm->value[2], cgms_env->status.sensor_status_field);
                
                if ((CGMS_FEAT_VALUE & CGMS_FEAT_E2E_CRC_SUP) == CGMS_FEAT_E2E_CRC_SUP)
                {
                    //TODO: Change to a CRC-CCITT generator function 
                }
                
                ke_msg_send(cfm);
                
            } break;
            case CGM_IDX_SESSION_START_TIME_VAL:
            {
                size_t val_len = 9;
                
                if ((CGMS_FEAT_VALUE & CGMS_FEAT_E2E_CRC_SUP) == CGMS_FEAT_E2E_CRC_SUP)
                {
                   val_len += 2;
                }
                
                struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, val_len);
                cfm->handle = param->handle;
                cfm->status = status;
                cfm->length = val_len;
                
                co_write16p(cfm->value, cgms_env->session_start_time[conidx].year);
                cfm->value[2] = cgms_env->session_start_time[conidx].month;
                cfm->value[3] = cgms_env->session_start_time[conidx].day;
                cfm->value[4] = cgms_env->session_start_time[conidx].hours;
                cfm->value[5] = cgms_env->session_start_time[conidx].minutes;
                cfm->value[6] = cgms_env->session_start_time[conidx].seconds;
                cfm->value[7] = cgms_env->session_start_time[conidx].time_zone;
                cfm->value[8] = cgms_env->session_start_time[conidx].dst_time_offset;
                
                if ((CGMS_FEAT_VALUE & CGMS_FEAT_E2E_CRC_SUP) == CGMS_FEAT_E2E_CRC_SUP)
                {
                    //TODO: Change to a CRC-CCITT generator function 
                }
                
                ke_msg_send(cfm);
            } break;
            case CGM_IDX_SESSION_RUN_TIME_VAL:
            {
                struct cgms_session_run_time run_time_test;
                size_t val_len = 2;
                
                if ((CGMS_FEAT_VALUE & CGMS_FEAT_E2E_CRC_SUP) == CGMS_FEAT_E2E_CRC_SUP)
                {
                   val_len += 2;
                }
                
                run_time_test.time = 1024;
                
                struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, val_len);
                cfm->handle = param->handle;
                cfm->status = status;
                cfm->length = val_len;
                
                co_write16p(cfm->value, run_time_test.time);
                
                if ((CGMS_FEAT_VALUE & CGMS_FEAT_E2E_CRC_SUP) == CGMS_FEAT_E2E_CRC_SUP)
                {
                    //TODO: Change to a CRC-CCITT generator function 
                }
                
                ke_msg_send(cfm);
            } break;
            case CGM_IDX_MEASUREMENT_NTF_CFG:
            {
                uint16_t res_val;
                
                struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(res_val));
                cfm->handle = param->handle;
                cfm->status = status;
                cfm->length = sizeof(res_val);
                co_write16p(cfm->value, ((cgms_env->ntf_ind_cfg[conidx] & CGMS_CFG_MEASUREMENT_NTF) != 0) ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND);
                ke_msg_send(cfm);         
            } break;
            case CGM_IDX_RACP_IND_CFG:
            {
                uint16_t res_val;
                
                struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(res_val));
                cfm->handle = param->handle;
                cfm->status = status;
                cfm->length = sizeof(res_val);
                co_write16p(cfm->value, ((cgms_env->ntf_ind_cfg[conidx] & CGMS_CFG_RACP_IND) != 0) ? PRF_CLI_START_IND : PRF_CLI_STOP_NTFIND);
                ke_msg_send(cfm);         
            } break;
            case CGM_IDX_SPEC_OPS_CP_IND_CFG:
            {
                uint16_t res_val;
                
                struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(res_val));
                cfm->handle = param->handle;
                cfm->status = status;
                cfm->length = sizeof(res_val);
                co_write16p(cfm->value, ((cgms_env->ntf_ind_cfg[conidx] & CGMS_CFG_SPEC_OPS_CP_IND) != 0) ? PRF_CLI_START_IND : PRF_CLI_STOP_NTFIND);
                ke_msg_send(cfm);         
            } break;
            default:
            {
                // Invalid handle
                struct gattc_read_cfm* cfm = KE_MSG_ALLOC(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm);
                cfm->handle = param->handle;
                cfm->status = ATT_ERR_INVALID_HANDLE;
                ke_msg_send(cfm);
                
                break;
            }
        }
    }
    // postpone request if profile is in a busy state - required for multipoint
    else if(state == CGMS_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }

    return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref CGMS_MEAS_SEND_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int cgms_meas_send_req_handler(ke_msg_id_t const msgid,
                                      struct cgms_send_meas_cmd const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t state  = ke_state_get(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

    if(state == CGMS_IDLE)
    {
        // Get the address of the environment
        struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);

        //Cannot send another measurement in parallel
        if(!CGMS_IS(conidx, SENDING_MEAS))
        {
            // inform that device is sending a measurement
            CGMS_SET(conidx, SENDING_MEAS);

            // check if notifications enabled
            if((cgms_env->ntf_ind_cfg[conidx] & CGMS_CFG_MEASUREMENT_NTF) == 0)
            {
                // Not allowed to send measurement if Notifications not enabled.
                status = (PRF_ERR_NTF_DISABLED);
            }
            else
            {
                // Allocate the GATT notification message
                struct gattc_send_evt_cmd *meas_val = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                        KE_BUILD_ID(TASK_GATTC, conidx), dest_id,
                        gattc_send_evt_cmd, CGM_MEAS_MAX_LEN);

                // Fill in the parameter structure
                meas_val->operation = GATTC_NOTIFY;
                meas_val->handle = CGMS_HANDLE(CGM_IDX_MEASUREMENT_VAL);
                // pack measured value in database
                meas_val->length = cgms_pack_meas_value(meas_val->value, &(param->meas));

                // Measurement value notification not yet sent
                CGMS_CLEAR(conidx, MEAS_SENT);

                // Send the event
                ke_msg_send(meas_val);

                status = (GAP_ERR_NO_ERROR);
            }
        }
    }

    // send command complete if an error occurs
    if(status != GAP_ERR_NO_ERROR)
    {
        // send completed information to APP task that contains error status
        struct cgms_cmp_evt * cmp_evt = KE_MSG_ALLOC(CGMS_CMP_EVT, src_id,
                dest_id, cgms_cmp_evt);

        cmp_evt->request    = CGMS_SEND_MEAS_REQ_NTF_CMP;
        cmp_evt->status     = status;

        ke_msg_send(cmp_evt);
    }


    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref CGMS_STATUS_SEND_RSP message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int cgms_status_send_rsp_handler(ke_msg_id_t const msgid,
                                      struct cgms_status_send_rsp const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    uint8_t state  = ke_state_get(dest_id);
    uint8_t status = ATT_ERR_NO_ERROR;

    if(state == CGMS_IDLE)
    {
        // Get the address of the environment
        struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);

        size_t val_len = 6;
        
        if ((CGMS_FEAT_VALUE & CGMS_FEAT_E2E_CRC_SUP) == CGMS_FEAT_E2E_CRC_SUP)
        {
           val_len += 2;
        }
               
        struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, val_len);
        cfm->handle = CGMS_HANDLE(CGM_IDX_STATUS_VAL);;
        cfm->status = status;
        cfm->length = val_len;
        
        co_write16p(cfm->value, param->status.time_offset);
        co_write32p(&cfm->value[2], param->status.sensor_status_field);
        
        if ((CGMS_FEAT_VALUE & CGMS_FEAT_E2E_CRC_SUP) == CGMS_FEAT_E2E_CRC_SUP)
        {
            //TODO: Change to a CRC-CCITT generator function 
        }
        
        ke_msg_send(cfm);
    }
    else
    {
        return (KE_MSG_SAVED);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref CGMS_SEND_RACP_RSP_CMD message.
 * Send when a RACP requests is finished
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int cgms_send_racp_rsp_cmd_handler(ke_msg_id_t const msgid,
                                     struct cgms_send_racp_rsp_cmd const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    // Status
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t state  = ke_state_get(dest_id);

    if(state == CGMS_IDLE)
    {
        // Get the address of the environment
        struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);

        // check if op code valid
        if((param->op_code < CGM_REQ_REP_STRD_RECS)
                || (param->op_code > CGM_REQ_REP_NUM_OF_STRD_RECS))
        {
            //Wrong op code
            status = PRF_ERR_INVALID_PARAM;
        }
        // check if RACP on going
        else if((param->op_code != CGM_REQ_ABORT_OP) && !(CGMS_IS(conidx, RACP_ON_GOING)))
        {
            //Cannot send response since no RACP on going
            status = PRF_ERR_REQ_DISALLOWED;
        }
        // Indication not enabled on peer device
        else if((cgms_env->ntf_ind_cfg[conidx] & CGMS_CFG_RACP_IND) == 0)
        {
            status = PRF_ERR_IND_DISABLED;
            // There is no more RACP on going
            CGMS_CLEAR(conidx, RACP_ON_GOING);
        }
        else
        {
            struct cgm_racp_rsp racp_rsp;
            // Allocate the GATT indication message
            struct gattc_send_evt_cmd *racp_rsp_val = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                    KE_BUILD_ID(TASK_GATTC, conidx), dest_id,
                    gattc_send_evt_cmd, CGM_REC_ACCESS_CTRL_MAX_LEN);

            // Fill in the parameter structure
            racp_rsp_val->operation = GATTC_INDICATE;
            racp_rsp_val->handle = CGMS_HANDLE(CGM_IDX_RACP_VAL);

            // Number of stored record calculation succeed.
            if((param->op_code == CGM_REQ_REP_NUM_OF_STRD_RECS)
                    && ( param->status == CGM_RSP_SUCCESS))
            {
                racp_rsp.op_code = CGM_REQ_NUM_OF_STRD_RECS_RSP;
                racp_rsp.operator = 0;
                racp_rsp.operand.num_of_record = param->num_of_record;
            }
            // Send back status information
            else
            {
                racp_rsp.op_code = CGM_REQ_RSP_CODE;
                racp_rsp.operand.rsp.op_code_req = param->op_code;
                racp_rsp.operand.rsp.value = param->status;
            }

            // pack measured value in database
            racp_rsp_val->length = cgms_pack_racp_rsp(racp_rsp_val->value, &racp_rsp);

            // There is no more RACP on going
            CGMS_CLEAR(conidx, RACP_ON_GOING);
            status = GAP_ERR_NO_ERROR;

            // Store that response sent by application
            CGMS_SET(conidx, RACP_RSP_SENT_BY_APP);

            // send RACP Response indication
            ke_msg_send(racp_rsp_val);
        }
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        // send completed information about request failed
        struct cgms_cmp_evt * cmp_evt = KE_MSG_ALLOC(CGMS_CMP_EVT, src_id, dest_id, cgms_cmp_evt);
        cmp_evt->request    = CGMS_SEND_RACP_RSP_IND_CMP;
        cmp_evt->status     = status;
        ke_msg_send(cmp_evt);
    }
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles @ref GATTC_CMP_EVT for GATTC_NOTIFY and GATT_INDICATE message meaning
 * that Measurement notification/indication has been correctly sent to peer device
 *
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_cmp_evt_handler(ke_msg_id_t const msgid, struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t state  = ke_state_get(dest_id);

    if(state == CGMS_IDLE)
    {
        // Get the address of the environment
        struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);

        switch(param->operation)
        {
            case GATTC_NOTIFY:
            {
                /* send message indication if an error occurs,
                 * or if all notification complete event has been received
                 */
                if((param->status != GAP_ERR_NO_ERROR) || !(CGMS_IS(conidx, MEAS_SENT)))
                {
                    CGMS_CLEAR(conidx, SENDING_MEAS);

                    // send completed information to APP task
                    struct cgms_cmp_evt * cmp_evt = KE_MSG_ALLOC(CGMS_CMP_EVT, prf_dst_task_get(&(cgms_env->prf_env), conidx),
                            dest_id, cgms_cmp_evt);

                    cmp_evt->request    = CGMS_SEND_MEAS_REQ_NTF_CMP;
                    cmp_evt->status     = param->status;

                    ke_msg_send(cmp_evt);
                }
                else
                {
                    // Measurement value notification sent
                    CGMS_SET(conidx, MEAS_SENT);
                }
            }
            break;
            case GATTC_INDICATE:
            {
                // verify if indication should be conveyed to application task
                if(CGMS_IS(conidx, RACP_RSP_SENT_BY_APP))
                {
                    // send completed information to APP task
                    struct cgms_cmp_evt * cmp_evt = KE_MSG_ALLOC(CGMS_CMP_EVT, prf_dst_task_get(&(cgms_env->prf_env), conidx),
                            dest_id, cgms_cmp_evt);

                    cmp_evt->request    = CGMS_SEND_RACP_RSP_IND_CMP;
                    cmp_evt->status     = param->status;

                    ke_msg_send(cmp_evt);
                    
                }
                CGMS_CLEAR(conidx, RACP_RSP_SENT_BY_APP);
            }
            break;

            default: break;
        }
    }

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler cgms_default_state[] =
{
    {CGMS_ENABLE_REQ,            (ke_msg_func_t) cgms_enable_req_handler},

    {GATTC_WRITE_REQ_IND,        (ke_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_READ_REQ_IND,         (ke_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_CMP_EVT,              (ke_msg_func_t) gattc_cmp_evt_handler},
    
    {CGMS_STATUS_SEND_RSP,       (ke_msg_func_t) cgms_status_send_rsp_handler},
    {CGMS_SEND_MEAS_CMD,         (ke_msg_func_t) cgms_meas_send_req_handler},
    {CGMS_SEND_RACP_RSP_CMD,     (ke_msg_func_t) cgms_send_racp_rsp_cmd_handler},
};


///Specifies the message handlers that are common to all states.
const struct ke_state_handler cgms_default_handler = KE_STATE_HANDLER(cgms_default_state);

#endif //BLE_CGM_SERVER

/// @} CGMSTASK

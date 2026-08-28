/**
 ****************************************************************************************
 *
 * @file plxs_task.c
 *
 * @brief Pulse Oximeter Service Server Task implementation.
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
 * @addtogroup PLXSTASK
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

#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "atts.h"
#include "plxs.h"
#include "plxs_task.h"
#include "prf_utils.h"


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref PLXS_ENABLE_REQ message.
 * The handler enables the Pulse Oximeter Profile Server Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int plxs_enable_req_handler(ke_msg_id_t const msgid,
                                   struct plxs_enable_req const *param,
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
    struct plxs_enable_rsp *rsp = KE_MSG_ALLOC(PLXS_ENABLE_RSP, src_id, dest_id, plxs_enable_rsp);
    rsp->conidx = param->conidx;
    rsp->status = status;
    ke_msg_send(rsp);

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the attribute info request message.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gattc_att_info_req_ind *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    struct plxs_env_tag* plxs_env = PRF_ENV_GET(PLXS, plxs);
    uint8_t att_idx = PLXS_IDX(param->handle);
    struct gattc_att_info_cfm * cfm;

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;

    switch(att_idx)
    {
        case PLX_IDX_SPOT_MEAS_VAL:
        case PLX_IDX_CONT_MEAS_VAL:
        {
            // force length to zero to reject any write starting from something != 0
            cfm->length = 0;
            cfm->status = GAP_ERR_NO_ERROR;
        }break;

        case PLX_IDX_SPOT_MEAS_IND_CFG:
        case PLX_IDX_CONT_MEAS_NTF_CFG:
        {
            cfm->length = PLXS_IND_NTF_CFG_MAX_LEN;
            cfm->status = GAP_ERR_NO_ERROR;
        }break;

        default:
        {
            cfm->status = ATT_ERR_REQUEST_NOT_SUPPORTED;
        }break;
    }

    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
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
    struct plxs_env_tag* plxs_env = PRF_ENV_GET(PLXS, plxs);
    uint8_t conidx = KE_IDX_GET(src_id);
    int msg_status = KE_MSG_CONSUMED;

    // retrieve handle information
    uint8_t status = GAP_ERR_NO_ERROR;

    // If the attribute has been found, status is GAP_ERR_NO_ERROR
    if (plxs_env != NULL)
    {
        // retrieve handle information
        uint8_t att_idx = PLXS_IDX(param->handle);
        
        switch(att_idx)
        {
            case PLX_IDX_SPOT_MEAS_IND_CFG:
                status = plxs_update_ntf_ind_cfg(conidx, PLXS_CFG_SPOT_MEAS_IND, PRF_CLI_START_IND, co_read16p(param->value));
                break;
            
            case PLX_IDX_CONT_MEAS_NTF_CFG:
                status = plxs_update_ntf_ind_cfg(conidx, PLXS_CFG_CONT_MEAS_NTF, PRF_CLI_START_NTF, co_read16p(param->value));
                break;
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

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
    cfm->handle = param->handle;
    cfm->status = status;
    ke_msg_send(cfm);

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
    uint8_t conidx  = KE_IDX_GET(src_id);
    ke_state_t state = ke_state_get(dest_id);

    if(state == PLXS_IDLE)
    {
        struct plxs_env_tag* plxs_env = PRF_ENV_GET(PLXS, plxs);
        uint8_t att_idx = PLXS_IDX(param->handle);
        
        switch(att_idx)
        {
            case PLX_IDX_FEAT_VAL:
            {
                size_t val_len = 2;
                const uint16_t feat_val = PLXS_FEAT_VALUE;
                if ((PLXS_FEAT_VALUE & PLXS_FEAT_MEASUREMENT_STAT_SUP) == PLXS_FEAT_MEASUREMENT_STAT_SUP)
                {
                   val_len += 2;
                }
                
                if ((PLXS_FEAT_VALUE & PLXS_FEAT_DEVICE_SENSOR_STAT_SUP) == PLXS_FEAT_DEVICE_SENSOR_STAT_SUP)
                {
                   val_len += 3;
                }
                
                struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, val_len);
                cfm->handle = param->handle;
                cfm->status = ATT_ERR_NO_ERROR;
                cfm->length = val_len;
                co_write16p(cfm->value, feat_val);
                if ((PLXS_FEAT_VALUE & PLXS_FEAT_MEASUREMENT_STAT_SUP) == PLXS_FEAT_MEASUREMENT_STAT_SUP)
                {
                   co_write16p(&cfm->value[2], PLXS_MEAS_STAT_VALUE);
                }
                if ((PLXS_FEAT_VALUE & PLXS_FEAT_DEVICE_SENSOR_STAT_SUP) == PLXS_FEAT_DEVICE_SENSOR_STAT_SUP)
                {
                   co_write24p(&cfm->value[4], PLXS_DEVICE_SENSOR_STAT_VALUE);
                }
                ke_msg_send(cfm);

            } break;
            case PLX_IDX_SPOT_MEAS_IND_CFG:
            {
                uint16_t res_val;
                
                struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(res_val));
                cfm->handle = param->handle;
                cfm->status = ATT_ERR_NO_ERROR;
                cfm->length = sizeof(res_val);
                co_write16p(cfm->value, ((plxs_env->ntf_ind_cfg[conidx] & PLXS_CFG_SPOT_MEAS_IND) != 0) ? PRF_CLI_START_IND : PRF_CLI_STOP_NTFIND);
                ke_msg_send(cfm);         
            } break;
            case PLX_IDX_CONT_MEAS_NTF_CFG:
            {
                uint16_t res_val;
                
                struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(res_val));
                cfm->handle = param->handle;
                cfm->status = ATT_ERR_NO_ERROR;
                cfm->length = sizeof(res_val);
                co_write16p(cfm->value, ((plxs_env->ntf_ind_cfg[conidx] & PLXS_CFG_CONT_MEAS_NTF) != 0) ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND);
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
    else if(state == PLXS_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }

    return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref PLXS_SPOT_MEAS_SEND_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int plxs_spot_meas_send_req_handler(ke_msg_id_t const msgid,
                                              struct plx_spot_check_meas* param,
                                              ke_task_id_t const dest_id,
                                              ke_task_id_t const src_id)
{
    
    // Status
    int msg_status = KE_MSG_SAVED; 
    uint8_t state = ke_state_get(dest_id);

    // check state of the task
    if(state == PLXS_IDLE)
    {
        // Get the address of the environment
        struct plxs_env_tag *plxs_env = PRF_ENV_GET(PLXS, plxs);

        // for spot-check measurement, feature must be enabled
        if(!PLXS_IS_FEATURE_SUPPORTED(plxs_env->features, PLXS_SPOT_MEAS_CHAR_SUP))
        {
            struct plxs_spot_meas_send_rsp *rsp = KE_MSG_ALLOC(PLXS_SPOT_MEAS_SEND_RSP, src_id, dest_id, plxs_spot_meas_send_rsp);
            rsp->status = PRF_ERR_FEATURE_NOT_SUPPORTED;
            ke_msg_send(rsp);
        }
        else
        { 
            // allocate operation to execute
            plxs_env->operation    = (struct plxs_op *) ke_malloc(sizeof(struct plxs_op) + PLXS_SPOT_MEAS_MAX_LEN, KE_MEM_ATT_DB);
            memset(plxs_env->operation, 0, sizeof(struct plxs_op) + PLXS_SPOT_MEAS_MAX_LEN);

            // Initialize operation parameters
            plxs_env->operation->cursor  = 0;
            plxs_env->operation->dest_id = src_id;
            plxs_env->operation->conidx  = GAP_INVALID_CONIDX;

            // Spot-check measurement indication or continuous measurement notification
            plxs_env->operation->op      = PLXS_CFG_SPOT_MEAS_IND;
            plxs_env->operation->handle  = PLXS_HANDLE(PLX_IDX_SPOT_MEAS_VAL);


            //Pack the temperature measurement value
            plxs_env->operation->length  = plxs_pack_spot_meas_value(&(plxs_env->operation->data[0]), *param);

            // put task in a busy state
            ke_state_set(dest_id, PLXS_BUSY);

            // execute operation
            plxs_exe_operation(); 
        }

        msg_status = KE_MSG_CONSUMED; 
    }

    return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref PLXS_CONT_MEAS_SEND_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int plxs_cont_meas_send_req_handler(ke_msg_id_t const msgid,
                                              struct plx_cont_meas* param,
                                              ke_task_id_t const dest_id,
                                              ke_task_id_t const src_id)
{
    
    // Status
    int msg_status = KE_MSG_SAVED; 
    uint8_t state = ke_state_get(dest_id);

    // check state of the task
    if(state == PLXS_IDLE)
    {
        // Get the address of the environment
        struct plxs_env_tag *plxs_env = PRF_ENV_GET(PLXS, plxs);

        // for spot-check measurement, feature must be enabled
        if(!PLXS_IS_FEATURE_SUPPORTED(plxs_env->features, PLXS_CONT_MEAS_CHAR_SUP))
        {
            struct plxs_cont_meas_send_rsp *rsp = KE_MSG_ALLOC(PLXS_CONT_MEAS_SEND_RSP, src_id, dest_id, plxs_cont_meas_send_rsp);
            rsp->status = PRF_ERR_FEATURE_NOT_SUPPORTED;
            ke_msg_send(rsp);
        }
        else
        { 
            // allocate operation to execute
            plxs_env->operation    = (struct plxs_op *) ke_malloc(sizeof(struct plxs_op) + PLXS_CONT_MEAS_MAX_LEN, KE_MEM_ATT_DB);
            memset(plxs_env->operation, 0, sizeof(struct plxs_op) + PLXS_CONT_MEAS_MAX_LEN);

            // Initialize operation parameters
            plxs_env->operation->cursor  = 0;
            plxs_env->operation->dest_id = src_id;
            plxs_env->operation->conidx  = GAP_INVALID_CONIDX;

            // Spot-check measurement indication or continuous measurement notification
            plxs_env->operation->op      = PLXS_CFG_CONT_MEAS_NTF;
            plxs_env->operation->handle  = PLXS_HANDLE(PLX_IDX_CONT_MEAS_VAL);


            //Pack the temperature measurement value
            plxs_env->operation->length  = plxs_pack_cont_meas_value(&(plxs_env->operation->data[0]), *param);

            // put task in a busy state
            ke_state_set(dest_id, PLXS_BUSY);

            // execute operation
            plxs_exe_operation(); 
        }

        msg_status = KE_MSG_CONSUMED; 
    }

    return (msg_status);
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
    // continue operation execution
    plxs_exe_operation();

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler plxs_default_state[] =
{
    {PLXS_ENABLE_REQ,            (ke_msg_func_t) plxs_enable_req_handler},

    {GATTC_ATT_INFO_REQ_IND,     (ke_msg_func_t) gattc_att_info_req_ind_handler},
    {GATTC_WRITE_REQ_IND,        (ke_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_READ_REQ_IND,         (ke_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_CMP_EVT,              (ke_msg_func_t) gattc_cmp_evt_handler},
    
    {PLXS_SPOT_MEAS_SEND_REQ,    (ke_msg_func_t) plxs_spot_meas_send_req_handler},
    {PLXS_CONT_MEAS_SEND_REQ,    (ke_msg_func_t) plxs_cont_meas_send_req_handler},
};


///Specifies the message handlers that are common to all states.
const struct ke_state_handler plxs_default_handler = KE_STATE_HANDLER(plxs_default_state);

#endif //BLE_PLX_SERVER

/// @} PLXSTASK

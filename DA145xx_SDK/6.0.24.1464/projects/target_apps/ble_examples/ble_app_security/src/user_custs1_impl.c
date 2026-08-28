/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.c
 *
 * @brief Peripheral project Custom1 Server implementation source code.
 *
 * Copyright (C) 2024-2025 Renesas Electronics Corporation and/or its affiliates.
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

#include "gpio.h"
#include "app_api.h"
#include "app.h"
#include "prf_utils.h"
#include "custs1.h"
#include "custs1_task.h"
#include "user_custs1_def.h"
#include "user_custs1_impl.h"
#include "user_periph_setup.h"
#include "user_data_db.h"

#if defined (CFG_PRINTF)
#include "arch_console.h"
#endif

#if defined (__DA14531__)
#include "battery.h"
#else
#include "adc_58x.h"
#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

bool ntf_enabled        __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

extern struct user_data_db usdb; 

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static void app_adcval1_ntf_send(uint16_t val)
{
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          sizeof(uint16_t));

    //req->conhdl = app_env->conhdl;
    req->handle = SVC1_IDX_ADC_VAL_1_VAL;
    req->length = sizeof(uint16_t);
    req->notification = true;
    memcpy(req->value, &val, sizeof(uint16_t));

    KE_MSG_SEND(req);
}

void user_svc1_ctrl_wr_ind_handler(ke_msg_id_t const msgid,
                                      struct custs1_val_write_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    uint8_t val = 0;
    uint16_t sample = 0;
    memcpy(&val, &param->value[0], param->length);

    switch(val)
    {
        case TRIGGER_ADC:
        {
            // ADC value to be sampled
            #if defined (__DA14531__)
            sample = battery_get_voltage(BATT_CR2032);
            #else
            if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
                sample = adc_get_vbat_sample(true);
            else
                sample = adc_get_vbat_sample(false);
            #endif
            
            // Store the aquired value in the database
            user_storage_save(sample);
            // Send the value via notification if 
            if (val != CUSTS1_CP_ADC_VAL1_DISABLE && ntf_enabled)
            {
                app_adcval1_ntf_send(sample);
            }
        } break;

        case TRIGGER_DT_ERASE_ALL:
        {
            if (user_app_usdb_erase())
            {
#if defined (CFG_PRINTF)
                arch_printf("\r\n User data have been Erased");
#endif
                if (app_easy_security_bdb_erase())
                {
#if defined (CFG_PRINTF)
                    arch_printf("\r\n Bond data have been Erased");
#endif
                    for(uint8_t i=0; i < CFG_MAX_CONNECTIONS; i++)
                    {
                        if (app_env[i].connection_active)
                            app_easy_gap_disconnect(app_env[i].conidx);
                    }
                    break;
                }
            }
#if defined (CFG_PRINTF)
            arch_printf("\r\n Failed erasing all NVM data");
#endif
        } break;
        
        default:
            break;
    }
}

void user_svc1_adc_val_1_cfg_ind_handler(ke_msg_id_t const msgid,
                                         struct custs1_val_write_ind const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
    ntf_enabled = (co_read16(param->value) == PRF_CLI_START_NTF) ? true : false;
}

void user_svc1_ctrl_rd_ind_handler(ke_msg_id_t const msgid,
                                           struct custs1_value_req_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    struct custs1_value_req_rsp *rsp = KE_MSG_ALLOC_DYN(CUSTS1_VALUE_REQ_RSP,
                                                        prf_get_task_from_id(TASK_ID_CUSTS1),
                                                        TASK_APP,
                                                        custs1_value_req_rsp,
                                                        DEF_SVC1_ADC_VAL_1_CHAR_LEN);

    // Provide the connection index.
    rsp->conidx  = app_env[param->conidx].conidx;
    // Provide the attribute index.
    rsp->att_idx = param->att_idx;
    // Force current length to zero.
    rsp->length  = sizeof(struct user_data_db);
    // Provide the ATT error code.
    rsp->status  = ATT_ERR_NO_ERROR;
    // Copy value
    memcpy(&rsp->value, &usdb, rsp->length);
    // Send message
    KE_MSG_SEND(rsp);
}

/**
 ****************************************************************************************
 *
 * @file system_library_585_586.c
 *
 * @brief System library source file.
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

#if !defined (__DA14531__)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <string.h>
#include "compiler.h"
#include "system_library.h"
#include "datasheet.h"
#include "arch.h"
#include "smpc_api.h"
#include "smpc.h"
#include "gapm_util.h"
#include "llc_util.h"
#include "lld_evt.h"
#include "llm.h"
#include "gapm_util.h"
#include "llc_task.h"
#include "l2cc_pdu.h"
#include "hci.h"
#include "ke_mem.h"
#include "gapc_task.h"
#include "l2cc_task.h"

/*
 * DEFINES
 *****************************************************************************************
 */

#if defined (CFG_START_SMP_TIMEOUT_TIMER_UPON_TX_SEC_REQ_CMD)
#define USE_START_SMP_TIMEOUT_TIMER_UPON_TX_SEC_REQ_CMD             (1)
#else
#define USE_START_SMP_TIMEOUT_TIMER_UPON_TX_SEC_REQ_CMD             (0)
#endif

#if defined (CFG_SKIP_SLAVE_LATENCY)
#define USE_SKIP_SLAVE_LATENCY                                      (1)
#else
#define USE_SKIP_SLAVE_LATENCY                                      (0)
#endif

#if defined (CFG_USE_HEAP_LOG)
#define USE_HEAP_LOG                                                (1)
#else
#define USE_HEAP_LOG                                                (0)
#endif

#define USE_FUNCTION_PATCH                                          (1)

#if defined (CFG_USE_DATA_PATCH_1)
#define USE_DATA_PATCH_1                                            (1)
#define DATA_PATCH_1_MASK                                           (0x00100000)
#else
#define USE_DATA_PATCH_1                                            (0)
#define DATA_PATCH_1_MASK                                           (0)
#endif

#if (CFG_USE_DATA_PATCH_2)
#define USE_DATA_PATCH_2                                            (1)
#define DATA_PATCH_2_MASK                                           (0x00200000)
#else
#define USE_DATA_PATCH_2                                            (0)
#define DATA_PATCH_2_MASK                                           (0)
#endif

#if (USE_HEAP_LOG)
    #if (!DEVELOPMENT_DEBUG)
        #error "USE_HEAP_LOG must not be set when building for production (DEVELOPMENT_DEBUG is 0)"
    #else
        #warning "USE_HEAP_LOG is set!"
    #endif
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

int atts_mtu_exc_req(uint8_t conidx, struct l2cc_att_mtu_req* req);
int PATCHED_atts_mtu_exc_req(uint8_t conidx, struct l2cc_att_mtu_req* req);
void PATCHED_smpc_dhkey_calc_start(void);
bool PATCHED_smpc_is_sec_mode_reached(uint8_t conidx, uint8_t role);
bool PATCHED_smpc_check_key_distrib(uint8_t conidx, uint8_t sec_level);
void smpc_construct_id_addr_info_pdu(struct l2cc_pdu *pdu, void *addr);
void PATCHED_smpc_construct_id_addr_info_pdu(struct l2cc_pdu *pdu, void *addr);
uint8_t PATCHED_gapm_get_ad_type_flag(uint8_t *data, uint8_t length);
void PATCHED_llc_util_gen_skdx(struct sess_k_div_x *skdx, struct rand_nb const *nb,
                                                        struct init_vect  const *ivx);
struct ea_elt_tag* PATCHED_lld_evt_move_to_master(struct ea_elt_tag *elt_scan, uint16_t conhdl,
                                                        struct llc_create_con_req_ind const *pdu_tx);
void PATCHED_llm_adv_report_set(struct hci_le_adv_report_evt *event, struct co_buf_rx_desc *desc);
uint8_t gapm_adv_sanity(uint8_t *adv_data, uint8_t adv_data_len,
                                                        uint8_t *scan_rsp_data, uint8_t scan_rsp_data_len);
uint8_t PATCHED_gapm_adv_sanity(uint8_t *adv_data, uint8_t adv_data_len,
                                                        uint8_t *scan_rsp_data, uint8_t scan_rsp_data_len);
void PATCHED_llc_ch_map_req_ind (uint16_t conhdl, struct llcp_channel_map_req const *param);
void PATCHED_llm_con_req_ind(struct co_buf_rx_desc *rxdesc);
void smpc_recv_pair_rand_pdu(uint8_t conidx, struct l2cc_pdu *pdu);
void PATCHED_smpc_recv_pair_rand_pdu(uint8_t conidx, struct l2cc_pdu *pdu);
void smpc_recv_public_key_exchange_pdu(uint8_t conidx, struct l2cc_pdu *pdu);
void PATCHED_smpc_recv_public_key_exchange_pdu(uint8_t conidx, struct l2cc_pdu *pdu);
void PATCHED_ke_free(void* mem_ptr);
void *PATCHED_ke_malloc(uint32_t size, uint8_t type);
void PATCHED_lld_evt_schedule_next(struct ea_elt_tag *elt);
int PATCHED_llcp_length_req_handler(ke_msg_id_t const msgid, struct llcp_length_req const *param,
                                        ke_task_id_t const dest_id, ke_task_id_t const src_id);
int PATCHED_hci_acl_data_rx_handler(ke_msg_id_t const msgid, struct hci_acl_data_rx const *param,
                                        ke_task_id_t const dest_id, ke_task_id_t const src_id);
int PATCHED_llcp_length_rsp_handler(ke_msg_id_t const msgid, struct llcp_length_rsp const *param,
                                        ke_task_id_t const dest_id, ke_task_id_t const src_id);
int PATCHED_gapm_reset_cmd_handler(ke_msg_id_t const msgid, struct gapm_reset_cmd *param,
                                        ke_task_id_t const dest_id, ke_task_id_t const src_id);
int PATCHED_hci_command_handler(ke_msg_id_t const msgid, void const *param,
                                        ke_task_id_t const dest_id, ke_task_id_t const src_id);
int PATCHED_llc_data_ind_handler(ke_msg_id_t const msgid, struct llc_data_ind const *param,
                                        ke_task_id_t const dest_id, ke_task_id_t const src_id);
int PATCHED_gapc_param_update_cmd_handler(ke_msg_id_t const msgid, struct  gapc_param_update_cmd *param,
                                        ke_task_id_t const dest_id, ke_task_id_t const src_id);
int PATCHED_l2cc_data_send_rsp_handler(ke_msg_id_t const msgid,
                                      struct l2cc_data_send_rsp *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id);
int hci_le_ltk_request_evt_handler(ke_msg_id_t const msgid,
                                      struct hci_le_ltk_request_evt const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id);
int PATCHED_hci_le_ltk_request_evt_handler(ke_msg_id_t const msgid,
                                      struct hci_le_ltk_request_evt const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id);
void smpc_pdu_send(uint8_t conidx, uint8_t cmd_code, void *value);
void PATCHED_smpc_pdu_send(uint8_t conidx, uint8_t cmd_code, void *value);
void smpc_recv_pair_req_pdu(uint8_t conidx, struct l2cc_pdu *pdu);
void PATCHED_smpc_recv_pair_req_pdu(uint8_t conidx, struct l2cc_pdu *pdu);
void smpc_recv_pair_fail_pdu(uint8_t conidx, struct l2cc_pdu *pdu);
void PATCHED_smpc_recv_pair_fail_pdu(uint8_t conidx, struct l2cc_pdu *pdu);

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint16_t heap_log                                   __SECTION_ZERO("retention_mem_area0");
uint8_t skip_slave_latency_mask                     __SECTION_ZERO("retention_mem_area0");

// Custom message handlers
const struct custom_msg_handler my_custom_msg_handlers[] =
{
    { TASK_LLC, LLCP_LENGTH_REQ, (ke_msg_func_t)PATCHED_llcp_length_req_handler},
    { TASK_L2CC, HCI_ACL_DATA_RX, (ke_msg_func_t)PATCHED_hci_acl_data_rx_handler},
    { TASK_LLC, LLCP_LENGTH_RSP, (ke_msg_func_t)PATCHED_llcp_length_rsp_handler},
    { TASK_GAPM, GAPM_RESET_CMD, (ke_msg_func_t)PATCHED_gapm_reset_cmd_handler},
    { TASK_LLM, HCI_COMMAND, (ke_msg_func_t)PATCHED_hci_command_handler},
    { TASK_LLC, LLC_DATA_IND, (ke_msg_func_t)PATCHED_llc_data_ind_handler},
#if USE_START_SMP_TIMEOUT_TIMER_UPON_TX_SEC_REQ_CMD
    { TASK_GAPC, L2CC_PDU_SEND_RSP, (ke_msg_func_t)PATCHED_l2cc_data_send_rsp_handler},
#endif
#if (USE_SKIP_SLAVE_LATENCY) && (!BLE_APP_PRESENT)
    { TASK_GAPC, GAPC_PARAM_UPDATE_CMD, (ke_msg_func_t)PATCHED_gapc_param_update_cmd_handler},
#endif
    // Must end with func = NULL so that ROM knows when this array ends
    { 0, 0, NULL}
};

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

#if (USE_FUNCTION_PATCH)
/*
* Up to 20 patch entries for ROM functions.
* Array that holds the ROM functions to be patched.
*/
static const void * const patch_orig_table[] = {
    (void *) atts_mtu_exc_req,                  // original function 1
    0, // reserved

    (void *) smpc_dhkey_calc_start,             // original function 2
    0, // reserved

    (void *) smpc_is_sec_mode_reached,          // original function 3
    0, // reserved

    (void *) smpc_check_key_distrib,            // original function 4
    0, // reserved

    (void *) smpc_construct_id_addr_info_pdu,   // original function 5
    0, // reserved

    (void *) gapm_get_ad_type_flag,             // original function 6
    0, // reserved

    (void *) llc_util_gen_skdx,                 // original function 7
    0, // reserved

    (void *) lld_evt_move_to_master,            // original function 8
    0, // reserved

    (void *) llm_adv_report_set,                // original function 9
    0, // reserved

    (void *) gapm_adv_sanity,                   // original function 10
    0, // reserved

    (void *) llc_ch_map_req_ind,                // original function 11
    0, // reserved

    (void *) llm_con_req_ind,                   // original function 12
    0, // reserved

    (void *) smpc_recv_pair_rand_pdu,           // original function 13
    0, // reserved

    (void *) smpc_recv_public_key_exchange_pdu, // original function 14
    0, // reserved

#if USE_START_SMP_TIMEOUT_TIMER_UPON_TX_SEC_REQ_CMD
    (void *) hci_le_ltk_request_evt_handler,    // original function 15
    0, // reserved

    (void *) smpc_pdu_send,                     // original function 16
    0, // reserved

    (void *) smpc_recv_pair_req_pdu,            // original function 17
    0, // reserved

    (void *) smpc_recv_pair_fail_pdu,           // original function 18
    0, // reserved
#endif

#if ((USE_SKIP_SLAVE_LATENCY) && (USE_HEAP_LOG))
    #error "USE_SKIP_SLAVE_LATENCY and USE_HEAP_LOG cannot be enabled at the same build."
#elif (USE_SKIP_SLAVE_LATENCY)
    (void *) lld_evt_schedule_next,             // original function 19
     0, // reserved
#elif (USE_HEAP_LOG)
    (void *) ke_free,                           // original function 19
    0, // reserved

    (void *) ke_malloc,                         // original function 20
    0, // reserved
#else
//    (void *) NULL,                           // original function 19
//    0, // reserved
//
//    (void *) NULL,                           // original function 20
//    0, // reserved
#endif
};

/*
* Up to 20 patch entries for ROM functions.
* Array that holds the function patches.
*/
static const void * const patch_new_table[] = {
    (void *) PATCHED_atts_mtu_exc_req,
    (void *) PATCHED_smpc_dhkey_calc_start,
    (void *) PATCHED_smpc_is_sec_mode_reached,
    (void *) PATCHED_smpc_check_key_distrib,
    (void *) PATCHED_smpc_construct_id_addr_info_pdu,
    (void *) PATCHED_gapm_get_ad_type_flag,
    (void *) PATCHED_llc_util_gen_skdx,
    (void *) PATCHED_lld_evt_move_to_master,
    (void *) PATCHED_llm_adv_report_set,
    (void *) PATCHED_gapm_adv_sanity,
    (void *) PATCHED_llc_ch_map_req_ind,
    (void *) PATCHED_llm_con_req_ind,
    (void *) PATCHED_smpc_recv_pair_rand_pdu,
    (void *) PATCHED_smpc_recv_public_key_exchange_pdu,
#if USE_START_SMP_TIMEOUT_TIMER_UPON_TX_SEC_REQ_CMD
    (void *) PATCHED_hci_le_ltk_request_evt_handler,
    (void *) PATCHED_smpc_pdu_send,
    (void *) PATCHED_smpc_recv_pair_req_pdu,
    (void *) PATCHED_smpc_recv_pair_fail_pdu,
#endif
#if ((USE_SKIP_SLAVE_LATENCY) && (USE_HEAP_LOG))
    #error "USE_SKIP_SLAVE_LATENCY and USE_HEAP_LOG cannot be enabled at the same build."
#elif (USE_SKIP_SLAVE_LATENCY)
    (void *) PATCHED_lld_evt_schedule_next,
#elif (USE_HEAP_LOG)
    (void *) PATCHED_ke_free,
    (void *) PATCHED_ke_malloc,
#else
//    (void *) NULL,
//    (void *) NULL,
#endif
};
#endif // (USE_FUNCTION_PATCH)

void patch_func(void)
{
    // Enable heap log
    heap_log = USE_HEAP_LOG;

    uint32_t bitmap = 0x00000000;

#if (USE_FUNCTION_PATCH)
#if defined (__IS_SDK6_COMPILER_GCC__) && !defined(__clang__)
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
    #pragma GCC diagnostic ignored "-Warray-bounds"
#endif
    memcpy((uint32_t *)PATCH_ADDR0_REG, patch_orig_table, sizeof(patch_orig_table));
    memcpy((uint32_t *)0x07fc00c0, patch_new_table, sizeof(patch_new_table)); // Function patches start at 0x07fc00c0
#if defined (__IS_SDK6_COMPILER_GCC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
    bitmap = 0xFFFFFFFF >> (32 - ARRAY_LEN(patch_new_table));
#endif

#if (USE_DATA_PATCH_1)
    patch_data1();
#endif

#if (USE_DATA_PATCH_2)
    patch_data2();
#endif

    // Enable HW patches
    SetWord32(PATCH_VALID_REG, bitmap | DATA_PATCH_1_MASK | DATA_PATCH_2_MASK);
}
#endif // DA14585, DA14586

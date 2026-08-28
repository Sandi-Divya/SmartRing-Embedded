/**
 ****************************************************************************************
 *
 * @file arch_rom.c
 *
 * @brief ROM initialization functions.
 *
 * Copyright (C) 2016-2024 Renesas Electronics Corporation and/or its affiliates.
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
 * INCLUDES
 ****************************************************************************************
 */

#include <string.h>
#include "compiler.h"
#include "sdk_defs.h"
#include "llm.h"
#include "gap_cfg.h"
#include "da1458x_scatter_config.h"
#include "_reg_ble_em_rx_buffer.h"
#include "_reg_ble_em_tx_buffer.h"
#include "em_map_ble.h"
#include "em_map.h"
#include "arch_rom.h"
#include "system_library.h"
#include "arch_ram.h"
#include "co_math.h"

#if defined (__DA14531__) && (USE_TRNG)
#include "trng.h"
#include "boot.h"
#endif

#ifdef CFG_DBG_TASK
#include "hci_vs.h"
#endif

/**
 * @addtogroup DRIVERS
 * @{
 */

/*
 * DEFINES
 ****************************************************************************************
 */

#if defined (__DA14531__)

#if (USE_TRNG)
#define RAM_UNINIT_START_ADDRESS        SDK_RAM_UNINIT_START_ADDRESS
#define RAM_UNINIT_END_ADDRESS          SDK_RAM_UNINIT_END_ADDRESS
#define RAM_UNINIT_SIZE_IN_BYTES        SDK_RAM_UNINIT_SIZE_IN_BYTES
#endif

#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

#if defined (__DA14531__)

extern const struct custom_msg_handler my_custom_msg_handlers[];
extern struct custom_msg_handler *custom_msg_handlers;

// _rand_state stdlib/rand.c RW variable provided by ROM project
extern uint32_t _rand_state_ROM_DATA;

#if (USE_TRNG)
volatile extern uint32_t trng_state_val;
#endif

#endif // defined (__DA14531__)

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void arch_rom_init(void)
{
#if defined (__DA14531__) && (!USE_CHACHA20_RAND)
    // Store value of _rand_state stdlib/rand.c RW variable
    uint32_t rand_temp = _rand_state_ROM_DATA;
#endif

#if defined (__DA14531__) && (USE_TRNG)
    // Given that the secondary bootloader and flash programmer
    // (UART version) leaves untouched the memory area between
    // RAM_UNINIT_START_ADDRESS and RAM_UNINIT_END_ADDRESS address,
    // this memory area can be used to feed the TRNG buffer with
    // true random data (random or unitialized values after RAM block power up).
    // The maximum size of the TRNG buffer is the difference between the
    // RAM_UNINIT_END_ADDRESS and RAM_UNINIT_START_ADDRESS, as defined in
    // `sdk_defs.h` header file.
    //
    // Note: If the FREE area (check scatter file), which may be used by the
    //       application, overlaps with the used TRNG buffer, then the
    //       application needs to make sure that the overlapped region does not
    //       contain initialized data or code. Read the SDK6 documentation for
    //       further details.
    if (trng_state_val != TRNG_MAGIC_NUMBER)
    {
        for (int i = 0; i <= RAM_UNINIT_SIZE_IN_BYTES; i = i + 4)
        {
            *(volatile uint32_t *) (DB_HEAP_BASE + i) = *(volatile uint32_t *) (RAM_UNINIT_START_ADDRESS + i);
        }
    }
#endif

    // Fill BLE EM + ROM data area in last RAM block, excluding Free Area
    //  region, (RAM4 for DA14585/586 - RAM3 for DA14531) with zeros.
    uint32_t *temp_ptr = (uint32_t *) __SCT_BLE_BASE;
#if defined(__IS_SDK6_COMPILER_GCC__) && !defined(__clang__)
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
    #pragma GCC diagnostic ignored "-Warray-bounds"
#endif
    memset(temp_ptr, 0, CO_ALIGN4_HI(__SCT_EM_BLE_END));
    temp_ptr = (uint32_t *) ROM_DATA_BASE_ADDR;
    memset(temp_ptr, 0 , RAM_END_ADDR - ROM_DATA_BASE_ADDR);
#if defined(__IS_SDK6_COMPILER_GCC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#if defined (__DA14531__) && (!USE_CHACHA20_RAND)
    // Restore _rand_state stdlib/rand.c RW variable
    _rand_state_ROM_DATA = rand_temp;
#endif

    // Initialize variables used by ROM patches
    patch_global_vars_init();

#if defined (__DA14531__)
    custom_msg_handlers = (struct custom_msg_handler *) my_custom_msg_handlers;
#endif

    // Mandatory HCI root table setup
    memcpy(hci_cmd_desc_root_tab, rom_hci_cmd_desc_root_tab, 48);

#if defined (__DA14531__)
    struct le_features my_llm_local_le_feats = {{ BLE_FEATURES_BYTE0, 0, 0, 0, 0, 0, 0, 0 }};
    memcpy(&llm_local_le_feats, &my_llm_local_le_feats, LE_FEATS_LEN);

    struct supp_cmds my_llm_local_cmds = {{BLE_CMDS_BYTE0, 0, BLE_CMDS_BYTE2, 0, 0, BLE_CMDS_BYTE5, 0, 0,
                                           0, 0, BLE_CMDS_BYTE10, 0, 0, 0, BLE_CMDS_BYTE14, BLE_CMDS_BYTE15,
                                           0, 0 ,0 ,0 ,0, 0, BLE_CMDS_BYTE22, 0,
                                           0, BLE_CMDS_BYTE25, BLE_CMDS_BYTE26, BLE_CMDS_BYTE27, BLE_CMDS_BYTE28, 0, 0, 0,
                                           BLE_CMDS_BYTE32, BLE_CMDS_BYTE33, BLE_CMDS_BYTE34, BLE_CMDS_BYTE35, 0, 0, 0, BLE_CMDS_BYTE39,
                                           0, BLE_CMDS_BYTE41, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0}};

    memcpy(&llm_local_cmds, &my_llm_local_cmds, SUPP_CMDS_LEN);
#endif

#if defined (__DA14531_01__)
    struct le_states my_llm_local_le_states = {{ BLE_STATES_BYTE0,
                                                 BLE_STATES_BYTE1,
                                                 BLE_STATES_BYTE2,
                                                 BLE_STATES_BYTE3,
                                                 BLE_STATES_BYTE4,
                                                 BLE_STATES_BYTE5,
                                                 BLE_STATES_BYTE6,
                                                 BLE_STATES_BYTE7 }};
    memcpy(&llm_local_le_states, &my_llm_local_le_states, LE_STATES_LEN);
#elif defined (__DA14535__)
    struct le_states my_llm_local_le_states = {{ BLE_STATES_BYTE0,
                                                 BLE_STATES_BYTE1,
                                                 BLE_STATES_BYTE2,
                                                 BLE_STATES_BYTE3,
                                                 BLE_STATES_BYTE4,
                                                 BLE_STATES_BYTE5,
                                                 BLE_STATES_BYTE6,
                                                 BLE_STATES_BYTE7 }};
    memcpy(&llm_local_le_states, &my_llm_local_le_states, LE_STATES_LEN);
#endif

#ifdef CFG_DBG_TASK
    // Vendor specific HCI commands initialization
    hci_vs_initialize_commands_tab();
#endif

#ifdef AUTO_DATA_LENGTH_NEGOTIATION_UPON_NEW_CONNECTION
    length_exchange_needed = true;
#endif

    use_h4tl = CFG_USE_H4TL;

    gap_cfg_user = (struct gap_cfg_user_struct *)&gap_cfg_user_var_struct;

    rom_func_addr_table = (uint32_t*)&rom_func_addr_table_var[0];
    rom_cfg_table = &rom_cfg_table_var[0];

#ifndef __DA14531_01__
    ble_duplicate_filter_max = CFG_BLE_DUPLICATE_FILTER_MAX;
#ifdef CFG_BLE_DUPLICATE_FILTER_FOUND
    ble_duplicate_filter_found = true;
#else
    ble_duplicate_filter_found = false;
#endif
#endif /* __DA14531_01__ */

#if !defined (__DA14535__)
    llm_resolving_list_max = CFG_LLM_RESOLVING_LIST_MAX;
#endif

    BLE_TX_DESC_DATA_USER           = __SCT_BLE_TX_DESC_DATA;
    BLE_TX_DESC_CNTL_USER           = __SCT_BLE_TX_DESC_CNTL;

    LLM_LE_ADV_DUMMY_IDX            = __SCT_BLE_TX_DESC_DATA          + __SCT_BLE_TX_DESC_CNTL - 1;
    LLM_LE_SCAN_CON_REQ_ADV_DIR_IDX = LLM_LE_ADV_DUMMY_IDX            + 1;
    LLM_LE_SCAN_RSP_IDX             = LLM_LE_SCAN_CON_REQ_ADV_DIR_IDX + 1;
    LLM_LE_ADV_IDX                  = LLM_LE_SCAN_RSP_IDX             + 1;

    REG_BLE_EM_RX_BUFFER_SIZE       = __SCT_REG_BLE_EM_RX_BUFFER_SIZE;
    REG_BLE_EM_TX_BUFFER_SIZE       = __SCT_REG_BLE_EM_TX_BUFFER_SIZE;

    ASSERT_ERROR(
        (BLE_TX_DESC_DATA == __SCT_BLE_TX_DESC_DATA) &&
        (BLE_TX_DESC_CNTL == __SCT_BLE_TX_DESC_CNTL) &&
        // offsets
        (_EM_BLE_RX_BUFFER_OFFSET == __SCT__EM_BLE_RX_BUFFER_OFFSET) &&
        (EM_BLE_RX_BUFFER_OFFSET  == __SCT_EM_BLE_RX_BUFFER_OFFSET ) &&
        (_EM_BLE_CNXADD_OFFSET    == __SCT__EM_BLE_CNXADD_OFFSET   ) &&
        (EM_BLE_CNXADD_OFFSET     == __SCT_EM_BLE_CNXADD_OFFSET    ) &&
        (_EM_BLE_END              == __SCT__EM_BLE_END             ) &&
        (EM_BLE_END               == __SCT_EM_BLE_END              )
    );

    _ble_base = __SCT_BLE_BASE; // Initialize BLE EM base
}

/// @} DRIVERS

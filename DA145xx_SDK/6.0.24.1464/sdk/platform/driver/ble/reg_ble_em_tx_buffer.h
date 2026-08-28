/**
 ****************************************************************************************
 *
 * @file reg_ble_em_tx_buffer.h
 *
 * @brief BLE Exchange Memory TX Buffer register definitions
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

#ifndef _REG_BLE_EM_TX_BUFFER_H_
#define _REG_BLE_EM_TX_BUFFER_H_

#include <stdint.h>
#include "_reg_ble_em_tx_buffer.h"
#include "compiler.h"
#include "arch.h"
#include "em_map.h"
#include "reg_access.h"

#define REG_BLE_EM_TX_BUFFER_COUNT 1

#define REG_BLE_EM_TX_BUFFER_DECODING_MASK 0x00000000

#define REG_BLE_EM_TX_BUFFER_ADDR_GET(idx) (EM_BLE_TX_BUFFER_OFFSET + (idx) * REG_BLE_EM_TX_BUFFER_SIZE)

/**
 * @brief TXBUF register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00                TXBUF   0x0
 * </pre>
 */
#define BLE_TXBUF_ADDR   (0x00 + (_ble_base) + EM_BLE_TX_BUFFER_OFFSET)
#define BLE_TXBUF_INDEX  0x00000000
#define BLE_TXBUF_RESET  0x00000000
#define BLE_TXBUF_COUNT  19

__STATIC_FORCEINLINE uint16_t ble_txbuf_get(int elt_idx, int reg_idx)
{
    ASSERT_ERR(reg_idx <= 18);
    return EM_BLE_RD(BLE_TXBUF_ADDR + elt_idx * REG_BLE_EM_TX_BUFFER_SIZE + reg_idx * 2);
}

__STATIC_FORCEINLINE void ble_txbuf_set(int elt_idx, int reg_idx, uint16_t value)
{
    ASSERT_ERR(reg_idx <= 18);
    EM_BLE_WR(BLE_TXBUF_ADDR + elt_idx * REG_BLE_EM_TX_BUFFER_SIZE + reg_idx * 2, value);
}

// field definitions
#define BLE_TXBUF_MASK   ((uint16_t)0x0000FFFF)
#define BLE_TXBUF_LSB    0
#define BLE_TXBUF_WIDTH  ((uint16_t)0x00000010)

#define BLE_TXBUF_RST    0x0

__STATIC_FORCEINLINE uint16_t ble_txbuf_getf(int elt_idx, int reg_idx)
{
    ASSERT_ERR(reg_idx <= 18);
    uint16_t localVal = EM_BLE_RD(BLE_TXBUF_ADDR + elt_idx * REG_BLE_EM_TX_BUFFER_SIZE + reg_idx * 2);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__STATIC_FORCEINLINE void ble_txbuf_setf(int elt_idx, int reg_idx, uint16_t txbuf)
{
    ASSERT_ERR(reg_idx <= 18);
    ASSERT_ERR((((uint16_t)txbuf << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_TXBUF_ADDR + elt_idx * REG_BLE_EM_TX_BUFFER_SIZE + reg_idx * 2, (uint16_t)txbuf << 0);
}


#endif // _REG_BLE_EM_TX_BUFFER_H_


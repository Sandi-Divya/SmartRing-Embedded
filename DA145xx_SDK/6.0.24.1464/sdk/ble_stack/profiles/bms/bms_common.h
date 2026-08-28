/**
 ****************************************************************************************
 *
 * @file bms_common.h
 *
 * @brief Header file - Bond Management Service common types.
 *
 * Copyright (C) 2015-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef BMS_COMMON_H_
#define BMS_COMMON_H_

/**
 ****************************************************************************************
 * @addtogroup BMS Bond Management Service
 * @ingroup BMS
 * @brief Bond Management Service
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_BMS_CLIENT || BLE_BMS_SERVER)

#include "prf_types.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// BMS v1.0.0 - OpCode definitions for BM Control Point Characteristic
#define BMS_OPCODE_DEL_BOND                         (0x03)
#define BMS_OPCODE_DEL_ALL_BOND                     (0x06)
#define BMS_OPCODE_DEL_ALL_BOND_BUT_PEER            (0x09)

/// BMS v1.0.0 - BM Feature Bit Description
#define BMS_FEAT_DEL_BOND                           (0x00000010)
#define BMS_FEAT_DEL_BOND_AUTHZ                     (0x00000020)
#define BMS_FEAT_DEL_ALL_BOND                       (0x00000400)
#define BMS_FEAT_DEL_ALL_BOND_AUTHZ                 (0x00000800)
#define BMS_FEAT_DEL_ALL_BOND_BUT_PEER              (0x00010000)
#define BMS_FEAT_DEL_ALL_BOND_BUT_PEER_AUTHZ        (0x00020000)

/// BMS v1.0.0 - Attribute Protocol Error codes
#define BMS_ATT_OPCODE_NOT_SUPPORTED                (0x80)
#define BMS_ATT_OPERATION_FAILED                    (0x81)

#define BMS_FEAT_DEL_BOND_SUPPORTED                 (BMS_FEAT_DEL_BOND | \
                                                     BMS_FEAT_DEL_BOND_AUTHZ)

#define BMS_FEAT_DEL_ALL_BOND_SUPPORTED             (BMS_FEAT_DEL_ALL_BOND | \
                                                     BMS_FEAT_DEL_ALL_BOND_AUTHZ)

#define BMS_FEAT_DEL_ALL_BOND_BUT_PEER_SUPPORTED    (BMS_FEAT_DEL_ALL_BOND_BUT_PEER | \
                                                     BMS_FEAT_DEL_ALL_BOND_BUT_PEER_AUTHZ)

/*
 * STRUCTURES DEFINITIONS
 ****************************************************************************************
 */

struct bms_cntl_point_op
{
    /// Operation code
    uint8_t op_code;

    /// Operand length
    uint16_t operand_length;

    /// Operand
    uint8_t operand[__ARRAY_EMPTY];
};

#endif // (BLE_BMS_CLIENT || BLE_BMS_SERVER)

/// @} BMS_common

#endif // BMS_COMMON_H_

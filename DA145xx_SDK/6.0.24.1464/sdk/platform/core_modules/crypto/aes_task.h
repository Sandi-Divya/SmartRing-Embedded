/**
 ****************************************************************************************
 * @addtogroup Core_Modules
 * @{
 * @addtogroup Crypto
 * @{
 * @addtogroup AES_TASK AES Task
 * @brief Task management for the Advanced Encryption Standard module
 *
 * The aes task is responsible for all security related functions not related to a
 * specific connection with a peer.
 * @{
 *
 * @file aes_task.h
 *
 * @brief AES task implementation header file.
 *
 * Copyright (C) 2017-2023 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef AES_TASK_H_
#define AES_TASK_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "rwip_config.h"
#include "gapm_task.h"          // GAP Manager Task Definitions
#include "gap.h"                // GAP Definitions

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum AES instance number
#define AES_IDX_MAX                    (0x01)

/**
 * Operation Codes (Mapped on GAPM Operation Codes)
 */

/// Use Encryption Block
#define AES_OP_USE_ENC_BLOCK           (GAPM_USE_ENC_BLOCK)
/// Generate a 8-byte random number
#define AES_OP_GEN_RAND_NB             (GAPM_GEN_RAND_NB)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// States of AES Task
enum aes_state_id
{
    /// IDLE state
    AES_IDLE,
    /// BUSY state: Communication with LLM ongoing state
    AES_BUSY,

    /// Number of defined states.
    AES_STATE_MAX
};


/// AES block API message IDs
enum aes_msg_id
{
    /// Encryption Toolbox Access
    AES_USE_ENC_BLOCK_CMD          = KE_FIRST_MSG(TASK_AES),

    /// Command Complete Event
    AES_CMP_EVT
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// AES Encrypt complete event structure
struct aes_cmp_evt
{
    /// Status of the command reception
    uint8_t status;
    ///Encrypted data to return to command source.
    uint8_t encrypted_data[16];
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

/// State handlers table
extern const struct ke_state_handler aes_state_handler[AES_STATE_MAX];

/// Default state handler
extern const struct ke_state_handler aes_default_handler;

/// States of all the task instances
extern ke_state_t aes_state[AES_IDX_MAX];

#endif // (AES_TASK_H_)

/// @}
/// @}
/// @}

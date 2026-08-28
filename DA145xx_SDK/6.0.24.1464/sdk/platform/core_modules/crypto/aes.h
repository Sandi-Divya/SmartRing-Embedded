/**
 ****************************************************************************************
 * @addtogroup Core_Modules
 * @{
 * @addtogroup Crypto
 * @{
 * @addtogroup AES_INIT AES Initialization
 * @brief Advanced Encryption Standard initialization API.
 * @{
 *
 * @file aes.h
 *
 * @brief AES header file.
 *
 * Copyright (C) 2017-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef AES_H_
#define AES_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "co_bt.h"
#include "ke_msg.h"
#include "sw_aes.h"

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// AES key
typedef AES_CTX AES_KEY;

/// AES environment struct
struct aes_env_tag
{
    /// Request operation Kernel message
    void *operation;
    
    /// Operation requester task id
    ke_task_id_t requester;

    /// Encryption/Decryption operation mode selection
    uint8_t enc_dec;
    
    /// Key length
    uint32_t key_len;
    
    /// Key
    uint8_t *key;
    
    /// AES key
    AES_KEY aes_key;
    
    /// Plaintext
    uint8_t *in;
    
    /// Plaintext length
    uint32_t in_len;
    
    /// Plaintext index
    uint32_t in_cur;
    
    /// Ciphered text
    uint8_t *out;
    
    /// Ciphered text length
    uint32_t out_len;
    
    /// Ciphered text index
    uint32_t out_cur;

    /// Callback function to be called upon encryption/decryption completion
    void (*aes_done_cb)(uint8_t status);

    /// Synchronous/asynchronous mode selection
    uint8_t ble_flags;
 };

/*
 * GLOBAL VARIABLES DECLARATION
 ****************************************************************************************
 */

/// AES environment
extern struct aes_env_tag aes_env;

/// AES output
extern uint8_t aes_out[ENC_DATA_LEN];

/*
 * PUBLIC FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the AES task.
 * @param[in] reset       FALSE will create the task, TRUE will just reset the environment.
 * @param[in] aes_done_cb Callback function to be called upon encryption/decryption completion
 ****************************************************************************************
 */
void aes_init(bool reset, void (*aes_done_cb)(uint8_t status));

/**
 ****************************************************************************************
 * @brief AES encrypt/decrypt operation.
 * @param[in] key           The key data.
 * @param[in] key_len       The key data length in bytes (should be 16).
 * @param[in] in            The input data block.
 * @param[in] in_len        The input data block length.
 * @param[in] out           The output data block.
 * @param[in] out_len       The output data block length.
 * @param[in] enc_dec       AES_ENCRYPT for encryption, AES_DECRYPT for decryption.
 * @param[in] aes_done_cb   The callback to be called at the end of each operation.
 * @param[in] ble_flags     Used to specify whether the encryption/decryption
 *                          will be performed synchronously or asynchronously (message based)
 *                          also if ble_safe is specified in ble_flags rwip_schedule() will be called
 *                          to avoid missing any BLE events.
 * @return                   0 if successful,
 *                          -1 if userKey or key are NULL,
 *                          -2 if AES task is busy,
 *                          -3 if enc_dec not 0/1,
 *                          -4 if key_len not 16.
 ****************************************************************************************
 */
int aes_operation(uint8_t *key,
                  uint8_t key_len,
                  uint8_t *in,
                  uint32_t in_len,
                  uint8_t *out,
                  uint32_t out_len,
                  uint8_t enc_dec,
                  void (*aes_done_cb)(uint8_t status),
                  uint8_t ble_flags);

/**
 ****************************************************************************************
 * @brief Send a aes_CMP_EVT message to the task which sent the request.
 *
 * @param[in] cmd_src_id            Command source ID
 * @param[in] operation             Command operation code
 * @param[in] status                Status of the request
 ****************************************************************************************
 */
void aes_send_cmp_evt(ke_task_id_t cmd_src_id, uint8_t operation, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send an encryption request to the LLM.
 * @param[in] operand_1             Operand 1
 * @param[in] operand_2             Operand 2
 ****************************************************************************************
 */
void aes_send_encrypt_req(uint8_t *operand_1, uint8_t *operand_2);

#endif // (AES_H_)

/// @}
/// @}
/// @}

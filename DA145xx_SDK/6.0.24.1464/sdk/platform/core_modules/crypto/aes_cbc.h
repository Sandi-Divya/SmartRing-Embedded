/**
 ****************************************************************************************
 * @addtogroup Core_Modules
 * @{
 * @addtogroup Crypto
 * @{
 * @addtogroup AES_CBC AES CBC
 * @brief Advanced Encryption Standard CBC API.
 * @{
 *
 * @file aes_cbc.h
 *
 * @brief AES-CBC header file.
 *
 * Copyright (C) 2018-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef AES_CBC_H_
#define AES_CBC_H_

/*
 * DEFINES
 ****************************************************************************************
 */

/// Input block size (128 bits)
#define AES_CBC_BLK_SIZE (16)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Status codes
enum
{
    AES_CBC_ERR_NO_ERR,
    AES_CBC_ERR_INVALID_PARAM,
};

/*
 * PUBLIC FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief CBC encryption using AES cipher
 * @param[in] blocks_128        Input data blocks
 * @param[in] block_count       Number of input blocks
 * @param[out] out_block_128    Output block for CBC mode result
 * @param[in] out_block_count   Number of output blocks
 * @param[in] key               Key
 * @param[in] key_size          Key length (in bytes)
 * @param[in] IV_128            Input vector for 128-bit data block cypher
 * @return                      error code if something goes wrong, 0 otherwise
 * @note Blocks-128 should point to a memory ares which is a multiple of 128-bit
 * data blocks in size. Additionally if there are fewer output blocks than the number of
 * input blocks, only the last ciphertext block will be stored in the first output
 * block (CBC-MAC mode).
 ****************************************************************************************
 */
uint8_t aes_cbc_encrypt(const uint8_t *blocks_128, const uint8_t block_count,
                        uint8_t *out_block_128, const uint8_t out_block_count,
                        const uint8_t *key, const uint8_t key_size, const uint8_t *IV_128);

/**
 ****************************************************************************************
 * @brief CBC decryption using AES cipher
 * @param[in] blocks_128        Input data blocks
 * @param[out] out_blocks_128   Output block for CBC mode result
 * @param[in] block_count       Number of output blocks
 * @param[in] key               Key
 * @param[in] key_size          Key length (in bytes)
 * @param[in] IV_128            Input vector for 128-bit data block cypher
 * @return                      error code if something goes wrong, 0 otherwise
 * @note Blocks-128 should point to a memory area whose size is a multiple of
 * 128-bit data blocks.
 ****************************************************************************************
 */
uint8_t aes_cbc_decrypt(const uint8_t *blocks_128, uint8_t *out_blocks_128,
                        const uint8_t block_count, const uint8_t *key,
                        const uint8_t key_size, const uint8_t *IV_128);

/**
 ****************************************************************************************
 * @brief Calculate number of blocks given the payload size.
 * @param[in] size                  payload size in bytes,
 * @return                          number of blocks of size ENC_DATA_LEN
 ****************************************************************************************
 */
uint8_t aes_get_blk_num(uint16_t size);

/**
 ****************************************************************************************
 * @brief Xor data buffers
 * @param[in] s1        1st data buffer
 * @param[in] s2        2nd data buffer
 * @param[in] size      number of bytes to be xored
 * @param[out] des      buffer for the result
 ****************************************************************************************
 */
void aes_array_xor(const uint8_t *s1, const uint8_t *s2, uint16_t size, uint8_t *des);

#endif // AES_CBC_H_

/// @}
/// @}
/// @}

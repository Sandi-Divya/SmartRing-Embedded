/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Easy_Crypto Easy Crypto
 * @brief Crypto-related helper functions API
 * @{
 *
 * @file app_easy_crypto.h
 *
 * @brief Application crypto helper functions header file.
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

#ifndef _APP_EASY_CRYPTO_H_
#define _APP_EASY_CRYPTO_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if defined (CFG_USE_AES) && defined (CFG_AES_DECRYPT)

#include "user_config.h"

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize and provide a key to the encrypt / decrypt mechanism
 ****************************************************************************************
 */
void app_easy_aes_crypto_init(void);

/**
 ****************************************************************************************
 * @brief Start the data encryption
 * @param[in]   key          The key data
 * @param[in]   key_len      The key data length in bytes (should be 16).
 * @param[in]   in           Data to be encrypted
 * @param[out]  out          Encrypted data output
 * @param[in]   size         Size of data to be encrypted
 ****************************************************************************************
 */
void app_easy_aes_encrypt_data(uint8_t *key, uint8_t key_len, uint8_t *in, uint8_t *out, uint16_t size);

/**
 ****************************************************************************************
 * @brief Start data decryption
 * @param[in]       key          The key data
 * @param[in]       key_len      The key data length in bytes (should be 16).
 * @param[in,out]   in_out       Data to be decrypted, overwriten with result upon return
 * @param[in]       size         Size of data to be decrypted
 ****************************************************************************************
 */
void app_easy_aes_decrypt_data(uint8_t *key, uint8_t key_len, uint8_t *in, uint8_t *out, uint16_t size);

#endif // SECURE_DATA_STORAGE

#endif // _APP_EASY_CRYPTO_H_

///@}
///@}

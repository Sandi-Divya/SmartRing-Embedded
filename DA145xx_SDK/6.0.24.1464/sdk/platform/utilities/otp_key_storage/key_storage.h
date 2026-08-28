/**
 ****************************************************************************************
 *
 * @file key_storage.h
 *
 * @brief Key Storage management header file
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
#ifndef KEY_STORAGE_H_
#define KEY_STORAGE_H_

#include <stdint.h>
#include "otp_utils.h"

#if SECURE_BOOT_ECC_IMAGE_SUPPORT
#define KEY_STORAGE_HAS_ECC_KEYS                             (1)
#else
#define KEY_STORAGE_HAS_ECC_KEYS                             (0)
#endif

#if AES_ENCRYPTED_IMAGE_SUPPORTED
#define KEY_STORAGE_HAS_AES_KEYS                             (1)
#else
#define KEY_STORAGE_HAS_AES_KEYS                             (0)
#endif

#include "key_storage_defs.h"

typedef enum {
#if KEY_STORAGE_HAS_AES_KEYS
    KEY_TYPE_AES,
#endif // KEY_STORAGE_HAS_AES_KEYS
#if KEY_STORAGE_HAS_ECC_KEYS
    KEY_TYPE_ECC,
#endif // KEY_STORAGE_HAS_ECC_KEYS
    KEY_TYPE_MAX,
} KEY_TYPE;

typedef struct {
    uint8_t key[AES_USED_KEY_SIZE];
    uint8_t iv[AES_USED_IV_SIZE];
} key_storage_aes_key_parts_t;

typedef struct {
    uint8_t key[ECC_USED_KEY_SIZE];
} key_storage_ecc_key_parts_t;

typedef union {
    key_storage_aes_key_parts_t aes;
    key_storage_ecc_key_parts_t ecc;
} key_storage_key_t;

/**
 *****************************************************************************************
 * @brief Load an AES key-pair (IV and key) or ECC key from the storage(RAM or OTP)
 *
 * @param[in]  key_type The key type that should be loaded from the storage
 * @param[in]  idx      The index of the key that we want to load (if we have multiple keys)
 * @return     if loading is succesfull a pointer to the key in the cache
 *         will be returned, NULL otherwise
 *****************************************************************************************
 */
key_storage_key_t *key_storage_load_key_in_cache(KEY_TYPE key_type, uint8_t idx);

/**
 *****************************************************************************************
 * @brief Check if a provided key index is valid
 *
 * @param[in]  key_type The key type whose index should be checked
 * @param[in]  idx      The index of the key that we want to check
 * @return     1 if index is valid, 0 if not
 *****************************************************************************************
 */
uint8_t key_storage_is_key_index_valid(KEY_TYPE key_type, uint8_t idx);

/**
 *****************************************************************************************
 * @brief Check the revokation flag of a key inside the OTP (either AES or ECC)
 *
 * @param[in] key_type The key type that should be checked for revokation
 * @param[in] idx      The index of the key to be checked for revokation
 * @return             1 if the requested key is revoked, 0 otherwise
 *                     Thi function will always return 0 if the keys are stored in RAM
 *****************************************************************************************
 */
uint8_t key_storage_otp_is_key_revoked(KEY_TYPE key_type, uint8_t key_idx);

/**
 *****************************************************************************************
 * @brief Revoke a key in the OTP. This functions will write the revokation flag
 * of the requested key with a value that marks it as revoked
 *
 * @param[in] key_type The key type that should be checked for revokation
 * @param[in] idx      The index of the key to be checked for revokation
 * @return             1 if the requested key has been revoked succesfully, 0 otherwise
 *                     Thi function will always return 0 if the keys are stored in RAM
 *****************************************************************************************
 */
uint8_t key_storage_otp_revoke(KEY_TYPE key_type, uint8_t key_idx);

#endif // KEY_STORAGE_H_

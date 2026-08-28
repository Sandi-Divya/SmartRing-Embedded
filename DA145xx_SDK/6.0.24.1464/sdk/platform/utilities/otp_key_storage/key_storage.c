/**
 ****************************************************************************************
 *
 * @file key_storage.c
 *
 * @brief Key Storage management source file
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

#include "key_storage.h"
#include "string.h"

#if 0
/*
 * Build Time information messages for the location of each key storage area
 */
#define STR(x) #x
#define XSTR(x) STR(x)

#pragma message("INFO: The OTP start address of the AES keys is at " XSTR(KEY_STORAGE_OTP_AES_KEY_BASE_ADDRESS))
#pragma message("INFO: The OTP start address of the AES keys revokation flags is at " XSTR(KEY_STORAGE_OTP_AES_KEY_RVK_FLAG_BASE_ADDRESS))

#pragma message("INFO: The OTP start address of the ECC keys is at " XSTR(KEY_STORAGE_OTP_ECC_KEY_BASE_ADDRESS))
#pragma message("INFO: The OTP start address of the ECC keys revokation flags is at " XSTR(KEY_STORAGE_OTP_ECC_KEY_RVK_FLAG_BASE_ADDRESS))
#endif

typedef struct {
    uint32_t key_base_addr;
    uint32_t key_rvk_base_addr;
    uint32_t key_size;
} key_storage_otp_properties_t;

static const key_storage_otp_properties_t key_storage_otp_properties[KEY_TYPE_MAX] = {
#if KEY_STORAGE_HAS_AES_KEYS
    [KEY_TYPE_AES] = {
                        .key_base_addr     = KEY_STORAGE_OTP_AES_KEY_BASE_ADDRESS,
                        .key_rvk_base_addr = KEY_STORAGE_OTP_AES_KEY_RVK_FLAG_BASE_ADDRESS,
                        .key_size          = AES_STORAGE_KEY_SIZE
                     },
#endif // KEY_STORAGE_HAS_AES_KEYS

#if KEY_STORAGE_HAS_ECC_KEYS
    [KEY_TYPE_ECC] = {
                        .key_base_addr     = KEY_STORAGE_OTP_ECC_KEY_BASE_ADDRESS,
                        .key_rvk_base_addr = KEY_STORAGE_OTP_ECC_KEY_RVK_FLAG_BASE_ADDRESS,
                        .key_size          = ECC_STORAGE_KEY_SIZE
                     },
#endif // KEY_STORAGE_HAS_ECC_KEYS
};

static key_storage_key_t key_storage_cache;

static uint8_t _key_storage_get_key(KEY_TYPE key_type, uint32_t idx, key_storage_key_t *key)
{
    if(key_type >= KEY_TYPE_MAX) return 0;

    if(!key_storage_is_key_index_valid(key_type, idx)) {
        return 0;
    }

    otp_utils_read_data(key_storage_otp_properties[key_type].key_base_addr + idx * key_storage_otp_properties[key_type].key_size, 
                    (uint8_t*)key, 
                    key_storage_otp_properties[key_type].key_size);
    return 1;
}

uint8_t key_storage_is_key_index_valid(KEY_TYPE key_type, uint8_t idx)
{
#if KEY_STORAGE_HAS_ECC_KEYS
    if(key_type == KEY_TYPE_ECC && idx < KEY_STORAGE_ECC_KEYS_NUM) {
        return 1;
    }
#endif // KEY_STORAGE_HAS_ECC_KEYS

#if KEY_STORAGE_HAS_AES_KEYS
    if(key_type == KEY_TYPE_AES && idx < KEY_STORAGE_AES_KEYS_NUM) {
        return 1;
    }
#endif // KEY_STORAGE_HAS_AES_KEYS

    return 0;
}

key_storage_key_t* key_storage_load_key_in_cache(KEY_TYPE key_type, uint8_t idx)
{
    if(key_type >= KEY_TYPE_MAX) return NULL;

    if(_key_storage_get_key(key_type, idx, &key_storage_cache)) {
        return &key_storage_cache;
    }

    return NULL;
}

uint8_t key_storage_otp_is_key_revoked(KEY_TYPE key_type, uint8_t key_idx)
{
    uint8_t key_revokation_flag[KEY_STORAGE_OTP_RVK_FLAG_SIZE];
    uint8_t i;

    // Read the revokation flag data from the OTP
    otp_utils_read_data(key_storage_otp_properties[key_type].key_rvk_base_addr + key_idx * KEY_STORAGE_OTP_RVK_FLAG_SIZE, key_revokation_flag, KEY_STORAGE_OTP_RVK_FLAG_SIZE);

    // Check if the revokation flag is written (which means the respective key is revoked)
    for (i = 0; i < KEY_STORAGE_OTP_RVK_FLAG_SIZE; i++) {
        if(key_revokation_flag[i] != OTP_EMPTY_CELL_VALUE_BYTE) {
            return 1;
        }
    }

    return 0;
}

uint8_t key_storage_otp_revoke(KEY_TYPE key_type, uint8_t key_idx)
{
    uint8_t key_revokation_flag[KEY_STORAGE_OTP_RVK_FLAG_SIZE];

    if(!key_storage_is_key_index_valid(key_type, key_idx)) {
        return 0;
    }

    // If the key is already revoked, return
    if(key_storage_otp_is_key_revoked(key_type, key_idx)) {
        return 1;
    }

    memset((uint8_t*)key_revokation_flag, OTP_EMPTY_CELL_VALUE_BYTE ^ 0x01, KEY_STORAGE_OTP_RVK_FLAG_SIZE);
    otp_utils_write_data(key_storage_otp_properties[key_type].key_rvk_base_addr + key_idx * KEY_STORAGE_OTP_RVK_FLAG_SIZE, key_revokation_flag, KEY_STORAGE_OTP_RVK_FLAG_SIZE);

    return 1;
}

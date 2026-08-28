/**
 ****************************************************************************************
 * @addtogroup Core_Modules
 * @{
 * @addtogroup Crypto
 * @{
 * @addtogroup AES
 * @brief Advanced Encryption Standard API.
 * @{
 *
 * @file aes_api.h
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

#ifndef _AES_API_H_
#define _AES_API_H_

#include <stdint.h>
#include "sw_aes.h"
#include "aes.h"

/// GETU32 helper macro
#define GETU32(pt)     (((uint32_t)(pt)[0] << 24) ^ ((uint32_t)(pt)[1] << 16) ^ ((uint32_t)(pt)[2] <<  8) ^ ((uint32_t)(pt)[3]))
/// PUTU32 helper macro
#define PUTU32(ct, st) { (ct)[0] = (uint8_t)((st) >> 24); (ct)[1] = (uint8_t)((st) >> 16); (ct)[2] = (uint8_t)((st) >>  8); (ct)[3] = (uint8_t)(st); }

/// BLE safe mask
#define BLE_SAFE_MASK   0x0F
/// BLE sync mask
#define BLE_SYNC_MASK   0xF0

/**
 ****************************************************************************************
 * @brief AES set encrypt/decrypt key.
 * @param[in] userKey   The key data.
 * @param[in] bits      Key number of bits. Should be 128
 * @param[in] key       AES_KEY structure pointer.
 * @param[in] enc_dec   0 set decrypt key, 1 set encrypt key.
 * @return               0 if successfull,
 *                      -1 if userKey or key are NULL,
 *                      -2 if bits is not 128.
 ****************************************************************************************
 */
int aes_set_key(const uint8_t *userKey, const uint32_t bits, AES_KEY *key, uint8_t enc_dec);

/**
 ****************************************************************************************
 * @brief AES encrypt/decrypt block.
 * @param[in] in            The input data block (16bytes).
 * @param[in] out           The output data block (16bytes).
 * @param[in] key           The key data.
 * @param[in] enc_dec       AES_ENCRYPT for encryption, AES_DECRYPT for decryption.
 * @param[in] ble_flags     Used to specify whether the encryption/decryption
 *                          will be performed synchronously or asynchronously (message based)
 *                          also if ble_safe is specified in ble_flags rwip_schedule() will be called
 *                          to avoid missing any BLE events.
 * @return                   0 if successfull,
 *                          -1 if the AES HW block is in use.
 ****************************************************************************************
 */
int aes_enc_dec(uint8_t *in, uint8_t *out, AES_KEY *key, uint8_t enc_dec, uint8_t ble_flags);

#endif  //_AES_API_H_

/// @}
/// @}
/// @}

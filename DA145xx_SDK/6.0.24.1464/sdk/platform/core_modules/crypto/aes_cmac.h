/**
 ****************************************************************************************
 * @addtogroup Core_Modules
 * @{
 * @addtogroup Crypto
 * @{
 * @addtogroup AES_CMAC AES CMAC
 * @brief Advanced Encryption Standard CMAC API.
 * @{
 *
 * @file aes_cmac.h
 *
 * @brief AES-CMAC header file.
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

#ifndef AES_CMAC_H_
#define AES_CMAC_H_

/*
 * DEFINES
 ****************************************************************************************
 */

/// Input block size (128 bits)
#define AES_CMAC_BLK_SIZE_128 (16)

/*
 * PUBLIC FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Generate tag using AES-CMAC
 * @param[in] payload           Input payload
 * @param[in] payload_len       Payload length (in bytes)
 * @param[in] key               Key (128bit)
 * @param[out] mac              MAC output buffer
 * @param[in] mac_len           MAC buffer length (in bytes)
 * @return                      Error code if something goes wrong, 0 otherwise
 ****************************************************************************************
 */
uint8_t aes_cmac_generate(const uint8_t *payload, uint16_t payload_len,
                          const uint8_t *key, uint8_t *mac, uint8_t mac_len);

/**
 ****************************************************************************************
 * @brief Verify tag of the message using AES-CMAC
 * @param[in] payload           Input payload
 * @param[in] payload_len       Payload length (in bytes)
 * @param[in] key               Key (128bit)
 * @param[in] mac               MAC to verify
 * @param[in] mac_len           MAC buffer length (in bytes)
 * @return                      True if provided mac was successfully verified, False otherwise
 ****************************************************************************************
 */
bool aes_cmac_verify(const uint8_t *payload, uint16_t payload_len,
                     const uint8_t *key, const uint8_t *mac, uint8_t mac_len);

#endif // AES_CMAC_H_

/// @}
/// @}
/// @}

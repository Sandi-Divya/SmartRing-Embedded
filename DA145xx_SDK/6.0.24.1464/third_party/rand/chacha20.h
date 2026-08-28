/**
 ****************************************************************************************
 * @addtogroup Third-party Third-party APIs
 * @{
 * @addtogroup Rand
 * @brief ChaCha20 API
 * @{
 *
 * @file chacha20.h
 *
 * @brief ChaCha20 random generator API.
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

#ifndef _CHACHA20_H_
#define _CHACHA20_H_

#if (USE_CHACHA20_RAND)

#if defined (__DA14531__) && !defined (__EXCLUDE_ROM_CHACHA20__)
/// ChaCha20 configuration
typedef struct
{
    /// Key
    uint32_t *key;

    /// Counter
    uint64_t *counter;

    /// Random output
    uint32_t *random_output;

    /// Random output left
    uint8_t *random_output_left;
} rom_chacha20_cfg_t;
#endif

/**
 ****************************************************************************************
 * @brief Initialize ChaCha20 seed
 * @param[in] key initialization value
 ****************************************************************************************
 */
void csprng_seed(const uint8_t key[16]);

/**
 ****************************************************************************************
 * @brief Get ChaCha20 random 32-bit number
 * @return Random number
 ****************************************************************************************
 */
uint32_t csprng_get_next_uint32(void);

#endif
#endif

///@}
///@}

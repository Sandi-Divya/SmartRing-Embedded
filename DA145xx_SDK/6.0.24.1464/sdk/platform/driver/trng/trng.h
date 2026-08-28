/**
 ****************************************************************************************
 * @addtogroup Drivers
 * @{
 * @addtogroup TRNG
 * @brief True Random Number Generator driver API
 * @{
 *
 * @file trng.h
 *
 * @brief True Random Number Generator header file.
 *
 * Copyright (C) 2014-2023 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _TRNG_H_
#define _TRNG_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "sdk_defs.h"

#if defined (__DA14531__)
#include "iid_irng.h"
#endif

/*
 * DEFINITIONS
 *****************************************************************************************
 */

#if defined (__DA14531__)

/// TRNG seed size in bytes
#define TRNG_SEED_SIZE      (IRNG_RANDOM_SEED_SIZE_BYTES)

/// Number of uninitialized RAM blocks used to generate random data
#define TRNG_NUM_BLOCKS     (IRNG_MINIMUM_SRAM_PUF_BLOCKS)

/// Size of one block of uninitialized RAM
#define TRNG_BLOCK_SIZE     (IRNG_BLOCK_SIZE_BYTES)

/// Size of used RAM
#define TRNG_RAM_SIZE_USED  (TRNG_NUM_BLOCKS * TRNG_BLOCK_SIZE)

/// Magic number to decide if TRNG must run or not
#define TRNG_MAGIC_NUMBER       (0x12348765)

#if TRNG_RAM_SIZE_USED > SDK_TRNG_RAM_SIZE
#error "The TRNG used size cannot not be greater than the max allocated size"
#endif

#else

/// TRNG seed size in bytes
#define TRNG_SEED_SIZE      (16)

#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if defined (__DA14531__)

/**
 ****************************************************************************************
 * @brief  Acquires 128 random bits using uninitialized RAM data.
 *
 * @param[out] trng_bits_ptr    Pointer to the buffer where random data will be stored.
 * @note Its address must be aligned to 32 bits.
 * @param[in] sram_puf          Pointer to the base address of the uninitialized SRAM area.
 * @note Its address must be aligned to 32 bits.
 * @param[in] sram_puf_blocks   Number of 16byte blocks to use (28 minimum).
 * @return Status code
 ****************************************************************************************
 */
uint8_t trng_acquire(uint8_t *trng_bits_ptr, uint8_t *sram_puf, const uint16_t sram_puf_blocks);
#define TRNG_ACQUIRE(a, b, c)                   trng_acquire(a, b, c)
#if defined (CFG_CHARACTERIZATION_SW) && defined (TRNG_SEED_TEST)
#undef TRNG_ACQUIRE
#define TRNG_ACQUIRE(a, b, c)                   NIST_TEST_TRNG_ACQUIRE(a, b, c)
#endif
#else

/**
 ****************************************************************************************
 * @brief  Acquires 128 bits, saves them in trng_bits[] and restores the modified regs.
 * @param[in] trng_bits_ptr Pointer to the buffer where random data will be stored
 ****************************************************************************************
 */
void trng_acquire(uint8_t *trng_bits_ptr);

#endif

/**
 ****************************************************************************************
 * @brief Initialize random number generator seed using random bits acquired from TRNG
 ****************************************************************************************
 */
void init_rand_seed_from_trng(void);

#endif // _TRNG_H_

///@}
///@}

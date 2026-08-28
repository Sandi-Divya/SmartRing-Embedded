/**
 ****************************************************************************************
 *
 * @file ecc_verify.c
 *
 * @brief ECC Verify API source file for secondary bootloader
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
 
#if SECURE_BOOT_ECC_IMAGE_SUPPORT

#include "sha2.h"
#include "stdint.h"
#include "ecc_verify.h"
#include "datasheet.h"
#include "stdbool.h"
#include "string.h"
#include "uecc_patch.h"

void rwip_schedule(void);

void dummy_rwip_schedule(void)
{
}

static const void * const patch_orig_table[] = {
    (void *) rwip_schedule,             // original function 1
    0, // reserved

    (void *) uECC_vli_mult,             // original function 2
    0, // reserved
};

static const void * const patch_new_table[] = {
    (void *) dummy_rwip_schedule,
    (void *) PATCHED_uECC_vli_mult,
};

static void _ECC_Verify_set(void)
{
    uint32_t bitmap = 0x00000000;

#ifdef __IS_SDK6_COMPILER_GCC__
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
    #pragma GCC diagnostic ignored "-Warray-bounds"
#endif
    memcpy((uint32_t *)PATCH_ADDR0_REG, patch_orig_table, sizeof(patch_orig_table));
    memcpy((uint32_t *)0x07fc00c0, patch_new_table, sizeof(patch_new_table)); // Function patches start at 0x07fc00c0
#ifdef __IS_SDK6_COMPILER_GCC__
    #pragma GCC diagnostic pop
#endif

    bitmap = 0xFFFFFFFF >> (32 - (sizeof(patch_new_table)/sizeof(patch_new_table[0])));

    // Enable HW patches
    SetWord32(PATCH_VALID_REG, bitmap );
}

static void _ECC_Verify_clear(void)
{
    // Disable HW patches
    SetWord32(PATCH_VALID_REG, 0);
}

typedef void *uECC_Curve;
void *uECC_secp256r1(void);

int uECC_verify(const uint8_t *public_key,
                const uint8_t *message_hash,
                unsigned hash_size,
                const uint8_t *signature,
                uECC_Curve curve);

int ECC_Verify_Image(uint8_t *sha256_hash, uint8_t *ecc_public_key, uint8_t *signature)
{
    int ret;

    // Enable the uECC_verify patch
    _ECC_Verify_set();

    // Call the uECC_verify function in order to verify the image
    ret = uECC_verify(ecc_public_key, sha256_hash, 32, signature, uECC_secp256r1());

    // Disable the uECC_verify patch
    _ECC_Verify_clear();

    // Return the verification result
    return ret;
}
#endif // SECURE_BOOT_ECC_IMAGE_SUPPORT

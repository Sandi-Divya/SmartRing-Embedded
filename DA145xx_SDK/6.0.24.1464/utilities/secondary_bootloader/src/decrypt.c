/**
 ****************************************************************************************
 *
 * @file decrypt.c
 *
 * @brief Decrypt API for secondary booloader
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
 
#if AES_ENCRYPTED_IMAGE_SUPPORTED

#include "sw_aes.h"
#include "uart_booter.h"
#if !defined (__DA14531__)
#include "datasheet.h"
#endif

AES_CTX ctx;

/**
 ****************************************************************************************
 * @brief Decrypts the encrypted image in place.
 * @param[in] nsize the size of the encrypted image which is expected to be a multiple
 *                  of AES_BLOCKSIZE.
 * @param[in] IV the AES initialization vector
 * @param[in] Key the AES decryption key
 * @param[in] base_addr_offset the offset from SYSRAM_BASE_ADDRESS where the image under
 *                  decryption is placed
 ****************************************************************************************
 */
void Decrypt_Image(int nsize, uint8_t *IV, uint8_t *Key, uint32_t base_addr_offset)
{
    uint32_t addr = (SYSRAM_BASE_ADDRESS + base_addr_offset);
    
    AES_set_key(&ctx,Key,IV,AES_MODE_128);
    AES_convert_key(&ctx);
#if defined (__DA14531__)
    AES_cbc_decrypt(&ctx, (const uint8_t *)addr, (uint8_t *)addr, nsize);
#else
    const int DECRYPT_CHUNK = 32 * AES_BLOCKSIZE;
    uint8_t *sys_ram = (uint8_t *)addr;

    for (int i = nsize; i >= 0; i -= DECRYPT_CHUNK)
    {
        AES_cbc_decrypt(&ctx, (const uint8_t *)sys_ram, sys_ram, DECRYPT_CHUNK);
        SetWord16(WATCHDOG_REG, WATCHDOG_REG_RESET);
        sys_ram += DECRYPT_CHUNK;
    }
#endif
}

#endif // AES_ENCRYPTED_IMAGE_SUPPORTED

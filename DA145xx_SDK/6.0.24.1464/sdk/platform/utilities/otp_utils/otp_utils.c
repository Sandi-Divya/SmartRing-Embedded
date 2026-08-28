/**
 ****************************************************************************************
 *
 * @file otp_utils.c
 *
 * @brief OTP useful functions source code
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
 
#include "otp_utils.h"
#include "string.h"

bool otp_utils_write_data(uint32_t otp_address, uint8_t *data, uint32_t len)
{
    uint32_t otp_addr_offset = otp_address - MEMORY_OTP_BASE;
    uint32_t word2write[HW_OTP_CELL_SIZE / 4];
    volatile size_t i;
    bool result = false;

    for (i = 0; i < len; i += HW_OTP_CELL_SIZE)
    {
        memcpy(word2write, data + i, sizeof(word2write));

        /*
         * OTP cell must be powered on, so we turn on the related clock.
         * However, the OTP LDO will not be able to sustain an infinite
         * amount of writes. So we keep our writes limited. Turning off the
         * LDO at the end of the operation ensures that we restart the
         * operation appropriately for the next write.
         */
        hw_otpc_init();

#if defined (__DA14531__)
        if (word2write[0] != 0xffffffff)
        {
            result = hw_otpc_prog_and_verify((word2write), otp_addr_offset >> 2, 1);
        }
#else
        /*
         * From the manual: The destination address is a word aligned address, that represents the OTP memory
         * address. It is not an AHB bus address. The lower allowed value is 0x0000 and the maximum allowed
         * value is 0x1FFF (8K words space). 
         */
        result = hw_otpc_fifo_prog((const uint32_t *) (&word2write),
                                   otp_addr_offset >> 3,
                                   HW_OTPC_WORD_LOW, 2, false);
#endif
        hw_otpc_disable();

        otp_addr_offset += HW_OTP_CELL_SIZE;

        if (!result)
            break;
    }

    return result;
}

void otp_utils_read_data(uint32_t otp_address, uint8_t *data, uint32_t len)
{
    hw_otpc_init();

#if defined (__DA14531__)
    hw_otpc_enter_mode(HW_OTPC_MODE_READ);
#else
    hw_otpc_manual_read_on(false);
#endif

    memcpy((void *)data, (void *)(otp_address), len);

    hw_otpc_disable();
}
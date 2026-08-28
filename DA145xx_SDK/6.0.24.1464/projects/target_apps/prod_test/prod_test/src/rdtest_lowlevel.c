/**
 ****************************************************************************************
 *
 * @file rdtest_lowlevel.c
 *
 * @brief RD test low level functions source code file.
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
 
/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rdtest_lowlevel.h"
#include "gpio.h"

#if !defined (__DA14531__)
void rdtest_init_ports(uint8_t version)
{
    // data
    SetWord16(P3_DATA_REG, 0);
    SetWord16(P30_MODE_REG, 0x300);
    SetWord16(P31_MODE_REG, 0x300);
    SetWord16(P32_MODE_REG, 0x300);
    SetWord16(P33_MODE_REG, 0x300);
    SetWord16(P34_MODE_REG, 0x300);
    SetWord16(P35_MODE_REG, 0x300);
    SetWord16(P36_MODE_REG, 0x300);
    SetWord16(P37_MODE_REG, 0x300);

    SetWord16(P2_DATA_REG, 0); 
    // address
    SetWord16(P20_MODE_REG, 0x300);
    SetWord16(P21_MODE_REG, 0x300);
    SetWord16(P22_MODE_REG, 0x300);
    SetWord16(P23_MODE_REG, 0x300);
    // latch
    SetWord16(P24_MODE_REG, 0x300);
    // enable
    SetWord16(P25_MODE_REG, 0x300);
}

void rdtest_ll_write_data(uint8_t data)
{
    SetWord16(P3_DATA_REG, ((data<<DATASHIFT)&DATAMASK));
}

void rdtest_ll_write_address(uint8_t address)
{
    uint16_t now_p2;
    now_p2 = GetWord16(P2_DATA_REG);
    SetWord16(P2_DATA_REG, (now_p2&~ADDRESSMASK) | ((address<<ADDRESSSHIFT)&ADDRESSMASK));
}

void rdtest_ll_write_latch(uint8_t latch)
{
    uint16_t now_p2;
    now_p2 = GetWord16(P2_DATA_REG);
    SetWord16(P2_DATA_REG, (now_p2&~LATCHMASK) | ((latch<<LATCHSHIFT)&LATCHMASK));
}

void rdtest_ll_write_enable(uint8_t enable)
{
    uint16_t now_p2;
    now_p2 = GetWord16(P2_DATA_REG);
    SetWord16(P2_DATA_REG, (now_p2&~ENABLEMASK) | ((enable<<ENABLESHIFT)&ENABLEMASK));
}

void rdtest_ll_reset(uint8_t reset)
{
    rdtest_updateregister(RESET_CTRL_REG, (uint8_t) reset);
    rdtest_enable();
}

void rdtest_updateregister(uint8_t address, uint8_t data)
{
    rdtest_ll_write_latch(0);
    rdtest_ll_write_data(data);
    rdtest_ll_write_address(address);
    rdtest_ll_write_latch(1);
    rdtest_ll_write_latch(0);
}

void rdtest_enable (void)
{
    rdtest_ll_write_enable(0);
    rdtest_ll_write_enable(1);
    rdtest_ll_write_enable(0);
}
#else
//DO NOTHING IN 531
#endif //!defined (__DA14531__)


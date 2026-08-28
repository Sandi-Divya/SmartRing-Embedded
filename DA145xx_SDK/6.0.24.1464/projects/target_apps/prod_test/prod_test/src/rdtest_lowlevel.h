/**
 ****************************************************************************************
 *
 * @file rdtest_lowlevel.h
 *
 * @brief RD test low level functions header file.
 *
 * Copyright (C) 2012-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _RDTEST_LOWLEVEL_H_
#define _RDTEST_LOWLEVEL_H_
 
/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>

/*
 * DEFINES
 ****************************************************************************************
 */

#define DATAMASK     0xFF
#define DATASHIFT    0
#define ADDRESSMASK  0x0F
#define ADDRESSSHIFT 0
#define LATCHMASK    0x10
#define LATCHSHIFT   4

#define ENABLEMASK  0x20
#define ENABLESHIFT 5

#define VBAT_CTRL_ADDRESS     0 
#define UART_CONNECT_ADDRESS  2 
#define LOOPBACK_CTRL_ADDRESS 4
#define GATE_TO_UART_ADDRESS  6
#define VPP_CTRL_ADDRESS      8 
#define STARTGATE_ADDRESS     9
#define RESET_CTRL_REG        0x0A
#define PW_CTRL_ADDRESS       0x0B

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize ports
 *
 * This function initialize ports used for communication with CPLD the version can later 
 * be used if different communication methods are implemented, not yet used
 *
 * @param[in] version
 *****************************************************************************************
 */
void rdtest_init_ports(uint8_t version);

/**
 ****************************************************************************************
 * @brief Access to databus
 *
 * This function access to databus (8bit) = P3.7, P3.6, P3.5, P3.4, P3.3, P3.2, P3.1, P3.0
 *
 * @param[in] data
 *****************************************************************************************
 */
void rdtest_ll_write_data(uint8_t data);

/**
 ****************************************************************************************
 * @brief Access to addressbus
 *
 * This function access to addressbus (4bit) = P2.3, P2.2, P2.1, P2.0
 *
 * @param[in] address
 *****************************************************************************************
 */
void rdtest_ll_write_address(uint8_t address);

/**
 ****************************************************************************************
 * @brief Latch-pin
 *
 * This function access to latch-pin CPLD = P2.4
 *
 * @param[in] latch
 *****************************************************************************************
 */
void rdtest_ll_write_latch(uint8_t latch);

/**
 ****************************************************************************************
 * @brief Enable-pin
 *
 * This function access to enable-pin CPLD = P2.5
 *
 * @param[in] enable
 *****************************************************************************************
 */
void rdtest_ll_write_enable(uint8_t enable);

/**
 ****************************************************************************************
 * @brief Reset register
 *
 * This function access to reset register/bit in CPLD
 *
 * @param[in] reset
 *****************************************************************************************
 */
void rdtest_ll_reset(uint8_t reset);

/**
 ****************************************************************************************
 * @brief Update register
 *
 * This function update register, method: provide data+address, latch the result
 *
 * @param[in] address
 *
 * @param[in] data
 *****************************************************************************************
 */
void rdtest_updateregister(uint8_t address, uint8_t data);

/**
 ****************************************************************************************
 * @brief Enable the CPLD
 *
 * This function provide a enable to the CPLD, used for multi-address transfers
 * required after registers are updated... may be delayed...
 *****************************************************************************************
 */
void rdtest_enable(void);

#endif // _RDTEST_LOWLEVEL_H_

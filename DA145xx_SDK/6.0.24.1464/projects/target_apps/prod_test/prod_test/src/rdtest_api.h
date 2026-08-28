/**
 ****************************************************************************************
 *
 * @file rdtest_api.h
 *
 * @brief RD test API header file.
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

#ifndef _RDTEST_API_H_
#define _RDTEST_API_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include <stdint.h>
#include "rdtest_lowlevel.h"
#include "rdtest_support.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize ports
 *
 * This function initialize ports and put CPLD in known (safe) state. Version can later
 * be used to control the lowlevel communication method, not yet implemented
 *
 * @param[in] version
 *****************************************************************************************
 */
void rdtest_initialize(uint8_t version);

/**
 ****************************************************************************************
 * @brief Controls the Vpp
 *
 * This function control the Vpp OTP programming voltage
 *
 * @param[in] state
 *****************************************************************************************
 */
void rdtest_vppcontrol(uint8_t state);

/**
 ****************************************************************************************
 * @brief Controls the pulsewidth
 *
 * This function controls the pulsewidth selection
 *
 * @param[in] length
 *****************************************************************************************
 */
void rdtest_select_pulsewidth(uint8_t length);

/**
 ****************************************************************************************
 * @brief Connect or disconnects
 *
 * This function connect or disconnects an FTDI-uart to DUT_connector
 *
 * @param[in] connectmap16
 *****************************************************************************************
 */
void rdtest_uart_connect(uint16_t connectmap16);

/**
 ****************************************************************************************
 * @brief Internal loopback
 *
 * This function control CPLD internal loopback, only port only!
 *
 * @param[in] port
 *****************************************************************************************
 */
void rdtest_make_loopback(uint8_t port);

/**
 ****************************************************************************************
 * @brief Connect Vbat
 *
 * This function connect Vbat to DUT_connector
 *
 * @param[in] connectmap16
 *****************************************************************************************
 */
void rdtest_vbatcontrol(uint16_t connectmap16);

/**
 ****************************************************************************************
 * @brief Reset pulse
 *
 * This function generate a reset pulse to all devices
 *
 * @param[in] delayms
 *****************************************************************************************
 */
void rdtest_resetpulse(uint16_t delayms);

/**
 ****************************************************************************************
 * @brief Xtal-measurement to uart pin
 *
 * This function route xtal-measurement pulse to uart pin
 *
 * @param[in] connectmap16
 *****************************************************************************************
 */
void rdtest_pulse_to_uart(uint16_t connectmap16);

/**
 ****************************************************************************************
 * @brief Xtal-measurement
 *
 * This function generate 1 xtal-measurement pulse
 *****************************************************************************************
 */
void rdtest_generate_pulse(void);

#endif // _RDTEST_API_H_

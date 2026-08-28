/**
 ****************************************************************************************
 *
 * @file otp_utils.h
 *
 * @brief OTP useful functions header file
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

#ifndef OTP_UTILS_H_
#define OTP_UTILS_H_

#include <stdint.h>
#include "hw_otpc.h"

#if defined (__DA14531__)
#include "otp_cs.h"
#include "otp_hdr.h"
#endif

#if defined (__DA14531__)
#define OTP_EMPTY_CELL_VALUE_BYTE       (0xFF)
#else
#define OTP_EMPTY_CELL_VALUE_BYTE       (0x00)
#endif

/**
 ****************************************************************************************
 * @brief Writes data in the OTP
 * @param[in] otp_address The OTP address to write to (the whole OTP address)
 * @param[in] data A pointer to the data that should be written in the OTP
 * @param[in] len The length of the data to write (in bytes)
 * @return True if all the data has been written succesfully, false otherwise
 ****************************************************************************************
 */
bool otp_utils_write_data(uint32_t otp_address, uint8_t *data, uint32_t len);

/**
 ****************************************************************************************
 * @brief Reads data from the OTP
 * @param[in] otp_address The OTP address to read from (the whole OTP address)
 * @param[in] data A pointer to where the read data will be placed
 * @param[in] len The length of the data to read (in bytes)
 ****************************************************************************************
 */
void otp_utils_read_data(uint32_t otp_address, uint8_t *data, uint32_t len);

#endif // OTP_UTILS_H_
/**
 ****************************************************************************************
 * @addtogroup ARCH Architecture
 * @{
 * @addtogroup ARCH_RAM RAM Utilities
 * @brief RAM utilities API
 * @{
 *
 * @file arch_ram.h
 *
 * @brief System RAM definitions.
 *
 * Copyright (C) 2017-2024 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _ARCH_RAM_H_
#define _ARCH_RAM_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "sdk_defs.h"
#include "datasheet.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#if defined (__DA14531__)
#define ALL_RAM_BLOCKS_OFF           (0x15)
#endif

#if defined (__DA14535__)
    #define RAM_1_BASE_ADDR                 (SDK_RAM_1_BASE_ADDR)
    #define RAM_2_BASE_ADDR                 (SDK_RAM_2_BASE_ADDR)
    #define RAM_END_ADDR                    (SDK_RAM_END_ADDR)
#elif defined (__DA14531__)
    #define RAM_1_BASE_ADDR                 (SDK_RAM_1_BASE_ADDR)
    #define RAM_2_BASE_ADDR                 (SDK_RAM_2_BASE_ADDR)
    #define RAM_3_BASE_ADDR                 (SDK_RAM_3_BASE_ADDR)
    #define RAM_END_ADDR                    (SDK_RAM_END_ADDR)
#else
    #define RAM_1_BASE_ADDR                 (SDK_RAM_1_BASE_ADDR)
    #define RAM_2_BASE_ADDR                 (SDK_RAM_2_BASE_ADDR)
    #define RAM_3_BASE_ADDR                 (SDK_RAM_3_BASE_ADDR)
    #define RAM_4_BASE_ADDR                 (SDK_RAM_4_BASE_ADDR)
    #define RAM_END_ADDR                    (SDK_RAM_END_ADDR)
#endif

/**
 ****************************************************************************************
 * @brief Sets the RAM retention mode.
 * @param[in] mode      RAM retention mode
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void arch_ram_set_retention_mode(uint8_t mode)
{
#if defined (__DA14531__)
    SetWord16(RAM_PWR_CTRL_REG, mode);
#else
    SetBits16(PMU_CTRL_REG, RETENTION_MODE, mode);
#endif
}

#if ((USE_HIGH_TEMPERATURE + USE_AMB_TEMPERATURE + USE_MID_TEMPERATURE + USE_EXT_TEMPERATURE) > 1)
    #error "Config error: Multiple temperature ranges were defined."
#endif

#if !defined (__DA14531__)
// Possible combinations of the 4 RAM blocks that produce a total size more than 64KB
#define RAM_SIZE_80KB_OPT1       (RAM_4_BLOCK | RAM_2_BLOCK | RAM_1_BLOCK)
#define RAM_SIZE_80KB_OPT2       (RAM_4_BLOCK | RAM_3_BLOCK | RAM_1_BLOCK)
#define RAM_SIZE_96KB_OPT1       (RAM_4_BLOCK | RAM_3_BLOCK | RAM_2_BLOCK | RAM_1_BLOCK)

#if (USE_HIGH_TEMPERATURE)
    #define DEFAULT_LDO_SET_64K         (0x05)
    #define DEFAULT_LDO_SET_96K         (0x04)
#elif (USE_EXT_TEMPERATURE)
    #define DEFAULT_LDO_SET_64K         (0x07)
    #define DEFAULT_LDO_SET_96K         (0x06)
#elif (USE_MID_TEMPERATURE)
    #define DEFAULT_LDO_SET             (0x08)
#endif
#endif

#endif // _ARCH_RAM_H_

/// @}
/// @}

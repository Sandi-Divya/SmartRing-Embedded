/**
 ****************************************************************************************
 * @addtogroup ARCH Architecture
 * @{
 * @addtogroup ARCH_SYSTEM System
 * @brief System API
 * @{
 *
 * @file arch_hibernation.h
 *
 * @brief Hibernation-related API
 *
 * Copyright (C) 2019-2024 Renesas Electronics Corporation and/or its affiliates.
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

#if defined (__DA14531__)

#ifndef _ARCH_HIBERNATION_H_
#define _ARCH_HIBERNATION_H_

#include <stdbool.h>
#include "arch_api.h"
#include "arch.h"

/*
 * DEFINITIONS
 *****************************************************************************************
 */
#if defined (__DA14535__)
#if (USE_AMB_TEMPERATURE)
#define CLAMP_FOR_RAM_OFF                                                 (0x0E)
#define CLAMP_FOR_RAM_ON                                                  (0x0C)
#elif (USE_MID_TEMPERATURE)
#define CLAMP_FOR_RAM_OFF                                                 (0x0D)
#define CLAMP_FOR_RAM_ON                                                  (0x0B)
#elif  (USE_EXT_TEMPERATURE)
#define CLAMP_FOR_RAM_OFF                                                 (0x0B)
#define CLAMP_FOR_RAM_ON                                                  (0x0A)
#else
#define CLAMP_FOR_RAM_OFF                                                 (0x0B)
#define CLAMP_FOR_RAM_ON                                                  (0x09)
#endif
#else
#if (USE_AMB_TEMPERATURE)
#define CLAMP_FOR_RAM_OFF                                                 (0x0E)
#define CLAMP_FOR_RAM_ON                                                  (0x0D)
#elif (USE_MID_TEMPERATURE)
#define CLAMP_FOR_RAM_OFF                                                 (0x0D)
#define CLAMP_FOR_RAM_ON                                                  (0x0C)
#elif  (USE_EXT_TEMPERATURE)
#define CLAMP_FOR_RAM_OFF                                                 (0x0B)
#define CLAMP_FOR_RAM_ON                                                  (0x0A)
#else
#define CLAMP_FOR_RAM_OFF                                                 (0x09)
#define CLAMP_FOR_RAM_ON                                                  (0x08)
#endif
#endif

/**
 ****************************************************************************************
 * @brief       Puts the system into hibernation mode.
 * @param[in]   wkup_mask      Selects the GPIO(s) mask which will wake up the system from
 *                             hibernation. The system can wake up from P01 to P05.
 *                             For example, write 0xA (0b1010) if P03 and P01 are used.
 * @param[in]   ram1           Selects the RAM1 state (on/off) during the hibernation.
 * @param[in]   ram2           Selects the RAM2 state (on/off) during the hibernation.
 * @param[in]   ram3           Selects the RAM3 state (on/off) during the hibernation.
 * @param[in]   remap_addr0    Selects which memory is located at address 0x0000 for
 *                             execution (after waking up from hibernation).
 * @param[in]   pad_latch_en   true = Enable latching of pads state during sleep,
 *                             false = Disable latching of pads state during sleep.
 * @note RAM3 is not implemented in DA14535.
 ****************************************************************************************
 */
void arch_set_hibernation(uint8_t wkup_mask,
                          pd_sys_down_ram_t ram1,
                          pd_sys_down_ram_t ram2,
                          pd_sys_down_ram_t ram3,
                          remap_addr0_t remap_addr0,
                          bool pad_latch_en);

/**
 ****************************************************************************************
 * @brief       Puts the system into stateful hibernation mode.
 * @param[in]   wkup_mask      Selects the GPIO(s) mask which will wake up the system from
 *                             hibernation. The system can wake up from P01 to P05.
 *                             For example, write 0xA (0b1010) if P03 and P01 are used.
 * @param[in]   ram1           Selects the RAM1 state (on/off) during the hibernation.
 * @param[in]   ram2           Selects the RAM2 state (on/off) during the hibernation.
 * @param[in]   ram3           Selects the RAM3 state (on/off) during the hibernation.
 * @param[in]   remap_addr0    Selects which memory is located at address 0x0000 for
 *                             execution (after waking up from hibernation).
 * @param[in]   pad_latch_en   true = Enable latching of pads state during sleep,
 *                             false = Disable latching of pads state during sleep.
 * @warning Since this function saves the system state (code and data), RAM blocks
 *          shall be retained according to application memory layout, e.g., using
 *          the default linker script, RAM1 and RAM3 blocks shall be always retained
 *          and RAM2 block shall be retained only if holding state code and/or data.
 * @note Assumption is that either RC32M or XTAL32M is the main clock before the
 *       system goes to stateful hibernation.
 * @note RAM3 is not implemented in DA14535.
 ****************************************************************************************
 */
void arch_set_stateful_hibernation(uint8_t wkup_mask,
                                   pd_sys_down_ram_t ram1,
                                   pd_sys_down_ram_t ram2,
                                   pd_sys_down_ram_t ram3,
                                   stateful_hibern_remap_addr0_t remap_addr0,
                                   bool pad_latch_en);

#endif // _ARCH_HIBERNATION_H_

#endif // __DA14531__

///@}
///@}

/**
 ****************************************************************************************
 * @addtogroup ARCH Architecture
 * @{
 * @addtogroup ARCH_WDG Watchdog
 * @brief Watchdog management API
 * @{
 *
 * @file arch_wdg.h
 *
 * @brief Watchdog helper functions.
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

#ifndef _ARCH_WDG_H_
#define _ARCH_WDG_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "datasheet.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define     WATCHDOG_DEFAULT_PERIOD 0xC8

/// Watchdog enable/disable
#ifndef USE_WDOG
#if defined(CFG_WDOG)
#define USE_WDOG            1
#else
#define USE_WDOG            0
#endif //CFG_WDOG
#endif

/**
 ****************************************************************************************
 * @brief     Watchdog resume.
 * About: Start the Watchdog
 ****************************************************************************************
 */
__STATIC_INLINE void  wdg_resume(void)
{
#if (USE_WDOG)
    // Start WDOG
    SetWord16(RESET_FREEZE_REG, FRZ_WDOG);
#endif
}

/**
****************************************************************************************
* @brief     Freeze Watchdog. Call wdg_resume() to resume operation.
* About: Freeze the Watchdog
****************************************************************************************
*/
__STATIC_INLINE void  wdg_freeze(void)
{
    // Freeze WDOG
    SetWord16(SET_FREEZE_REG, FRZ_WDOG);
}

/**
 ****************************************************************************************
 * @brief     Watchdog reload.
 * @param[in] period measured in 10.24ms units.
 * About: Load the default value and resume the watchdog
 ****************************************************************************************
 */
__STATIC_INLINE void  wdg_reload(const int period)
{
#if (USE_WDOG)
    // WATCHDOG_DEFAULT_PERIOD * 10.24ms
    SetWord16(WATCHDOG_REG, WATCHDOG_DEFAULT_PERIOD);
#endif
}

/**
 ****************************************************************************************
 * @brief     Initiliaze and start the Watchdog unit.
 * @param[in] freeze If 0 Watchdog is SW freeze capable
 * About: Enable the Watchdog and configure it to
 ****************************************************************************************
 */
 __STATIC_INLINE void  wdg_init (const int freeze)
{
#if (USE_WDOG)
    wdg_reload(WATCHDOG_DEFAULT_PERIOD);
    // if freeze equals to zero WDOG can be frozen by SW!
    // it will generate an NMI when counter reaches 0 and a WDOG (SYS) Reset when it reaches -16!
    SetWord16(WATCHDOG_CTRL_REG, (freeze&0x1));
    wdg_resume ();
#else
    wdg_freeze();
#endif
}

#endif // _ARCH_WDG_H_

/// @}
/// @}

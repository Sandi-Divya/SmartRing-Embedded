/**
 ****************************************************************************************
 * @addtogroup Drivers
 * @{
 * @addtogroup SysTick
 * @brief SysTick driver API
 * @{
 *
 * @file systick.h
 *
 * @brief SysTick driver header file.
 *
 * Copyright (C) 2014-2025 Renesas Electronics Corporation and/or its affiliates.
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
#ifndef _SYSTICK_H_
#define _SYSTICK_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdbool.h>
#include <stdint.h>

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Callback type to be associated with SysTick_IRQn.
typedef void (*systick_callback_function_t)(void);

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Register Callback function for SysTick exception.
 * @param[in] callback Callback function's reference
 ****************************************************************************************
 */
void systick_register_callback(systick_callback_function_t callback);

/**
 ****************************************************************************************
 * @brief Function to start the SysTick timer.
 * @param[in] usec      The duration of the countdown
 * @param[in] exception Set to TRUE to generate an exception when the timer counts down
 *                      to 0, FALSE not to
 ****************************************************************************************
 */
void systick_start(uint32_t usec, uint8_t exception);

/**
 ****************************************************************************************
 * @brief Function to create a delay.
 * @param[in] usec the duration of the delay
 ****************************************************************************************
 */
void systick_wait(uint32_t usec);

/**
 ****************************************************************************************
 * @brief Function to stop the SysTick timer.
 ****************************************************************************************
 */
void systick_stop(void);

/**
 ****************************************************************************************
 * @brief Function to read the current value of the timer.
 * @return the current value of the timer
 ****************************************************************************************
 */
uint32_t systick_value(void);

#endif // _SYSTICK_H_

///@}
///@}

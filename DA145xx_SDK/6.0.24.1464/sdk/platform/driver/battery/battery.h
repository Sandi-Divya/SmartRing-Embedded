/**
 ****************************************************************************************
 * @addtogroup Drivers
 * @{
 * @addtogroup Battery
 * @brief Battery driver API
 * @{
 *
 * @file battery.h
 *
 * @brief Battery driver header file.
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
 
#ifndef _BATTERY_H_
#define _BATTERY_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>

/*
 * DEFINES
 ****************************************************************************************
 */

#if defined (__DA14531__)
/// Supported battery types
typedef enum
{
    /// CR2032 lithium cell battery
    BATT_CR2032   = 0,

    /// Alkaline cell battery
    BATT_ALKALINE = 1
} batt_t;
#else
/// Supported battery types
typedef enum
{
    /// CR2032 lithium cell battery
    BATT_CR2032   = 0,

    /// CR1225 lithium cell battery
    BATT_CR1225   = 1,

    /// Alkaline cell battery (1 cell in boost, 2 cells in buck mode)
    BATT_ALKALINE = 2
} batt_t;

/// @name Battery measurements
///@{
/** ADC value */
#define BATTERY_MEASUREMENT_BOOST_AT_1V5    (0x340)
#define BATTERY_MEASUREMENT_BOOST_AT_1V0    (0x230)
#define BATTERY_MEASUREMENT_BOOST_AT_0V9    (0x1F0)
#define BATTERY_MEASUREMENT_BOOST_AT_0V8    (0x1B0)

#define BATTERY_MEASUREMENT_BUCK_AT_3V0     (0x6B0)
#define BATTERY_MEASUREMENT_BUCK_AT_2V8     (0x640)
#define BATTERY_MEASUREMENT_BUCK_AT_2V6     (0x5D0)
#define BATTERY_MEASUREMENT_BUCK_AT_2V4     (0x550)
///@}
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Returns voltage of the @p batt_type battery.
 * @param[in] batt_type Battery type.
 * @return battery voltage in mV
 ****************************************************************************************
 */
uint16_t battery_get_voltage(const batt_t batt_type);
#endif

/**
 ****************************************************************************************
 * @brief Returns battery level for @p batt_type.
 * @param[in] batt_type Battery type.
 * @return battery level (0-100%)
 ****************************************************************************************
 */
uint8_t battery_get_lvl(const batt_t batt_type);

#endif // _BATTERY_H_

///@}
///@}

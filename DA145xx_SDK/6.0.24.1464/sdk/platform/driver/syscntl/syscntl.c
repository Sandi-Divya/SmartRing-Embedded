/**
 ****************************************************************************************
 *
 * @file syscntl.c
 *
 * @brief System controller driver.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "datasheet.h"
#include "syscntl.h"

#if defined (__DA14531__)
#include <stdbool.h>
#include "hw_otpc.h"
#include "arch.h"
#endif

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

#if defined (__DA14531__)
/// DCDC converter state
syscntl_dcdc_state_t syscntl_dcdc_state;
#endif

// Required DA14531-01, DA14535 ROM symbols
#if (defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_SYSCNTL__)
extern bool dcdc_reserved;
extern void syscntl_dcdc_cfg(syscntl_dcdc_level_t level);
extern int8_t syscntl_dcdc_turn_off_ROM(bool dcdc_reserved);
extern int8_t syscntl_dcdc_turn_on_in_boost_ROM(syscntl_dcdc_level_t level, syscntl_dcdc_state_t *ptr_dcdc_state, bool dcdc_reserved);
extern int8_t syscntl_dcdc_turn_on_in_buck_ROM(syscntl_dcdc_level_t level);
#endif // __EXCLUDE_ROM_SYSCNTL__

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
#if (!defined (__DA14531_01__) && !defined (__DA14535__)) || defined (__EXCLUDE_ROM_SYSCNTL__)
void syscntl_use_lowest_amba_clocks(void)
{
    SetBits16(CLK_AMBA_REG, PCLK_DIV, 3);   // lowest is 250KHz (div 8, source is @2MHz)
    SetBits16(CLK_AMBA_REG, HCLK_DIV, 3);   // lowest is 2MHz (div 8, source is @16MHz)
}

void syscntl_use_highest_amba_clocks(void)
{
    SetBits16(CLK_AMBA_REG, PCLK_DIV, 0);
    SetBits16(CLK_AMBA_REG, HCLK_DIV, 0);
}

#if defined (__DA14531__)
void syscntl_cfg_xtal32m_amp_reg(xtal32m_amp_reg_t mode)
{
    SetBits16(XTAL32M_TESTCTRL0_REG, CORE_HOLD_AMP_REG_OVERRIDE, (uint8_t) mode);
}

void syscntl_por_vbat_high_cfg(por_vbat_mask_t mask)
{
    // Enable POR_VBAT_HIGH
    SetBits16(POWER_CTRL_REG, POR_VBAT_HIGH_DISABLE, 0);

    // Set POR_VBAT_HIGH mask mode
    SetBits(POWER_AON_CTRL_REG, POR_VBAT_HIGH_RST_MASK, mask);
}

void syscntl_por_vbat_low_cfg(por_vbat_mask_t mask)
{
    // Enable POR_VBAT_LOW
    SetBits16(POWER_CTRL_REG, POR_VBAT_LOW_DISABLE, 0);

    // Set POR_VBAT_LOW mask mode
    SetBits(POWER_AON_CTRL_REG, POR_VBAT_LOW_RST_MASK, mask);
}

/**
 ****************************************************************************************
 * @brief Configures and enables DCDC converter in the provided voltage level.
 * @param[in] level The voltage level of DCDC converter (nominal level + trim value).
 ****************************************************************************************
 */
#if (!USE_POWER_MODE_BYPASS)
static void syscntl_dcdc_cfg(syscntl_dcdc_level_t level)
{
    syscntl_dcdc_set_level(level);

    // Enable the DCDC converter
    SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 1);

    // Wait for the indication that VBAT_LOW (buck mode) or VBAT_HIGH (boost mode) is OK
    while (!GetBits(ANA_STATUS_REG, DCDC_OK));
}
#endif

syscntl_dcdc_level_t syscntl_dcdc_get_level(void)
{
    uint16_t dcdc_level = GetBits16(POWER_LEVEL_REG, DCDC_LEVEL);
    uint16_t dcdc_trim = GetBits16(POWER_LEVEL_REG, DCDC_TRIM);
    return ((syscntl_dcdc_level_t) ((dcdc_level << 3U) + dcdc_trim));
}

void syscntl_dcdc_set_level(syscntl_dcdc_level_t level)
{
    // Set DCDC converter nominal voltage level
    SetBits16(POWER_LEVEL_REG, DCDC_LEVEL, level >> 3U);

    // Set DCDC converter trim value
    SetBits16(POWER_LEVEL_REG, DCDC_TRIM, level & 0x7);
}
#endif // __DA14531__
#endif // __EXCLUDE_ROM_SYSCNTL__

#if defined (__DA14531__)
#if (defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_SYSCNTL__) && (!USE_POWER_MODE_BYPASS)
int8_t syscntl_dcdc_turn_off(void)
{
    return (syscntl_dcdc_turn_off_ROM(dcdc_reserved));
}
#else
int8_t syscntl_dcdc_turn_off(void)
{
#if (USE_POWER_MODE_BYPASS)
    return SYSCNTL_DCDC_ERR_IN_BYPASS;
#else
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
        if (hw_otpc_is_dcdc_reserved())
        {
            return SYSCNTL_DCDC_ERR_RESERVED_BY_OTP;
        }
        if ((GetBits16(POWER_CTRL_REG, POR_VBAT_HIGH_DISABLE) == 0x0) &&
            (GetBits16(POWER_AON_CTRL_REG, POR_VBAT_HIGH_RST_MASK) == 0x0))
        {
            return SYSCNTL_DCDC_ERR_POR_TRIGGER;
        }
    }
    else
    {
        // Enable LDO_LOW
        SetBits16(POWER_CTRL_REG, LDO_LOW_CTRL_REG, 2);
    }

    // Disable the DCDC
    SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 0);

    return SYSCNTL_DCDC_ERR_NO_ERROR;
#endif
}
#endif

#if (defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_SYSCNTL__) && (!USE_POWER_MODE_BYPASS)
int8_t syscntl_dcdc_turn_on_in_boost(syscntl_dcdc_level_t level)
{
    return (syscntl_dcdc_turn_on_in_boost_ROM(level, &syscntl_dcdc_state, dcdc_reserved));
}
#else
int8_t syscntl_dcdc_turn_on_in_boost(syscntl_dcdc_level_t level)
{
#if (USE_POWER_MODE_BYPASS)
    return SYSCNTL_DCDC_ERR_IN_BYPASS;
#else
    if (!GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
        return SYSCNTL_DCDC_ERR_NOT_IN_BOOST;
    }
    if (hw_otpc_is_dcdc_reserved() && (level < syscntl_dcdc_get_level()))
    {
        return SYSCNTL_DCDC_ERR_RESERVED_BY_OTP;
    }
    // POR is generated if VBAT_HIGH is below 1.66V
    if ((GetBits16(POWER_CTRL_REG, POR_VBAT_HIGH_DISABLE) == 0x0) &&
        (GetBits16(POWER_AON_CTRL_REG, POR_VBAT_HIGH_RST_MASK) == 0x0) &&
        (level < SYSCNTL_DCDC_LEVEL_1V725))
    {
        return SYSCNTL_DCDC_WARNING_POR_TRIGGER;
    }

    syscntl_dcdc_state.level = level;
    syscntl_dcdc_state.status = true;
    syscntl_dcdc_cfg(level);

    return SYSCNTL_DCDC_ERR_NO_ERROR;
#endif
}
#endif

#if (defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_SYSCNTL__) && (!USE_POWER_MODE_BYPASS)
int8_t syscntl_dcdc_turn_on_in_buck(syscntl_dcdc_level_t level)
{
    return syscntl_dcdc_turn_on_in_buck_ROM(level);
}
#else
int8_t syscntl_dcdc_turn_on_in_buck(syscntl_dcdc_level_t level)
{
#if (USE_POWER_MODE_BYPASS)
    return SYSCNTL_DCDC_ERR_IN_BYPASS;
#else
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
        return SYSCNTL_DCDC_ERR_NOT_IN_BUCK;
    }

    syscntl_dcdc_cfg(level);

    // Disable LDO_LOW
    SetBits16(POWER_CTRL_REG, LDO_LOW_CTRL_REG, 1);

    return SYSCNTL_DCDC_ERR_NO_ERROR;
#endif
}
#endif // __EXCLUDE_ROM_SYSCNTL__
#endif // __DA14531__

#if !defined (__DA14531__)
void syscntl_set_dcdc_vbat3v_level(enum SYSCNTL_DCDC_VBAT3V_LEVEL level)
{
    SetBits16(DCDC_CTRL2_REG, DCDC_VBAT3V_LEV, level);
}
#endif

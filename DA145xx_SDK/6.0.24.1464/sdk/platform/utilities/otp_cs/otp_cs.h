/**
 ****************************************************************************************
 * @addtogroup UTILITIES
 * @{
 * @addtogroup OTP_CS OTP Configuration Script
 * @brief OTP Configuration Script API
 * @{
 *
 * @file otp_cs.h
 *
 * @brief OTP Configuration Script header file (only for DA14531, DA14531-01, DA14535).
 *
 * Copyright (C) 2019-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _OTP_CS_H_
#define _OTP_CS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if defined (__DA14531__)

#include <stdint.h>
#include "datasheet.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * OTP configuration script field offset from OTP memory starting address
 */
#if defined (__DA14535__)
#define OTP_CS_BASE_OFFSET                  (0x2ED0)
#elif defined (__DA14531__)
#define OTP_CS_BASE_OFFSET                  (0x7ED0)
#endif

/*
 * OTP configuration script field base memory address
 */
#define OTP_CS_BASE_ADDR                    (MEMORY_OTP_BASE + OTP_CS_BASE_OFFSET)

/*
 * OTP configuration script constant values
 */
#define OTP_CS_MAX_ENTRIES                  (60)                           // OTP CS can have maximum 60 32-bit entries
#define OTP_CS_MAX_SIZE                     (OTP_CS_MAX_ENTRIES * 4)       // 240 bytes, 60 entries of 4 bytes
#define OTP_CS_END_ADDR                     (OTP_CS_BASE_ADDR + OTP_CS_MAX_SIZE)

#define OTP_CS_CMD_START                    (0xA5A5A5A5)
#define OTP_CS_CMD_DEBUG_VAL                (0x60000000)
#define OTP_CS_CMD_SWD_MODE                 (0x70000000)
#define OTP_CS_CMD_UART_STX                 (0x80000000)
#define OTP_CS_CMD_SDK_VAL                  (0x90000000)
#define OTP_CS_CMD_SPI_CLK                  (0xA0000000)
#if defined (__DA14535__)
#define OTP_CS_CMD_SPI_FLASH_WAKEUP_TIME    (0xB0000000)
#endif
#define OTP_CS_EMPTY_VAL                    (0xFFFFFFFF)

/*
 * Supported IDs of the command 0x9000xxyy (yy is the ID, xx is the number of items)
 */
#define OTP_CS_PD_SYS                       (1)
#define OTP_CS_PD_SLP                       (2)
#define OTP_CS_PD_TMR                       (4)
#define OTP_CS_PD_RAD                       (6)
#define OTP_CS_PD_ADPLL                     (7)
#define OTP_CS_GP_ADC_SINGLE                (17)
#define OTP_CS_GP_ADC_DIFF                  (18)
#if !defined (__DA14535__)
#define OTP_CS_GP_ADC_GEN                   (19) // Applies only to DA14531, DA14531-01
#endif
#define OTP_CS_GP_TEMP                      (20)
#if defined (__DA14535__)
#define OTP_CS_GP_ADC_SINGLE_ATTN2X         (21) // Applies only to DA14535
#endif
#define OTP_CS_LP_CLK_SET                   (22)
#define OTP_CS_XTAL_TRIM                    (23)
#define OTP_CS_PD_ADPLL_BYPASS              (30)

/*
 * Max number of items per ID
 */
#if defined (__DA14535__)
#define OTP_CS_PD_RAD_LEN                   (6) // number of pairs of address/value
#else
#define OTP_CS_PD_RAD_LEN                   (2) // number of pairs of address/value
#endif
#define OTP_CS_PD_ADPLL_LEN                 (6) // number of pairs of address/value
#define OTP_CS_GP_ADC_SINGLE_LEN            (1) // single values
#define OTP_CS_GP_ADC_DIFF_LEN              (1) // single values
#define OTP_CS_GP_ADC_GEN_LEN               (2) // single values (ADC trim value, GPADC_SHIFTER_10, GPADC_SHIFTER_90 in DA14531, DA14531-01)
#define OTP_CS_GP_TEMP_LEN                  (1) // single values
#define OTP_CS_LP_CLK_SET_LEN               (1) // single values
#define OTP_CS_XTAL_TRIM_LEN                (1) // single values

/*
 * Error codes
 */
#define OTP_CS_ERROR_OK                     (0)
#define OTP_CS_ERROR_NO_CS_SECTION_FOUND    (-1)
#define OTP_CS_ERROR_NOT_VALID_ENTRY        (-2)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef void (*otp_cs_store_handler1_t)(uint32_t id, uint32_t num, uint32_t offset);
typedef void (*otp_cs_store_handler2_t)(uint32_t value);

#if defined (__DA14535__)
typedef struct
{
    otp_cs_store_handler1_t pd_sys_func;        // Hook to hanlde extra PD_SYS settings
    otp_cs_store_handler1_t pd_rad_func;        // Hook to hanlde extra PD_RAD settings
    otp_cs_store_handler1_t pd_adpll_func;      // Hook to handle extra PD_ADPLL settings
    otp_cs_store_handler1_t pd_others_func;     // Hook to handle PD_SLP, PD_TMR settings
    otp_cs_store_handler1_t adc_attn2x_func;    // Hook to handle Id 21
    otp_cs_store_handler1_t user_func;          // Hook to hanlde user settings
    otp_cs_store_handler2_t command_func;       // Hook to handle command settings
    uint32_t otp_cs_base_addr;                  // Base address
    uint32_t otp_cs_end_addr;                   // End address
} otp_cs_store_handler_t;
#endif

/// Register address/value pairs
typedef struct
{
    /// Register address
    uint32_t addr;

    /// Register value
    uint32_t val;
} otp_cs_addr_value_t;

/// Trim values
typedef struct
{
    /// RF_PA_CTRL_REG content for 3dBm
    uint32_t rf_pa_ctrl_3dBm;

    /// ADPLL_DIV_CTRL_REG content for 3dBm
    // NOTE: Applicable to T2 version samples only
    uint32_t adpll_div_ctrl_3dBm;

    /// RF_PA_CTRL_REG content for 0dBm
    uint32_t rf_pa_ctrl_0dBm;

    /// ADPLL_DIV_CTRL_REG content for 0dBm
    // NOTE: Applicable to T2 version samples only
    uint32_t adpll_div_ctrl_0dBm;

    /// Trim values for LP_CLK_SET
    uint32_t lp_clk_set[OTP_CS_LP_CLK_SET_LEN];

    /// Calculated ADC Gain Error based on GPADC_GE_OFFS_SE_10 and GPADC_GE_OFFS_SE_90
    /// (GP_ADC_SINGLE group in CS)
    // NOTE: To be taken into account when in single-ended mode
    // with the input shifter being disabled
    int16_t gp_adc_single_ge;

    /// Calculated ADC Offset Error based on GPADC_GE_OFFS_SE_10 and GPADC_GE_OFFS_SE_90
    /// (GP_ADC_SINGLE group in CS)
    // NOTE: To be taken into account when in single-ended mode
    // with the input shifter being disabled
    int16_t gp_adc_single_offset;

    /// Calculated ADC Gain Error based on GPADC_GE_OFFS_DIF_m80 and GPADC_GE_OFFS_DIF_p80
    /// (GP_ADC_DIFF group in CS)
    // NOTE: To be taken into account when in differential mode
    // with the input shifter being disabled
    int16_t gp_adc_diff_ge;

    /// Calculated ADC Offset Error based on GPADC_GE_OFFS_DIF_m80 and GPADC_GE_OFFS_DIF_p80
    /// (GP_ADC_DIFF group in CS)
    // NOTE: To be taken into account when in differential mode
    // with the input shifter being disabled
    int16_t gp_adc_diff_offset;

    /// GP_ADC_TRIM_REG trim value
    /// GP_ADC_GEN group in CS (applies only to DA14531, DA14531-01)
    /// PD_SYS group in CS (applies only to DA14535)
    uint16_t gp_adc_trim_reg;

    /// Calculated ADC Gain Error based on GPADC_SHIFTER_10 and GPADC_SHIFTER_90 (applies only to DA14531, DA14531-01)
    /// Calculated ADC Gain Error based on GPADC_ATTN2X_10 and GPADC_ATTN2X_90 (applies only to DA14535)
    /// (GP_ADC_GEN group in CS)
    // NOTE: To be taken into account when the input shifter or attenuator x2 is enabled
    int16_t gp_adc_offsh_ge;

    /// Calculated ADC Offset Error based on GPADC_SHIFTER_10 and GPADC_SHIFTER_90 (applies only to DA14531, DA14531-01)
    /// Calculated ADC Offset Error based on GPADC_ATTN2X_10 and GPADC_ATTN2X_90 (applies only to DA14535)
    /// (GP_ADC_GEN group in CS)
    // NOTE: To be taken into account when the input shifter or attenuator x2 is enabled
    int16_t gp_adc_offsh_offset;

    /// Extracted temp sensor calibration data (GP_TEMP group in CS)
    uint16_t adc_25_cal;

    /// XTAL32M wait trim value
    uint16_t xtal_wait_trim;

    /// XTAL32M trim value
    uint16_t xtal_trim_value;
} otp_cs_trim_value_t;

/// Configuration script storage structure
typedef struct
{
    /// PD_RAD settings stored in address/value pairs
    otp_cs_addr_value_t pd_rad[OTP_CS_PD_RAD_LEN];

    /// PD_ADPLL settings stored in address/value pairs
    otp_cs_addr_value_t pd_adpll[OTP_CS_PD_ADPLL_LEN];

    /// Trim values
    otp_cs_trim_value_t trim_values;

    /// PD_RAD pairs found
    uint8_t pd_rad_pairs;

    /// PD_ADPLL pairs found
    uint8_t pd_adpll_pairs;
} otp_cs_t;

/// Stucture containing info about debugger configuration and OTP CS registers
/// normally handled by the Booter - needed in case system wakes up from
/// hibernation and remaps ADDR0 to RAM.
typedef struct
{
    /// Debugger configuration
    uint16_t dbg_cfg;

    /// BANDGAP_REG value
    uint16_t bandgap_reg;

    /// CLK_RC32M_REG value
    uint16_t clk_rc32m_reg;

    /// CLK_RC32K_REG value
    uint16_t clk_rc32k_reg;

#if defined (CFG_CHARACTERIZATION_SW)
    /// Characterization SW trap data value
    volatile uint8_t char_sw_trap_data;

    /// Characterization SW trap state value
    volatile uint8_t char_sw_trap_state;
#endif
} otp_cs_booter_val_t;

extern otp_cs_t otp_cs;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Stores OTP configuration script values to retention memory.
 * @note OTP memory has to be enabled in read mode, prior to calling this function.
 * @return error code
 ****************************************************************************************
 */
int8_t otp_cs_store(void);

/**
 ****************************************************************************************
 * @brief Loads values from retention memory to respective PD_RAD registers.
 * @note Call @c otp_cs_load_pd_rad() before @c otp_cs_load_pd_adpll().
 * return void
 ****************************************************************************************
 */
void otp_cs_load_pd_rad(void);

/**
 ****************************************************************************************
 * @brief Loads values from retention memory to respective PD_ADPLL registers.
 ****************************************************************************************
 */
void otp_cs_load_pd_adpll(void);

/**
 ****************************************************************************************
 * @brief Get the low power clock configuration.
 * @return low power clock configuration
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint32_t otp_cs_get_low_power_clock(void)
{
    return otp_cs.trim_values.lp_clk_set[0];
}

/**
 ****************************************************************************************
 * @brief Returns the ADC Gain Error in single-ended mode.
 * @return ADC Gain Error in single-ended mode
 ****************************************************************************************
 */
__STATIC_FORCEINLINE int16_t otp_cs_get_adc_single_ge(void)
{
    return otp_cs.trim_values.gp_adc_single_ge;
}

/**
 ****************************************************************************************
 * @brief Returns the ADC Offset Error in single-ended mode.
 * @return ADC Offset Error in single-ended mode
 ****************************************************************************************
 */
__STATIC_FORCEINLINE int16_t otp_cs_get_adc_single_offset(void)
{
    return otp_cs.trim_values.gp_adc_single_offset;
}

/**
 ****************************************************************************************
 * @brief Returns the ADC Gain Error in differential mode.
 * @return ADC Gain Error in differential mode
 ****************************************************************************************
 */
__STATIC_FORCEINLINE int16_t otp_cs_get_adc_diff_ge(void)
{
    return otp_cs.trim_values.gp_adc_diff_ge;
}

/**
 ****************************************************************************************
 * @brief Returns the ADC Offset Error in differential mode.
 * @return ADC Offset Error in differential mode
 ****************************************************************************************
 */
__STATIC_FORCEINLINE int16_t otp_cs_get_adc_diff_offset(void)
{
    return otp_cs.trim_values.gp_adc_diff_offset;
}

/**
 ****************************************************************************************
 * @brief Returns the GP_ADC_TRIM_REG trim value.
 * @return GP_ADC_TRIM_REG trim value
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t otp_cs_get_adc_trim_val(void)
{
    return otp_cs.trim_values.gp_adc_trim_reg;
}

/**
 ****************************************************************************************
 * @brief Returns the ADC Gain Error.
 * @return ADC Gain Error
 * @note DA14531, DA14531-01: Used when input shifter is enabled.
 *       DA14535: Used when attenuator x2 is enabled, since shifter is not implemented.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE int16_t otp_cs_get_adc_offsh_ge(void)
{
    return otp_cs.trim_values.gp_adc_offsh_ge;
}

/**
 ****************************************************************************************
 * @brief Returns the ADC Offset Error
 * @return ADC Offset Error
 * @note DA14531, DA14531-01: Used when input shifter is enabled.
 *       DA14535: Used when attenuator x2 is enabled, since shifter is not implemented.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE int16_t otp_cs_get_adc_offsh_offset(void)
{
    return otp_cs.trim_values.gp_adc_offsh_offset;
}

/**
 ****************************************************************************************
 * @brief Returns the 25 degrees Celsius calibration value, needed to calculate
 * calibrated temperature based on raw ADC value.
 * @return 25 degrees Celsius calibration value
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t otp_cs_get_adc_25_cal(void)
{
    return otp_cs.trim_values.adc_25_cal;
}

/**
 ****************************************************************************************
 * @brief Returns the content of @c RF_PA_CTRL_REG for 3dBm.
 * @return @c RF_PA_CTRL_REG content for 3dBm
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint32_t otp_cs_get_rf_pa_ctrl_3dBm(void)
{
    return otp_cs.trim_values.rf_pa_ctrl_3dBm;
}

/**
 ****************************************************************************************
 * @brief Returns the content of @c ADPLL_DIV_CTRL_REG for 3dBm.
 * @return @c ADPLL_DIV_CTRL_REG content for 3dBm
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint32_t otp_cs_get_adpll_div_ctrl_3dBm(void)
{
    return otp_cs.trim_values.adpll_div_ctrl_3dBm;
}

/**
 ****************************************************************************************
 * @brief Returns the content of @c RF_PA_CTRL_REG for 0dBm.
 * @return @c RF_PA_CTRL_REG content for 0dBm
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint32_t otp_cs_get_rf_pa_ctrl_0dBm(void)
{
    return otp_cs.trim_values.rf_pa_ctrl_0dBm;
}

/**
 ****************************************************************************************
 * @brief Returns the content of @c ADPLL_DIV_CTRL_REG for 0dBm.
 * @return @c ADPLL_DIV_CTRL_REG content for 0dBm
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint32_t otp_cs_get_adpll_div_ctrl_0dBm(void)
{
    return otp_cs.trim_values.adpll_div_ctrl_0dBm;
}

/**
 ****************************************************************************************
 * @brief Get the XTAL32M wait trim value.
 * @return The value represents time measured in usec.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t otp_cs_get_xtal32m_wait_trim(void)
{
    return otp_cs.trim_values.xtal_wait_trim;
}

/**
 ****************************************************************************************
 * @brief Returns the XTAL32M trim value which is written in OTP CS section.
 * @return XTAL32M trim value
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t otp_cs_get_xtal32m_trim_value(void)
{
    return otp_cs.trim_values.xtal_trim_value;
}

#endif // __DA14531__

#endif //_OTP_CS_H_

/// @}
/// @}

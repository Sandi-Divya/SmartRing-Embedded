/**
 ****************************************************************************************
 *
 * @file otp_cs.c
 *
 * @brief OTP Configuration Script source file (only for DA14531, DA14531-01, DA14535).
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if defined (__DA14531__)

#include <stdint.h>
#include <string.h>
#include "arch.h"
#include "otp_hdr.h"
#include "otp_cs.h"
#include "rf_531.h"
#include "compiler.h"
#if defined (CFG_CHARACTERIZATION_SW)
#include "char_sw_config.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * Default XTAL32M wait time in usec (applicable only to DA14531, DA14531-01, DA14535).
 * The wait time in usec is either provided by the following fixed value or by OTP CS
 * entry.
 ****************************************************************************************
 */
#define XTAL32M_WAIT_TRIM_TIME_USEC      (1000)  // 1msec

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

// Required DA14531-01 ROM symbols
#if defined (__DA14531_01__) && !defined (__EXCLUDE_ROM_OTP_CS__)
extern int8_t otp_cs_store_ROM(otp_cs_t *ptr_otp_cs, otp_cs_booter_val_t *ptr_boot_val, uint32_t *ptr_txdiv_trim, otp_cs_store_handler1_t func1, otp_cs_store_handler2_t func2);
#endif

#if defined (__DA14535__) && !defined (__EXCLUDE_ROM_OTP_CS__)
extern int8_t otp_cs_store_ROM(otp_cs_t *ptr_otp_cs, otp_cs_booter_val_t *ptr_boot_val, uint32_t *ptr_txdiv_trim, otp_cs_store_handler_t *func);
extern void otp_cs_unpack_pd_rad_enhanced_tx_pwr_ctrl_ROM(otp_cs_t *ptr_otp_cs, uint32_t *ptr_txdiv_trim);
extern void otp_cs_unpack_pd_adpll_enhanced_tx_pwr_ctrl_ROM(otp_cs_t *ptr_otp_cs, uint32_t txdiv_trim);
#endif

#if (defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_OTP_CS__)
extern void otp_cs_load_pd_rad_ROM(otp_cs_t *ptr_otp_cs, uint32_t *ptr_txdiv_trim, bool cfg_enhanced_tx_pwr_ctrl, rf_tx_pwr_lvl_t rf_tx_pwr_lvl);
extern void otp_cs_load_pd_adpll_ROM(otp_cs_t *ptr_otp_cs, uint32_t txdiv_trim, bool cfg_enhanced_tx_pwr_ctrl, rf_tx_pwr_lvl_t rf_tx_pwr_lvl);

#if defined (__FPGA__)
rf_tx_pwr_lvl_t rf_tx_pwr_lvl = RF_TX_PWR_LVL_0d0; // not used in FPGA setup
#else
#if !defined (__NON_BLE_EXAMPLE__)
extern rf_tx_pwr_lvl_t rf_tx_pwr_lvl;
#endif // __NON_BLE_EXAMPLE__
#endif // __FPGA__

#if defined (CFG_ENHANCED_TX_PWR_CTRL)
bool cfg_enhanced_tx_pwr_ctrl = true;
#else
bool cfg_enhanced_tx_pwr_ctrl = false;
#endif //CFG_ENHANCED_TX_PWR_CTRL
#endif // __EXCLUDE_ROM_OTP_CS__

/*
 * Structure containing info about debugger configuration and OTP CS registers
 * normally handled by the Booter - resides in the otp_cs_booter section
 * within ER_IROM1 (ER_IROM1 is 192 bytes long and meant for the vector table -
 * the latter is 160 bytes long so we utilize available space within ER_IROM1)
 ****************************************************************************************
 */
otp_cs_booter_val_t booter_val  __SECTION("otp_cs_booter") =
{
    // Debugger configuration
    .dbg_cfg       = 0,

    // BANDGAP_REG value
    .bandgap_reg   = BANDGAP_REG_RESET,

    // CLK_RC32M_REG value
    .clk_rc32m_reg = CLK_RC32M_REG_RESET,

    // CLK_RC32K_REG value
    .clk_rc32k_reg = CLK_RC32K_REG_RESET,

#if defined (CFG_CHARACTERIZATION_SW)
    /// Characterization SW trap data value
    .char_sw_trap_data = DEFAULT_TRAP_DATA_VALUE,

    /// Characterization SW trap state value
    .char_sw_trap_state = 0,
#endif
};

/*
 * LOCAL VARIABLES
 ****************************************************************************************
 */

otp_cs_t otp_cs                 __SECTION_ZERO("retention_mem_area0");
static uint32_t txdiv_trim;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static void store_default_xtal32m_trim_value(void)
{
    uint32_t device_package = GetWord32(OTP_HDR_PACKAGE_ADDR);
#if defined (__DA14535__)
    if ((device_package & 0xFF00) == DEVICE_DA14535)
    {
        otp_cs.trim_values.xtal_trim_value = DEFAULT_XTAL32M_TRIM_DA14535;
    }
    else if ((device_package & 0xFF00) == DEVICE_DA14533)
    {
        otp_cs.trim_values.xtal_trim_value = DEFAULT_XTAL32M_TRIM_DA14533;
    }
    else if ((device_package & 0xFF00) == DEVICE_DA14534)
    {
        otp_cs.trim_values.xtal_trim_value = DEFAULT_XTAL32M_TRIM_DA14534;
    }
#if defined (CFG_DA14535_T0)
    else
    {
        otp_cs.trim_values.xtal_trim_value = DEFAULT_XTAL32M_TRIM_DA14535;
    }
#endif
#else
    if ((device_package & 0xFF) == PACKAGE_QFN_24)
    {
        otp_cs.trim_values.xtal_trim_value = DEFAULT_XTAL32M_TRIM_VALUE_QFN;
    }
    else
    {
        otp_cs.trim_values.xtal_trim_value = DEFAULT_XTAL32M_TRIM_VALUE_WLCSP;
    }
#endif
}

#if (!defined (__DA14531_01__) && !defined (__DA14535__)) || defined (__EXCLUDE_ROM_OTP_CS__)
/**
 ****************************************************************************************
 * @brief Calculates the ADC Gain Error based on two points.
 * @param[in] low Measurement at the low end of the full scale.
 * @param[in] high Measurement at the high end of the full scale.
 * @return ADC Gain Error
 ****************************************************************************************
 */
static int16_t calc_adc_ge(uint16_t low, uint16_t high)
{
    return (((high - low) + ((uint16_t) (high - low) >> 2)) - UINT16_MAX);
}

/**
 ****************************************************************************************
 * @brief Calculates the ADC Offset Error based on two points.
 * @param[in] low Measurement at the low end of the full scale.
 * @param[in] high Measurement at the high end of the full scale.
 * @return ADC Offset Error
 ****************************************************************************************
 */
static int16_t calc_adc_offset(uint16_t low, uint16_t high)
{
    return ((int16_t) ((9 * low) - high) >> 3);
}
#endif // __EXCLUDE_ROM_OTP_CS__

#if (!defined (__DA14531_01__) && !defined (__DA14535__)) || defined (__EXCLUDE_ROM_OTP_CS__)
static void enhanced_tx_pwr_ctrl_unpack(void);

static void enhanced_tx_pwr_ctrl_unpack(void)
{
#if defined (CFG_ENHANCED_TX_PWR_CTRL)
    for (int i = 0; i < otp_cs.pd_rad_pairs; i++)
    {
        if ((otp_cs.pd_rad[i].addr == RF_PA_CTRL_REG) &&
            // Distinguish T2 from T1 - assumed that T2 samples have at least one nonzero bit
            // spread from bit 24 to bit 10, at RF_PA_CTRL_REG index
            (((otp_cs.pd_rad[i].val >> 10U) & 0x7FFF) != 0))
        {
            // Store the RF_PA_CTRL_REG content for 3dBm
            otp_cs.trim_values.rf_pa_ctrl_3dBm = ((otp_cs.pd_rad[i].val & PA_RAMP_STEP_SPEED) | ((otp_cs.pd_rad[i].val >> 10) & (TRIM_DUTY_NEG | TRIM_DUTY_POS)));

            // Store the 9-bit value for 3dBm (ADPLL_DIV_CTRL_REG.TXDIV_TRIM setting)
            txdiv_trim = (otp_cs.pd_rad[i].val >> 16U) & 0x01FF;

            // Store the RF_PA_CTRL_REG content for 0dBm
            otp_cs.trim_values.rf_pa_ctrl_0dBm = ((otp_cs.pd_rad[i].val & PA_RAMP_STEP_SPEED) | (otp_cs.pd_rad[i].val & (TRIM_DUTY_NEG | TRIM_DUTY_POS)));

            break;
        }
    }

    /* Special handling for ADPLL_DIV_CTRL_REG:
     *      PrefValues: TXDIV_TRIM = OTP CS value, RXDIV_TRIM = will be applied by `set_recommended_settings()`.
     *      The rest fields (RXDIV_FB_EN_TX, RXDIV_FB_EN_RX, FBDIV_EN) keep their reset values.
     * Reset values for ADPLL_DIV_CTRL_REG are: TXDIV_TRIM = 0xCC, RXDIV_TRIM = 0xFF, RXDIV_FB_EN_TX = 0x0, RXDIV_FB_EN_RX = 0x1, FBDIV_EN = 0x1.
     */
    for (int i = 0; i < otp_cs.pd_adpll_pairs; i++)
    {
        if (otp_cs.pd_adpll[i].addr == ADPLL_DIV_CTRL_REG)
        {
            // Store the ADPLL_DIV_CTRL_REG content for 3dBm
            otp_cs.trim_values.adpll_div_ctrl_3dBm = ((otp_cs.pd_adpll[i].val & (~TXDIV_TRIM)) | (txdiv_trim << 17U));

            // Store the ADPLL_DIV_CTRL_REG content for 0dBm
            otp_cs.trim_values.adpll_div_ctrl_0dBm = otp_cs.pd_adpll[i].val;

            break;
        }
    }
#endif // CFG_ENHANCED_TX_PWR_CTRL
}
#endif

/**
 ****************************************************************************************
 * @brief Handler for extra/non-default OTP CS items
 * @param[in] id The OTP CS item id
 * @param[in] num The number of items under the specific ID
 * @param[in] offset The OTP offset of that id's values
 * @return error code
 ****************************************************************************************
 */
void otp_cs_store_user_handler(uint32_t id, uint32_t num, uint32_t offset)
{
    switch (id)
    {
        case OTP_CS_PD_ADPLL_BYPASS:
        {
            uint32_t pd_adpll_bypass_cal_reg;
            uint32_t pd_adpll_bypass_cal_value;
            uint8_t i;
            uint32_t current_otp_addr = OTP_CS_BASE_ADDR + offset + 4;
            uint8_t sdk_reg_value_pairs = num/2;

            while(sdk_reg_value_pairs) {
                // Get the new sdk register/value pair
                pd_adpll_bypass_cal_reg = GetWord32(current_otp_addr); current_otp_addr += 4;
                pd_adpll_bypass_cal_value = GetWord32(current_otp_addr); current_otp_addr += 4;
                
                // Search and update/bypass the otp_cs.pd_adpll struct with the new values
                for (i = 0; i < OTP_CS_PD_ADPLL_LEN; i++)
                {
                    if (otp_cs.pd_adpll[i].addr == pd_adpll_bypass_cal_reg)
                    {
                        otp_cs.pd_adpll[i].val = pd_adpll_bypass_cal_value;
                        break;
                    }
                }
                --sdk_reg_value_pairs;
            }
            break;
        }

        // Handling for User OTP CS items can be added here
        default:
            break;
    }
}

#if defined (__DA14531_01__) && !defined (__EXCLUDE_ROM_OTP_CS__)
int8_t otp_cs_store(void)
{
    __WEAK void sb_set_debugger_mode(bool mode);
    // Used only by the secondary bootloader application
    sb_set_debugger_mode(true);

    __WEAK void otp_cs_store_cmd_handler(uint32_t value);

    int8_t error = otp_cs_store_ROM(&otp_cs, &booter_val, &txdiv_trim, otp_cs_store_user_handler, otp_cs_store_cmd_handler);

    // Use default value if respective OTP CS entry is not written
    if (!otp_cs.trim_values.xtal_wait_trim)
    {
        otp_cs.trim_values.xtal_wait_trim = XTAL32M_WAIT_TRIM_TIME_USEC;
    }

    // Use default value if respective OTP CS entry is not written
    if (!otp_cs.trim_values.xtal_trim_value)
    {
        store_default_xtal32m_trim_value();
    }

    return error;
}
#elif defined (__DA14535__) && !defined (__EXCLUDE_ROM_OTP_CS__)

static void pd_sys_func(uint32_t id, uint32_t num, uint32_t offset);
static void pd_rad_func(uint32_t id, uint32_t num, uint32_t offset);
static void pd_adpll_func(uint32_t id, uint32_t num, uint32_t offset);
static void pd_id21_func(uint32_t id, uint32_t num, uint32_t offset);

static void pd_sys_func(uint32_t id, uint32_t num, uint32_t offset)
{
    UNUSED(id);

    // Store the GP_ADC_TRIM_REG trim value
    otp_cs.trim_values.gp_adc_trim_reg = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset + (num * sizeof(uint32_t)));
}

static void pd_rad_func(uint32_t id, uint32_t num, uint32_t offset)
{
    UNUSED(id);

    memcpy(&otp_cs.pd_rad[otp_cs.pd_rad_pairs], (void *) (OTP_CS_BASE_ADDR + offset + 4), num * sizeof(uint32_t));
    otp_cs.pd_rad_pairs = otp_cs.pd_rad_pairs + num;
    ASSERT_WARNING(otp_cs.pd_rad_pairs <= (OTP_CS_PD_RAD_LEN * 2));
}

static void pd_adpll_func(uint32_t id, uint32_t num, uint32_t offset)
{
    UNUSED(id);

    memcpy(&otp_cs.pd_adpll[otp_cs.pd_adpll_pairs], (void *) (OTP_CS_BASE_ADDR + offset + 4), num * sizeof(uint32_t));
    otp_cs.pd_adpll_pairs = otp_cs.pd_adpll_pairs + num;
    ASSERT_WARNING(otp_cs.pd_adpll_pairs <= (OTP_CS_PD_ADPLL_LEN * 2));
}

static void pd_id21_func(uint32_t id, uint32_t num, uint32_t offset)
{
    UNUSED(id);
    UNUSED(num);

    // Extract GPADC_ATTN2X_10 and GPADC_ATTN2X_90
    uint16_t low  = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset + 4);
    uint16_t high = (uint16_t) (GetWord32(OTP_CS_BASE_ADDR + offset + 4) >> 16);

    // Calculate ADC Gain Error based on GPADC_ATTN2X_10 and GPADC_ATTN2X_90 and retain that
    otp_cs.trim_values.gp_adc_offsh_ge = (((high - low) + ((uint16_t) (high - low) >> 2)) - UINT16_MAX); // calc_adc_ge(low, high);

    // Calculate ADC Offset Error based on GPADC_ATTN2X_10 and GPADC_ATTN2X_90 and retain that
    otp_cs.trim_values.gp_adc_offsh_offset = ((int16_t) ((9 * low) - high) >> 3);                        // calc_adc_offset(low, high);
}

int8_t otp_cs_store(void)
{
    __WEAK void sb_set_debugger_mode(bool mode);
    // Used only by the secondary bootloader application
    sb_set_debugger_mode(true);

    // Keep track of the available empty positions in the storage area (per ID)
    otp_cs.pd_rad_pairs = 0;
    otp_cs.pd_adpll_pairs = 0;

    otp_cs_store_handler_t func = {
                                    pd_sys_func,                // Hook to hanlde extra PD_SYS settings
                                    pd_rad_func,                // Hook to hanlde extra PD_RAD settings
                                    pd_adpll_func,              // Hook to handle extra PD_ADPLL settings
                                    NULL,                       // Hook to handle PD_SLP or PD_TMR settings
                                    pd_id21_func,               // Hook to handle Id 21
                                    otp_cs_store_user_handler,  // Hook to hanlde user settings
                                    NULL,                       // Hook to handle command settings
                                    OTP_CS_BASE_ADDR,
                                    OTP_CS_END_ADDR,
                                  };

    int8_t error = otp_cs_store_ROM(&otp_cs, &booter_val, NULL, &func);

    // Store the number of <register addr>/<value> pairs that belong to PD_RAD domain
    ASSERT_WARNING(!(otp_cs.pd_rad_pairs & 1U))
    otp_cs.pd_rad_pairs = otp_cs.pd_rad_pairs >> 1U;

    // Store the number of <register addr>/<value> pairs that belong to PD_ADPLL domain
    ASSERT_WARNING(!(otp_cs.pd_adpll_pairs & 1U))
    otp_cs.pd_adpll_pairs = otp_cs.pd_adpll_pairs >> 1U;

    // Use default value if respective OTP CS entry is not written
    if (!otp_cs.trim_values.xtal_wait_trim)
    {
        otp_cs.trim_values.xtal_wait_trim = XTAL32M_WAIT_TRIM_TIME_USEC;
    }

    // Use default value if respective OTP CS entry is not written
    if (!otp_cs.trim_values.xtal_trim_value)
    {
        store_default_xtal32m_trim_value();
    }

#if defined (CFG_ENHANCED_TX_PWR_CTRL)
    otp_cs_unpack_pd_rad_enhanced_tx_pwr_ctrl_ROM(&otp_cs, &txdiv_trim);
    otp_cs_unpack_pd_adpll_enhanced_tx_pwr_ctrl_ROM(&otp_cs, txdiv_trim);
#endif

    return error;
}
#else
int8_t otp_cs_store(void)
{
    volatile uint32_t value;
    uint32_t offset = 0;
    uint32_t num = 0;
    uint32_t id;

#if defined (CFG_DA14535_T0)
    otp_cs.trim_values.xtal_wait_trim = XTAL32M_WAIT_TRIM_TIME_USEC;    // needed since OTP is blank in T0 version
#endif

#if !defined (__DA14535__)
    // Array to temporarily store the GP_ADC_GEN trim values read from CS
    uint32_t gp_adc_gen[OTP_CS_GP_ADC_GEN_LEN];
#endif

    // Keep track of the available empty positions in the storage area (per ID)
    otp_cs.pd_rad_pairs = 0;
    otp_cs.pd_adpll_pairs = 0;

#if !defined (__DA14535__)
    uint32_t gp_adc_gen_idx = 0;
#endif

    __WEAK void sb_set_debugger_mode(bool mode);
    // Used only by the secondary bootloader application
    sb_set_debugger_mode(true);

    // Read 1st entry from OTP CS section
    if(GetWord32(OTP_CS_BASE_ADDR) != OTP_CS_CMD_START)
    {
        return OTP_CS_ERROR_NO_CS_SECTION_FOUND; // no CS found;
    }

    // Prepare for CS parsing
    offset = offset + 4;

    // Valid CS was found, continue processing
    while (OTP_CS_BASE_ADDR + offset < OTP_CS_END_ADDR)
    {
        value = GetWord32(OTP_CS_BASE_ADDR + offset);

        if ((value & 0xF0000000) == OTP_CS_CMD_SDK_VAL)
        {
            id = value & 0x000000FF;
            num = (value & 0x0000FF00) >> 8;

            switch (id)
            {
#if defined (__DA14535__)
                case OTP_CS_PD_SYS:
                {
                    // Store the GP_ADC_TRIM_REG trim value
                    otp_cs.trim_values.gp_adc_trim_reg = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset + (num * sizeof(uint32_t)));
                    break;
                }
#else
                case OTP_CS_PD_SYS:
#endif
                case OTP_CS_PD_SLP:
                case OTP_CS_PD_TMR:
                {
                    break;
                }
                case OTP_CS_PD_RAD:
                {
                    memcpy(&otp_cs.pd_rad[otp_cs.pd_rad_pairs], (void *) (OTP_CS_BASE_ADDR + offset + 4), num * sizeof(uint32_t));
                    otp_cs.pd_rad_pairs = otp_cs.pd_rad_pairs + num;
                    ASSERT_WARNING(otp_cs.pd_rad_pairs <= (OTP_CS_PD_RAD_LEN * 2));
                    break;
                }
                case OTP_CS_PD_ADPLL:
                {
                    memcpy(&otp_cs.pd_adpll[otp_cs.pd_adpll_pairs], (void *) (OTP_CS_BASE_ADDR + offset + 4), num * sizeof(uint32_t));
                    otp_cs.pd_adpll_pairs = otp_cs.pd_adpll_pairs + num;
                    ASSERT_WARNING(otp_cs.pd_adpll_pairs <= (OTP_CS_PD_ADPLL_LEN * 2));
                    break;
                }
                case OTP_CS_GP_ADC_SINGLE:
                {
                    // Extract GPADC_GE_OFFS_SE_10 and GPADC_GE_OFFS_SE_90
                    uint16_t low  = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset + 4);
                    uint16_t high = (uint16_t) (GetWord32(OTP_CS_BASE_ADDR + offset + 4) >> 16);

                    // Calculate ADC Gain Error based on GPADC_GE_OFFS_SE_10 and GPADC_GE_OFFS_SE_90 and retain that
                    otp_cs.trim_values.gp_adc_single_ge = calc_adc_ge(low, high);

                    // Calculate ADC Offset Error based on GPADC_GE_OFFS_SE_10 and GPADC_GE_OFFS_SE_90 and retain that
                    otp_cs.trim_values.gp_adc_single_offset = calc_adc_offset(low, high);

                    break;
                }
                case OTP_CS_GP_ADC_DIFF:
                {
                    // Extract GPADC_GE_OFFS_DIF_m80 and GPADC_GE_OFFS_DIF_p80
                    uint16_t low  = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset + 4);
                    uint16_t high = (uint16_t) (GetWord32(OTP_CS_BASE_ADDR + offset + 4) >> 16);

                    // Calculate ADC Gain Error based on GPADC_GE_OFFS_DIF_m80, GPADC_GE_OFFS_DIF_p80 and retain that
                    otp_cs.trim_values.gp_adc_diff_ge = calc_adc_ge(low, high);

                    // Calculate ADC Offset Error based on GPADC_GE_OFFS_DIF_m80, GPADC_GE_OFFS_DIF_p80 and retain that
                    otp_cs.trim_values.gp_adc_diff_offset = calc_adc_offset(low, high);

                    break;
                }
#if !defined (__DA14535__)
                case OTP_CS_GP_ADC_GEN:
                {
                    memcpy(&gp_adc_gen[gp_adc_gen_idx], (void *) (OTP_CS_BASE_ADDR + offset + 4), num * sizeof(uint32_t));
                    gp_adc_gen_idx = gp_adc_gen_idx + num;
                    ASSERT_WARNING(gp_adc_gen_idx <= OTP_CS_GP_ADC_GEN_LEN)
                    break;
                }
#endif
                case OTP_CS_GP_TEMP:
                {
                    // Extract and retain the 25 degrees Celsius calibration value
                    otp_cs.trim_values.adc_25_cal = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset + 4);
                    break;
                }
#if defined (__DA14535__)
                case OTP_CS_GP_ADC_SINGLE_ATTN2X:
                {
                    // Extract GPADC_ATTN2X_10 and GPADC_ATTN2X_90
                    uint16_t low  = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset + 4);
                    uint16_t high = (uint16_t) (GetWord32(OTP_CS_BASE_ADDR + offset + 4) >> 16);

                    // Calculate ADC Gain Error based on GPADC_ATTN2X_10 and GPADC_ATTN2X_90 and retain that
                    otp_cs.trim_values.gp_adc_offsh_ge = calc_adc_ge(low, high);

                    // Calculate ADC Offset Error based on GPADC_ATTN2X_10 and GPADC_ATTN2X_90 and retain that
                    otp_cs.trim_values.gp_adc_offsh_offset = calc_adc_offset(low, high);

                    break;
                }
#endif
                case OTP_CS_LP_CLK_SET:
                {
                    otp_cs.trim_values.lp_clk_set[0] = GetWord32(OTP_CS_BASE_ADDR + offset + 4);
                    break;
                }
                case OTP_CS_XTAL_TRIM:
                {
                    // Low 16-bit hold the XTAL32M wait trim value in usec
                    otp_cs.trim_values.xtal_wait_trim = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset + 4);
                    ASSERT_WARNING(otp_cs.trim_values.xtal_wait_trim) // zero value is not allowed
                    break;
                }
                default:
                {
                    // User hook to handle extra OTP CS items 
                    otp_cs_store_user_handler(id, num, offset);
                    break;
                }
            }
            offset = offset + 4 + (num * 4);
        }
        else if (value >= 0x40000000 && value <= 0x5000424C)
        {
            // CS reached a register address/value pair (already handled by the booter)
            offset = offset + 4 + 4;

            if (value == BANDGAP_REG)
            {
                // BANDGAP_REG value exists in OTP CS
                booter_val.bandgap_reg = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset - 4);
            }
            else if (value == CLK_FREQ_TRIM_REG)
            {
                // XTAL32M trim value exists in OTP CS
                otp_cs.trim_values.xtal_trim_value = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset - 4);
            }
            else if (value == CLK_RC32M_REG)
            {
                // CLK_RC32M_REG value exists in OTP CS
                booter_val.clk_rc32m_reg = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset - 4);
            }
            else if (value == CLK_RC32K_REG)
            {
                // CLK_RC32K_REG value exists in OTP CS
                booter_val.clk_rc32k_reg = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset - 4);
            }
        }
        else if ((value & 0xF0000000) == OTP_CS_CMD_SWD_MODE)
        {
            // Used only by the secondary bootloader application
            sb_set_debugger_mode(false);
            // CS reached development mode value (already handled by the booter)
            offset = offset + 4;
        }
        else if ((value & 0xF0000000) == OTP_CS_CMD_UART_STX)
        {
            // CS reached UART STX timeout value (already handled by the booter)
            offset = offset + 4;
        }
        else if ((value & 0xF0000000) == OTP_CS_CMD_SPI_CLK)
        {
            // CS reached SPI clock value (already handled by the booter)
            offset = offset + 4;
        }
        else if ((value & 0xF0000000) == OTP_CS_CMD_DEBUG_VAL)
        {
            // CS reached debug value (already handled by the booter)
            offset = offset + 4;
        }
#if defined (__DA14535__)
        else if ((value & 0xF0000000) == OTP_CS_CMD_SPI_FLASH_WAKEUP_TIME)
        {
            // CS reached SPI flash memory wakeup times (already handled by the booter)
            offset = offset + 4;
        }
#endif
        else if (value == OTP_CS_EMPTY_VAL)
        {
            break; // End of CS
        }
        else
        {
            // should not enter here
            ASSERT_WARNING(0)
        }
    }

#if !defined (__DA14535__)
    // Store the GP_ADC_TRIM_REG trim value of the GP_ADC_GEN group
    otp_cs.trim_values.gp_adc_trim_reg = (uint16_t) gp_adc_gen[0];

    // Extract GPADC_SHIFTER_10 and GPADC_SHIFTER_90
    uint16_t low  = (uint16_t) gp_adc_gen[1];
    uint16_t high = (uint16_t) (gp_adc_gen[1] >> 16);

    // Calculate ADC Gain Error based on the above values and store it
    otp_cs.trim_values.gp_adc_offsh_ge = calc_adc_ge(low, high);

    // Calculate ADC Offset Error based on the above values and store it
    otp_cs.trim_values.gp_adc_offsh_offset = calc_adc_offset(low, high);
#endif

    // Store the number of <register addr>/<value> pairs that belong to PD_RAD domain
    ASSERT_WARNING(!(otp_cs.pd_rad_pairs & 1U))
    otp_cs.pd_rad_pairs = otp_cs.pd_rad_pairs >> 1U;

    // Store the number of <register addr>/<value> pairs that belong to PD_ADPLL domain
    ASSERT_WARNING(!(otp_cs.pd_adpll_pairs & 1U))
    otp_cs.pd_adpll_pairs = otp_cs.pd_adpll_pairs >> 1U;

    // Use default value if respective OTP CS entry is not written
    if (!otp_cs.trim_values.xtal_wait_trim)
    {
        otp_cs.trim_values.xtal_wait_trim = XTAL32M_WAIT_TRIM_TIME_USEC;
    }

    // Use default value if respective OTP CS entry is not written
    if (!otp_cs.trim_values.xtal_trim_value)
    {
        store_default_xtal32m_trim_value();
    }

    enhanced_tx_pwr_ctrl_unpack();

    return OTP_CS_ERROR_OK;
}
#endif // __EXCLUDE_ROM_OTP_CS__

#if (defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_OTP_CS__)
#if !defined (__NON_BLE_EXAMPLE__)
void otp_cs_load_pd_rad(void)
{
#if defined (__DA14535__)
    // Add 1 to `rf_tx_pwr_lvl` since the DA14535 ROM functions were built with the DA14531 dBm enumeration.
    // Based on the configured Tx ouput power level, we need to select between the 0dBm and 3dBm settings.
    rf_tx_pwr_lvl++;
#endif
    otp_cs_load_pd_rad_ROM(&otp_cs, &txdiv_trim, cfg_enhanced_tx_pwr_ctrl, rf_tx_pwr_lvl);
#if defined (__DA14535__)
    // Restore the `rf_tx_pwr_lvl` value after calling the ROM function.
    rf_tx_pwr_lvl--;
#endif
}
#endif // __NON_BLE_EXAMPLE__
#else
void otp_cs_load_pd_rad(void)
{
    for (int i = 0; i < otp_cs.pd_rad_pairs; i++)
    {
#if defined (CFG_ENHANCED_TX_PWR_CTRL)
        if (otp_cs.pd_rad[i].addr == RF_PA_CTRL_REG)
        {
            continue;
        }
#else
        if ((otp_cs.pd_rad[i].addr == RF_PA_CTRL_REG) &&
            (((otp_cs.pd_rad[i].val >> 10U) & 0x7FFF) != 0) &&  // Distinguish T2 from T1 - assumed that T2 samples have at least one non zero bit spread from bit 24 to bit 10, at RF_PA_CTRL_REG index
            (rf_pa_pwr_get() > RF_TX_PWR_LVL_0d0))         // Select between 0dBm and 3dBm settings
            {
                SetWord32(otp_cs.pd_rad[i].addr, (otp_cs.pd_rad[i].val & PA_RAMP_STEP_SPEED) | ((otp_cs.pd_rad[i].val >> 10) & 0x03F));
                txdiv_trim = (otp_cs.pd_rad[i].val >> 16U) & 0x01FF; // store the 9-bit value for 3dBm (power setting 10 and above) ADPLL_DIV_CTRL_REG.TXDIV_TRIM setting
                continue;
            }
#endif
        SetWord32(otp_cs.pd_rad[i].addr, otp_cs.pd_rad[i].val);
    }
}
#endif // __EXCLUDE_ROM_OTP_CS__

#if (defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_OTP_CS__)
#if !defined (__NON_BLE_EXAMPLE__)
void otp_cs_load_pd_adpll(void)
{
#if defined (__DA14535__)
    // Add 1 to `rf_tx_pwr_lvl` since the DA14535 ROM functions were built with the DA14531 dBm enumeration.
    // Based on the configured Tx ouput power level, we need to select between the 0dBm and 3dBm settings.
    rf_tx_pwr_lvl++;
#endif
    otp_cs_load_pd_adpll_ROM(&otp_cs, txdiv_trim, cfg_enhanced_tx_pwr_ctrl, rf_tx_pwr_lvl);
#if defined (__DA14535__)
    // Restore the `rf_tx_pwr_lvl` value after calling the ROM function.
    rf_tx_pwr_lvl--;
#endif
}
#endif // __NON_BLE_EXAMPLE__
#else
void otp_cs_load_pd_adpll(void)
{
    for (int i = 0; i < otp_cs.pd_adpll_pairs; i++)
    {
#if defined (CFG_ENHANCED_TX_PWR_CTRL)
        if (otp_cs.pd_adpll[i].addr == ADPLL_DIV_CTRL_REG)
        {
            continue;
        }
#else
        /* Special handling for ADPLL_DIV_CTRL_REG:
         *      PrefValues: TXDIV_TRIM = OTP CS value, RXDIV_TRIM = will be applied by `set_recommended_settings()`.
         *      The rest fields (RXDIV_FB_EN_TX, RXDIV_FB_EN_RX, FBDIV_EN) keep their reset values.
         * Reset values for ADPLL_DIV_CTRL_REG are: TXDIV_TRIM = 0xCC, RXDIV_TRIM = 0xFF, RXDIV_FB_EN_TX = 0x0, RXDIV_FB_EN_RX = 0x1, FBDIV_EN = 0x1.
        */
        if ((otp_cs.pd_adpll[i].addr == ADPLL_DIV_CTRL_REG) &&
            (rf_pa_pwr_get() > RF_TX_PWR_LVL_0d0))    // Select between 0dBm and 3dBm settings
            {
                SetWord32(otp_cs.pd_adpll[i].addr, ((otp_cs.pd_adpll[i].val & (~TXDIV_TRIM)) | (txdiv_trim << 17U)));
                continue;
            }
#endif
        SetWord32(otp_cs.pd_adpll[i].addr, otp_cs.pd_adpll[i].val);
    }
}
#endif // __EXCLUDE_ROM_OTP_CS__

#endif // __DA14531__

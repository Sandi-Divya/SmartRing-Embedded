/**
 ****************************************************************************************
 *
 * @file adc_531.c
 *
 * @brief DA14531/535 General Purpose ADC driver source file.
 *
 * Copyright (C) 2019-2023 Renesas Electronics Corporation and/or its affiliates.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "adc.h"
#include "compiler.h"
#include "datasheet.h"
#include "otp_cs.h"
#include "arch.h"

#if defined (CFG_ADC_DMA_SUPPORT)
#include "dma.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */

#define ADC_531_MIN(a, b)   (((a) < (b)) ? (a) : (b))

/// Callback function to be called within ADC_Handler.
static adc_interrupt_cb_t intr_cb   __SECTION_ZERO("retention_mem_area0");

/// 25 degrees Celsius calibration value
static uint16_t cal_val;

/// Store offset calibration values when OTP calibration value is used,
/// else - use reset values.
static uint16_t offp __SECTION_ZERO("retention_mem_area0");
static uint16_t offn __SECTION_ZERO("retention_mem_area0");

#if defined (CFG_ADC_DMA_SUPPORT)
/// DMA Channel for ADC RX.
static DMA_ID dma_channel           __SECTION_ZERO("retention_mem_area0");

/// IRQ state of DMA channel for ADC RX.
static DMA_IRQ_CFG dma_irq_state    __SECTION_ZERO("retention_mem_area0");
#endif

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

#if defined (__DA14535__) && !defined (__EXCLUDE_ROM_ADC_531__)
typedef struct
{
    uint16_t cal_val;
    uint8_t sample_time;
    uint8_t oversample;
    uint8_t ldo_level;
    int us_delay;
} adc_531_fixed_values_t;
#endif

// Required DA14531-01 ROM symbols
#if defined (__DA14531_01__) && !defined (__EXCLUDE_ROM_ADC_531__)
extern void adc_init_ROM(const adc_config_t *cfg, uint16_t *ptr_cal_val, otp_cs_t *ptr_otp_cs);
extern void adc_offset_calibrate_ROM(adc_input_mode_t input_mode, uint16_t *ptr_cal_val, otp_cs_t *ptr_otp_cs);
extern uint16_t adc_correct_sample_ROM(const uint16_t adc_val, otp_cs_t *ptr_otp_cs);
extern int8_t adc_get_temp_ROM(uint16_t cal_val);
extern void adc_get_temp_async_ROM(uint16_t *adc_samples, uint16_t len, int8_t *temp_values, uint16_t cal_val);
#endif // __EXCLUDE_ROM_ADC_531__

// Required DA14535 ROM symbols
#if defined (__DA14535__) && !defined (__EXCLUDE_ROM_ADC_531__)
extern void adc_init_ROM(const adc_config_t *cfg, uint16_t *ptr_cal_val, otp_cs_t *ptr_otp_cs, adc_531_fixed_values_t *fixed);
extern void adc_offset_calibrate_ROM(adc_input_mode_t input_mode, uint16_t *ptr_cal_val, otp_cs_t *ptr_otp_cs, adc_531_fixed_values_t *fixed);
extern uint16_t adc_correct_sample_ROM(const uint16_t adc_val, otp_cs_t *ptr_otp_cs, uint8_t oversample);
extern uint16_t adc_get_temp_ROM(void);
#endif // __EXCLUDE_ROM_ADC_531__

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

extern void arch_asm_delay_us(int nof_us);

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if defined (CFG_ADC_DMA_SUPPORT)
/**
 ****************************************************************************************
 * @brief Initializes DMA for ADC according to @p cfg.
 * @param[in] cfg The ADC configuration.
 ****************************************************************************************
 */
static void adc_dma_init(const adc_config_t *cfg)
{
    // Set and retain DMA Channel for ADC RX
    dma_channel = (cfg->dma_channel == ADC_DMA_CHANNEL_01) ? DMA_CHANNEL_0
                                                           : DMA_CHANNEL_2;

    // DMA channel configuration for ADC RX
    dma_cfg_t dma_cfg =
    {
        .bus_width       = DMA_BW_HALFWORD,
        .irq_enable      = DMA_IRQ_STATE_ENABLED,
        .dreq_mode       = DMA_DREQ_TRIGGERED,
        .src_inc         = DMA_INC_FALSE,
        .dst_inc         = DMA_INC_TRUE,
        .circular        = DMA_MODE_NORMAL,
        .dma_prio        = cfg->dma_priority,
        .dma_idle        = DMA_IDLE_BLOCKING_MODE,
        .dma_init        = DMA_INIT_AX_BX_AY_BY,
        .dma_sense       = DMA_SENSE_LEVEL_SENSITIVE,
        .dma_req_mux     = DMA_TRIG_ADC_RX,
        .src_address     = (uint32_t) GP_ADC_RESULT_REG,
        .dst_address     = cfg->dst_addr,
        .irq_nr_of_trans = 0,
        .length          = cfg->len,
        .cb              = cfg->rx_cb,
        .user_data       = cfg->user_data
    };

    // Retain IRQ state of DMA channel for ADC RX
    dma_irq_state = dma_cfg.irq_enable;

    // Enable DMA functionality
    adc_dma_enable();

    // Initialize DMA channel for ADC RX
    dma_initialize(dma_channel, &dma_cfg);
}
#endif // CFG_ADC_DMA_SUPPORT

/**
 ****************************************************************************************
 * @brief Configures ADC according to @p cfg.
 * @param[in] cfg The ADC configuration.
 * @note No configuration is performed if @p cfg is NULL.
 ****************************************************************************************
 */
__attribute__((unused)) static void adc_configure(const adc_config_t *cfg)
{
    if (cfg != NULL)
    {
        adc_set_input_mode(cfg->input_mode);

        // Set input channels as appropriate
        adc_set_se_input((adc_input_se_t) cfg->input);
        if (cfg->input_mode == ADC_INPUT_MODE_DIFFERENTIAL)
        {
            adc_set_diff_input((adc_input_diff_t) (cfg->input >> 4U));
        }

        // Check whether Continuous mode shall be enabled
        (cfg->continuous == true) ? adc_continuous_enable() : adc_continuous_disable();

        adc_set_interval(cfg->interval_mult);

        // Check whether the die temperature sensor shall be enabled - if so,
        // configure ADC accordingly
        if (adc_get_se_input() == ADC_INPUT_SE_TEMP_SENS)
        {
            // ADC_OTP_CAL_25C
            cal_val = otp_cs_get_adc_25_cal();
            if (cal_val == 0)
            {
                // OTP calibration value is not present - use typical value instead
                cal_val = DEFAULT_TEMPERATURE_CAL_VAL;
            }
            SetBits16(GP_ADC_CTRL_REG, DIE_TEMP_EN, 1);
#if defined (__DA14535__)
            SetBits16(GP_ADC_CTRL_REG, GP_ADC_EN, 1);
            // Guideline from the Analog IC Team: Wait for 50us to let the temperature
            // sensor settle just after enabling it
            arch_asm_delay_us(50);
#else
            // Guideline from the Analog IC Team: Wait for 25us to let the temperature
            // sensor settle just after enabling it
            arch_asm_delay_us(25);
#endif
            adc_chopper_enable();
            adc_set_input_mode(ADC_INPUT_MODE_SINGLE_ENDED);
            adc_ldo_const_current_enable();
            SetBits16(GP_ADC_CTRL2_REG, GP_ADC_STORE_DEL, 0);
#if defined (__DA14535__)
            adc_set_sample_time(5);
            adc_set_oversampling(2);
#else
            adc_set_sample_time(15);
            adc_set_oversampling(6);
#endif
            adc_attn_config(ADC_INPUT_ATTN_NO);
        }
        else
        {
            adc_temp_sensor_disable();
            adc_set_sample_time(cfg->smpl_time_mult);
            adc_attn_config(cfg->input_attenuator);

            // Check whether Chopper mode shall be enabled
            (cfg->chopping == true) ? adc_chopper_enable() : adc_chopper_disable();

            adc_set_oversampling(cfg->oversampling);
        }

#if defined (CFG_ADC_DMA_SUPPORT)
        adc_dma_init(cfg);
#endif
    }
}

/**
 ****************************************************************************************
 * @details Writes the default values to the CTRL registers as well as the trim
 * value from OTP CS to the GP_ADC_TRIM_REG register and configures ADC
 * according to @p cfg.
 * @note No configuration is performed if @p cfg is NULL.
 ****************************************************************************************
 */
#if ((defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_ADC_531__))
void adc_init(const adc_config_t *cfg)
{
#if defined (__DA14535__)
    adc_531_fixed_values_t fixed = {DEFAULT_TEMPERATURE_CAL_VAL, 5, 2, 0, 50};
    fixed.ldo_level = (otp_cs_get_adc_trim_val()) >> 4;
    adc_init_ROM(cfg, &cal_val, &otp_cs, &fixed);
#else
    adc_init_ROM(cfg, &cal_val, &otp_cs);
#endif
}
#else
void adc_init(const adc_config_t *cfg)
{
    // Write the default values
    SetWord16(GP_ADC_CTRL_REG, GP_ADC_CTRL_REG_RESET);
    SetWord16(GP_ADC_CTRL2_REG, GP_ADC_CTRL2_REG_RESET);
    SetWord16(GP_ADC_CTRL3_REG, GP_ADC_CTRL3_REG_RESET);

    // Write the trim value from OTP CS to the GP_ADC_TRIM_REG register
    SetWord16(GP_ADC_TRIM_REG, otp_cs_get_adc_trim_val());
    // DA14531: Set GP_ADC_LDO_LEVEL to the preferred level of 925mV (used fixed value for DA14531)
#if !defined (__DA14535__)
    SetBits16(GP_ADC_TRIM_REG, GP_ADC_LDO_LEVEL, 4);
#endif

    NVIC_DisableIRQ(ADC_IRQn);

    adc_configure(cfg);

    adc_enable();
}
#endif // __EXCLUDE_ROM_ADC_531__

#if (!defined (__DA14531_01__) && !defined (__DA14535__)) || defined (__EXCLUDE_ROM_ADC_531__)
void adc_reset(void)
{
    // Write the default values without disabling LDO and ADC
    SetWord16(GP_ADC_CTRL_REG, GP_ADC_EN);
    SetWord16(GP_ADC_CTRL2_REG, GP_ADC_CTRL2_REG_RESET);
    SetWord16(GP_ADC_CTRL3_REG, GP_ADC_CTRL3_REG_RESET);

    NVIC_DisableIRQ(ADC_IRQn);
}
#endif // __EXCLUDE_ROM_ADC_531__

void adc_input_shift_enable(void)
{
#if !defined (__DA14535__)
    SetBits16(GP_ADC_CTRL2_REG, GP_ADC_OFFS_SH_EN, 1);

    // Guideline from the Analog IC Team: Wait for 16us to let the input shifter
    // settle just after enabling it
    arch_asm_delay_us(16);
#endif
}

void adc_input_shift_disable(void)
{
#if !defined (__DA14535__)
    SetBits16(GP_ADC_CTRL2_REG, GP_ADC_OFFS_SH_EN, 0);
#endif
}

void adc_input_shift_config(adc_input_sh_gain_t gain, adc_input_sh_cm_t cm)
{
#if !defined (__DA14535__)
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_OFFS_SH_GAIN_SEL, gain);
    SetBits16(GP_ADC_CTRL2_REG, GP_ADC_OFFS_SH_CM, cm);

    adc_input_shift_enable();
#endif
}

#if (!defined (__DA14531_01__) && !defined (__DA14535__)) || defined (__EXCLUDE_ROM_ADC_531__)
void adc_set_se_input(adc_input_se_t input)
{
    SetBits16(GP_ADC_SEL_REG, GP_ADC_SEL_P, input);
}

void adc_set_diff_input(adc_input_diff_t input)
{
    SetBits16(GP_ADC_SEL_REG, GP_ADC_SEL_N, input);
}
#endif // __EXCLUDE_ROM_ADC_531__

void adc_register_interrupt(adc_interrupt_cb_t callback)
{
    intr_cb = callback;
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_MINT, 1);

    // Set the priority of ADC interrupt to level 2
    NVIC_SetPriority(ADC_IRQn, 2);
    // Enable ADC interrupt
    NVIC_EnableIRQ(ADC_IRQn);
}

void adc_unregister_interrupt(void)
{
    NVIC_DisableIRQ(ADC_IRQn);
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_MINT, 0);
    NVIC_ClearPendingIRQ(ADC_IRQn);
    intr_cb = NULL;
}

#if (!defined (__DA14531_01__) && !defined (__DA14535__)) || defined (__EXCLUDE_ROM_ADC_531__)
/**
 ****************************************************************************************
 * @note Sampling gets started; conversion gets started as soon as
 * adc_start() is called.
 * @sa adc_delay_set
 * @sa adc_start
 ****************************************************************************************
 */
void adc_enable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_EN, 1);
}

/**
 ****************************************************************************************
 * @note Conversion shall have completed before disabling ADC.
 * When in continuous mode, continuous mode shall be disabled and the pending
 * conversion shall complete in order for ADC to be in a defined state.
 * @sa adc_in_progress
 * @sa adc_set_continuous
 ****************************************************************************************
 */
void adc_disable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_EN, 0);
}

void adc_start(void)
{
#if defined (CFG_ADC_DMA_SUPPORT)
    // Start DMA channel for ADC RX
    dma_channel_start(dma_channel, dma_irq_state);
#endif
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_START, 1);
}

/**
 ****************************************************************************************
 * @note The corrected conversion result may be read calling @c adc_correct_sample.
 ****************************************************************************************
 */
uint16_t adc_get_sample(void)
{
    adc_start();
    while (adc_in_progress())
        ;
    adc_clear_interrupt();

    return (GetWord16(GP_ADC_RESULT_REG));
}
#endif // __EXCLUDE_ROM_ADC_531__

#if (defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_ADC_531__)
void adc_offset_calibrate(adc_input_mode_t input_mode)
{
#if defined (__DA14535__)
    adc_531_fixed_values_t fixed = {DEFAULT_TEMPERATURE_CAL_VAL, 5, 2, 0, 50};
    fixed.ldo_level = (otp_cs_get_adc_trim_val()) >> 4;
    adc_offset_calibrate_ROM(input_mode, &cal_val, &otp_cs, &fixed);
#else
    adc_offset_calibrate_ROM(input_mode, &cal_val, &otp_cs);
#endif
}
#else
void adc_offset_calibrate(adc_input_mode_t input_mode)
{
#if !defined (__DA14535__)
    // Input shifter shall be disabled
    ASSERT_WARNING(GetBits16(GP_ADC_CTRL2_REG, GP_ADC_OFFS_SH_EN) == 0);
#endif

    uint16_t adc_val, adc_off_p, adc_off_n;
    uint32_t diff;

    // Store the current content of the following registers:
    // - GP_ADC_CTRL_REG,
    // - GP_ADC_CTRL2_REG.
    uint16_t tmp_adc_ctrl_reg  = GetWord16(GP_ADC_CTRL_REG);
    uint16_t tmp_adc_ctrl2_reg = GetWord16(GP_ADC_CTRL2_REG);

    // Write the default values to the CTRL registers and enable ADC
    adc_init(NULL);

    // Before starting the offset calibration itself, set the following:
    // - GP_ADC_I20U      = 1,
    // - GP_ADC_SMPL_TIME = 1,
    // - GP_ADC_CHOP      = 0,
    // - GP_ADC_CONV_NRS  > 3.
    adc_ldo_const_current_enable();
    adc_set_sample_time(1);
    adc_chopper_disable();
    adc_set_oversampling(7);

    // Set the provided input mode just to be on the safe side
    adc_set_input_mode(input_mode);

    // Try once to calibrate correctly

    adc_set_offset_positive(0x200);
    adc_set_offset_negative(0x200);
    adc_input_mute();

    adc_sign_change_disable();

    adc_val   = adc_get_sample();
    adc_off_p = (adc_val >> 6) - 0x200;

    adc_sign_change_enable();

    adc_val   = adc_get_sample();
    adc_off_n = (adc_val >> 6) - 0x200;

    if (input_mode == ADC_INPUT_MODE_SINGLE_ENDED)
    {
        adc_set_offset_positive(0x200 - 2 * adc_off_p);
        adc_set_offset_negative(0x200 - 2 * adc_off_n);
    }
    else
    {
        adc_set_offset_positive(0x200 - adc_off_p);
        adc_set_offset_negative(0x200 - adc_off_n);
    }

    adc_sign_change_disable();

    // Verify the calibration result

    adc_val = adc_get_sample() >> 6;

    if (adc_val > 0x200)
    {
        diff = adc_val - 0x200;
    }
    else
    {
        diff = 0x200 - adc_val;
    }

    if (diff >= 0x8)
    {
        // Calibration failed
        ASSERT_WARNING(0);
    }

    // Restore the content of the ADC CTRL and CTRL2 registers
    SetWord16(GP_ADC_CTRL_REG, tmp_adc_ctrl_reg);
    SetWord16(GP_ADC_CTRL2_REG, tmp_adc_ctrl2_reg);
}
#endif // __EXCLUDE_ROM_ADC_531__

#if !defined (__EXCLUDE_ADC_HANDLER__)
void ADC_Handler(void)
{
    adc_clear_interrupt();

    if (intr_cb != NULL)
    {
        intr_cb();
    }
}
#endif

#if (!defined (__DA14531_01__) && !defined (__DA14535__)) || defined (__EXCLUDE_ROM_ADC_531__)
/**
 ****************************************************************************************
 * @brief Returns the corrected conversion result.
 * @param[in] input Raw ADC value.
 * @return corrected conversion result
 ****************************************************************************************
 */
static uint16_t adc_correction_apply(uint16_t input)
{
    int16_t gain_error, offset;
    uint32_t res;

#if defined (__DA14535__)
    if (GetBits16(GP_ADC_CTRL2_REG, GP_ADC_ATTN) == 1)      // Use Attenuator x2 instead of Shifter in DA14535
#else
    if (GetBits16(GP_ADC_CTRL2_REG, GP_ADC_OFFS_SH_EN) == 1)
#endif
    {
        // Attenuator x2 is enabled
        // Set Gain Error and Offset accordingly
        gain_error = otp_cs_get_adc_offsh_ge();
        offset     = otp_cs_get_adc_offsh_offset();
    }
    else if (adc_get_input_mode() == ADC_INPUT_MODE_SINGLE_ENDED)
    {
        // Single-ended mode; input shifter is disabled
        // Set Gain Error and Offset accordingly
        gain_error = otp_cs_get_adc_single_ge();
        offset     = otp_cs_get_adc_single_offset();
    }
    else
    {
        // Differential mode; input shifter is disabled
        // Set Gain Error and Offset accordingly
        gain_error = otp_cs_get_adc_diff_ge();
        offset     = otp_cs_get_adc_diff_offset();
    }

    // Gain error and offset correction in two steps - not that efficient -
    // to avoid possible overflows
    res = (UINT16_MAX * (uint32_t) input) / (UINT16_MAX + gain_error);
    res = res - (UINT16_MAX * offset) / (UINT16_MAX + gain_error);

    // Boundary check for lower limit
    if (res > 2 * UINT16_MAX)
    {
        return 0;
    }

    // Boundary check for upper limit
    if (res > UINT16_MAX)
    {
        return UINT16_MAX;
    }

    return ((uint16_t) res);
}
#endif // __EXCLUDE_ROM_ADC_531__

#if defined (__DA14531_01__) && !defined (__EXCLUDE_ROM_ADC_531__)
uint16_t adc_correct_sample(const uint16_t adc_val)
{
    return adc_correct_sample_ROM(adc_val, &otp_cs);
}
#elif defined (__DA14535__) && !defined (__EXCLUDE_ROM_ADC_531__)
uint16_t adc_correct_sample(const uint16_t adc_val)
{
    uint8_t oversample = 6;
    return adc_correct_sample_ROM(adc_val, &otp_cs, oversample);
}
#else
uint16_t adc_correct_sample(const uint16_t adc_val)
{
    return (adc_correction_apply(adc_val) >> (6 - ADC_531_MIN(6, adc_get_oversampling())));
}
#endif // __EXCLUDE_ROM_ADC_531__

#if !defined (__DA14531_01__) || defined (__EXCLUDE_ROM_ADC_531__)
void adc_temp_sensor_enable(void)
{
    SetBits16(GP_ADC_CTRL_REG, DIE_TEMP_EN, 1);

#if !defined (__DA14535__)
    // Guideline from the Analog IC Team: Wait for 25us to let the temperature
    // sensor settle just after enabling it
    arch_asm_delay_us(25);
#endif
}
#endif // __EXCLUDE_ROM_ADC_531__

#if !defined (__DA14531_01__) || defined (__EXCLUDE_ROM_ADC_531__)
/**
 ****************************************************************************************
 * @brief Performs calculations related to the @c ADC_INPUT_SE_TEMP_SENS input channel.
 * @param[in] adc_sample Raw ADC sample being read from the @c ADC_INPUT_SE_TEMP_SENS
 *                       input channel.
 * @return temperature value in degrees Celsius
 ****************************************************************************************
 */
static int8_t temp_calc(uint16_t adc_sample)
{
#if defined (__DA14535__)
    const int32_t temp_coef = 2300;
#else
    const int32_t temp_coef = 1450;
#endif
    // Tx (Celsius) = 25 + (ADCx - ADC_OTP_CAL_25C) / ((temp_coef/1000) * 64) (+/- 0.5, to improve accuracy)
    // cal_val is either ADC_OTP_CAL_25C or 30272 (the latter when OTP calibration value is not present)
    int32_t temp = (int32_t) ((adc_sample - cal_val) * 1000);
    if (temp > 0)
    {
        temp = temp + (temp_coef / 2) ;
    }
    else if (temp < 0)
    {
        temp = temp - (temp_coef / 2);
    }
    return ((int8_t) (25 + ((temp / temp_coef) >> 6)));
}
#endif // __EXCLUDE_ROM_ADC_531__

#if defined (__DA14531_01__) && !defined (__EXCLUDE_ROM_ADC_531__)
int8_t adc_get_temp(void)
{
    return adc_get_temp_ROM(cal_val);
}
#elif defined (__DA14535__) && !defined (__EXCLUDE_ROM_ADC_531__)
int8_t adc_get_temp(void)
{
    return temp_calc(adc_get_temp_ROM());
}
#else
int8_t adc_get_temp(void)
{
    adc_start();
    while (adc_in_progress())
        ;
    adc_clear_interrupt();

    uint16_t adc_sample = GetWord16(GP_ADC_RESULT_REG);

    int8_t temp = temp_calc(adc_sample);

    return temp;
}
#endif // __EXCLUDE_ROM_ADC_531__

#if defined (__DA14531_01__) && !defined (__EXCLUDE_ROM_ADC_531__)
void adc_get_temp_async(uint16_t *adc_samples, uint16_t len, int8_t *temp_values)
{
    adc_get_temp_async_ROM(adc_samples, len, temp_values, cal_val);
}
#else
void adc_get_temp_async(uint16_t *adc_samples, uint16_t len, int8_t *temp_values)
{
    // Clear the ADC interrupt just to be on the safe side
    adc_clear_interrupt();

    for (uint16_t i = 0; i < len; i++)
    {
        temp_values[i] = temp_calc(adc_samples[i]);
    }
}
#endif // __EXCLUDE_ROM_ADC_531__

void adc_store_offsets(void)
{
    offp = adc_get_offset_positive();
    offn = adc_get_offset_negative();
}

void adc_load_offsets(void)
{
    adc_set_offset_positive(offp);
    adc_set_offset_negative(offn);
}

void adc_reset_offsets(void)
{
    adc_set_offset_positive(GP_ADC_OFFP_REG_RESET);
    adc_set_offset_negative(GP_ADC_OFFN_REG_RESET);
}

#endif // __DA14531__

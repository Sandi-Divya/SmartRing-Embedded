/**
 ****************************************************************************************
 * @addtogroup Drivers
 * @{
 * @addtogroup ADC
 * @brief General Purpose ADC driver API
 * @{
 *
 * @file adc_58x.h
 *
 * @brief DA14585/DA14586 General Purpose ADC driver header file.
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

#if !defined (__DA14531__)

#ifndef _ADC_58X_H_
#define _ADC_58X_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * DEFINES
 ****************************************************************************************
 */
#define ADC_TIMEOUT             (100000)

/// ADC input channel mode
typedef enum
{
    /// Differential mode
    ADC_INPUT_MODE_DIFFERENTIAL = 0,

    /// Single-ended mode
    ADC_INPUT_MODE_SINGLE_ENDED = 1
} adc_input_mode_t;

/// ADC single-ended input channels
typedef enum
{
    /// P0_0
    ADC_INPUT_SE_P0_0    = 0,

    /// P0_1
    ADC_INPUT_SE_P0_1    = 1,

    /// P0_2
    ADC_INPUT_SE_P0_2    = 2,

    /// P0_3
    ADC_INPUT_SE_P0_3    = 3,

    /// AVS
    ADC_INPUT_SE_AVS     = 4,

    /// VDD_REF
    ADC_INPUT_SE_VDD_REF = 5,

    /// VDD_RTT
    ADC_INPUT_SE_VDD_RTT = 6,

    /// VBAT3V
    ADC_INPUT_SE_VBAT3V  = 7,

    /// VDCDC
    ADC_INPUT_SE_VDCDC   = 8,

    /// VBAT1V
    ADC_INPUT_SE_VBAT1V  = 9
} adc_input_se_t;

/// ADC differential input channels
typedef enum
{
    /// P0_0 vs P0_1
    ADC_INPUT_DIFF_P0_0_P0_1 = 0,

    /// P0_2 vs P0_3
    ADC_INPUT_DIFF_P0_2_P0_3 = 1
} adc_input_diff_t;

/// Callback type to be associated with ADC_IRQn.
typedef void (*adc_interrupt_cb_t)(void);

/// ADC configuration
typedef struct
{
    /// Input channel mode
    adc_input_mode_t    mode;

    /// Input channel; possible values: @c adc_input_se_t, @c adc_input_diff_t
    uint8_t             input;

    /// Input/output sign change off/on
    bool                sign;

    /// Input attenuator off/on
    bool                attn;
} adc_config_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Gets ADC sample from VBAT1V or VBAT3V power supplies using the 20 usec delay.
 * @param[in] sample_vbat1v :true = sample VBAT1V, false = sample VBAT3V
 * @return ADC VBAT1V or VBAT3V sample
 ****************************************************************************************
 */
uint32_t adc_get_vbat_sample(bool sample_vbat1v);

#endif // _ADC_58X_H_

#endif // DA14585, DA14586

///@}
///@}

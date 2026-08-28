/**
 ****************************************************************************************
 *
 * @file rc32m_cal.c
 *
 * @brief RC32M Calibration functions source code
 *
 * Copyright (C) 2024-2025 Renesas Electronics Corporation and/or its affiliates.
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
#include <string.h>
#include "rc32m_cal.h"

#ifndef dg_configDIVN_FREQ
#define dg_configDIVN_FREQ              (16000) // In KHz
#endif

#define MAX_CALIBRATION_COUNT(x)        (dg_configDIVN_FREQ * x)

/* ---------- #define the RC32M calibration parameters ---------------------- */
#define RC32M_CALIBRATION_CYCLES        (4*126)
#define RC32M_RECOMMENDED_BIAS          0x0B
#define RC32M_TARGET_FREQ               30250 // RC32M target frequency in KHz. RC32M allowable frequency should be < 32MHz
#define RC32M_MEAS_TARGET               (MAX_CALIBRATION_COUNT(RC32M_CALIBRATION_CYCLES)/ (RC32M_TARGET_FREQ))

/**
 * \brief The type of clock to be calibrated
 */
typedef enum cal_clk_sel_type 
{
    CALIBRATE_RC32M = 1,
} cal_clk_t;

static void _rc32m_cal_start_calibration(cal_clk_t clk_type, uint16_t cycles)
{
    ASSERT_WARNING(!GetBits16(CLK_REF_SEL_REG, REF_CAL_START)); // Must be disabled

    SetWord16(CLK_REF_CNT_REG, cycles);                        // # of cal clock cycles
    SetBits16(CLK_REF_SEL_REG, REF_CLK_SEL, clk_type);
    SetBits16(CLK_REF_SEL_REG, REF_CAL_START,1);
}

static uint32_t _rc32m_cal_get_calibration_data(void)
{
    while (GetBits16(CLK_REF_SEL_REG, REF_CAL_START)) {          // Wait until it's finished
    }
    volatile uint32_t high = GetWord16(CLK_REF_VAL_H_REG);
    return ( high << 16 ) + GetWord16(CLK_REF_VAL_L_REG);
}

uint32_t rc32m_cal_execute(void)
{
    uint32_t m_range_0_del_4 = 0;
    uint32_t m_range_1_del_4 = 0;
    uint32_t m_trimmed = 0;
    uint32_t m_last = 0;
    uint32_t d_last = 0;
    uint32_t d_trimmed = 0;

    /* Set the recommended bias setting for the RC32M oscillator */
    SetBits16(CLK_RC32M_REG, RC32M_BIAS, RC32M_RECOMMENDED_BIAS);


    /*      Coarse trimming: RC32M__RANGE (range) = 0, 1, or 2
     *
     *      The fine tuning of the RC32M will be done after the right coarse
     *      band has been selected. Two frequency measurements are done to
     *      determine the proper coarse band. The measurements are done on
     *      the lower two coarse bands while the fine tuning is set to 3/4
     *      of the highest band frequency setting.
     *
     *      Procedure:
     *          Measure the frequency for (range, del) = (0, 4) and (1, 4)
     *          and check whether the frequency is larger than that of the
     *          target frequency.
     *
     *          Next select the proper coarse trimming band.
     *
     */
    SetBits16(CLK_RC32M_REG, RC32M_COSC, 4);
    SetBits16(CLK_RC32M_REG, RC32M_RANGE, 0);

    _rc32m_cal_start_calibration(CALIBRATE_RC32M, RC32M_CALIBRATION_CYCLES);
    m_range_0_del_4 = _rc32m_cal_get_calibration_data();

    SetBits16(CLK_RC32M_REG, RC32M_RANGE, 1);
    _rc32m_cal_start_calibration(CALIBRATE_RC32M, RC32M_CALIBRATION_CYCLES);
    m_range_1_del_4 = _rc32m_cal_get_calibration_data();

    /* Select and program the proper coarse trimming */
    uint8_t range = 0;
    if (m_range_0_del_4 > RC32M_MEAS_TARGET) {
        range++;
    }
    if (m_range_1_del_4 > RC32M_MEAS_TARGET) {
        range++;
    }

    SetBits16(CLK_RC32M_REG, RC32M_RANGE, range);

    /*
     *      Fine trimming procedure:
     *
     *      Use binary search to find the frequency closest to the target
     *      frequency. So start in the middle (= 8) of the range.
     *
     *      Note that only if m_trimmed is not equal to RC32M_MEAS_TARGET the
     *      value of RC32M_COSC must be updated and the loop could be exited.
     *      However, for the sake of a fixed pattern length this will not
     *      be done.
     *
     */
    SetBits16(CLK_RC32M_REG, RC32M_COSC, 8);

    uint8_t previous = 0;

    for (int8_t i = 2; i >= 0; i--) {

        previous = GetBits16(CLK_RC32M_REG, RC32M_COSC);
        _rc32m_cal_start_calibration(CALIBRATE_RC32M, RC32M_CALIBRATION_CYCLES);
        m_trimmed = _rc32m_cal_get_calibration_data();

        if (m_trimmed > RC32M_MEAS_TARGET) {
            uint8_t tmp = previous - (1 << i);
            SetBits16(CLK_RC32M_REG, RC32M_COSC, tmp);
        } else if (m_trimmed < RC32M_MEAS_TARGET) {
            uint8_t tmp = previous + (1 << i);
            SetBits16(CLK_RC32M_REG, RC32M_COSC, tmp);
        }
    }

    /*
     *      Notice that with the final step in the loop a last measured needs
     *      to be done.
     *
     *      The last step in the loop could have increased the distance from
     *      the target. In that case the previous setting should be selected.
     *      Otherwise, update m_trimmed with the final measurement: m_last.
     */

    _rc32m_cal_start_calibration(CALIBRATE_RC32M, RC32M_CALIBRATION_CYCLES);
    m_last = _rc32m_cal_get_calibration_data();

    /* Find the distance of the target frequency */
    if (m_last > RC32M_MEAS_TARGET) {
        d_last = m_last - RC32M_MEAS_TARGET;
    } else {
        d_last = RC32M_MEAS_TARGET - m_last;
    }

    /* Find the distance of the target frequency */
    if (m_trimmed > RC32M_MEAS_TARGET) {
        d_trimmed = m_trimmed - RC32M_MEAS_TARGET;
    } else {
        d_trimmed = RC32M_MEAS_TARGET - m_trimmed;
    }

    if (d_last > d_trimmed) {
        SetBits16(CLK_RC32M_REG, RC32M_COSC, previous);
    }
#if dg_configUNDISCLOSED_RC_CLOCKS_DIAGN_CONFIG
    if (d_last < d_trimmed) {
        rc32m_freq = MAX_CALIBRATION_COUNT(RC32M_CALIBRATION_CYCLES) / m_last;
    } else {
        rc32m_freq = MAX_CALIBRATION_COUNT(RC32M_CALIBRATION_CYCLES) / m_trimmed;
    }
#endif
    return GetWord32(CLK_RC32M_REG);
}
#endif // __DA14531__

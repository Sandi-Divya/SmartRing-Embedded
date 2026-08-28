/**
 ****************************************************************************************
 *
 * @file ate_kdco_cal.c
 *
 * @brief ATE KDCO calibration function for DA14531
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

#include <stdbool.h>
#include <stdint.h>
#include "rf_531.h"
#include "otp_cs.h"
#include "ate_kdco_cal.h"

#define LEN_FOUT        64
static int32_t FSTEP[LEN_FOUT];

#define LEN_ARRAY       513
#define TIME_OUT_CMT    7
#define FAIL_CMT_BANK   5 // CMT cap bank

__STATIC_FORCEINLINE void delay_us(uint32_t nof_us)
{
    arch_asm_delay_us(nof_us);
}

volatile uint16_t TST_ERROR_CODE;
volatile uint16_t KDCO_0;

// frequencies measured for all cap sweeps
volatile  int32_t CMT_FOUT[32];

// This is the function used by ATE.However the sdk has already on function for this usage
static void adpll_hclk_en(void)
{
    // enable hclk for ADPLL dig
    SetByte(RF_RADIO_INIT_REG + 0x1, (ADPLLDIG_HRESET_N | ADPLLDIG_HCLK_EN) >> 8);
}

static void adpll_en(uint8_t tx)
{
    if (tx == 1) {
        SetBits(RF_OVERRULE_REG, TX_EN_OVR, 3);
    }
    else {
        SetBits(RF_OVERRULE_REG, RX_EN_OVR, 3);
    }
    delay_us(80);
}

// adpll rf operation
static void adpll_dis(void)
{
    // manually disable adpll
    SetWord32(RF_OVERRULE_REG, 5);
    delay_us(20);
}

static void adpll_openloop_prepare(void)
{
    adpll_hclk_en();

    SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOCOARSE_SEL, 1);
    SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOMEDIUM_SEL, 1);
    SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOFINE_SEL, 1);
    SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOMOD_SEL, 1);
    SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOCOARSE_WR, 5);
    SetBits(ADPLL_MON_CTRL_REG, ENRFMEAS, 1);

    adpll_en(1);

    delay_us(20);

    SetBits(ADPLL_OVERRULE_CTRL1_REG, OVR_ACTIVE_WR, 1);
    SetBits(ADPLL_OVERRULE_CTRL1_REG, OVR_ACTIVE_SEL, 1);
}

static void adpll_manual_active(uint8_t en)
{
    SetBits(ADPLL_OVERRULE_CTRL1_REG, OVR_ACTIVE_WR, en);
}

static uint32_t adpll_openloop_freq_meas(uint8_t tm, uint16_t meas_id)
{
    uint32_t f;

    adpll_manual_active(0);

    SetBits(ADPLL_MON_CTRL_REG, ENRFMEAS, 1);
    SetBits(ADPLL_MON_CTRL_REG, TFREQMEAS, tm);

    uint16_t tdelay = 1+((((tm&0x3)+4)*(1<<(tm>>2)))>>1)+5;

    adpll_manual_active(1);

    delay_us(tdelay);
    delay_us(100);

    f = GetBits(ADPLL_FREQMEAS_RD_REG, FREQDIFF);

    adpll_manual_active(0);
    return f;
}

static void mma_q31(volatile int32_t * pSrc, 
                    uint32_t blockSize,
                    int32_t * vmin,
                    int32_t * vavg,
                    int32_t * vmax)
{
    // return the min, avg, max of an array
    uint32_t i=0;
    int32_t minval = 0, maxval = 0, sumval = 0, meanval = 0;

    for(i = 0; i < blockSize; i++)
    {
        if((i == 0) || pSrc[i] < minval) minval = pSrc[i];
        if((i == 0) || pSrc[i] > maxval) maxval = pSrc[i];
        sumval += pSrc[i];
    }

    meanval = sumval/blockSize;

    *vmin = minval;
    *vmax = maxval;
    *vavg = meanval;
}

static void adpll_openloop_set_banks(int8_t cc, int8_t cm, int8_t cf, uint8_t cmod)
{
    SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOCOARSE_WR, cc&15);
    SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOMEDIUM_WR, cm&7);
    SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOFINE_WR, cf&63);
    SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOMOD_WR, cmod);
}

static void mdiff_q31(volatile int32_t * pSrcA,
                      volatile int32_t * pSrcB,
                      int32_t * pDst,
                      uint32_t blockSize)
{
    uint32_t i;

    for(i = 0;i < blockSize; i++)
    {
        pDst[i] = pSrcA[i]-pSrcB[i];
    }
}

static void adpll_update_kdco(uint8_t kdco)
{
    SetBits(ADPLL_KDCO_CAL_CTRL1_REG, KDCOHFIC_1M, kdco);
    SetBits(ADPLL_KDCO_CAL_CTRL1_REG, KDCOLF_IN_1M, kdco);
}


/*
    The following registers must be stored before making changes
    for the kdco calibration algorithm and must be restored when 
    the process is finished

    ADPLL_CN_CTRL_REG
    RF_OVERRULE_REG
    RF_ENABLE_CONFIG19_REG

    For the ADPLL_CN_CTRL_REG and RF_ENABLE_CONFIG19_REG the 
    process is straightforward - they are stored before making
    any changes and are restored afterwards. For the RF_OVERRULE_REG
    the situation is more complex as this register controls the adpll
    and some additional delay is needed.
 */

typedef struct 
{
    uint32_t reg;
    volatile uint32_t value;
    uint32_t delay_in_us;
} reg_value_pair_t;

reg_value_pair_t reg_value_pair[3];

void reg_save_values(void)
{
    memset(reg_value_pair,(int)0,sizeof(reg_value_pair));
    reg_value_pair[0].reg = ADPLL_CN_CTRL_REG;
    reg_value_pair[0].value = GetWord32(ADPLL_CN_CTRL_REG);
    reg_value_pair[0].delay_in_us = 0;

    reg_value_pair[1].reg = RF_OVERRULE_REG;
    reg_value_pair[1].value = GetWord32(RF_OVERRULE_REG);
    reg_value_pair[1].delay_in_us = 80;

    reg_value_pair[2].reg = RF_ENABLE_CONFIG19_REG;
    reg_value_pair[2].value = GetWord32(RF_ENABLE_CONFIG19_REG);
    reg_value_pair[2].delay_in_us = 0;
}

void reg_restore_values(void)
{
    for (int i=0;i<sizeof(reg_value_pair)/sizeof(reg_value_pair_t);i++)
    {
        SetWord32(reg_value_pair[i].reg,reg_value_pair[i].value);
        if (reg_value_pair[i].delay_in_us > 0)
        {
            delay_us(reg_value_pair[i].delay_in_us);
        }
    }
}

// This is the ATE kdco calibration function 
// Please note that the radio power domain must be powered for the function to be called 

uint32_t ate_kdco_calibration(bool apply_value)
{
    uint16_t i;
    uint32_t freq;
    int32_t fstep_mean, fstep_max, fstep_min;
    uint32_t temp;

    reg_save_values();

    // Tune to channel 20 
    SetBits(ADPLL_CN_CTRL_REG, CN, 20);

    // The pll should be disabled and enabled 
    adpll_dis();

#if defined(CFG_DISABLE_POWER_AMPLIFIER)
    // The PA may need to be disabled at this point
    SetBits(RF_ENABLE_CONFIG19_REG, PA_EN_DCF_TX, 0);
    SetBits(RF_ENABLE_CONFIG19_REG,PA_EN_DCF_RX, 0);
#endif

    // Enable the pll again 
    adpll_en(1); // 1 will enable the Tx part of the adpll

    // Retrieve the ccoart_at_c, cmedium_at_c and cfine_at_c values
    temp = GetWord32(ADPLL_DCO_RD_REG); //cache the values
    int8_t ccoart_at_c = GetBits32(&temp, DCOCOARSE);
    int8_t cmedium_at_c = GetBits32(&temp, DCOMEDIUM);
    int8_t cfine_at_c = GetBits32(&temp, DCOFINE);

    adpll_openloop_prepare();

    for(i  = 0; i <= 31; i++)
    {
        adpll_openloop_set_banks(ccoart_at_c,cmedium_at_c,cfine_at_c,(31-i)*8);
        delay_us(5);
        freq = adpll_openloop_freq_meas(31, TIME_OUT_CMT)*4000/897;
        CMT_FOUT[i] = freq;
    }

    // test step and mono
    mdiff_q31(CMT_FOUT+1,CMT_FOUT,FSTEP, 31);
    mma_q31(FSTEP, 31, &fstep_min, &fstep_mean, &fstep_max);

    uint32_t ldco = 32000/fstep_mean;
    uint32_t exp_kdco = (ldco>>8)&1;
    uint32_t lin_kdco = (ldco>>exp_kdco)&127;
    KDCO_0 = ((exp_kdco<<7)+lin_kdco+3)&255;

    // At this point the value generated by the kdco algorithm is in KDCO_0
    // The ADPLL_KDCO_CAL_CTRL1_REG must be updated depending on the variable
    if (apply_value)
    {
        adpll_update_kdco((uint8_t)KDCO_0);
    }

    reg_restore_values();

    if (apply_value)
    {
        return GetWord16(ADPLL_KDCO_CAL_CTRL1_REG);
    }
    else
    {
        return ( (KDCO_0<<8) + KDCO_0 );
    }
}
#endif // __DA14531__

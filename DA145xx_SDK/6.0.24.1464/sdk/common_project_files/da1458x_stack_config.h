/**
 ****************************************************************************************
 *
 * @file da1458x_stack_config.h
 *
 * @brief RW stack configuration file.
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

#ifndef DA1458X_STACK_CONFIG_H_
#define DA1458X_STACK_CONFIG_H_

/////////////////////////////////////////////////////////////
/* Do not alter */

#define CFG_EMB
#define CFG_HOST
#define CFG_ALLROLES        1
#define CFG_GTL
#define CFG_BLE
#define CFG_EXT_DB
#if defined (__DA14531__) || defined (__DA14535__)
    #define CFG_CON             3
#else
    #define CFG_CON             8
#endif
#define CFG_SECURITY_ON     1
#define CFG_ATTC
#define CFG_ATTS
#define CFG_PRF
#if defined (__DA14531__) || defined (__DA14535__)
    #define CFG_NB_PRF          10
#else
    #define CFG_NB_PRF          32
#endif
#define CFG_H4TL

#define CFG_CHNL_ASSESS

/* Enable power optimizations */
#define CFG_POWER_OPTIMIZATIONS

/*Misc*/
#define __NO_EMBEDDED_ASM

/****************************************************************************************************************/
/* Use the 580 PLL LUT RF calibration or the 585 VCO coarse RF calibration.                                     */
/* - DEFINE   : 580 PLL LUT RF calibration.                                                                     */
/* - UNDEFINE : 585 VCO coarse RF calibration (must be kept undefined by default).                              */
/****************************************************************************************************************/
#undef CFG_LUT_PATCH

/*
 * Automatically enable RF DIAG interrupt:
 * - DA14585/586: To be enabled for PLL LUT and production test.
 * - DA14531: To be enabled for production test.
 */
#if !defined (__DA14531__)              // DA14585/586
    #if defined (CFG_PRODUCTION_TEST) || defined (CFG_LUT_PATCH)
        #define CFG_RF_DIAG_INT
    #else
        #undef CFG_RF_DIAG_INT
    #endif
#else                                   // DA14531
    #if defined (CFG_PRODUCTION_TEST)
        #define CFG_RF_DIAG_INT
    #else
        #undef CFG_RF_DIAG_INT
    #endif
#endif

#if defined (__FPGA__)
    #define XTAL_32000HZ
#endif

/*Radio interface*/
#if defined (__FPGA__)
    #if defined (__DA14531__)
        #define RADIO_RIPPLE        1
        #define RADIO_EXPOSED       0
        #define RADIO_ANALOG        0
    #else
        #define RADIO_ANALOG        0
        #define RADIO_EXPOSED       0
    #endif

    #define RIPPLE_ID           66
    #define RADIO_585           0
#else
    #define RADIO_ANALOG        0
    #define RADIO_RIPPLE        0
#endif

#if !defined (__DA14531__) && defined (CFG_LUT_PATCH) && !defined (CFG_RF_DIAG_INT)
#error "DA14585/586: If PLL LUT is used the CFG_RF_DIAG_INT flag must be defined"
#endif

#endif // DA1458X_STACK_CONFIG_H_

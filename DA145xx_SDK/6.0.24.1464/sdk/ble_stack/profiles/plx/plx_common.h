/**
 ****************************************************************************************
 *
 * @file plxs_common.h
 *
 * @brief Header file - Pulse Oximeter Service common types.
 *
 * Copyright (C) 2015-2024 Renesas Electronics Corporation and/or its affiliates.
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
 ***************************************************************************************
 */


#ifndef _PLX_COMMON_H_
#define _PLX_COMMON_H_

/**
 ****************************************************************************************
 * @addtogroup PLX Pulse Oximeter Profile
 * @ingroup PROFILE
 * @brief Pulse Oximeter Profile
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_PLX_SERVER)

#include "prf_types.h"

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * ENUMS
 ****************************************************************************************
 */
 
//PLX Features Flags 
enum plxs_feat_flags
{
    PLXS_FEAT_MEASUREMENT_STAT_SUP                  = 0x0001,
    PLXS_FEAT_DEVICE_SENSOR_STAT_SUP                = 0x0002,
    PLXS_FEAT_MEASUREMENT_STOR_SPOT_CHECK_SUP       = 0x0004,
    PLXS_FEAT_TIMESTAMP_SPOT_CHECK_SUP              = 0x0008,
    PLXS_FEAT_SPO2_FAST_METRIC_SUP                  = 0x0010,
    PLXS_FEAT_SPO2_SLOW_METRIC_SUP                  = 0x0020,
    PLXS_FEAT_PULSE_AMPL_INDEX_SUP                  = 0x0040,
    PLXS_FEAT_MULTI_BOND_SUP                        = 0x0080,
};

//PLX Measurement Status Support Flags
enum plxs_meas_stat_flags
{
    PLXS_MEAS_STAT_MEAS_ONGOING                     = 0x0020,
    PLXS_MEAS_STAT_EARLY_EST_DATA                   = 0x0040,
    PLXS_MEAS_STAT_VALIDATED_DATA                   = 0x0080,
    PLXS_MEAS_STAT_FULLY_QUAL_DATA                  = 0x0100,
    PLXS_MEAS_STAT_DATA_FROM_MEAS_STOR              = 0x0200,
    PLXS_MEAS_STAT_DATA_FOR_DEMO                    = 0x0400,
    PLXS_MEAS_STAT_DATA_FOR_TEST                    = 0x0800,
    PLXS_MEAS_STAT_CALIBR_ONGOING                   = 0x1000,
    PLXS_MEAS_STAT_MEAS_UNAVAIL                     = 0x2000,
    PLXS_MEAS_STAT_QUESTIONABLE_MEAS_DET            = 0x4000,
    PLXS_MEAS_STAT_INVALID_MEAS_DET                 = 0x8000,
    PLXS_MEAS_STAT_ALL_FIELDS_SET                   = 0xFFE0,
};

//PLXS Device and Sensor Status Support Flags
enum plxs_dev_sens_stat_flag
{
    PLXS_DEV_SENS_STAT_EXT_DISP_UPD_ONGOING         = 0x000001,
    PLXS_DEV_SENS_STAT_EQUIP_MALFUNC_DET            = 0x000002,
    PLXS_DEV_SENS_STAT_SIGNAL_PROC_IRREG_DET        = 0x000004,
    PLXS_DEV_SENS_STAT_INADEQ_SIGNAL_DET            = 0x000008,
    PLXS_DEV_SENS_STAT_POOR_SIGNAL_DET              = 0x000010,
    PLXS_DEV_SENS_STAT_LOW_PERFUSION_DET            = 0x000020,
    PLXS_DEV_SENS_STAT_ERRATIC_SIGNAL_DET           = 0x000040,
    PLXS_DEV_SENS_STAT_NONPULS_SIGNAL_DET           = 0x000080,
    PLXS_DEV_SENS_STAT_QUESTIONABLE_PULSE_DET       = 0x000100,
    PLXS_DEV_SENS_STAT_SIGNAL_ANALYSIS_ONGOING      = 0x000200,
    PLXS_DEV_SENS_STAT_SENSOR_INTERF_DET            = 0x000400,
    PLXS_DEV_SENS_STAT_SENSOR_UNCONN_TO_USER        = 0x000800,
    PLXS_DEV_SENS_STAT_UNKNOWN_SENSOR_CONN          = 0x001000,
    PLXS_DEV_SENS_STAT_SENSOR_DISPLACED             = 0x002000,
    PLXS_DEV_SENS_STAT_SENSOR_MALFUNC               = 0x004000,
    PLXS_DEV_SENS_STAT_SENSOR_DISCONN               = 0x008000,
    PLXS_DEV_SENS_STAT_ALL_FIELDS_SET               = 0x00FFFF,
};

///PLX Spot-Check Measurement Characteristic Flags
enum plxs_spot_meas_flags
{
    PLXS_SPOT_MEAS_FLAGS_TIMESTAMP_PRESENT      =   0x01,
    PLXS_SPOT_MEAS_FLAGS_MEAS_STAT_PRESENT      =   0x02,
    PLXS_SPOT_MEAS_FLAGS_DEV_SENS_STAT_PRESENT  =   0x04,
    PLXS_SPOT_MEAS_FLAGS_PA_IDX_PRESENT         =   0x08,
    PLXS_SPOT_MEAS_FLAGS_DEV_CLOCK_NOT_SET      =   0x10,
    PLXS_SPOT_MEAS_FLAGS_ALL_SET                =   0x1F,
};

///PLX Continuous Measurement Characteristic Flags
enum plxs_cont_meas_flags
{
    PLXS_CONT_MEAS_FLAGS_FAST_FIELD_PRESENT     =   0x01,
    PLXS_CONT_MEAS_FLAGS_SLOW_FIELD_PRESENT     =   0x02,
    PLXS_CONT_MEAS_FLAGS_MEAS_STAT_PRESENT      =   0x04,
    PLXS_CONT_MEAS_FLAGS_DEV_SENS_STAT_PRESENT  =   0x08,
    PLXS_CONT_MEAS_FLAGS_PA_IDX_FIELD_PRESENT   =   0x10,
    PLXS_CONT_MEAS_FLAGS_ALL_SET                =   0x1F,
};


// /*
// * STRUCTURES
// ****************************************************************************************
// */

///PLX Spot-Check Measurement Characteristic Fields
struct plx_spot_check_meas
{
    uint8_t flags;
    prf_sfloat sp02;
    prf_sfloat pr;
    struct prf_date_time time_stamp;
    uint16_t meas_stat;
    uint32_t dev_sens_stat;
    prf_sfloat pa_idx;
};

///PLX Continuous Measurement Characteristic Fields
struct plx_cont_meas
{
    uint8_t flags;
    struct sp02pr_normal
    {
        prf_sfloat sp02;
        prf_sfloat pr;
    } sp02pr_normal;
    struct sp02pr_fast
    {
        prf_sfloat sp02;
        prf_sfloat pr;
    } sp02pr_fast;
    struct sp02pr_slow
    {
        prf_sfloat sp02;
        prf_sfloat pr;
    } sp02pr_slow;
    uint16_t meas_stat;
    uint32_t dev_sens_stat;
    prf_sfloat pa_idx;
};

#endif /* #if (BLE_PLX_SERVER) */

/// @} plx_common

#endif /* _PLX_COMMON_H_ */

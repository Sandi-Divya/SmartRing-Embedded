/**
 ****************************************************************************************
 *
 * @file wss_common.h
 *
 * @brief Header file - Weight Scale Service common types.
 *
 * Copyright (C) 2017-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef WSS_COMMON_H_
#define WSS_COMMON_H_
/**
 ************************************************************************************
 * @addtogroup WSSS Weight Scale Service Collector
 * @ingroup WSS
 * @brief Weight Scale Service Collector
 * @{
 ************************************************************************************
 */

/*
 * INCLUDE FILES
 ************************************************************************************
 */

#if (BLE_WSS_SERVER || BLE_WSS_COLLECTOR)

#include "prf_types.h"


/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Weight Measurement Flags field bit values
enum
{
    /// Imperial Measurement Units (weight in lb and height in inches)
    WSS_MEAS_FLAG_UNIT_IMPERIAL      = 0x01,
    /// Time Stamp present
    WSS_MEAS_FLAG_TIME_STAMP         = 0x02,
    /// User ID present
    WSS_MEAS_FLAG_USERID_PRESENT     = 0x04,
    /// BMI & Height present
    WSS_MEAS_FLAG_BMI_HT_PRESENT     = 0x08,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Weight measurement structure - shall be dynamically allocated
struct wss_wt_meas
{
    /// Flags
    uint8_t flags;
    /// Weight
    float weight;
    /// Time stamp
    struct prf_date_time datetime;
    /// User ID
    uint8_t userid;
    /// BMI
    float bmi;
    /// Height
    float height;
};

#endif
/// @} wss_common

#endif /* WSS_COMMON_H_ */

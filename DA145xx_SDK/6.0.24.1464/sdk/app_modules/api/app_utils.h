/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Utils
 * @brief Application utilities API
 * @{
 *
 * @file app_utils.h
 *
 * @brief Application generic utilities helper functions header file.
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

#ifndef _APP_UTILS_H_
#define _APP_UTILS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "rwip_config.h"

#if defined (__DA14531__)
#include "otp_hdr.h"
#include "otp_cs.h"
#endif

/*
 * DEFINITIONS
 ****************************************************************************************
 */

#if defined (__DA14531__)
// Translate macro index to otp address
#define OTP_CS_IDX_2_ADDR(idx)      (OTP_CS_BASE_ADDR + (idx << 2))

#if defined (__DA14535__)
// Trimming value indexes
#define PD_RAD_VAL_1        12
#define PD_RAD_VAL_2        14
#define PD_ADPLL_VAL_1      21
#define PD_ADPLL_VAL_2      23
#define PD_ADPLL_VAL_3      25
#define PD_ADPLL_VAL_4      27
#define GP_TEMP_VAL         35
#define GP_ADC_SINGLE_VAL   29
#define GP_ADC_DIFF_VAL     31
#else
// Trimming value indexes
#define PD_RAD_VAL_1        9
#define PD_RAD_VAL_2        11
#define PD_ADPLL_VAL_1      14
#define PD_ADPLL_VAL_2      16
#define PD_ADPLL_VAL_3      18
#define PD_ADPLL_VAL_4      20
#define GP_TEMP_VAL         22
#define GP_ADC_SINGLE_VAL   24
#define GP_ADC_DIFF_VAL     26
#endif
#else
#define OTP_TIMESTAMP_ADDR              0x07F8FEC8
#define OTP_SITE_ID_ADDR                0x07F8FED0
#define OTP_LNA_ADDR                    0x07F8FEF8
#define OTP_BANDGAP_ADDR                0x07F8FF08
#define OTP_CRC_TRIM_CAL_ADDR           0x07F8FE18
#define OTP_OTP_CONTROL_ADDR            0x07F8FEB0
#define OTP_CALIBRATION_FLAGS_ADDR      0x07F8FEF0
#define OTP_TRIM_RFIO_CAP_ADDR          0x07F8FF00
#endif

#if defined (__DA14531__)
#define OTP_PARAM_VAL_1     OTP_HDR_TESTER_ADDR
#define OTP_PARAM_VAL_2     OTP_HDR_TIMESTAMP_ADDR
#define OTP_PARAM_VAL_3     OTP_CS_IDX_2_ADDR(PD_RAD_VAL_1)
#define OTP_PARAM_VAL_4     OTP_CS_IDX_2_ADDR(PD_RAD_VAL_2)
#define OTP_PARAM_VAL_5     OTP_CS_IDX_2_ADDR(PD_ADPLL_VAL_1)
#define OTP_PARAM_VAL_6     OTP_CS_IDX_2_ADDR(PD_ADPLL_VAL_2)
#define OTP_PARAM_VAL_7     OTP_CS_IDX_2_ADDR(PD_ADPLL_VAL_3)
#define OTP_PARAM_VAL_8     OTP_CS_IDX_2_ADDR(PD_ADPLL_VAL_4)
#else
#define OTP_PARAM_VAL_1     (OTP_TIMESTAMP_ADDR)
#define OTP_PARAM_VAL_2     (OTP_SITE_ID_ADDR)
#define OTP_PARAM_VAL_3     (OTP_LNA_ADDR)
#define OTP_PARAM_VAL_4     (OTP_BANDGAP_ADDR)
#define OTP_PARAM_VAL_5     (OTP_CRC_TRIM_CAL_ADDR)
#define OTP_PARAM_VAL_6     (OTP_OTP_CONTROL_ADDR)
#define OTP_PARAM_VAL_7     (OTP_CALIBRATION_FLAGS_ADDR)
#define OTP_PARAM_VAL_8     (OTP_TRIM_RFIO_CAP_ADDR)
#endif

#define MAX_STATIC_KEYS_LEN_W           4
#define OTP_SEED(x)                     OTP_PARAM_VAL_##x
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef struct {
    uint32_t            addr_arr[MAX_STATIC_KEYS_LEN_W];
    uint8_t             arr_size;
} static_key_t;

/// Address types
enum app_addr_types
{
    /// Public address
    APP_PUBLIC_ADDR_TYPE,
    /// Random static address
    APP_RANDOM_STATIC_ADDR_TYPE,
    /// Resolvable private address
    APP_RANDOM_PRIVATE_RESOLV_ADDR_TYPE,
    /// Non-resolvable private address
    APP_RANDOM_PRIVATE_NONRESOLV_ADDR_TYPE,
    /// Address resolved by the Controller
    APP_ID_ADDR_TYPE,
    /// Unknown address type
    APP_ERROR_ADDR_TYPE,
};

/// Resolving address list operations
enum app_ral_operations
{
    /// Get resolving address list size
    APP_GET_RAL_SIZE = GAPM_GET_RAL_SIZE,
    /// Get resolving local address
    APP_GET_RAL_LOC_ADDR = GAPM_GET_RAL_LOC_ADDR,
    /// Get resolving peer address
    APP_GET_RAL_PEER_ADDR = GAPM_GET_RAL_PEER_ADDR,
    /// Add device in resolving address list
    APP_ADD_DEV_IN_RAL = GAPM_ADD_DEV_IN_RAL,
    /// Remove device from resolving address list
    APP_RMV_DEV_FRM_RAL = GAPM_RMV_DEV_FRM_RAL,
    /// Clear resolving address list
    APP_CLEAR_RAL = GAPM_CLEAR_RAL,
    /// Set Network Privacy Mode for peer in resolving list
    APP_NETWORK_MODE_RAL = GAPM_NETWORK_MODE_RAL,
    /// Set Network Privacy Mode for peer in resolving list
    APP_DEVICE_MODE_RAL = GAPM_DEVICE_MODE_RAL,
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Find the type of Bluetooth Device Address
 * @param[in] addr_type PUBLIC or RANDOM type of BDA
 * @param[in] addr      Bluetooth Device Address
 * @return              Type of Bluetooth Device Address
 ****************************************************************************************
 */
enum app_addr_types app_get_address_type(uint8_t addr_type, struct bd_addr addr);

/**
 ****************************************************************************************
 * @brief Fills an array with random bytes (starting from the end of the array)
 *          Assuming an array of array_size and a dynamic key size, so that key_size = M*4+N:
 *          Calls co_rand_word() M times and appends the 4 bytes of each 32 bit return value
 *          to the output array
 *          Calls co_rand_word() once and appends the N most significant bytes of the 32 bit
 *          return value to the output array
 *          Fills the rest bytes of the array with zeroes
 *
 * @param[in] *dst              The address of the array
 * @param[in] key_size          Number of bytes that shall be randomized
 * @param[in] array_size        Total size of the array
 ****************************************************************************************
 */
void app_fill_random_byte_array(uint8_t *dst, uint8_t key_size, uint8_t array_size);

/**
 ****************************************************************************************
 * @brief Generate a static key
 *
 * @param[in] otp_seed          Pointer to the static_key_t struct holding the otp address
 * @param[in,out] key           Pointer to the output array
 * @param[in] key_len           size of the output array
 * @return                      size of the key created in bytes 
 ****************************************************************************************
 */
uint8_t app_generate_static_key(static_key_t *otp_seed, uint8_t *key, uint8_t key_len);

#endif // _APP_UTILS_H_

///@}
///@}

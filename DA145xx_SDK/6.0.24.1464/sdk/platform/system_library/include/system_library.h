/**
 ****************************************************************************************
 *
 * @file system_library.h
 *
 * @brief System library header file.
 *
 * Copyright (C) 2012-2024 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _SYSTEM_LIBRARY_H_
#define _SYSTEM_LIBRARY_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>

/**
 ****************************************************************************************
 * @addtogroup SYSTEM_LIBRARY
 * @brief System Library
 * @{
 ****************************************************************************************
 */

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// ROM Patch definitions
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/*
 * DEFINES
 ****************************************************************************************
 */

#if defined (__DA14531__)
#if defined (__DA14535__)
// RF LDO CALIBRATION is disabled in DA14535
#else
#define CFG_RF_LDO_CALIBRATION
#endif
#endif

#define RC16_CAL_REF_CYCLES                 (0x400)
    
#define RF_DIAG_IRQ_MODE_RX_RISING          (0x01)
#define RF_DIAG_IRQ_MODE_TX_FALLING         (0x02)
#define RF_DIAG_IRQ_MODE_RX_FALLING         (0x04)
#define RF_DIAG_IRQ_MODE_TX_RISING          (0x08)

#define RF_DIAG_IRQ_MODE_LDO_CALIBRATION    (RF_DIAG_IRQ_MODE_RX_RISING | RF_DIAG_IRQ_MODE_RX_FALLING)
#define RF_DIAG_IRQ_MODE_ALL                (RF_DIAG_IRQ_MODE_RX_FALLING | RF_DIAG_IRQ_MODE_TX_FALLING | RF_DIAG_IRQ_MODE_RX_RISING | RF_DIAG_IRQ_MODE_TX_RISING)

#if defined(CFG_RF_LDO_CALIBRATION) && !defined (CFG_PRODUCTION_TEST)
#define RF_DIAG_IRQ_MODE_RXTX           (RF_DIAG_IRQ_MODE_LDO_CALIBRATION)
#else
#define RF_DIAG_IRQ_MODE_RXTX           (RF_DIAG_IRQ_MODE_ALL)
#endif

/****************************************************************************************************************/
/* Function patch for SMP timeout timer which is not started upon the transmission of the Security Request      */
/* command.                                                                                                     */
/*                                                                                                              */
/* Macro:                                                                                                       */
/* CFG_START_SMP_TIMEOUT_TIMER_UPON_TX_SEC_REQ_CMD                                                              */
/*                                                                                                              */
/* Description: Starts an SMP timeout timer upon the transmission of the Security Request command.              */
/*              As it is stated in Core Specification v5.0 and later, to protect the Security Manager protocol  */
/*              from stalling, a Security Manager Timer is used. Upon transmission of the Security Request      */
/*              command or reception of the Security Request command, the Security Manager Timer shall be reset */
/*              and restarted. Upon transmission of the Pairing Request command or reception of the Pairing     */
/*              Request command, the Security Manager Timer shall be reset and started.                         */
/* Note: Already fixed in DA14531-01, DA14535, DA14533 ROMs. The fix is not currently available for DA14531.    */
/****************************************************************************************************************/

/****************************************************************************************************************/
/* Function patch for skip slave latency.                                                                       */
/*                                                                                                              */
/* Macro:                                                                                                       */
/* CFG_SKIP_SLAVE_LATENCY                                                                                       */
/*                                                                                                              */
/* Description: Allow the Slave to skip slave latency and send packets at the next connection event.            */
/*              To skip slave latency in the slave and listen to a packet from master without any               */
/*              delay the following API must be used:                                                           */
/*              - Use ble_skip_slave_latency_set(uint8_t conidx, bool enable) to enable/disable skip slave      */
/*                latency for the specified connection.                                                         */
/*              - Use ble_skip_slave_latency_set_for_all_connections(bool enable) to enable/disable skip slave  */
/*                latency for all connections.                                                                  */
/*              - Use ble_skip_slave_latency_get(uint8_t conidx) to get the status of the feature for the       */
/*                specified connection.                                                                         */
/****************************************************************************************************************/

/****************************************************************************************************************/
/* Function patch for heap logging.                                                                             */
/*                                                                                                              */
/* Macro:                                                                                                       */
/* CFG_USE_HEAP_LOG                                                                                             */
/*                                                                                                              */
/* Description: Enables the logging of heap memories usage. The feature can be used in development/debug mode.  */
/*              Application must be executed in Keil debugger environment. Developer must stop execution and    */
/*              type disp_heaplog() in debugger's command window. Heap memory statistics will be displayed on   */
/*              window.                                                                                         */
/****************************************************************************************************************/

/****************************************************************************************************************/
/* Data patch 1.                                                                                                */
/*                                                                                                              */
/* Macro:                                                                                                       */
/* CFG_USE_DATA_PATCH_1                                                                                         */
/*                                                                                                              */
/* Description:                                                                                                 */
/****************************************************************************************************************/

/****************************************************************************************************************/
/* Data patch 2.                                                                                                */
/*                                                                                                              */
/* Macro:                                                                                                       */
/* CFG_USE_DATA_PATCH_2                                                                                         */
/*                                                                                                              */
/* Description:                                                                                                 */
/****************************************************************************************************************/

#if defined (__DA14535__)

#undef CFG_SKIP_SLAVE_LATENCY
#undef CFG_USE_HEAP_LOG
#undef CFG_USE_DATA_PATCH_1
#undef CFG_USE_DATA_PATCH_2

#elif defined (__DA14531_01__)

#undef CFG_SKIP_SLAVE_LATENCY
#undef CFG_USE_HEAP_LOG
#undef CFG_USE_DATA_PATCH_1
#undef CFG_USE_DATA_PATCH_2

#elif defined (__DA14531__)

#undef CFG_SKIP_SLAVE_LATENCY
#undef CFG_USE_HEAP_LOG
#define CFG_USE_DATA_PATCH_1
#undef CFG_USE_DATA_PATCH_2

#else

#define CFG_START_SMP_TIMEOUT_TIMER_UPON_TX_SEC_REQ_CMD
#undef CFG_SKIP_SLAVE_LATENCY
#undef CFG_USE_HEAP_LOG
#define CFG_USE_DATA_PATCH_1
#undef CFG_USE_DATA_PATCH_2

#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief ROM patch function.
 ****************************************************************************************
 */
void patch_func(void);

/**
 ****************************************************************************************
 * @brief ROM patch function for 1st ROM data patch.
 ****************************************************************************************
 */
void patch_data1(void);

 /**
 ****************************************************************************************
 * @brief ROM patch function for 2st ROM data patch.
 ****************************************************************************************
 */
void patch_data2(void);

/**
 ****************************************************************************************
 * @brief Initializes global variables used by the ROM patched functions.
 ****************************************************************************************
 */
void patch_global_vars_init(void);

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// RF Calibration utilities
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void enable_rf_diag_irq(uint8_t mode);

void set_gauss_modgain(uint16_t cn);

bool IffCalibration_580(void);
bool DCoffsetCalibration_580(void);
bool rf_calibration_580(void);

bool rf_calibration_585(void);
extern void PllLF_IFF_MIX_CapCalibration(void); // defined in ROM
extern void DCoffsetCalibration(void);          // defined in ROM

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// PLL LUT utilities (used by the old 580 calibration method)
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/*
 * DEFINES
 ****************************************************************************************
 */

// Visible to the user
#define LUT_INIT        (0)
#define LUT_UPDATE      (1)

struct LUT_CFG_struct
{
    int8_t      HW_LUT_MODE;              // 1: HW LUT mode, 0: SW LUT mode using ISR
    int8_t      RX_HSI_ENABLED;           // 1: HSI mode, 0:LSI mode
    int8_t      PA_PULLING_OFFSET;        // in channel numbers
    int8_t      NR_CCUPD_1ST;             // max number of real time iterations during the first min_cn calcap update routine
    int8_t      NR_CCUPD_REST;            // max number of real time iterations during the rest min_cn / max_cn updates
    int8_t      NR_CCUPD_OL;              // Extra number of iterations during the LUT INIT (non real time mode). Should be less than 126 in total
    int8_t      BLE_BAND_MARGIN;          // Extention of BLE band in MHz, to make sure that we keep updating enough number of calcaps
    int8_t      EST_HALF_OVERLAP;         // Estimated half overlap between successive calcaps, in MHz
    int8_t      REQUIRED_CHAN_OVERLAP;    // Min required overlap in channels
    uint8_t     PLL_LOCK_VTUNE_NUMAVGPOW;
    int16_t     PLL_LOCK_VTUNE_LIMIT_LO;
    int16_t     PLL_LOCK_VTUNE_LIMIT_HI;
    int16_t     PLL_LOCK_VTUNE_P2PVAR;
    int16_t     PLL_LOCK_TIMING;
    uint16_t    VCO_CALCNT_STARTVAL;
    uint16_t    VCO_CALCNT_TIMEOUT;
};

extern const struct LUT_CFG_struct LUT_CFG;

extern volatile uint16_t vcocal_ctrl_reg_val;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

uint8_t pll_vcocal_LUT_InitUpdate(uint8_t mode);

void set_rf_cal_cap(uint16_t cn); // This is used for overrule based testing

/**
 ****************************************************************************************
 * @brief Verify CRC of OTP calibration values and apply them to the device.
 * @return true on success else false.
 * @note This function assumes that the system clock frequency is 16MHz.
 ****************************************************************************************
 */
bool otp_verify_crc(void);

/**
 ****************************************************************************************
 * @brief End of event/frame interrupt handler. This function replaces lld_evt_end_isr()
 *        to add skip slave latency feature. It is called under interrupt when
 *        an end of event/frame interrupt is generated by the BLE/BT core.
 * @param[in] apfm  - Indicate if end of event is due to an apfm interrupt
 * @note This function is called from SDK when skip slave latency feature is enabled.
 ****************************************************************************************
 */
void lld_evt_end_isr_sdk(bool apfm);

/**
 ****************************************************************************************
 * @brief Set skip slave latency state. If skip slave latency is enabled the
 *        device appears in every connection event.
 * @param[in] conidx The connection index
 * @param[in] enable Enable skip slave latency if true, disable it otherwise
 * @return true if successful, false otherwise
 * @note This function is used when skip slave latency feature is enabled.
 ****************************************************************************************
 */
bool ble_skip_slave_latency_set(uint8_t conidx, bool enable);

/**
 ****************************************************************************************
 * @brief Set skip slave latency state for all connections. If skip slave
 *        latency is enabled the device appears in every connection event.
 * @param[in] enable Enable skip slave latency if true, disable it otherwise
 * @note This function is used when skip slave latency feature is enabled.
 ****************************************************************************************
 */
void ble_skip_slave_latency_set_for_all_connections(bool enable);

/**
 ****************************************************************************************
 * @brief Get skip slave latency state
 * @param[in] conidx The connection index
 * @return true if slave latency is enabled, false otherwise
 * @note This function is used when skip slave latency feature is enabled.
 ****************************************************************************************
 */
bool ble_skip_slave_latency_get(uint8_t conidx);

/**
 *****************************************************************************************
 * @brief RF LDO calibration.
 *****************************************************************************************
 */
void rf_ldo_calibration(void);

/// @} SYSTEM_LIBRARY

#endif // _SYSTEM_LIBRARY_H_

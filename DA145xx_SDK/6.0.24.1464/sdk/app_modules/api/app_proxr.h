/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup PROXR
 * @brief Proximity Profile Reporter Application API
 * @{
 *
 * @file app_proxr.h
 *
 * @brief Proximity Reporter application.
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

#ifndef _APP_PROXR_H_
#define _APP_PROXR_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_PROX_REPORTER)
#include "gpio.h"
#include "proxr.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// APP alert state
typedef struct
{
    /// Blink timeout
    uint32_t     blink_timeout;

    /// Blink toggle
    uint8_t      blink_toggle;

    /// Level
    uint8_t      lvl;
 
    /// Alert level
    uint8_t      ll_alert_lvl;

    /// TX power level
    int8_t       txp_lvl;

    /// GPIO port
    GPIO_PORT    port;

    /// GPIO pin
    GPIO_PIN     pin;
} app_alert_state;

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// PROXR APP callbacks
struct app_proxr_cb
{
    /// Callback upon 'alert_ind'
    void (*on_proxr_alert_ind)(uint8_t conidx, uint8_t alert_lvl, uint8_t char_code);
};

/*
 * EXTERNAL VARIABLES DECLARATION
 ****************************************************************************************
 */

/// Alert state
extern app_alert_state alert_state;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Proximity Reporter application.
 ****************************************************************************************
 */
void app_proxr_init(void);

/**
 ****************************************************************************************
 * @brief Create proximity reporter Database.
 ****************************************************************************************
 */
void app_proxr_create_db(void);

/**
 ****************************************************************************************
 * @brief Start proximity Alert.
 * @param[in] lvl               Alert level. Mild or High
 ****************************************************************************************
 */
void app_proxr_alert_start(uint8_t lvl);

/**
 ****************************************************************************************
 * @brief Stop proximity Alert.
 ****************************************************************************************
 */
void app_proxr_alert_stop(void);

/**
 ****************************************************************************************
 * @brief Reinit of proximity reporter LED pins and push button. Called by periph_init().
 * param[in] port               GPIO port
 * param[in] pin                GPIO pin
 ****************************************************************************************
 */
void app_proxr_port_reinit(GPIO_PORT port, GPIO_PIN pin);

/**
 ****************************************************************************************
 * @brief Default application handler for handling a request to start an alert on the
 *        device.
 * param[in] conidx             Connection index
 * param[in] alert_lvl          Alert level
 * param[in] char_code          Char Code - Indicate if LLS or IAS
 ****************************************************************************************
 */
void default_proxr_alert_ind_handler(uint8_t conidx, uint8_t alert_lvl, uint8_t char_code);

#endif // (BLE_PROX_REPORTER)

#endif // _APP_PROXR_H_

///@}
///@}
///@}

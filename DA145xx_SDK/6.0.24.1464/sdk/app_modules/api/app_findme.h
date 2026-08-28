/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup FMP
 * @brief Find Me Profile Application API
 * @{
 *
 * @file app_findme.h
 *
 * @brief Findme Target/Locator application.
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

#ifndef _APP_FINDME_H_
#define _APP_FINDME_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdbool.h>
#include "user_profiles_config.h"

#if (BLE_FINDME_TARGET)
#include "gpio.h"
#include "findt_task.h"
#endif

#if (BLE_FINDME_LOCATOR)
#include "findl_task.h"
#endif

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

#if (BLE_FINDME_TARGET) || (BLE_FINDME_LOCATOR)
/// Find Me APP callbacks
struct app_findt_cb
{
#if (BLE_FINDME_TARGET)
    /// Callback upon 'findt_alert_ind'
    void (*on_findt_alert_ind)(uint8_t conidx, uint8_t alert_lvl);
#endif

#if (BLE_FINDME_LOCATOR)
    /// Callback upon 'findl_enable_rsp'
    void (*on_findl_enable_rsp)(struct findl_enable_rsp const *param);

    /// Callback upon 'findl_set_alert_rsp'
    void (*on_findl_set_alert_rsp)(uint8_t status);
#endif
};
#endif //(BLE_FINDME_TARGET) || (BLE_FINDME_LOCATOR)

#if (BLE_FINDME_TARGET)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// APP alert state
typedef struct
{
    /// Blink timeout
    uint32_t blink_timeout;

    /// Blink toggle
    uint8_t blink_toggle;

    /// Level
    uint8_t lvl;

    /// GPIO port
    GPIO_PORT port;

    /// GPIO pin
    GPIO_PIN pin;
} app_alert_state;

/// Alert state
extern app_alert_state alert_state;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Findme Target profile.
 ****************************************************************************************
 */
void app_findt_init(void);

/**
 ****************************************************************************************
 * @brief Create the Findme Target task.
 ****************************************************************************************
 */
void app_findt_create_db(void);

/**
 ****************************************************************************************
 * @brief Start the FindMe Target alert logic.
 * @param[in] lvl       The alert level. The valid values are:
 *                          FINDL_ALERT_NONE
 *                          FINDL_ALERT_MILD
 *                          FINDL_ALERT_HIGH
 ****************************************************************************************
 */
void app_findt_alert_start(uint8_t lvl);

/**
 ****************************************************************************************
 * @brief Stop the FindMe Target alert logic.
 ****************************************************************************************
 */
void app_findt_alert_stop(void);

/**
 ****************************************************************************************
 * @brief Default Findme Target alert indication handler.
 * @param[in] conidx            The connection id number
 * @param[in] lvl               The alert level
 ****************************************************************************************
 */
void default_findt_alert_ind_handler(uint8_t conidx, uint8_t lvl);
#endif // BLE_FINDME_TARGET

#if (BLE_FINDME_LOCATOR)
/**
 ****************************************************************************************
 * @brief Initialize Findme Locator profile.
 ****************************************************************************************
 */
void app_findl_init(void);

/**
 ****************************************************************************************
 * @brief Create the Findme Locator task.
 ****************************************************************************************
 */
void app_findl_create_task(void);

/**
 ****************************************************************************************
 * @brief Enable Findme Locator profile.
 * @param[in] conidx            The connection id number
 ****************************************************************************************
 */
void app_findl_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Set alert level.
 * @param[in] lvl       The alert level. The valid values are:
 *                          FINDL_ALERT_NONE
 *                          FINDL_ALERT_MILD
 *                          FINDL_ALERT_HIGH
 ****************************************************************************************
 */
void app_findl_set_alert(uint8_t lvl);
#endif // BLE_FINDME_LOCATOR

#endif // _APP_FINDME_H_

///@}
///@}
///@}

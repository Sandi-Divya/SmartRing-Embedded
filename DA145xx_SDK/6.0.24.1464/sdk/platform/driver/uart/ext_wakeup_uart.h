/**
 ****************************************************************************************
 * @addtogroup Drivers
 * @{
 * @addtogroup UART
 * @brief UART driver API
 * @{
 *
 * @file ext_wakeup_uart.h
 *
 * @brief External wake up via 4W-UART for external processor applications for 
 * DA14585/586/531/535 devices.
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

#ifndef _EXT_WAKEUP_UART_H_
#define _EXT_WAKEUP_UART_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>
#include <string.h>
 
 /*
 * DEFINES
 *****************************************************************************************
 */
 #if ((UART_FLOW_CTRL_WAKEUP) && (!BLE_APP_PRESENT)) || (WKUP_EXT_PROCESSOR)
 #define WAKEUP_RTS_LEN_US          50
 #endif
 
 /*
 * TYPE DEFINITIONS
 *****************************************************************************************
 */
#if ((UART_FLOW_CTRL_WAKEUP) && (!BLE_APP_PRESENT))
/// UART TX RX Channel
struct uart_txrxchannel
{
    /// size
    uint32_t  size;
    /// buffer pointer
    uint8_t  *bufptr;
    /// call back function pointer
    void (*callback) (uint8_t);
};

/// UART environment structure
struct uart_env_tag
{
    /// tx channel
    struct uart_txrxchannel tx;
    /// rx channel
    struct uart_txrxchannel rx;
    /// error detect
    uint8_t errordetect;
    //flow off retries
    uint32_t flow_off_retries;
    //flow off retries_limit
    uint32_t flow_off_retries_limit;
};
#endif

typedef struct {
#if ((UART_FLOW_CTRL_WAKEUP) && (!BLE_APP_PRESENT))
    // port for DA14531 wake up
    uint8_t cts_uart_port;
    // pin for DA14531 wake up
    uint8_t cts_uart_pin;
#endif
    // port for external processor wake up
    uint8_t rts_uart_port;
    // pin for external processor wake up
    uint8_t rts_uart_pin;
}ext_wkup_uart_pins_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if ((UART_FLOW_CTRL_WAKEUP) && (!BLE_APP_PRESENT))

/**
 ****************************************************************************************
 * @brief Initialize the wake up pins used for uart bidirectional wake up.
 * @param[in] wkup_pins    set UART RTS (wakeup the processor) and CTS (DA145xx wake up)
 ****************************************************************************************
 */
void ext_wakeup_uart_init(ext_wkup_uart_pins_t wkup_pins);

/**
 ****************************************************************************************
 * @brief Get the configured wakeup pins.
 * @return configured external wake up pins
 ****************************************************************************************
 */
ext_wkup_uart_pins_t* ext_uart_get_wakeup_pins(void);

/**
 ****************************************************************************************
 * @brief Enable the wake up controller on the initialized pins.
 ****************************************************************************************
 */
void ext_wakeup_uart_enable(void);

/**
 ****************************************************************************************
 * @brief Disable the wake up controller.
 ****************************************************************************************
 */
void ext_wakeup_uart_disable(void);

#endif

#endif /* _EXT_WAKEUP_UART_H_ */

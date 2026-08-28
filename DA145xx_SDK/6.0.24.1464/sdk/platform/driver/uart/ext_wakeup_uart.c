/**
 ****************************************************************************************
 *
 * @file ext_wakeup_uart.c
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>
#include <string.h>
#include "compiler.h"
#include "uart.h"
#include "ext_wakeup_uart.h"

#if (UART_FLOW_CTRL_WAKEUP)
#include "wkupct_quadec.h"
#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
#if ((UART_FLOW_CTRL_WAKEUP) && (!BLE_APP_PRESENT)) || (WKUP_EXT_PROCESSOR)
static bool ext_wake_up_req                     __SECTION_ZERO("retention_mem_area0");
static ext_wkup_uart_pins_t wkup_pins_conf      __SECTION_ZERO("retention_mem_area0");
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
extern bool _uart_flow_off_func(bool afce_en);
extern void uart_flow_on_func(void);
extern void uart_write_func(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));
extern void uart_init_func(uint16_t baudr,uint8_t dlf_value, uint8_t mode );

#if ((UART_FLOW_CTRL_WAKEUP) && (!BLE_APP_PRESENT)) || (WKUP_EXT_PROCESSOR)

/**
 ****************************************************************************************
 * @brief Execute the wake up sequence for waking up the external host.
 ****************************************************************************************
 */
static void ext_wakeup_sequence(void)
{
    // Make sure that the external host is triggered via toggling again the RTSN pin if RTS allready low
    if(!GPIO_GetPinStatus(wkup_pins_conf.rts_uart_port, wkup_pins_conf.rts_uart_pin))
    {
        SetWord32(UART_MCR_REG, UART_AFCE);     // Set UART_RTS to 0 to de-assert the RTS
        arch_asm_delay_us(WAKEUP_RTS_LEN_US);
    }
    uart_flow_on_func();                        // Re-enable the AFCE and set UART_RTS to high to assert the RTS
}

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief uart write function for external wake up flow control mechanism.
 * @note overrides the standard uart_write_func in the jump table.
 ****************************************************************************************
 */
void ext_wakeup_uart_write_func(uint8_t *bufptr, uint32_t size,void (*callback) (uint8_t))
{
    /* When uart is initialized the AFCE remains disabled, the AFCE is enabled when flow_on is invoked */
    /* UART is enabled during the periph_init in every wake up from the resume from sleep hook */

    /* 
     * Perform the write regardless the state of the CTS,
     * the UART will send out the data as soon as the CTS goes LOW
     */
    uart_write_func(bufptr, size, callback);
    
   /* 
    * Issue a flow on
    * The transition of RTS from HIGH to LOW is also used as a wake up signal to the other side
    * this should occur regardless if the other side is available or not
    */
    ext_wakeup_sequence();
}

#endif

#if ((UART_FLOW_CTRL_WAKEUP) && (!BLE_APP_PRESENT)) // external wake up, only in full embedded designs

/**
 ****************************************************************************************
 * @brief external wake up callback function.
 ****************************************************************************************
 */
static void ext_wakeup_uart_cb(void)
{
    arch_ble_force_wakeup();
    /* Indicate that this is a wake up request from the external processor */
    ext_wake_up_req = true;
    /* Reactivate the external interrupt as soon as the previous one is served */
    ext_wakeup_uart_enable();
}

/**
 ****************************************************************************************
 * @brief uart flow off function for external wake up flow control mechanism.
 * @note overrides the standard uart_flow_off_func in the jump table.
 ****************************************************************************************
 */
bool ext_wakeup_uart_flow_off_func(void)
{
    return _uart_flow_off_func(true);
}

/**
 ****************************************************************************************
 * @brief uart flow on function for external wake up flow control mechanism.
 * @note overrides the standard uart_flow_on_func in the jump table.
 ****************************************************************************************
 */
void ext_wakeup_uart_flow_on_func(void)
{
    SetWord32(UART_MCR_REG, UART_AFCE);
}

/**
 ****************************************************************************************
 * @brief Check before going to sleep if there is a pending wake up and don't go to sleep.
 * @note Function is invoked in the main while() loop during arch_goto_sleep().
 ****************************************************************************************
 */
void ext_wakeup_pending_req_sleep_prep(sleep_mode_t *sleep_mode)
{
    if (ext_wake_up_req)
        *sleep_mode = mode_active;
}

/**
 ****************************************************************************************
 * @brief Check while active if there is a pending and force active.
 * @note Function is invoked in the main while() loop during schedule_while_ble_on().
 ****************************************************************************************
 */
bool ext_wakeup_pending_req_active(void)
{
    if (ext_wake_up_req)
    {
        ext_wakeup_sequence();
        ext_wake_up_req = false;
        return true;
    }
    return false;
}

void ext_wakeup_uart_init(ext_wkup_uart_pins_t wkup_pins)
{
    wkup_pins_conf.cts_uart_port = wkup_pins.cts_uart_port;
    wkup_pins_conf.cts_uart_pin = wkup_pins.cts_uart_pin;
    wkup_pins_conf.rts_uart_port = wkup_pins.rts_uart_port;
    wkup_pins_conf.rts_uart_pin = wkup_pins.rts_uart_pin;
}

ext_wkup_uart_pins_t* ext_uart_get_wakeup_pins(void)
{
    return &wkup_pins_conf;
}

void ext_wakeup_uart_enable(void)
{
    wkupct_register_callback(ext_wakeup_uart_cb);

    wkupct_enable_irq(WKUPCT_PIN_SELECT((GPIO_PORT) wkup_pins_conf.cts_uart_port, (GPIO_PIN) wkup_pins_conf.cts_uart_pin), // Select pin
                      WKUPCT_PIN_POLARITY((GPIO_PORT) wkup_pins_conf.cts_uart_port, (GPIO_PIN) wkup_pins_conf.cts_uart_pin, WKUPCT_PIN_POLARITY_LOW), // Polarity
                      1, // 1 event
                      0); // debouncing time = 0
}

void ext_wakeup_uart_disable(void)
{
    wkupct_disable_irq();
}

#endif // UART_FLOW_CTRL_WAKEUP




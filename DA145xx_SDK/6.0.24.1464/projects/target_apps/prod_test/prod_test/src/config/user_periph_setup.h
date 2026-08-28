/**
 ****************************************************************************************
 *
 * @file user_periph_setup.h
 *
 * @brief Peripherals setup header file.
 *
 * Copyright (C) 2015-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _USER_PERIPH_SETUP_H_
#define _USER_PERIPH_SETUP_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gpio.h"
#include "uart.h"
#include "dialog_prod.h"

/// Divider for 1000000 bits/s
#define UART_BAUDRATE_1M            BAUD_RATE_DIV(UART_BAUDRATE_1000000)
#define UART_FRAC_BAUDRATE_1M       BAUD_RATE_FRAC(UART_BAUDRATE_1000000)

/// Divider for 921600 bits/s
#define UART_BAUDRATE_921K6         BAUD_RATE_DIV(UART_BAUDRATE_921600)
#define UART_FRAC_BAUDRATE_921K6    BAUD_RATE_FRAC(UART_BAUDRATE_921600)

/// Divider for 500000 bits/s
#define UART_BAUDRATE_500K          BAUD_RATE_DIV(UART_BAUDRATE_500000)
#define UART_FRAC_BAUDRATE_500K     BAUD_RATE_FRAC(UART_BAUDRATE_500000)

/// Divider for 460800 bits/s
#define UART_BAUDRATE_460K8         BAUD_RATE_DIV(UART_BAUDRATE_460800)
#define UART_FRAC_BAUDRATE_460K8    BAUD_RATE_FRAC(UART_BAUDRATE_460800)

/// Divider for 230400 bits/s
#define UART_BAUDRATE_230K4         BAUD_RATE_DIV(UART_BAUDRATE_230400)
#define UART_FRAC_BAUDRATE_230K4    BAUD_RATE_FRAC(UART_BAUDRATE_230400)

/// Divider for 115200 bits/s
#define UART_BAUDRATE_115K2         BAUD_RATE_DIV(UART_BAUDRATE_115200)
#define UART_FRAC_BAUDRATE_115K2    BAUD_RATE_FRAC(UART_BAUDRATE_115200)

/// Divider for 57600 bits/s
#define UART_BAUDRATE_57K6          BAUD_RATE_DIV(UART_BAUDRATE_57600)
#define UART_FRAC_BAUDRATE_57K6     BAUD_RATE_FRAC(UART_BAUDRATE_57600)

/// Divider for 38400 bits/s
#define UART_BAUDRATE_38K4          BAUD_RATE_DIV(UART_BAUDRATE_38400)
#define UART_FRAC_BAUDRATE_38K4     BAUD_RATE_FRAC(UART_BAUDRATE_38400)

/// Divider for 28800 bits/s
#define UART_BAUDRATE_28K8          BAUD_RATE_DIV(UART_BAUDRATE_28800)
#define UART_FRAC_BAUDRATE_28K8     BAUD_RATE_FRAC(UART_BAUDRATE_28800)

/// Divider for 19200 bits/s
#define UART_BAUDRATE_19K2          BAUD_RATE_DIV(UART_BAUDRATE_19200)
#define UART_FRAC_BAUDRATE_19K2     BAUD_RATE_FRAC(UART_BAUDRATE_19200)

/// Divider for 14400 bits/s
#define UART_BAUDRATE_14K4          BAUD_RATE_DIV(UART_BAUDRATE_14400)
#define UART_FRAC_BAUDRATE_14K4     BAUD_RATE_FRAC(UART_BAUDRATE_14400)

/// Divider for 9600 bits/s
#define UART_BAUDRATE_9K6           BAUD_RATE_DIV(UART_BAUDRATE_9600)
#define UART_FRAC_BAUDRATE_9K6      BAUD_RATE_FRAC(UART_BAUDRATE_9600)

/// Divider for 4800 bits/s
#define UART_BAUDRATE_4K8           BAUD_RATE_DIV(UART_BAUDRATE_4800)
#define UART_FRAC_BAUDRATE_4K8      BAUD_RATE_FRAC(UART_BAUDRATE_4800)

/// Divider for 2400 bits/s
#define UART_BAUDRATE_2K4           BAUD_RATE_DIV(UART_BAUDRATE_2400)
#define UART_FRAC_BAUDRATE_2K4      BAUD_RATE_FRAC(UART_BAUDRATE_2400)

/// Character format
enum
{
    /// char format 5
    UART_CHARFORMAT_5 = UART_DATABITS_5,
    /// char format 6
    UART_CHARFORMAT_6 = UART_DATABITS_6,
    /// char format 7
    UART_CHARFORMAT_7 = UART_DATABITS_7,
    /// char format 8
    UART_CHARFORMAT_8 = UART_DATABITS_8
};

/*
 * VARIABLE DEFINITIONS
 ****************************************************************************************
 */
extern void uart_init(uint16_t baudr, uint8_t dlf_value, uint8_t mode);
volatile extern uint8_t port_sel;
extern bool device_wake_up;

typedef struct __uart_sel_pins_t
{
    uint8_t uart_port_tx;
    uint8_t uart_tx_pin;
    uint8_t uart_port_rx;
    uint8_t uart_rx_pin;
}_uart_sel_pins_t;

extern _uart_sel_pins_t     uart_sel_pins;
extern  uint8_t             baud_rate_sel;
extern  uint8_t             baud_rate_frac_sel;

#if defined (__DA14531__)
// Reset function options.
typedef enum __reset_options
{
    // Enables the reset functionality.
    ENABLE_HW_RESET = 0,
    // Use POR.
    ENABLE_POR_RESET,
    // Disables the reset functionality.
    DISABLE_RESET,
    RESET_MAX_VAL,
}_reset_options;

extern  _reset_options      reset_mode;
#endif

/*
 * DEFINES
 ****************************************************************************
 */

/****************************************************************************/
/* UART pin configuration                                                   */
/****************************************************************************/

/****************************************************************************/
/* CONFIG_UART_GPIOS                                                        */
/*    -defined      Uart Port/Pins are defined by external tool             */
/*    -undefined    Uart Port/Pins are defined in the current project       */
/****************************************************************************/
#if !defined (CFG_UART_P04_P05) && !defined (CFG_UART_P03) && !defined (CFG_UART_P05) && !defined (CFG_UART_P00_P01)
#define CONFIG_UART_GPIOS
#endif

/****************************************************************************/
/* UART pin configuration                                                   */
/* DA14531 Supported Port/Pin Combinations:                                 */
/* Tx: P00, Rx: P01                                                         */
/* Tx/Rx: P03 (1-Wire UART)                                                 */
/* Tx/Rx: P05 (1-Wire UART)                                                 */
/* DA14585/DA14586 Supported Port/Pin Combinations:                         */
/* Tx: P04, Rx: P05                                                         */
/****************************************************************************/

#if defined (__DA14531__)
    #if defined (CFG_UART_P00_P01)
        #define UART1_TX_GPIO_PORT  GPIO_PORT_0
        #define UART1_TX_GPIO_PIN   GPIO_PIN_0
        #define UART1_RX_GPIO_PORT  GPIO_PORT_0
        #define UART1_RX_GPIO_PIN   GPIO_PIN_1
    #elif defined (CFG_UART_P03)
        #define UART1_TX_GPIO_PORT  GPIO_PORT_0
        #define UART1_TX_GPIO_PIN   GPIO_PIN_3
        #define UART1_RX_GPIO_PORT  GPIO_PORT_0
        #define UART1_RX_GPIO_PIN   GPIO_PIN_3
    #elif defined (CFG_UART_P05)
        #define UART1_TX_GPIO_PORT  GPIO_PORT_0
        #define UART1_TX_GPIO_PIN   GPIO_PIN_5
        #define UART1_RX_GPIO_PORT  GPIO_PORT_0
        #define UART1_RX_GPIO_PIN   GPIO_PIN_5
    #else
        #define UART1_TX_GPIO_PORT  GPIO_PORT_0
        #define UART1_TX_GPIO_PIN   GPIO_PIN_0
        #define UART1_RX_GPIO_PORT  GPIO_PORT_0
        #define UART1_RX_GPIO_PIN   GPIO_PIN_1
    #endif
#else
    #define UART1_TX_GPIO_PORT  GPIO_PORT_0
    #define UART1_TX_GPIO_PIN   GPIO_PIN_4
    #define UART1_RX_GPIO_PORT  GPIO_PORT_0
    #define UART1_RX_GPIO_PIN   GPIO_PIN_5
#endif // defined (__DA14531__)

enum
{
    P0_0_AND_P0_1_INITIALIZED_FROM_EXT_TOOL = 0x00,
    P0_2_AND_P0_3_INITIALIZED_FROM_EXT_TOOL = 0x02,
    P0_3_SINGLE_W_INITIALIZED_FROM_EXT_TOOL = 0x03,
    P0_4_AND_P0_5_INITIALIZED_FROM_EXT_TOOL = 0x04,
    P0_5_SINGLE_W_INITIALIZED_FROM_EXT_TOOL = 0x05,
    P0_6_AND_P0_7_INITIALIZED_FROM_EXT_TOOL = 0x06,
};

/****************************************************************************************/
/* UART2 configuration to use with arch_console print messages                          */
/****************************************************************************************/
// Define UART2 Tx Pad
#if defined (__DA14531__)
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_4
#else
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_4
#endif

// Define UART2 Settings
#define UART2_BAUDRATE              UART_BAUDRATE_115200
#define UART2_DATABITS              UART_DATABITS_8
#define UART2_PARITY                UART_PARITY_NONE
#define UART2_STOPBITS              UART_STOPBITS_1
#define UART2_AFCE                  UART_AFCE_DIS
#define UART2_FIFO                  UART_FIFO_EN
#define UART2_TX_FIFO_LEVEL         UART_TX_FIFO_LEVEL_0
#define UART2_RX_FIFO_LEVEL         UART_RX_FIFO_LEVEL_0

/****************************************************************************************/
/* SPI FLASH configuration                                                              */
/****************************************************************************************/
#ifndef __DA14586__
#define SPI_FLASH_DEV_SIZE  (256 * 1024)    // SPI Flash memory size in bytes
#else // DA14586
    #define SPI_EN_PORT    GPIO_PORT_2
    #define SPI_EN_PIN     GPIO_PIN_3

    #define SPI_CLK_PORT   GPIO_PORT_2
    #define SPI_CLK_PIN    GPIO_PIN_0

    #define SPI_DO_PORT    GPIO_PORT_2
    #define SPI_DO_PIN     GPIO_PIN_9

    #define SPI_DI_PORT    GPIO_PORT_2
    #define SPI_DI_PIN     GPIO_PIN_4
#endif

/***************************************************************************************/
/* Production debug output configuration                                               */
/***************************************************************************************/
#if PRODUCTION_DEBUG_OUTPUT
#if defined (__DA14531__)
    #define PRODUCTION_DEBUG_PORT   GPIO_PORT_0
    #define PRODUCTION_DEBUG_PIN    GPIO_PIN_11
#else
    #define PRODUCTION_DEBUG_PORT   GPIO_PORT_2
    #define PRODUCTION_DEBUG_PIN    GPIO_PIN_5
#endif
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Enable pad and peripheral clocks assuming that peripheral power domain
 *        is down. The UART and SPI clocks are set.
 ****************************************************************************************
 */
void periph_init(void);

/**
 ****************************************************************************************
 * @brief Map port pins. The UART and SPI port pins and GPIO ports are mapped.
 ****************************************************************************************
 */
void set_pad_functions(void);

/**
 ****************************************************************************************
 * @brief Each application reserves its own GPIOs here.
 ****************************************************************************************
 */
void GPIO_reservations(void);

/**
 ****************************************************************************************
 * @brief Map port pins. The UART pins are mapped.
 ****************************************************************************************
 */
void set_pad_uart(void);

/**
 ****************************************************************************************
 * @brief Unset UART port pins. UART pins are set to input and returned to normal GPIOs
 ****************************************************************************************
 */
void unset_pad_uart(void);

#if defined (__DA14531__)

/**
 ****************************************************************************************
 * @brief Releases P0_0 line.
 * @param[in] port Check input port.
 * @param[in] pin  Check input pin.
 ****************************************************************************************
 */
void unset_rst_line(GPIO_PORT port, GPIO_PIN pin);

/**
 ****************************************************************************************
 * @brief Restores P0_0 line.
 * @param[in] port Check input port.
 * @param[in] pin  Check input pin.
 ****************************************************************************************
 */
void set_rst_line(GPIO_PORT port, GPIO_PIN pin);

/**
 ****************************************************************************************
 * @brief Enables the hw_reset functionality when P0_0 GPIO is not used for any other operation.
 ****************************************************************************************
 */
void set_hw_reset(void);

/**
 ****************************************************************************************
 * @brief Sets the hw_reset functionality depending on the UART configuration and the P0_0 GPIO assignment.
 * @param[in] port Check input port.
 * @param[in] pin  Check input pin.
 ****************************************************************************************
 */
void handle_reset_state(GPIO_PORT port, GPIO_PIN pin);
#endif

/**
 ****************************************************************************************
 * @brief Disable JTAG operation if needed.
 * @param[in] port Check input port.
 * @param[in] pin  Check input pin.
 ****************************************************************************************
 */
void handle_jtag(GPIO_PORT port, GPIO_PIN pin);

/**
 ****************************************************************************************
 * @brief Update port pins. The UART pins are stored to retention memory.
 * @param[in] tx_port Port for UART TX
 * @param[in] tx_pin Pin for UART TX
 * @param[in] rx_port Port for UART RX
 * @param[in] rx_pin Pin for UART RX
 ****************************************************************************************
 */
void update_uart_pads(GPIO_PORT tx_port, GPIO_PIN tx_pin, GPIO_PORT rx_port, GPIO_PIN rx_pin);

#if !defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Initialize TXEN and RXEN.
 ****************************************************************************************
 */
void init_TXEN_RXEN_irqs(void);
#endif

#endif // _USER_PERIPH_SETUP_H_

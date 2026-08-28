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
#include "i2c_eeprom.h"
#include "spi.h"

/*
 * DEFINES
 ****************************************************************************************
 */

// Secondary bootloader does not use GPIO interrupts
#define GPIO_DRV_IRQ_HANDLING_DISABLED

// Select EEPROM characteristics
#define I2C_EEPROM_DEV_SIZE   0x20000           // EEPROM size in bytes
#define I2C_EEPROM_PAGE_SIZE   256               // EEPROM page size in bytes
#define I2C_SLAVE_ADDRESS 0x50              // Set slave device address
#define I2C_SPEED_MODE    I2C_SPEED_FAST    // Speed mode: I2C_SPEED_STANDARD (100 kbits/s), I2C_SPEED_FAST (400 kbits/s)
#define I2C_ADDRESS_MODE  I2C_ADDRESSING_7B // Addressing mode: {I2C_ADDRESSING_7B, I2C_ADDRESSING_10B}
#define I2C_ADDRESS_SIZE  I2C_2BYTES_ADDR   // Address width: {I2C_1BYTE_ADDR, I2C_2BYTES_ADDR, I2C_3BYTES_ADDR}

// SPI Flash settings
// SPI Flash Manufacturer and ID
#define W25X10CL_MANF_DEV_ID   (0xEF10)    // W25X10CL Manufacturer and ID
#define W25X20CL_MANF_DEV_ID   (0xEF11)    // W25X10CL Manufacturer and ID
#define MX25R2035F_MANF_DEV_ID (0xC212)    // MX25R2035F Manufacturer and ID

// SPI Flash options
#define W25X10CL_SIZE   131072            // SPI Flash memory size in bytes
#define W25X20CL_SIZE   262144            // SPI Flash memory size in bytes
#define MX25R2035F_SIZE 262144            // SPI Flash memory size in bytes
#define W25X10CL_PAGE   256               // SPI Flash memory page size in bytes
#define W25X20CL_PAGE   256               // SPI Flash memory page size in bytes
#define MX25R2035F_PAGE 256               // SPI Flash memory page size in bytes

#if !defined (__DA14586__)
#define SPI_FLASH_DEV_SIZE          (256 * 1024)
#endif

// SPI initialization parameters
#define SPI_WORD_MODE  SPI_8BIT_MODE
#define SPI_SMN_MODE   SPI_MASTER_MODE
#define SPI_POL_MODE   SPI_CLK_INIT_HIGH
#define SPI_PHA_MODE   SPI_PHASE_1
#define SPI_MINT_EN    SPI_NO_MINT
#if !defined (__DA14531__)
#define SPI_CLK_DIV    SPI_XTAL_DIV_2
#endif

// 2-wire UART GPIOs assignment
#if defined (__DA14531__)
#define UART_TX_PORT         GPIO_PORT_0
#define UART_TX_PIN          GPIO_PIN_0
#define UART_RX_PORT         GPIO_PORT_0
#define UART_RX_PIN          GPIO_PIN_1
#define UART_FRAC_BAUDRATE   UART_BAUDRATE_115200
#else
#define UART_TX_PORT         GPIO_PORT_0
#define UART_TX_PIN          GPIO_PIN_4
#define UART_RX_PORT         GPIO_PORT_0
#define UART_RX_PIN          GPIO_PIN_5
#define UART_FRAC_BAUDRATE   UART_BAUDRATE_57600
#endif

// 1-wire UART GPIOs assignment
#define UART_ONE_WIRE_TX_RX_PORT    GPIO_PORT_0
#define UART_ONE_WIRE_TX_RX_PIN     GPIO_PIN_5

// SPI GPIO assignment
/****************************************************************************************/
/* SPI configuration                                                                    */
/****************************************************************************************/
// Define SPI Pads
#if defined (__DA14531__)
#if !defined (__FPGA__)
    #define SPI_EN_PORT             GPIO_PORT_0
    #define SPI_EN_PIN              GPIO_PIN_1

    #define SPI_CLK_PORT            GPIO_PORT_0
    #define SPI_CLK_PIN             GPIO_PIN_4

    #define SPI_DO_PORT             GPIO_PORT_0
    #define SPI_DO_PIN              GPIO_PIN_0

    #define SPI_DI_PORT             GPIO_PORT_0
    #define SPI_DI_PIN              GPIO_PIN_3
#else
    #define SPI_EN_PORT             GPIO_PORT_0
    #define SPI_EN_PIN              GPIO_PIN_11

    #define SPI_CLK_PORT            GPIO_PORT_0
    #define SPI_CLK_PIN             GPIO_PIN_7

    #define SPI_DO_PORT             GPIO_PORT_0
    #define SPI_DO_PIN              GPIO_PIN_9

    #define SPI_DI_PORT             GPIO_PORT_0
    #define SPI_DI_PIN              GPIO_PIN_10
#endif

#elif !defined (__DA14586__)
    #define SPI_EN_PORT             GPIO_PORT_0
    #define SPI_EN_PIN              GPIO_PIN_3

    #define SPI_CLK_PORT            GPIO_PORT_0
    #define SPI_CLK_PIN             GPIO_PIN_0

    #define SPI_DO_PORT             GPIO_PORT_0
    #define SPI_DO_PIN              GPIO_PIN_6

    #define SPI_DI_PORT             GPIO_PORT_0
    #define SPI_DI_PIN              GPIO_PIN_5
#endif

// EEPROM GPIO assignment
// Define I2C Pads
#define I2C_SCL_PORT                GPIO_PORT_0
#define I2C_SCL_PIN                 GPIO_PIN_2

#define I2C_SDA_PORT                GPIO_PORT_0
#define I2C_SDA_PIN                 GPIO_PIN_3

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief   Initializes application's peripherals and pins
 ****************************************************************************************
 */
void periph_init(void);

/**
 ****************************************************************************************
 * @brief Set gpio port function mode
 ****************************************************************************************
 */
void set_pad_functions(void);

#endif // _USER_PERIPH_SETUP_H_

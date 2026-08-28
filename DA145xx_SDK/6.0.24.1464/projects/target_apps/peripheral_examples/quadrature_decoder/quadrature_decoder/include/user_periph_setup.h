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

#include "uart.h"
#include "gpio.h"
#include "wkupct_quadec.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/****************************************************************************************/
/* UART2 configuration to print messages                                                */
/****************************************************************************************/
#define UART                        UART2
// Define UART2 Tx Pad
#if defined (__DA14531__)
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_8
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
/* Quadrature Decoder options                                                           */
/****************************************************************************************/
#define QUADRATURE_ENCODER_CHY_CONFIGURATION QUAD_DEC_CHYA_NONE_AND_CHYB_NONE
#define QUADRATURE_ENCODER_CHZ_CONFIGURATION QUAD_DEC_CHZA_NONE_AND_CHZB_NONE
#if defined (__DA14531__)
#define QUADRATURE_ENCODER_CHX_A_PORT        GPIO_PORT_0
#define QUADRATURE_ENCODER_CHX_A_PIN         GPIO_PIN_6
#define QUADRATURE_ENCODER_CHX_B_PORT        GPIO_PORT_0
#define QUADRATURE_ENCODER_CHX_B_PIN         GPIO_PIN_7
#define QUADRATURE_ENCODER_CHX_CONFIGURATION QUAD_DEC_CHXA_P06_AND_CHXB_P07
#define WKUP_TEST_BUTTON_1_PORT              GPIO_PORT_0
#define WKUP_TEST_BUTTON_1_PIN               GPIO_PIN_11
#define WKUP_TEST_BUTTON_2_PORT              GPIO_PORT_0
#if !defined (__FPGA__)
#define WKUP_TEST_BUTTON_2_PIN               GPIO_PIN_1
#else
#define WKUP_TEST_BUTTON_2_PIN               GPIO_PIN_9
#endif // __FPGA__
#else
#define QUADRATURE_ENCODER_CHX_A_PORT        GPIO_PORT_0
#define QUADRATURE_ENCODER_CHX_A_PIN         GPIO_PIN_0
#define QUADRATURE_ENCODER_CHX_B_PORT        GPIO_PORT_0
#define QUADRATURE_ENCODER_CHX_B_PIN         GPIO_PIN_1
#define QUADRATURE_ENCODER_CHX_CONFIGURATION QUAD_DEC_CHXA_P00_AND_CHXB_P01
#define WKUP_TEST_BUTTON_1_PORT              GPIO_PORT_0
#define WKUP_TEST_BUTTON_1_PIN               GPIO_PIN_6
#define WKUP_TEST_BUTTON_2_PORT              GPIO_PORT_1
#define WKUP_TEST_BUTTON_2_PIN               GPIO_PIN_1
#endif
#define QDEC_CLOCK_DIVIDER                   (1)
#define QDEC_EVENTS_COUNT_TO_INT             (1)

#define DEVELOPMENT_DEBUG                    0

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

#endif // _USER_PERIPH_SETUP_H_

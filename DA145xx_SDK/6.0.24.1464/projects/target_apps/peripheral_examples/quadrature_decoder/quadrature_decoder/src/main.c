/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Quadrature decoder example
 *
 * Copyright (C) 2012-2023 Renesas Electronics Corporation and/or its affiliates.
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

#include <stdio.h>
#include <stdint.h>
#include "arch_system.h"
#include "user_periph_setup.h"
#include "wkupct_quadec.h"
#include "gpio.h"
#include "uart.h"
#include "uart_utils.h"



/**
 ****************************************************************************************
 * @brief Quadrature Decoder test function
 ****************************************************************************************
 */
void quad_decoder_test(void);

volatile uint8_t terminate_quad_test;
volatile uint8_t quad_started = 0;
uint8_t qdec_xcnt;
uint8_t qdec_ycnt;
uint8_t qdec_zcnt;
uint8_t qdec_new_data = false;

/**
 ****************************************************************************************
 * @brief Main routine of the quadrature decoder example
 *
 ****************************************************************************************
 */
int main (void)
{
    system_init();
    quad_decoder_test();
    while(1);
}

/**
 ****************************************************************************************
 * @brief  Quadrature Decoder callback function
 *
 ****************************************************************************************
 */
void quad_decoder_user_callback_function(int16_t qdec_xcnt_reg, int16_t qdec_ycnt_reg, int16_t qdec_zcnt_reg)
{
    qdec_xcnt = qdec_xcnt_reg;
    qdec_ycnt = qdec_ycnt_reg;
    qdec_zcnt = qdec_zcnt_reg;
    qdec_new_data = true;

    quad_decoder_enable_irq(1);
}

/**
 ****************************************************************************************
 * @brief WKUP callback function
 *
 ****************************************************************************************
 */
void wkup_callback_function(void)
{
    if(!GPIO_GetPinStatus(WKUP_TEST_BUTTON_1_PORT, WKUP_TEST_BUTTON_1_PIN))
    {
        terminate_quad_test = true;
    }

    if(!GPIO_GetPinStatus(WKUP_TEST_BUTTON_2_PORT, WKUP_TEST_BUTTON_2_PIN))
    {
        quad_started ^= true;
    }

    wkupct_enable_irq(WKUPCT_PIN_SELECT(WKUP_TEST_BUTTON_1_PORT, WKUP_TEST_BUTTON_1_PIN) ^
                      WKUPCT_PIN_SELECT(WKUP_TEST_BUTTON_2_PORT, WKUP_TEST_BUTTON_2_PIN),
                      WKUPCT_PIN_POLARITY(WKUP_TEST_BUTTON_1_PORT, WKUP_TEST_BUTTON_1_PIN,
                      WKUPCT_PIN_POLARITY_LOW) ^ WKUPCT_PIN_POLARITY(WKUP_TEST_BUTTON_2_PORT,
                      WKUP_TEST_BUTTON_2_PIN, WKUPCT_PIN_POLARITY_LOW), // polarity low
                      1,    // 1 event
                      40);  // debouncing time = 40

    return;
}

void quad_decoder_test(void)
{
    int16_t qdec_xcnt_reg;

    printf_string(UART, "Quadrature Decoder / WKUP controller\n\r");
    printf_string(UART, "SW3 button to start/stop Quadec polling\n\r");
    printf_string(UART, "SW2 button to terminate Quadec test\n\r");

    printf_string(UART, "\n\r Quadrature Decoder Test started!.");
    printf_string(UART, "\n\r Press SW2 button to terminate test.\n\r");

    QUAD_DEC_INIT_PARAMS_t quad_dec_init_param = {.chx_port_sel = QUADRATURE_ENCODER_CHX_CONFIGURATION,
                                                  .chy_port_sel = QUADRATURE_ENCODER_CHY_CONFIGURATION,
                                                  .chz_port_sel = QUADRATURE_ENCODER_CHZ_CONFIGURATION,
                                                  .qdec_clockdiv = QDEC_CLOCK_DIVIDER,
                                                  .qdec_events_count_to_trigger_interrupt = QDEC_EVENTS_COUNT_TO_INT,};
    quad_decoder_init(&quad_dec_init_param);
    quad_decoder_register_callback(quad_decoder_user_callback_function);
    quad_decoder_enable_irq(1);

    wkupct_register_callback(wkup_callback_function);
    wkupct_enable_irq(WKUPCT_PIN_SELECT(WKUP_TEST_BUTTON_1_PORT, WKUP_TEST_BUTTON_1_PIN) ^
                      WKUPCT_PIN_SELECT(WKUP_TEST_BUTTON_2_PORT, WKUP_TEST_BUTTON_2_PIN),
                      WKUPCT_PIN_POLARITY(WKUP_TEST_BUTTON_1_PORT, WKUP_TEST_BUTTON_1_PIN,
                      WKUPCT_PIN_POLARITY_LOW) ^ WKUPCT_PIN_POLARITY(WKUP_TEST_BUTTON_2_PORT,
                      WKUP_TEST_BUTTON_2_PIN, WKUPCT_PIN_POLARITY_LOW), // polarity low
                      1,    // 1 event
                      40);  // debouncing time = 40

    quad_started = false;
    terminate_quad_test = false;
    while(terminate_quad_test == false)
    {
        // polling
        if (quad_started)
        {
            qdec_xcnt_reg = quad_decoder_get_x_counter();
            printf_string(UART, "\n\r Quadec Polling DX: ");
            printf_byte(UART, qdec_xcnt_reg >> 8);
            printf_byte(UART, qdec_xcnt_reg & 0xFF);
        }

        // interrupt triggered
        if (qdec_new_data == true)
        {
            qdec_new_data = false;

            printf_string(UART, "\n\n\rQuadec ISR report:");

            printf_string(UART, "\n\r DX: ");
            printf_byte(UART, qdec_xcnt >> 8);
            printf_byte(UART, qdec_xcnt & 0xFF);

            printf_string(UART, " DY: ");
            printf_byte(UART, qdec_ycnt >> 8);
            printf_byte(UART, qdec_ycnt & 0xFF);

            printf_string(UART, " DZ: ");
            printf_byte(UART, qdec_zcnt >> 8);
            printf_byte(UART, qdec_zcnt & 0xFF);
        }
    }
    printf_string(UART, "\n\r Quadrature Decoder Test terminated!");
    printf_string(UART, "\n\rEnd of test\n\r");
}

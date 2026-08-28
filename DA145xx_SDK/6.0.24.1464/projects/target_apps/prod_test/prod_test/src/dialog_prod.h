/**
 ****************************************************************************************
 *
 * @file dialog_prod.h
 *
 * @brief Dialog production header file.
 *
 * Copyright (C) 2017-2023 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _DIALOG_PROD_H_
#define _DIALOG_PROD_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "systick.h"
#include "gpio.h"
#include "i2c.h"

extern volatile uint8_t test_state;
extern volatile uint8_t test_freq;
extern volatile uint8_t test_data_pattern;
extern volatile uint16_t test_tx_packet_nr;
extern volatile uint16_t text_tx_nr_of_packets;
extern volatile uint16_t test_rx_irq_cnt;
extern volatile GPIO_PORT WD_port;
extern volatile GPIO_PIN WD_pin;
extern volatile uint8_t WD_volt_rail;

enum
{
    ///NO_TEST_RUNNING
    STATE_IDLE = 0x00,
    ///START_TX_TEST
    STATE_START_TX,            //1
    ///START_RX_TEST
    STATE_START_RX,            //2
    ///DIRECT_TX_TEST
    STATE_DIRECT_TX_TEST,      //3 activated via default hci command
    ///DIRECT_RX_TEST
    STATE_DIRECT_RX_TEST,      //4 activated via default hci command
    ///CONTINUE_TX
    STATE_START_CONTINUE_TX,   //5
    ///UNMODULATED_ON
    STATE_UNMODULATED_ON,      //6
#if defined (__DA14531__)
    STATE_UNMODULATED_RX_ON,   //7
#endif
};

void user_app_on_init(void);
void otp_read(uint32_t otp_pos, uint8_t *val, uint8_t len);
int otp_write_words(uint32_t otp_pos, uint32_t *val_addr, uint8_t bytes_count);
void set_state_start_continue_tx(void);
void set_state_stop(void);
void set_state_start_tx(void);
void set_state_start_rx(void);
uint8_t check_uart_pins_cmd(uint8_t *ptr_data);
void change_uart_pins_cmd(uint8_t *ptr_data);
void sensor_spi_write_byte(uint8_t address, uint8_t wr_data, GPIO_PORT cs_port, GPIO_PIN cs_pin);
uint8_t sensor_spi_read_byte(uint8_t address, GPIO_PORT cs_port, GPIO_PIN cs_pin);
void sensor_i2c_write_byte(uint32_t address, uint8_t wr_data);
uint8_t sensor_i2c_read_byte(uint8_t address);
uint32_t find_mode_register(uint8_t port_number);
void gpio_wd_timer0_start(void);
void gpio_wd_timer0_stop(void);

#if defined (__DA14531__)
void start_refresh_timer(void);
void stop_refresh_timer(void);
void set_radio_ldo_configuration(bool enable);
void configure_radio_ldo(void);
void apply_radio_arp_tx_configuration(void);
uint32_t adc_531_get_vbat_sample(uint8_t vbat_type);
#endif

#endif // _DIALOG_PROD_H_

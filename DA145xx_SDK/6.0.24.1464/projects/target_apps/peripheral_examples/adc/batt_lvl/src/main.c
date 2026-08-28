/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Battery level example
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
#include "arch_system.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "uart.h"
#include "uart_utils.h"
#include "battery.h"
#include "datasheet.h"
#if defined (__DA14531__)
#include "hw_otpc.h"
#include "syscntl.h"
#include "otp_hdr.h"
#include "otp_cs.h"
#include "arch.h"
#include "adc.h"
#include "adc_531.h"
#endif

/**
 ****************************************************************************************
 * @brief Battery Level test function
 ****************************************************************************************
 */
static void batt_test(batt_t battery_type);

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Temperature Sensor test function
 ****************************************************************************************
 */
static void temp_sens_test(void);
#endif

/**
 ****************************************************************************************
 * @brief Main routine of the battery level example
 ****************************************************************************************
 */
int main (void)
{
    system_init();

    batt_t battery_type;
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) != 0x1)
    {
        battery_type = BATT_CR2032;
    }
    else
    {
        // Device operates in boost mode
        battery_type = BATT_ALKALINE;
    }
#if !defined (__ONE_SHOT__)
    while (1)
    {
#endif
    batt_test(battery_type);
#if defined (__DA14531__)
    temp_sens_test();
#endif
#if !defined (__ONE_SHOT__)
    }
#endif
    while(1);
}

void batt_test(batt_t battery_type)
{
    printf_string(UART, "\n\r\n\r");
    printf_string(UART, "*******************\n\r");
    printf_string(UART, "* BATTERY TEST *\n\r");
    printf_string(UART, "*******************\n\r");

    if (battery_type == BATT_CR2032)
    {
        printf_string(UART, "\n\rBattery type: CR2032");
        printf_string(UART, "\n\rCurrent battery level (%): ");
        printf_byte_dec(UART, battery_get_lvl(BATT_CR2032));
#if defined (__DA14531__)
        printf_string(UART, "\n\rBattery voltage in mV ");
        uint16_t bat_volt_lvl = battery_get_voltage(BATT_CR2032);
        printf_word_dec(UART, bat_volt_lvl);
#endif
    }
    else
    {
        printf_string(UART, "\n\rBattery type: Alkaline");
        printf_string(UART, "\n\rCurrent battery level (%): ");
        printf_byte_dec(UART, battery_get_lvl(BATT_ALKALINE));
#if defined (__DA14531__)
        printf_string(UART, "\n\rBattery voltage in mV ");
        uint16_t bat_volt_lvl = battery_get_voltage(BATT_ALKALINE);
        printf_word_dec(UART, bat_volt_lvl);
#endif
    }

    printf_string(UART, "\n\rEnd of battery test\n\r");
}

#if defined (__DA14531__)
static void temp_sens_test(void)
{
    int8_t current_temp;
    printf_string(UART, "\n\r\n\r");
    printf_string(UART, "******************************\n\r");
    printf_string(UART, "* TEMPERATURE SENSOR TEST *\n\r");
    printf_string(UART, "******************************\n\r");

    uint16_t tmp_adc_ctrl_reg  = GetWord16(GP_ADC_CTRL_REG);
    uint16_t tmp_adc_ctrl2_reg = GetWord16(GP_ADC_CTRL2_REG);
    uint16_t tmp_adc_ctrl3_reg = GetWord16(GP_ADC_CTRL3_REG);
    uint16_t tmp_adc_sel_reg   = GetWord16(GP_ADC_SEL_REG);

    adc_config_t cfg =
    {
        .input_mode = ADC_INPUT_MODE_SINGLE_ENDED,
        .input      = ADC_INPUT_SE_TEMP_SENS,
        .continuous = false
    };

    // Initialize and enable ADC
    adc_init(&cfg);

    adc_store_offsets();
    adc_reset_offsets();

    current_temp = adc_get_temp();
#if defined (ADC_RAW_VALUE)
    uint16_t adc_raw_val = adc_get_sample();
#endif

    adc_load_offsets();

    // Restore the content of the ADC CTRL and SEL registers
    SetWord16(GP_ADC_CTRL2_REG, tmp_adc_ctrl2_reg);
    SetWord16(GP_ADC_CTRL3_REG, tmp_adc_ctrl3_reg);
    SetWord16(GP_ADC_SEL_REG, tmp_adc_sel_reg);
    SetWord16(GP_ADC_CTRL_REG, tmp_adc_ctrl_reg);


#if defined (ADC_RAW_VALUE)
    printf_string(UART, "\n\rADC raw value ");
    printf_word_dec(UART, adc_raw_val);
#else
    printf_string(UART, "\n\rTemperature ");
    printf_word_dec(UART, current_temp);
#endif
    printf_string(UART, "\n\rEnd of temperature sensor test\n\r");
}
#endif

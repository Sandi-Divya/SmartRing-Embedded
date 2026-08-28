/**
 ****************************************************************************************
 * @file user_periph_setup.c
 * @brief Peripherals setup for SSD1306 OLED on P0_0 (SCL), P0_1 (SDA) and LED on P1_0.
 ****************************************************************************************
 */

#include "user_periph_setup.h"
#include "datasheet.h"
#include "system_library.h"
#include "rwip_config.h"
#include "gpio.h"
#include "uart.h"
#include "syscntl.h"
#include "display.h"

// I2C Pin Mapping: SCL -> P0_0, SDA -> P0_1
#define I2C_SCL_PORT    GPIO_PORT_0
#define I2C_SCL_PIN     GPIO_PIN_0

#define I2C_SDA_PORT    GPIO_PORT_0
#define I2C_SDA_PIN     GPIO_PIN_1

#define GPIO_LED_PORT   GPIO_PORT_1
#define GPIO_LED_PIN    GPIO_PIN_0

#if DEVELOPMENT_DEBUG
void GPIO_reservations(void)
{
#if !defined (__DA14586__)
    RESERVE_GPIO(SPI_EN, SPI_EN_PORT, SPI_EN_PIN, PID_SPI_EN);
#endif

    // Built-in Diagnostic LED on P1_0
    RESERVE_GPIO(DESK_LED, GPIO_LED_PORT, GPIO_LED_PIN, PID_GPIO);

    // OLED Software I2C (P0_0 / P0_1) as standard GPIOs
    RESERVE_GPIO(I2C_SCL, I2C_SCL_PORT, I2C_SCL_PIN, PID_GPIO);
    RESERVE_GPIO(I2C_SDA, I2C_SDA_PORT, I2C_SDA_PIN, PID_GPIO);

    // Touch Key (P1_3)
    RESERVE_GPIO(TOUCH_KEY, GPIO_PORT_1, GPIO_PIN_3, PID_GPIO);
}
#endif

void set_pad_functions(void)
{
#if defined (__DA14586__)
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#else
    GPIO_ConfigurePin(SPI_EN_PORT, SPI_EN_PIN, OUTPUT, PID_SPI_EN, true);
#endif

    // Diagnostic LED (P1_0) Default LOW (OFF)
    GPIO_ConfigurePin(GPIO_LED_PORT, GPIO_LED_PIN, OUTPUT, PID_GPIO, false);

    // OLED I2C Pins configured as standard GPIO Outputs (Default HIGH / Idle)
    GPIO_ConfigurePin(I2C_SCL_PORT, I2C_SCL_PIN, OUTPUT, PID_GPIO, true);
    GPIO_ConfigurePin(I2C_SDA_PORT, I2C_SDA_PIN, OUTPUT, PID_GPIO, true);

    // Touch Pin (P1_3)
    GPIO_ConfigurePin(GPIO_PORT_1, GPIO_PIN_3, INPUT_PULLUP, PID_GPIO, false);
}

void periph_init(void)
{
#if defined (__DA14531__)
    FPGA_HELPER(FPGA_GPIO_MAP_1, SWD_DATA_AT_P0_5);
    syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);
#else
    // Power up peripherals power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif

    // Apply ROM patches
    patch_func();

    // Set pin assignments and latch GPIOs
    set_pad_functions();
    GPIO_set_pad_latch_en(true);

    // Initialize OLED Screen
    display_init();
}
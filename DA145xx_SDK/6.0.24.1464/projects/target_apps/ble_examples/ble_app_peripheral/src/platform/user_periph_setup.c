/**
 ****************************************************************************************
 * @file user_periph_setup.c
 * @brief Peripherals setup for SSD1306 OLED on P0_0 (SCL), P0_1 (SDA),
 *        LED on P1_0 and ADXL362 accelerometer.
 *
 * ADXL362:
 *      SCLK -> P0_2
 *      CS   -> P0_3
 *      MOSI -> P0_6
 *      MISO -> P0_7
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


/*
 * =============================================================================
 * OLED I2C PIN MAPPING
 * =============================================================================
 */

#define I2C_SCL_PORT    GPIO_PORT_0
#define I2C_SCL_PIN     GPIO_PIN_0

#define I2C_SDA_PORT    GPIO_PORT_0
#define I2C_SDA_PIN     GPIO_PIN_1


/*
 * =============================================================================
 * LED
 * =============================================================================
 */

#define GPIO_LED_PORT   GPIO_PORT_1
#define GPIO_LED_PIN    GPIO_PIN_0


/*
 * =============================================================================
 * ADXL362 ACCELEROMETER
 * =============================================================================
 *
 * J4:
 *
 *      P0_2 -> SCLK
 *      P0_3 -> CS
 *      P0_6 -> MOSI
 *      P0_7 -> MISO
 *
 * IMPORTANT:
 *
 * P0_3 is used ONLY as ADXL362 CS.
 *
 * The SDK SPI_EN configuration is intentionally NOT configured here,
 * because SPI_EN also uses P0_3 on DA14585 and would cause a GPIO
 * reservation conflict.
 */

#define ACCEL_SCLK_PORT    GPIO_PORT_0
#define ACCEL_SCLK_PIN     GPIO_PIN_2

#define ACCEL_CS_PORT           GPIO_PORT_1
#define ACCEL_CS_PIN            GPIO_PIN_1

#define ACCEL_MOSI_PORT    GPIO_PORT_0
#define ACCEL_MOSI_PIN     GPIO_PIN_6

#define ACCEL_MISO_PORT    GPIO_PORT_0
#define ACCEL_MISO_PIN     GPIO_PIN_7


/*
 * =============================================================================
 * GPIO RESERVATIONS
 * =============================================================================
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
    /*
     * DO NOT reserve/configure SPI_EN here.
     *
     * SPI_EN uses P0_3 in the SDK configuration,
     * but P0_3 is now ADXL362 CS.
     */


    /*
     * Built-in diagnostic LED
     */
    RESERVE_GPIO(
        DESK_LED,
        GPIO_LED_PORT,
        GPIO_LED_PIN,
        PID_GPIO
    );


    /*
     * OLED software I2C
     */
    RESERVE_GPIO(
        I2C_SCL,
        I2C_SCL_PORT,
        I2C_SCL_PIN,
        PID_GPIO
    );

    RESERVE_GPIO(
        I2C_SDA,
        I2C_SDA_PORT,
        I2C_SDA_PIN,
        PID_GPIO
    );


    /*
     * Touch key
     */
    RESERVE_GPIO(
        TOUCH_KEY,
        GPIO_PORT_1,
        GPIO_PIN_3,
        PID_GPIO
    );


    /*
     * ========================================================================
     * ADXL362
     * ========================================================================
     */

    RESERVE_GPIO(
        ACCEL_CS,
        ACCEL_CS_PORT,
        ACCEL_CS_PIN,
        PID_GPIO
    );

    RESERVE_GPIO(
        ACCEL_SCLK,
        ACCEL_SCLK_PORT,
        ACCEL_SCLK_PIN,
        PID_GPIO
    );

    RESERVE_GPIO(
        ACCEL_MOSI,
        ACCEL_MOSI_PORT,
        ACCEL_MOSI_PIN,
        PID_GPIO
    );

    RESERVE_GPIO(
        ACCEL_MISO,
        ACCEL_MISO_PORT,
        ACCEL_MISO_PIN,
        PID_GPIO
    );
}

#endif


/*
 * =============================================================================
 * PAD CONFIGURATION
 * =============================================================================
 */

void set_pad_functions(void)
{
    /*
     * IMPORTANT:
     *
     * Do NOT configure SPI_EN here.
     *
     * The original SDK code configured:
     *
     *     SPI_EN_PORT = P0_0 / P0_3 depending on device configuration
     *
     * P0_3 is now required by the ADXL362 as CS.
     */


    /*
     * Diagnostic LED
     *
     * LOW = OFF
     */
    GPIO_ConfigurePin(
        GPIO_LED_PORT,
        GPIO_LED_PIN,
        OUTPUT,
        PID_GPIO,
        false
    );


    /*
     * ========================================================================
     * OLED I2C
     * ========================================================================
     *
     * Idle state HIGH.
     */

    GPIO_ConfigurePin(
        I2C_SCL_PORT,
        I2C_SCL_PIN,
        OUTPUT,
        PID_GPIO,
        true
    );

    GPIO_ConfigurePin(
        I2C_SDA_PORT,
        I2C_SDA_PIN,
        OUTPUT,
        PID_GPIO,
        true
    );


    /*
     * ========================================================================
     * TOUCH
     * ========================================================================
     */

    GPIO_ConfigurePin(
        GPIO_PORT_1,
        GPIO_PIN_3,
        INPUT_PULLUP,
        PID_GPIO,
        false
    );


    /*
     * ========================================================================
     * ADXL362 CHIP SELECT
     * ========================================================================
     *
     * HIGH = accelerometer not selected.
     */

    GPIO_ConfigurePin(
        ACCEL_CS_PORT,
        ACCEL_CS_PIN,
        OUTPUT,
        PID_GPIO,
        true
    );


    /*
     * ========================================================================
     * ADXL362 CLOCK
     * ========================================================================
     *
     * Software SPI.
     *
     * Clock starts LOW.
     */

    GPIO_ConfigurePin(
        ACCEL_SCLK_PORT,
        ACCEL_SCLK_PIN,
        OUTPUT,
        PID_GPIO,
        false
    );


    /*
     * ========================================================================
     * ADXL362 MOSI
     * ========================================================================
     *
     * Master output.
     */

    GPIO_ConfigurePin(
        ACCEL_MOSI_PORT,
        ACCEL_MOSI_PIN,
        OUTPUT,
        PID_GPIO,
        false
    );


    /*
     * ========================================================================
     * ADXL362 MISO
     * ========================================================================
     *
     * Master input.
     */

    GPIO_ConfigurePin(
        ACCEL_MISO_PORT,
        ACCEL_MISO_PIN,
        INPUT,
        PID_GPIO,
        false
    );
}


/*
 * =============================================================================
 * PERIPHERAL INITIALIZATION
 * =============================================================================
 */

void periph_init(void)
{
	
// Enable SysTick with processor clock, counts down from 0xFFFFFF
SysTick->LOAD = 0x00FFFFFF;
SysTick->VAL  = 0x00FFFFFF;
SysTick->CTRL = 0x00000005; // ENABLE + CLKSOURCE (processor clock)

#if defined (__DA14531__)

    FPGA_HELPER(
        FPGA_GPIO_MAP_1,
        SWD_DATA_AT_P0_5
    );

    syscntl_dcdc_turn_on_in_boost(
        SYSCNTL_DCDC_LEVEL_3V0
    );

#else

    /*
     * Power up peripheral power domain.
     */

    SetBits16(
        PMU_CTRL_REG,
        PERIPH_SLEEP,
        0
    );

    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));


    SetBits16(
        CLK_16M_REG,
        XTAL16_BIAS_SH_ENABLE,
        1
    );

#endif


    /*
     * Apply ROM patches.
     */
    patch_func();


    /*
     * Configure GPIOs.
     */
    set_pad_functions();


    /*
     * Latch GPIO configuration.
     */
    GPIO_set_pad_latch_en(true);
		
		
		/*
     * ========================================================================
     * ENABLE EXTENDED SLEEP MODE
     * ========================================================================
     */
    arch_set_sleep_mode(ARCH_EXT_SLEEP_ON);


    /*
     * Initialize OLED.
     */
    display_init();
}


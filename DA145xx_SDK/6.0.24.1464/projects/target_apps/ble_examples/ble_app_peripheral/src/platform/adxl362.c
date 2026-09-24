/**
 ****************************************************************************************
 *
 * @file adxl362.c
 * @brief ADXL362 accelerometer software SPI driver.
 *
 ****************************************************************************************
 */

#include "adxl362.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "uart.h"
#include <stdint.h>


/*
 * ADXL362 timing
 *
 * The ADXL362 supports SPI mode 0.
 * Data is sampled on the rising edge and changed on the falling edge.
 */
static void adxl362_delay(void)
{
    volatile uint32_t i;

    for (i = 0; i < 5; i++)
    {
        __asm volatile ("nop");
    }
}


static void adxl362_sclk(uint8_t level)
{
    if (level)
    {
        GPIO_SetActive(ACCEL_SCLK_PORT, ACCEL_SCLK_PIN);
    }
    else
    {
        GPIO_SetInactive(ACCEL_SCLK_PORT, ACCEL_SCLK_PIN);
    }
}


static void adxl362_cs(uint8_t level)
{
    if (level)
    {
        GPIO_SetActive(ACCEL_CS_PORT, ACCEL_CS_PIN);
    }
    else
    {
        GPIO_SetInactive(ACCEL_CS_PORT, ACCEL_CS_PIN);
    }
}


static void adxl362_mosi(uint8_t level)
{
    if (level)
    {
        GPIO_SetActive(ACCEL_MOSI_PORT, ACCEL_MOSI_PIN);
    }
    else
    {
        GPIO_SetInactive(ACCEL_MOSI_PORT, ACCEL_MOSI_PIN);
    }
}


/*
 * Read MISO
 */
static uint8_t adxl362_miso(void)
{
    return GPIO_GetPinStatus(
        ACCEL_MISO_PORT,
        ACCEL_MISO_PIN
    );
}


/*
 * Software SPI transfer
 */
static uint8_t adxl362_spi_transfer(uint8_t tx)
{
    uint8_t rx = 0;
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        /*
         * Put next bit on MOSI.
         */
        if (tx & 0x80)
        {
            adxl362_mosi(1);
        }
        else
        {
            adxl362_mosi(0);
        }

        tx <<= 1;

        adxl362_delay();

        /*
         * Rising edge.
         */
        adxl362_sclk(1);

        adxl362_delay();

        /*
         * Sample MISO.
         */
        rx <<= 1;

        if (adxl362_miso())
        {
            rx |= 1;
        }

        /*
         * Falling edge.
         */
        adxl362_sclk(0);

        adxl362_delay();
    }

    return rx;
}


/*
 * Write one ADXL362 register.
 */
void adxl362_write_register(
    uint8_t reg,
    uint8_t value
)
{
    /*
     * CS LOW = select device.
     */
    adxl362_cs(0);

    adxl362_delay();

    /*
     * Command.
     */
    adxl362_spi_transfer(
        ADXL362_CMD_WRITE
    );

    /*
     * Register address.
     */
    adxl362_spi_transfer(reg);

    /*
     * Data.
     */
    adxl362_spi_transfer(value);

    adxl362_delay();

    /*
     * CS HIGH = deselect device.
     */
    adxl362_cs(1);

    adxl362_delay();
}


/*
 * Read one ADXL362 register.
 */
uint8_t adxl362_read_register(uint8_t reg)
{
    uint8_t value;

    /*
     * CS LOW = select device.
     */
    adxl362_cs(0);

    adxl362_delay();

    /*
     * READ command.
     */
    adxl362_spi_transfer(
        ADXL362_CMD_READ
    );

    /*
     * Register address.
     */
    adxl362_spi_transfer(reg);

    /*
     * Read returned byte.
     */
    value = adxl362_spi_transfer(0x00);

    adxl362_delay();

    /*
     * CS HIGH = deselect device.
     */
    adxl362_cs(1);

    adxl362_delay();

    return value;
}


/*
 * Initialize ADXL362.
 */
void adxl362_init(void)
{
    /*
     * Make sure the bus starts idle.
     */
    adxl362_cs(1);

    adxl362_sclk(0);

    adxl362_mosi(0);

    adxl362_delay();

    /*
     * Put ADXL362 into measurement mode.
     *
     * POWER_CTL:
     *
     * 0x00 = standby
     * 0x02 = measurement mode
     */
    adxl362_write_register(
        ADXL362_REG_POWER_CTL,
        0x02
    );

    adxl362_delay();
}


/*
 * Check whether ADXL362 is connected.
 */
uint8_t adxl362_is_connected(void)
{
    uint8_t devid_ad;
    uint8_t devid_mst;
    uint8_t partid;

    devid_ad =
        adxl362_read_register(
            ADXL362_REG_DEVID_AD
        );

    devid_mst =
        adxl362_read_register(
            ADXL362_REG_DEVID_MST
        );

    partid =
        adxl362_read_register(
            ADXL362_REG_PARTID
        );

    if ((devid_ad == 0xAD) &&
        (devid_mst == 0x1D) &&
        (partid == 0xF2))
    {
        return 1;
    }

    return 0;
}


/*
 * Read X/Y/Z acceleration.
 */
void adxl362_read_xyz(
    int16_t *x,
    int16_t *y,
    int16_t *z
)
{
    uint8_t xl;
    uint8_t xh;
    uint8_t yl;
    uint8_t yh;
    uint8_t zl;
    uint8_t zh;

    if ((x == 0) ||
        (y == 0) ||
        (z == 0))
    {
        return;
    }

    xl =
        adxl362_read_register(
            ADXL362_REG_XDATA_L
        );

    xh =
        adxl362_read_register(
            ADXL362_REG_XDATA_H
        );

    yl =
        adxl362_read_register(
            ADXL362_REG_YDATA_L
        );

    yh =
        adxl362_read_register(
            ADXL362_REG_YDATA_H
        );

    zl =
        adxl362_read_register(
            ADXL362_REG_ZDATA_L
        );

    zh =
        adxl362_read_register(
            ADXL362_REG_ZDATA_H
        );

    *x =
        (int16_t)(
            ((uint16_t)xh << 8) |
            xl
        );

    *y =
        (int16_t)(
            ((uint16_t)yh << 8) |
            yl
        );

    *z =
        (int16_t)(
            ((uint16_t)zh << 8) |
            zl
        );
}

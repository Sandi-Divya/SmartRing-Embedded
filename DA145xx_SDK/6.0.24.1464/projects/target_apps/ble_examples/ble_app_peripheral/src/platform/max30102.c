/**
 ****************************************************************************************
 *
 * @file max30102.c
 * @brief Maxim MAX30102 Pulse Oximeter and Heart-Rate Sensor Driver.
 *
 * MAX30102 is connected on the shared I2C bus:
 *      SCL -> P0_0
 *      SDA -> P0_1
 *
 * Uses software I2C bit-banging compatible with SSD1306 OLED on the same bus.
 *
 ****************************************************************************************
 */

#include "max30102.h"
#include "gpio.h"
#include "datasheet.h"
#include "arch.h"
#include "user_periph_setup.h"
#include <stdint.h>
#include <stdbool.h>

/*
 * =============================================================================
 * I2C PIN CONFIGURATION
 * =============================================================================
 */

#define I2C_SCL_PORT    GPIO_PORT_0
#define I2C_SCL_PIN     GPIO_PIN_0

#define I2C_SDA_PORT    GPIO_PORT_0
#define I2C_SDA_PIN     GPIO_PIN_1

/*
 * =============================================================================
 * SOFTWARE I2C PRIMITIVES
 * =============================================================================
 */

static void max30102_i2c_delay(void)
{
    for (volatile int i = 0; i < 8; i++)
    {
        __NOP();
    }
}

static inline void max30102_scl_hi(void)
{
    GPIO_ConfigurePin(
        I2C_SCL_PORT,
        I2C_SCL_PIN,
        INPUT_PULLUP,
        PID_GPIO,
        false
    );
    max30102_i2c_delay();
}

static inline void max30102_scl_lo(void)
{
    GPIO_ConfigurePin(
        I2C_SCL_PORT,
        I2C_SCL_PIN,
        OUTPUT,
        PID_GPIO,
        false
    );
    GPIO_SetInactive(
        I2C_SCL_PORT,
        I2C_SCL_PIN
    );
    max30102_i2c_delay();
}

static inline void max30102_sda_hi(void)
{
    GPIO_ConfigurePin(
        I2C_SDA_PORT,
        I2C_SDA_PIN,
        INPUT_PULLUP,
        PID_GPIO,
        false
    );
    max30102_i2c_delay();
}

static inline void max30102_sda_lo(void)
{
    GPIO_ConfigurePin(
        I2C_SDA_PORT,
        I2C_SDA_PIN,
        OUTPUT,
        PID_GPIO,
        false
    );
    GPIO_SetInactive(
        I2C_SDA_PORT,
        I2C_SDA_PIN
    );
    max30102_i2c_delay();
}

static inline bool max30102_sda_read(void)
{
    return GPIO_GetPinStatus(
        I2C_SDA_PORT,
        I2C_SDA_PIN
    );
}

static void max30102_i2c_start(void)
{
    max30102_sda_hi();
    max30102_scl_hi();
    max30102_sda_lo();
    max30102_scl_lo();
}

static void max30102_i2c_stop(void)
{
    max30102_sda_lo();
    max30102_scl_hi();
    max30102_sda_hi();
}

static bool max30102_i2c_write_byte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        if (byte & 0x80)
        {
            max30102_sda_hi();
        }
        else
        {
            max30102_sda_lo();
        }

        max30102_scl_hi();
        max30102_scl_lo();

        byte <<= 1;
    }

    /*
     * Read ACK from slave
     */
    max30102_sda_hi();
    max30102_scl_hi();
    bool ack = (max30102_sda_read() == false);
    max30102_scl_lo();

    return ack;
}

static uint8_t max30102_i2c_read_byte(bool send_ack)
{
    uint8_t byte = 0;

    max30102_sda_hi();

    for (uint8_t i = 0; i < 8; i++)
    {
        byte <<= 1;
        max30102_scl_hi();

        if (max30102_sda_read())
        {
            byte |= 0x01;
        }

        max30102_scl_lo();
    }

    /*
     * Send ACK (pull SDA low) or NACK (leave SDA high)
     */
    if (send_ack)
    {
        max30102_sda_lo();
    }
    else
    {
        max30102_sda_hi();
    }

    max30102_scl_hi();
    max30102_scl_lo();
    max30102_sda_hi();

    return byte;
}

/*
 * =============================================================================
 * REGISTER READ / WRITE UTILITIES
 * =============================================================================
 */

static bool max30102_write_reg(uint8_t reg, uint8_t val)
{
    max30102_i2c_start();

    if (!max30102_i2c_write_byte(MAX30102_I2C_ADDR_WRITE))
    {
        max30102_i2c_stop();
        return false;
    }

    if (!max30102_i2c_write_byte(reg))
    {
        max30102_i2c_stop();
        return false;
    }

    if (!max30102_i2c_write_byte(val))
    {
        max30102_i2c_stop();
        return false;
    }

    max30102_i2c_stop();
    return true;
}

static bool max30102_read_reg(uint8_t reg, uint8_t *val)
{
    max30102_i2c_start();

    if (!max30102_i2c_write_byte(MAX30102_I2C_ADDR_WRITE))
    {
        max30102_i2c_stop();
        return false;
    }

    if (!max30102_i2c_write_byte(reg))
    {
        max30102_i2c_stop();
        return false;
    }

    max30102_i2c_start();

    if (!max30102_i2c_write_byte(MAX30102_I2C_ADDR_READ))
    {
        max30102_i2c_stop();
        return false;
    }

    *val = max30102_i2c_read_byte(false);

    max30102_i2c_stop();
    return true;
}

static bool max30102_read_bytes(uint8_t reg, uint8_t *data, uint8_t len)
{
    if (len == 0)
        return true;

    max30102_i2c_start();

    if (!max30102_i2c_write_byte(MAX30102_I2C_ADDR_WRITE))
    {
        max30102_i2c_stop();
        return false;
    }

    if (!max30102_i2c_write_byte(reg))
    {
        max30102_i2c_stop();
        return false;
    }

    max30102_i2c_start();

    if (!max30102_i2c_write_byte(MAX30102_I2C_ADDR_READ))
    {
        max30102_i2c_stop();
        return false;
    }

    for (uint8_t i = 0; i < len; i++)
    {
        data[i] = max30102_i2c_read_byte(i < (len - 1));
    }

    max30102_i2c_stop();
    return true;
}

/*
 * =============================================================================
 * SENSOR STATE AND SIGNAL PROCESSING
 * =============================================================================
 */

static bool s_sensor_present = false;

/* Digital filter variables */
static int32_t s_dc_filter = 0;
static int32_t s_lp_buf[3] = {0, 0, 0};
static uint8_t s_lp_idx = 0;
static int32_t s_prev_sample = 0;
static int32_t s_prev2_sample = 0;

static uint32_t s_sample_count = 0;
static uint32_t s_last_peak_sample = 0;
static bool s_finger_present = false;

#define MAX_BEATS_IN_WINDOW 16
static uint8_t s_beat_bpms[MAX_BEATS_IN_WINDOW];
static uint8_t s_beat_count = 0;

static uint8_t s_last_valid_bpm = 0;

/*
 * =============================================================================
 * INITIALIZATION & CONNECTION STATUS
 * =============================================================================
 */

bool max30102_is_connected(void)
{
    uint8_t id = 0;
    if (max30102_read_reg(MAX30102_REG_PART_ID, &id))
    {
        return (id == MAX30102_EXPECTED_PARTID);
    }
    return false;
}

bool max30102_init(void)
{
    /*
     * Soft reset the MAX30102
     */
    max30102_write_reg(MAX30102_REG_MODE_CONFIG, MAX30102_MODE_RESET);

    /*
     * Wait ~10ms for reset to complete
     */
    for (volatile uint32_t i = 0; i < 20000; i++)
    {
        __NOP();
    }

    /*
     * Check Part ID
     */
    uint8_t partid = 0;
    if (!max30102_read_reg(MAX30102_REG_PART_ID, &partid) ||
        partid != MAX30102_EXPECTED_PARTID)
    {
        s_sensor_present = false;
        return false;
    }
    s_sensor_present = true;

    /*
     * FIFO Configuration:
     * SMP_AVE = 000 (no averaging)
     * FIFO_ROLLOVER_EN = 1 (bit 4: rollover enabled)
     * FIFO_A_FULL = 0000
     */
    max30102_write_reg(MAX30102_REG_FIFO_CONFIG, 0x10);

    /*
     * SpO2 Configuration:
     * SPO2_ADC_RGE = 01 (4096 nA full scale, bit [6:5] = 01 -> 0x20)
     * SPO2_SR = 000 (50 samples per second, bits [4:2] = 000 -> 0x00)
     * LED_PW = 11 (411 us, 18-bit resolution, bits [1:0] = 11 -> 0x03)
     * Total = 0x23
     */
    max30102_write_reg(MAX30102_REG_SPO2_CONFIG, 0x23);

    /*
     * LED Pulse Amplitudes (~7.2 mA each)
     */
    max30102_write_reg(MAX30102_REG_LED1_PA, 0x24); /* Red */
    max30102_write_reg(MAX30102_REG_LED2_PA, 0x24); /* IR */

    /*
     * Clear FIFO pointers
     */
    max30102_write_reg(MAX30102_REG_FIFO_WR_PTR, 0x00);
    max30102_write_reg(MAX30102_REG_OVF_COUNTER, 0x00);
    max30102_write_reg(MAX30102_REG_FIFO_RD_PTR, 0x00);

    /*
     * Put sensor in shutdown initially to save battery until measurement is scheduled
     */
    max30102_shutdown();

    return true;
}

void max30102_shutdown(void)
{
    /*
     * Write 0x83 (SHDN = 1, SpO2 mode)
     */
    max30102_write_reg(
        MAX30102_REG_MODE_CONFIG,
        MAX30102_MODE_SHDN | MAX30102_MODE_SPO2
    );
}

void max30102_wakeup(void)
{
    /*
     * Write 0x03 (SHDN = 0, SpO2 mode active)
     */
    max30102_write_reg(
        MAX30102_REG_MODE_CONFIG,
        MAX30102_MODE_SPO2
    );

    /*
     * Clear FIFO pointers for fresh acquisition
     */
    max30102_write_reg(MAX30102_REG_FIFO_WR_PTR, 0x00);
    max30102_write_reg(MAX30102_REG_OVF_COUNTER, 0x00);
    max30102_write_reg(MAX30102_REG_FIFO_RD_PTR, 0x00);
}

uint8_t max30102_get_available_samples(void)
{
    uint8_t wr_ptr = 0;
    uint8_t rd_ptr = 0;

    if (!max30102_read_reg(MAX30102_REG_FIFO_WR_PTR, &wr_ptr) ||
        !max30102_read_reg(MAX30102_REG_FIFO_RD_PTR, &rd_ptr))
    {
        return 0;
    }

    int8_t diff = (int8_t)wr_ptr - (int8_t)rd_ptr;
    if (diff < 0)
    {
        diff += 32;
    }

    return (uint8_t)diff;
}

bool max30102_read_fifo(uint32_t *red, uint32_t *ir)
{
    uint8_t buf[6];

    if (!max30102_read_bytes(MAX30102_REG_FIFO_DATA, buf, 6))
    {
        return false;
    }

    *red = (((uint32_t)(buf[0] & 0x03)) << 16) |
           (((uint32_t)buf[1]) << 8) |
           ((uint32_t)buf[2]);

    *ir  = (((uint32_t)(buf[3] & 0x03)) << 16) |
           (((uint32_t)buf[4]) << 8) |
           ((uint32_t)buf[5]);

    return true;
}

/*
 * =============================================================================
 * SIGNAL PROCESSING & BEAT DETECTION
 * =============================================================================
 */

static void max30102_process_sample(uint32_t red, uint32_t ir)
{
    s_sample_count++;

    /*
     * Check finger presence
     */
    if (ir < MAX30102_FINGER_THRESHOLD)
    {
        return;
    }
    s_finger_present = true;

    /*
     * High-pass DC baseline removal (exponential moving average)
     */
    if (s_dc_filter == 0)
    {
        s_dc_filter = (int32_t)ir;
    }
    else
    {
        s_dc_filter += (((int32_t)ir - s_dc_filter) >> 4);
    }
    int32_t ac = (int32_t)ir - s_dc_filter;

    /*
     * Low-pass smoothing (3-point moving average)
     */
    s_lp_buf[s_lp_idx] = ac;
    s_lp_idx = (s_lp_idx + 1) % 3;
    int32_t filtered_ac = (s_lp_buf[0] + s_lp_buf[1] + s_lp_buf[2]) / 3;

    /*
     * Wait for DC filter to settle (at least 20 samples = 400 ms)
     */
    if (s_sample_count < 20)
    {
        s_prev2_sample = s_prev_sample;
        s_prev_sample = filtered_ac;
        return;
    }

    /*
     * Peak detection with refractory period:
     *
     * Sample rate = 50 Hz -> 20 ms per sample.
     * Minimum interval = 15 samples (300 ms -> 200 BPM).
     * Maximum interval = 75 samples (1500 ms -> 40 BPM).
     */
    if ((s_prev_sample > 25) &&
        (s_prev_sample > s_prev2_sample) &&
        (s_prev_sample >= filtered_ac))
    {
        if (s_last_peak_sample > 0)
        {
            uint32_t interval = s_sample_count - s_last_peak_sample;

            if (interval >= 15 && interval <= 75)
            {
                uint8_t bpm = (uint8_t)((60 * 50) / interval);

                if (bpm >= 45 && bpm <= 180)
                {
                    if (s_beat_count < MAX_BEATS_IN_WINDOW)
                    {
                        s_beat_bpms[s_beat_count++] = bpm;
                    }
                }
            }
        }

        s_last_peak_sample = s_sample_count;
    }

    s_prev2_sample = s_prev_sample;
    s_prev_sample = filtered_ac;
}

/*
 * =============================================================================
 * MEASUREMENT SESSION APIS
 * =============================================================================
 */

void max30102_start_measurement(void)
{
    if (!s_sensor_present)
    {
        if (!max30102_init())
        {
            return;
        }
    }

    max30102_wakeup();

    s_dc_filter = 0;
    s_lp_buf[0] = 0;
    s_lp_buf[1] = 0;
    s_lp_buf[2] = 0;
    s_lp_idx = 0;
    s_prev_sample = 0;
    s_prev2_sample = 0;
    s_sample_count = 0;
    s_last_peak_sample = 0;
    s_finger_present = false;
    s_beat_count = 0;
}

void max30102_poll_fifo(void)
{
    if (!s_sensor_present)
    {
        return;
    }

    uint8_t avail = max30102_get_available_samples();
    if (avail > 16)
    {
        avail = 16;
    }

    for (uint8_t i = 0; i < avail; i++)
    {
        uint32_t red = 0;
        uint32_t ir = 0;

        if (max30102_read_fifo(&red, &ir))
        {
            max30102_process_sample(red, ir);
        }
    }
}

uint8_t max30102_finish_measurement(void)
{
    /*
     * Put sensor to sleep to conserve battery
     */
    max30102_shutdown();

    if (!s_sensor_present || !s_finger_present)
    {
        /*
         * No finger detected on sensor
         */
        return 0;
    }

    if (s_beat_count == 0)
    {
        /*
         * Finger detected, but insufficient clean peaks in 3-second window
         */
        return s_last_valid_bpm;
    }

    /*
     * Average detected beats
     */
    uint32_t sum = 0;
    for (uint8_t i = 0; i < s_beat_count; i++)
    {
        sum += s_beat_bpms[i];
    }

    uint8_t final_bpm = (uint8_t)(sum / s_beat_count);

    if (final_bpm < 40) final_bpm = 40;
    if (final_bpm > 200) final_bpm = 200;

    s_last_valid_bpm = final_bpm;
    return final_bpm;
}

uint8_t max30102_read_bpm_blocking(void)
{
    max30102_start_measurement();

    for (uint8_t step = 0; step < 25; step++)
    {
        SetWord16(WATCHDOG_REG, 0xFF);

        for (volatile uint32_t d = 0; d < 160000; d++)
        {
            __NOP();
        }

        max30102_poll_fifo();
    }

    return max30102_finish_measurement();
}

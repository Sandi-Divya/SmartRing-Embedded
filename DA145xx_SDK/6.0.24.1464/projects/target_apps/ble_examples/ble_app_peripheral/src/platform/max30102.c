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
#define LP_FILTER_TAPS 5
static int32_t s_lp_buf[LP_FILTER_TAPS] = {0};
static uint8_t s_lp_idx = 0;
static int32_t s_prev_sample = 0;
static int32_t s_prev2_sample = 0;

static uint32_t s_sample_count = 0;
static uint32_t s_last_peak_sample = 0;
static bool s_finger_present = false;

/* Dynamic peak and interval tracker */
static int32_t s_peak_amplitude = 120;
static uint32_t s_avg_interval = 38;

#define MAX_BEATS_IN_WINDOW 16
static uint16_t s_beat_intervals[MAX_BEATS_IN_WINDOW];
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
     * SMP_AVE = 001 (2 samples averaged in hardware)
     * FIFO_ROLLOVER_EN = 1 (bit 4: rollover enabled)
     * FIFO_A_FULL = 0000
     * Register value = 0x20 | 0x10 = 0x30
     */
    max30102_write_reg(MAX30102_REG_FIFO_CONFIG, 0x30);

    /*
     * SpO2 Configuration:
     * SPO2_ADC_RGE = 01 (4096 nA full scale, bit [6:5] = 01 -> 0x20)
     * SPO2_SR = 001 (100 samples per second, bits [4:2] = 001 -> 0x04)
     * LED_PW = 11 (411 us, 18-bit resolution, bits [1:0] = 11 -> 0x03)
     * With 100 Hz ADC sample rate and 2x hardware averaging, output FIFO rate is 50 Hz!
     * Total = 0x20 | 0x04 | 0x03 = 0x27
     */
    max30102_write_reg(MAX30102_REG_SPO2_CONFIG, 0x27);

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
     * High-pass DC baseline removal (exponential moving average: alpha ~ 0.97)
     */
    if (s_dc_filter == 0)
    {
        s_dc_filter = (int32_t)ir;
    }
    else
    {
        s_dc_filter += (((int32_t)ir - s_dc_filter) >> 5);
    }
    int32_t ac = (int32_t)ir - s_dc_filter;

    /*
     * 5-point weighted triangular low-pass smoothing filter
     * Weights: [1, 2, 4, 2, 1] / 10
     */
    s_lp_buf[s_lp_idx] = ac;
    s_lp_idx = (s_lp_idx + 1) % LP_FILTER_TAPS;

    int32_t filtered_ac = (s_lp_buf[s_lp_idx] +
                           2 * s_lp_buf[(s_lp_idx + 1) % 5] +
                           4 * s_lp_buf[(s_lp_idx + 2) % 5] +
                           2 * s_lp_buf[(s_lp_idx + 3) % 5] +
                           s_lp_buf[(s_lp_idx + 4) % 5]) / 10;

    /*
     * Allow filter to settle during the first 35 samples (~700 ms)
     */
    if (s_sample_count < 35)
    {
        s_prev2_sample = s_prev_sample;
        s_prev_sample = filtered_ac;
        return;
    }

    /*
     * Adaptive dynamic threshold:
     * Dicrotic notch / diastolic wave amplitude is typically <= 45% of systolic peak.
     * We set the threshold to 60% of estimated systolic peak amplitude.
     */
    int32_t dyn_threshold = (s_peak_amplitude * 60) / 100;
    if (dyn_threshold < 40)
    {
        dyn_threshold = 40;
    }
    if (dyn_threshold > 2500)
    {
        dyn_threshold = 2500;
    }

    /*
     * Adaptive refractory period:
     * A true heartbeat cannot occur sooner than 60% of the average beat interval.
     * At 50 Hz, clamp between 20 samples (400 ms -> 150 BPM) and 45 samples (900 ms).
     */
    uint32_t refractory_samples = (s_avg_interval * 60) / 100;
    if (refractory_samples < 20)
    {
        refractory_samples = 20;
    }
    if (refractory_samples > 45)
    {
        refractory_samples = 45;
    }

    /*
     * Peak detection criteria:
     * 1. Previous sample is a local maximum (prev > prev2 and prev >= current).
     * 2. Previous sample exceeds dynamic threshold (strictly above dicrotic notch).
     * 3. Time since last peak exceeds adaptive refractory period.
     */
    if ((s_prev_sample >= dyn_threshold) &&
        (s_prev_sample > s_prev2_sample) &&
        (s_prev_sample >= filtered_ac))
    {
        if (s_last_peak_sample > 0)
        {
            uint32_t interval = s_sample_count - s_last_peak_sample;

            /*
             * Accept valid interval between 40 BPM (75 samples) and 180 BPM (16 samples)
             */
            if ((interval >= refractory_samples) && (interval <= 75))
            {
                if (s_beat_count < MAX_BEATS_IN_WINDOW)
                {
                    s_beat_intervals[s_beat_count++] = (uint16_t)interval;
                }

                /*
                 * Update running average interval
                 */
                s_avg_interval = (s_avg_interval * 3 + interval) / 4;

                /*
                 * Update running peak amplitude
                 */
                s_peak_amplitude = (s_peak_amplitude * 3 + s_prev_sample) / 4;
            }
        }
        else
        {
            /* First peak detected in this window */
            s_peak_amplitude = (s_peak_amplitude * 3 + s_prev_sample) / 4;
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
    for (uint8_t i = 0; i < LP_FILTER_TAPS; i++)
    {
        s_lp_buf[i] = 0;
    }
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
         * Finger detected, but insufficient clean peaks in measurement window
         */
        return s_last_valid_bpm;
    }

    /*
     * Calculate interval with outlier and noise rejection
     */
    uint32_t calc_interval = 0;

    if (s_beat_count == 1)
    {
        calc_interval = s_beat_intervals[0];
    }
    else if (s_beat_count == 2)
    {
        calc_interval = (s_beat_intervals[0] + s_beat_intervals[1]) / 2;
    }
    else
    {
        /*
         * 3 or more beat intervals: apply median and outlier rejection.
         * Simple in-place insertion sort of intervals:
         */
        uint16_t sorted[MAX_BEATS_IN_WINDOW];
        for (uint8_t i = 0; i < s_beat_count; i++)
        {
            sorted[i] = s_beat_intervals[i];
        }

        for (uint8_t i = 1; i < s_beat_count; i++)
        {
            uint16_t key = sorted[i];
            int8_t j = (int8_t)i - 1;
            while (j >= 0 && sorted[j] > key)
            {
                sorted[j + 1] = sorted[j];
                j--;
            }
            sorted[j + 1] = key;
        }

        uint16_t median = sorted[s_beat_count / 2];

        /*
         * Average intervals that are within 25% of median (reject motion glitches)
         */
        uint32_t sum = 0;
        uint8_t valid_cnt = 0;
        uint16_t delta_max = (median * 25) / 100;

        for (uint8_t i = 0; i < s_beat_count; i++)
        {
            uint16_t diff = (s_beat_intervals[i] > median) ?
                            (s_beat_intervals[i] - median) :
                            (median - s_beat_intervals[i]);

            if (diff <= delta_max)
            {
                sum += s_beat_intervals[i];
                valid_cnt++;
            }
        }

        if (valid_cnt > 0)
        {
            calc_interval = sum / valid_cnt;
        }
        else
        {
            calc_interval = median;
        }
    }

    if (calc_interval == 0)
    {
        return s_last_valid_bpm;
    }

    uint8_t bpm = (uint8_t)(3000 / calc_interval);

    /*
     * Anti-Harmonic Double-Counting Filter:
     * If the calculated BPM is high (>= 125 BPM) while the user's previous baseline
     * is in the resting range (50 - 95 BPM), check if double-counting occurred:
     */
    if (bpm >= 125)
    {
        if (s_last_valid_bpm >= 50 && s_last_valid_bpm <= 95)
        {
            uint8_t half_bpm = bpm / 2;
            int16_t diff = (int16_t)half_bpm - (int16_t)s_last_valid_bpm;
            if (diff < 0) diff = -diff;
            if (diff <= 18)
            {
                /* Harmonic confirmed -> correct to primary fundamental frequency */
                bpm = half_bpm;
            }
        }
    }

    /*
     * Exponential Moving Average (EMA) with previous valid reading:
     * Provides smooth, natural heart rate transitions without erratic single-reading jumps
     */
    uint8_t final_bpm = bpm;
    if (s_last_valid_bpm > 0)
    {
        int16_t step_diff = (int16_t)bpm - (int16_t)s_last_valid_bpm;
        if (step_diff < 0) step_diff = -step_diff;

        if (step_diff > 30)
        {
            /* Large sudden jump: apply dampening */
            final_bpm = (uint8_t)((s_last_valid_bpm * 2 + bpm * 1) / 3);
        }
        else
        {
            /* Normal variance: smooth transition */
            final_bpm = (uint8_t)((s_last_valid_bpm * 1 + bpm * 2) / 3);
        }
    }

    /*
     * Plausibility clamp for human heart rate
     */
    if (final_bpm < 45) final_bpm = 45;
    if (final_bpm > 190) final_bpm = 190;

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

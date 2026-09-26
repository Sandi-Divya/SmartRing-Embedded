/**
 ****************************************************************************************
 *
 * @file max30102.h
 * @brief Driver header for Maxim MAX30102 Pulse Oximeter and Heart-Rate Sensor.
 *
 * Hardware connection to DA14585:
 *      VIN -> 3.3V
 *      GND -> GND
 *      SCL -> P0_0 (shared with SSD1306 OLED)
 *      SDA -> P0_1 (shared with SSD1306 OLED)
 *
 ****************************************************************************************
 */

#ifndef _MAX30102_H_
#define _MAX30102_H_

#include <stdint.h>
#include <stdbool.h>

/*
 * =============================================================================
 * I2C DEFINITIONS
 * =============================================================================
 */

#define MAX30102_I2C_ADDR           0x57
#define MAX30102_I2C_ADDR_WRITE     (0x57 << 1)       /* 0xAE */
#define MAX30102_I2C_ADDR_READ      ((0x57 << 1) | 1) /* 0xAF */

/*
 * =============================================================================
 * REGISTERS
 * =============================================================================
 */

#define MAX30102_REG_INTR_STATUS_1  0x00
#define MAX30102_REG_INTR_STATUS_2  0x01
#define MAX30102_REG_INTR_ENABLE_1  0x02
#define MAX30102_REG_INTR_ENABLE_2  0x03
#define MAX30102_REG_FIFO_WR_PTR    0x04
#define MAX30102_REG_OVF_COUNTER    0x05
#define MAX30102_REG_FIFO_RD_PTR    0x06
#define MAX30102_REG_FIFO_DATA      0x07
#define MAX30102_REG_FIFO_CONFIG    0x08
#define MAX30102_REG_MODE_CONFIG    0x09
#define MAX30102_REG_SPO2_CONFIG    0x0A
#define MAX30102_REG_LED1_PA        0x0C /* Red LED pulse amplitude */
#define MAX30102_REG_LED2_PA        0x0D /* IR LED pulse amplitude */
#define MAX30102_REG_PILOT_PA       0x10
#define MAX30102_REG_MULTI_LED_1    0x11
#define MAX30102_REG_MULTI_LED_2    0x12
#define MAX30102_REG_DIE_TEMP_INT   0x1F
#define MAX30102_REG_DIE_TEMP_FRAC  0x20
#define MAX30102_REG_DIE_TEMP_CFG   0x21
#define MAX30102_REG_REV_ID         0xFE
#define MAX30102_REG_PART_ID        0xFF

#define MAX30102_EXPECTED_PARTID    0x15

/*
 * =============================================================================
 * MODE CONFIGURATION
 * =============================================================================
 */

#define MAX30102_MODE_SHDN          0x80
#define MAX30102_MODE_RESET         0x40
#define MAX30102_MODE_HR_ONLY       0x02
#define MAX30102_MODE_SPO2          0x03
#define MAX30102_MODE_MULTI_LED     0x07

/*
 * Finger threshold on IR channel:
 * If raw IR reading is below this value, no finger is touching the sensor.
 */
#define MAX30102_FINGER_THRESHOLD   10000

/*
 * =============================================================================
 * PUBLIC DRIVER APIS
 * =============================================================================
 */

/**
 * @brief Initialize the MAX30102 sensor.
 * @return true if communication succeeded and Part ID is verified, false otherwise.
 */
bool max30102_init(void);

/**
 * @brief Check if the MAX30102 sensor is connected and responsive.
 * @return true if Part ID matches 0x15.
 */
bool max30102_is_connected(void);

/**
 * @brief Put MAX30102 into ultra-low-power shutdown mode (LEDs turned off).
 */
void max30102_shutdown(void);

/**
 * @brief Wake up MAX30102 and clear FIFO pointers to begin sampling.
 */
void max30102_wakeup(void);

/**
 * @brief Read available sample count currently held in MAX30102 hardware FIFO.
 */
uint8_t max30102_get_available_samples(void);

/**
 * @brief Read a single Red + IR sample (18-bit each) from FIFO.
 */
bool max30102_read_fifo(uint32_t *red, uint32_t *ir);

/**
 * @brief Start a non-blocking measurement session (wakes sensor, resets filters).
 */
void max30102_start_measurement(void);

/**
 * @brief Poll FIFO, read new samples and update digital filters and peak detector.
 *        Called periodically (e.g. every 100 ms) during measurement session.
 */
void max30102_poll_fifo(void);

/**
 * @brief Finish measurement session, put sensor to shutdown, and calculate final BPM.
 * @return BPM value (40 to 200), or 0 if no finger was detected.
 */
uint8_t max30102_finish_measurement(void);

/**
 * @brief Blocking helper to perform a self-contained ~2.5 second measurement.
 * @return BPM value (40 to 200), or 0 if no finger was detected.
 */
uint8_t max30102_read_bpm_blocking(void);

#endif /* _MAX30102_H_ */

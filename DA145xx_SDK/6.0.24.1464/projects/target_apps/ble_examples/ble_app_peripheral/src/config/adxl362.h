#ifndef ADXL362_H
#define ADXL362_H

#include <stdint.h>

#define ADXL362_REG_DEVID_AD       0x00
#define ADXL362_REG_DEVID_MST      0x01
#define ADXL362_REG_PARTID         0x02

#define ADXL362_REG_XDATA_L        0x0E
#define ADXL362_REG_XDATA_H        0x0F
#define ADXL362_REG_YDATA_L        0x10
#define ADXL362_REG_YDATA_H        0x11
#define ADXL362_REG_ZDATA_L        0x12
#define ADXL362_REG_ZDATA_H        0x13

#define ADXL362_REG_POWER_CTL      0x2D

#define ADXL362_CMD_WRITE          0x0A
#define ADXL362_CMD_READ           0x0B
#define ADXL362_CMD_FIFO_READ      0x0D

#define ADXL362_EXPECTED_DEVID_AD  0xAD
#define ADXL362_EXPECTED_PARTID    0xF2

void adxl362_init(void);
uint8_t adxl362_read_register(uint8_t reg);
void adxl362_write_register(uint8_t reg, uint8_t value);
void adxl362_read_xyz(int16_t *x, int16_t *y, int16_t *z);
uint8_t adxl362_is_connected(void);

#endif


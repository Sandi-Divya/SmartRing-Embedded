#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

void display_init(void);

void display_clear(void);

void display_all_on(void);

void display_draw_char(
    uint8_t page,
    uint8_t col,
    char c
);

void display_draw_string(
    uint8_t page,
    uint8_t col,
    const char *str
);

void display_show_time(
    uint8_t hour,
    uint8_t minute
);

void display_show_battery(
    uint8_t percentage
);

void display_show_steps(
    uint16_t count
);

void display_show_accel_status(
    uint8_t connected
);

void display_show_accel_ids(
    uint8_t devid_ad,
    uint8_t devid_mst,
    uint8_t partid
);

void display_show_accel_xyz(
    int16_t x,
    int16_t y,
    int16_t z
);

void display_show_accel_data(
    int16_t x,
    int16_t y,
    int16_t z,
    uint32_t steps,
    uint8_t activity
);

#endif /* DISPLAY_H */
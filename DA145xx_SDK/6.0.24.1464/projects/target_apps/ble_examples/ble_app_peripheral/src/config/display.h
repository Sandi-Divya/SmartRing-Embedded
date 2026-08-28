#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>

void display_init(void);
void display_clear(void);
void display_all_on(void);
void display_draw_char(uint8_t page, uint8_t col, char c);
void display_draw_string(uint8_t page, uint8_t col, const char *str);
void display_show_digit(uint8_t value);
void display_show_digit_at(uint8_t digit_idx, uint8_t value);
void display_set_segments(uint8_t segment_mask);

// Smart Ring UI helpers
void display_show_battery(uint8_t percentage);
void display_show_steps(uint16_t count);
void display_show_ble_write(const char *text);

#endif // _DISPLAY_H_
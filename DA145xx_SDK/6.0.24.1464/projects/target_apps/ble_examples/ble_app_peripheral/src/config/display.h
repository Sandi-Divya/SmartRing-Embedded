#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>

void display_init(void);
void display_clear(void);
void display_all_on(void);

void display_draw_char(uint8_t page, uint8_t col, char c);
void display_draw_string(uint8_t page, uint8_t col, const char *str);

void display_show_time(uint8_t hour, uint8_t minute);
void display_show_battery(uint8_t percentage);
void display_show_steps(uint16_t count);

#endif // _DISPLAY_H_
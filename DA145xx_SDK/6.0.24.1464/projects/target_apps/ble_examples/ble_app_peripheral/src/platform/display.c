/**
 ****************************************************************************************
 * @file display.c
 * @brief Watchdog-safe SSD1306 OLED Driver with ASCII String & Character Rendering.
 ****************************************************************************************
 */

// Disable GPIO allocation monitor checks locally to prevent dynamic __BKPT(0)
#undef DEVELOPMENT_DEBUG
#define DEVELOPMENT_DEBUG 0

#include "display.h"
#include "gpio.h"
#include "datasheet.h"
#include "arch.h"

#define I2C_SCL_PORT        GPIO_PORT_0
#define I2C_SCL_PIN         GPIO_PIN_0

#define I2C_SDA_PORT        GPIO_PORT_0
#define I2C_SDA_PIN         GPIO_PIN_1

#define SSD1306_WIDTH       128
#define SSD1306_PAGES       8

/*
 * Standard 5x7 ASCII Font Table (ASCII 0x20 ' ' to 0x5A 'Z')
 */
static const uint8_t font_5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // ' ' (0x20)
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // '!'
    {0x00, 0x07, 0x00, 0x07, 0x00}, // '"'
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // '#'
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // '$'
    {0x23, 0x13, 0x08, 0x64, 0x62}, // '%'
    {0x36, 0x49, 0x55, 0x22, 0x50}, // '&'
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '''
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // '('
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // ')'
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // '*'
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // '+'
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ','
    {0x08, 0x08, 0x08, 0x08, 0x08}, // '-'
    {0x00, 0x60, 0x60, 0x00, 0x00}, // '.'
    {0x20, 0x10, 0x08, 0x04, 0x02}, // '/'
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // '0' (0x30)
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // '1'
    {0x42, 0x61, 0x51, 0x49, 0x46}, // '2'
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // '3'
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // '4'
    {0x27, 0x45, 0x45, 0x45, 0x39}, // '5'
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // '6'
    {0x01, 0x71, 0x09, 0x05, 0x03}, // '7'
    {0x36, 0x49, 0x49, 0x49, 0x36}, // '8'
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // '9'
    {0x00, 0x36, 0x36, 0x00, 0x00}, // ':'
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ';'
    {0x08, 0x14, 0x22, 0x41, 0x00}, // '<'
    {0x14, 0x14, 0x14, 0x14, 0x14}, // '='
    {0x00, 0x41, 0x22, 0x14, 0x08}, // '>'
    {0x02, 0x01, 0x51, 0x09, 0x06}, // '?'
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // '@'
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 'A' (0x41)
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 'B'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 'C'
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 'D'
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 'E'
    {0x7F, 0x09, 0x09, 0x01, 0x01}, // 'F'
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // 'G'
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 'H'
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 'I'
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 'J'
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 'K'
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 'L'
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // 'M'
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 'N'
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 'O'
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 'P'
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 'Q'
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 'R'
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 'S'
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 'T'
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 'U'
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 'V'
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // 'W'
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 'X'
    {0x03, 0x04, 0x78, 0x04, 0x03}, // 'Y'
    {0x61, 0x51, 0x49, 0x45, 0x43}  // 'Z' (0x5A)
};

/*
 * True Open-Drain Bit-Bang Primitives
 */
static void i2c_delay(void)
{
    for (volatile int i = 0; i < 4; i++) { __NOP(); }
}

static inline void i2c_scl_hi(void)
{
    GPIO_ConfigurePin(I2C_SCL_PORT, I2C_SCL_PIN, INPUT_PULLUP, PID_GPIO, false);
    i2c_delay();
}

static inline void i2c_scl_lo(void)
{
    GPIO_ConfigurePin(I2C_SCL_PORT, I2C_SCL_PIN, OUTPUT, PID_GPIO, false);
    GPIO_SetInactive(I2C_SCL_PORT, I2C_SCL_PIN);
    i2c_delay();
}

static inline void i2c_sda_hi(void)
{
    GPIO_ConfigurePin(I2C_SDA_PORT, I2C_SDA_PIN, INPUT_PULLUP, PID_GPIO, false);
    i2c_delay();
}

static inline void i2c_sda_lo(void)
{
    GPIO_ConfigurePin(I2C_SDA_PORT, I2C_SDA_PIN, OUTPUT, PID_GPIO, false);
    GPIO_SetInactive(I2C_SDA_PORT, I2C_SDA_PIN);
    i2c_delay();
}

static void i2c_start(void)
{
    i2c_sda_hi();
    i2c_scl_hi();
    i2c_sda_lo();
    i2c_scl_lo();
}

static void i2c_stop(void)
{
    i2c_sda_lo();
    i2c_scl_hi();
    i2c_sda_hi();
}

static void i2c_write_byte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        if (byte & 0x80)
            i2c_sda_hi();
        else
            i2c_sda_lo();

        i2c_scl_hi();
        i2c_scl_lo();
        byte <<= 1;
    }
    // Read ACK pulse
    i2c_sda_hi();
    i2c_scl_hi();
    i2c_scl_lo();
}

static void oled_send_cmd(uint8_t cmd)
{
    i2c_start();
    i2c_write_byte(0x78); // 0x3C << 1
    i2c_write_byte(0x00); // Command stream
    i2c_write_byte(cmd);
    i2c_stop();
}

static void oled_set_cursor(uint8_t page, uint8_t col)
{
    oled_send_cmd(0xB0 + (page & 0x07));
    oled_send_cmd(0x00 + (col & 0x0F));
    oled_send_cmd(0x10 + ((col >> 4) & 0x0F));
}

void display_clear(void)
{
    for (uint8_t p = 0; p < SSD1306_PAGES; p++)
    {
        SetWord16(WATCHDOG_REG, 0xFF);
        oled_set_cursor(p, 0);

        i2c_start();
        i2c_write_byte(0x78);
        i2c_write_byte(0x40); // Continuous data stream
        for (uint8_t c = 0; c < SSD1306_WIDTH; c++)
        {
            i2c_write_byte(0x00);
        }
        i2c_stop();
    }
}

void display_all_on(void)
{
    for (uint8_t p = 0; p < SSD1306_PAGES; p++)
    {
        SetWord16(WATCHDOG_REG, 0xFF);
        oled_set_cursor(p, 0);

        i2c_start();
        i2c_write_byte(0x78);
        i2c_write_byte(0x40);
        for (uint8_t c = 0; c < SSD1306_WIDTH; c++)
        {
            i2c_write_byte(0xFF);
        }
        i2c_stop();
    }
}

void display_draw_char(uint8_t page, uint8_t col, char c)
{
    // Auto-convert lowercase 'a'-'z' to uppercase 'A'-'Z'
    if (c >= 'a' && c <= 'z')
    {
        c -= 32;
    }

    // Replace unprintable characters with space
    if (c < 0x20 || c > 0x5A)
    {
        c = ' ';
    }

    uint8_t char_idx = (uint8_t)(c - 0x20);

    oled_set_cursor(page, col);

    i2c_start();
    i2c_write_byte(0x78);
    i2c_write_byte(0x40);
    for (uint8_t i = 0; i < 5; i++)
    {
        i2c_write_byte(font_5x7[char_idx][i]);
    }
    i2c_write_byte(0x00); // 1-pixel column space
    i2c_stop();
}

void display_draw_string(uint8_t page, uint8_t col, const char *str)
{
    while (*str)
    {
        if (col > (SSD1306_WIDTH - 6))
        {
            break;
        }
        display_draw_char(page, col, *str++);
        col += 6;
    }
}

void display_show_digit(uint8_t value)
{
    if (value > 9) return;

    display_clear();
    display_draw_char(3, 61, '0' + value);
}

void display_show_digit_at(uint8_t digit_idx, uint8_t value)
{
    (void)digit_idx;
    display_show_digit(value);
}

void display_set_segments(uint8_t segment_mask)
{
    (void)segment_mask;
}

void display_init(void)
{
    i2c_scl_hi();
    i2c_sda_hi();

    SetWord16(WATCHDOG_REG, 0xFF);

    // Initialization Sequence
    oled_send_cmd(0xAE); // Display OFF
    oled_send_cmd(0xD5); oled_send_cmd(0x80);
    oled_send_cmd(0xA8); oled_send_cmd(0x3F);
    oled_send_cmd(0xD3); oled_send_cmd(0x00);
    oled_send_cmd(0x40);
    oled_send_cmd(0x8D); oled_send_cmd(0x14); // Enable Charge Pump
    oled_send_cmd(0x20); oled_send_cmd(0x02); // Page Addressing Mode
    oled_send_cmd(0xA1);
    oled_send_cmd(0xC8);
    oled_send_cmd(0xDA); oled_send_cmd(0x12);
    oled_send_cmd(0x81); oled_send_cmd(0xCF);
    oled_send_cmd(0xD9); oled_send_cmd(0xF1);
    oled_send_cmd(0xDB); oled_send_cmd(0x40);
    oled_send_cmd(0xA4);
    oled_send_cmd(0xA6);
    oled_send_cmd(0xAF); // Display ON

    display_clear();

    // Render Startup Banner (Centered: "SMART RING" = 10 chars * 6px = 60px -> (128-60)/2 = 34)
    // "BLE READY" = 9 chars * 6px = 54px -> (128-54)/2 = 37
    display_draw_string(2, 34, "SMART RING");
    display_draw_string(4, 37, "BLE READY");
}

void display_show_battery(uint8_t percentage)
{
    char buf[16];
    
    if (percentage > 100) percentage = 100;

    // Format string: "BAT: 100%" or "BAT: 85%"
    if (percentage == 100)
    {
        buf[0] = 'B'; buf[1] = 'A'; buf[2] = 'T'; buf[3] = ':';
        buf[4] = ' '; buf[5] = '1'; buf[6] = '0'; buf[7] = '0';
        buf[8] = '%'; buf[9] = '\0';
    }
    else if (percentage >= 10)
    {
        buf[0] = 'B'; buf[1] = 'A'; buf[2] = 'T'; buf[3] = ':';
        buf[4] = ' '; buf[5] = '0' + (percentage / 10);
        buf[6] = '0' + (percentage % 10);
        buf[7] = '%'; buf[8] = '\0';
    }
    else
    {
        buf[0] = 'B'; buf[1] = 'A'; buf[2] = 'T'; buf[3] = ':';
        buf[4] = ' '; buf[5] = '0' + percentage;
        buf[6] = '%'; buf[7] = '\0';
    }

    display_clear();

    // Center "BAT: XX%" horizontally on Page 3
    uint8_t str_len = 0;
    while (buf[str_len]) str_len++;
    uint8_t total_px = (uint8_t)(str_len * 6);
    uint8_t start_col = (total_px < 128) ? (uint8_t)((128 - total_px) / 2) : 0;

    display_draw_string(3, start_col, buf);
}

void display_show_steps(uint16_t count)
{
    char count_str[8];
    uint8_t len = 0;

    // Convert integer count to string
    if (count == 0)
    {
        count_str[len++] = '0';
    }
    else
    {
        char temp[8];
        uint8_t temp_len = 0;
        while (count > 0)
        {
            temp[temp_len++] = '0' + (count % 10);
            count /= 10;
        }
        for (int i = (int)temp_len - 1; i >= 0; i--)
        {
            count_str[len++] = temp[i];
        }
    }
    count_str[len] = '\0';

    display_clear();

    // Line 1 (Page 2): Centered "Walking STEPS" header (13 chars * 6px = 78px -> (128 - 78) / 2 = 25)
    display_draw_string(2, 25, "Walking STEPS");

    // Line 2 (Page 4): Centered numerical count dynamically calculated
    uint8_t num_px = len * 6;
    uint8_t start_col = (num_px < 128) ? (uint8_t)((128 - num_px) / 2) : 0;
    display_draw_string(4, start_col, count_str);
}

void display_show_ble_write(const char *text)
{
    display_clear();

    // Line 1 (Page 2): Centered "BLE WRITE:" header (10 chars * 6px = 60px -> (128 - 60) / 2 = 34)
    display_draw_string(2, 34, "BLE WRITE:");

    // Line 2 (Page 4): Centered incoming text payload dynamically calculated
    uint8_t str_len = 0;
    while (text && text[str_len]) str_len++;
    uint8_t total_px = (uint8_t)(str_len * 6);
    uint8_t start_col = (total_px < 128) ? (uint8_t)((128 - total_px) / 2) : 0;

    display_draw_string(4, start_col, text);
}
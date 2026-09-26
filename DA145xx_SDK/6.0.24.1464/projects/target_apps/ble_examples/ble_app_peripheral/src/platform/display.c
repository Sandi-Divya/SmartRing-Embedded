/**
 ****************************************************************************************
 *
 * @file display.c
 * @brief Watchdog-safe SSD1306 OLED Driver.
 *
 * OLED:
 *      SCL -> P0_0
 *      SDA -> P0_1
 *
 * UI:
 *      - OLED OFF after boot
 *      - Current time
 *      - Battery percentage
 *      - Heart rate
 *      - ADXL362 X/Y/Z values
 *      - Step count
 *      - Activity state
 *
 * No startup message is displayed.
 *
 * Framebuffer update is used for the accelerometer/activity screen
 * to reduce visible flickering during periodic refresh.
 *
 ****************************************************************************************
 */

#undef DEVELOPMENT_DEBUG
#define DEVELOPMENT_DEBUG 0

#include "display.h"
#include "gpio.h"
#include "datasheet.h"
#include "arch.h"
#include "sleep_tracker.h"


#define I2C_SCL_PORT GPIO_PORT_0
#define I2C_SCL_PIN  GPIO_PIN_0

#define I2C_SDA_PORT GPIO_PORT_0
#define I2C_SDA_PIN  GPIO_PIN_1


#define SSD1306_WIDTH 128
#define SSD1306_PAGES 8


/*
 * =============================================================================
 * OLED FRAMEBUFFER
 * =============================================================================
 *
 * 128 columns x 8 pages = 1024 bytes.
 *
 * Instead of clearing and redrawing the physical OLED every time,
 * the new screen is first constructed in this RAM buffer.
 *
 * The complete frame is then sent to the OLED.
 *
 * This prevents the visible blank-screen period that caused flickering.
 *
 * =============================================================================
 */

static uint8_t oled_buffer[
    SSD1306_WIDTH * SSD1306_PAGES
];


/*
 * =============================================================================
 * 5x7 FONT
 * =============================================================================
 */

static const uint8_t font_5x7[][5] =
{
    {0x00,0x00,0x00,0x00,0x00}, // space
    {0x00,0x00,0x5F,0x00,0x00}, // !
    {0x00,0x07,0x00,0x07,0x00}, // "
    {0x14,0x7F,0x14,0x7F,0x14}, // #
    {0x24,0x2A,0x7F,0x2A,0x12}, // $
    {0x23,0x13,0x08,0x64,0x62}, // %
    {0x36,0x49,0x55,0x22,0x50}, // &
    {0x00,0x05,0x03,0x00,0x00}, // '
    {0x00,0x1C,0x22,0x41,0x00}, // (
    {0x00,0x41,0x22,0x1C,0x00}, // )
    {0x08,0x2A,0x1C,0x2A,0x08}, // *
    {0x08,0x08,0x3E,0x08,0x08}, // +
    {0x00,0x50,0x30,0x00,0x00}, // ,
    {0x08,0x08,0x08,0x08,0x08}, // -
    {0x00,0x60,0x60,0x00,0x00}, // .
    {0x20,0x10,0x08,0x04,0x02}, // /

    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9

    {0x00,0x36,0x36,0x00,0x00}, // :
    {0x00,0x56,0x36,0x00,0x00}, // ;
    {0x08,0x14,0x22,0x41,0x00}, // <
    {0x14,0x14,0x14,0x14,0x14}, // =
    {0x00,0x41,0x22,0x14,0x08}, // >
    {0x02,0x01,0x51,0x09,0x06}, // ?
    {0x32,0x49,0x79,0x41,0x3E}, // @

    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x01,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x7F,0x20,0x18,0x20,0x7F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x03,0x04,0x78,0x04,0x03}, // Y
    {0x61,0x51,0x49,0x45,0x43}  // Z
};


/*
 * =============================================================================
 * SOFTWARE I2C
 * =============================================================================
 */

static void i2c_delay(void)
{
    for (volatile int i = 0; i < 4; i++)
    {
        __NOP();
    }
}


static inline void i2c_scl_hi(void)
{
    GPIO_ConfigurePin(
        I2C_SCL_PORT,
        I2C_SCL_PIN,
        INPUT_PULLUP,
        PID_GPIO,
        false
    );

    i2c_delay();
}


static inline void i2c_scl_lo(void)
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

    i2c_delay();
}


static inline void i2c_sda_hi(void)
{
    GPIO_ConfigurePin(
        I2C_SDA_PORT,
        I2C_SDA_PIN,
        INPUT_PULLUP,
        PID_GPIO,
        false
    );

    i2c_delay();
}


static inline void i2c_sda_lo(void)
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
        {
            i2c_sda_hi();
        }
        else
        {
            i2c_sda_lo();
        }

        i2c_scl_hi();
        i2c_scl_lo();

        byte <<= 1;
    }

    /*
     * ACK clock.
     */
    i2c_sda_hi();
    i2c_scl_hi();
    i2c_scl_lo();
}


/*
 * =============================================================================
 * SSD1306 COMMANDS
 * =============================================================================
 */

static void oled_send_cmd(uint8_t cmd)
{
    i2c_start();

    /*
     * SSD1306 7-bit address = 0x3C
     * Write address = 0x78
     */
    i2c_write_byte(0x78);

    /*
     * Command stream.
     */
    i2c_write_byte(0x00);
    i2c_write_byte(cmd);

    i2c_stop();
}


static void oled_set_cursor(
    uint8_t page,
    uint8_t col
)
{
    oled_send_cmd(
        0xB0 + (page & 0x07)
    );

    oled_send_cmd(
        0x00 + (col & 0x0F)
    );

    oled_send_cmd(
        0x10 + ((col >> 4) & 0x0F)
    );
}


/*
 * =============================================================================
 * CLEAR DISPLAY
 * =============================================================================
 */

void display_clear(void)
{
    for (uint8_t p = 0; p < SSD1306_PAGES; p++)
    {
        /*
         * Keep watchdog alive during long OLED transfer.
         */
        SetWord16(
            WATCHDOG_REG,
            0xFF
        );

        oled_set_cursor(
            p,
            0
        );

        i2c_start();

        i2c_write_byte(0x78);

        /*
         * Data stream.
         */
        i2c_write_byte(0x40);

        for (uint8_t c = 0;
             c < SSD1306_WIDTH;
             c++)
        {
            i2c_write_byte(0x00);
        }

        i2c_stop();
    }
}


/*
 * =============================================================================
 * ALL ON
 * =============================================================================
 */

void display_all_on(void)
{
    for (uint8_t p = 0;
         p < SSD1306_PAGES;
         p++)
    {
        SetWord16(
            WATCHDOG_REG,
            0xFF
        );

        oled_set_cursor(
            p,
            0
        );

        i2c_start();

        i2c_write_byte(0x78);
        i2c_write_byte(0x40);

        for (uint8_t c = 0;
             c < SSD1306_WIDTH;
             c++)
        {
            i2c_write_byte(0xFF);
        }

        i2c_stop();
    }
}


/*
 * =============================================================================
 * TEXT
 * =============================================================================
 */

void display_draw_char(
    uint8_t page,
    uint8_t col,
    char c
)
{
    /*
     * Convert lowercase to uppercase.
     */
    if (c >= 'a' && c <= 'z')
    {
        c -= 32;
    }

    /*
     * Unsupported character -> space.
     */
    if (c < 0x20 || c > 0x5A)
    {
        c = ' ';
    }

    uint8_t char_idx =
        (uint8_t)(c - 0x20);

    oled_set_cursor(
        page,
        col
    );

    i2c_start();

    i2c_write_byte(0x78);

    /*
     * Data stream.
     */
    i2c_write_byte(0x40);

    /*
     * 5x7 font.
     */
    for (uint8_t i = 0; i < 5; i++)
    {
        i2c_write_byte(
            font_5x7[char_idx][i]
        );
    }

    /*
     * Character spacing.
     */
    i2c_write_byte(0x00);

    i2c_stop();
}


void display_draw_string(
    uint8_t page,
    uint8_t col,
    const char *str
)
{
    if (str == NULL)
    {
        return;
    }

    while (*str)
    {
        if (col > (SSD1306_WIDTH - 6))
        {
            break;
        }

        display_draw_char(
            page,
            col,
            *str++
        );

        col += 6;
    }
}


/*
 * =============================================================================
 * OLED INITIALIZATION
 * =============================================================================
 */

void display_init(void)
{
    /*
     * I2C idle state.
     */
    i2c_scl_hi();
    i2c_sda_hi();

    SetWord16(
        WATCHDOG_REG,
        0xFF
    );

    /*
     * SSD1306 initialization.
     */
    oled_send_cmd(0xAE); /* Display OFF */

    oled_send_cmd(0xD5);
    oled_send_cmd(0x80);

    oled_send_cmd(0xA8);
    oled_send_cmd(0x3F);

    oled_send_cmd(0xD3);
    oled_send_cmd(0x00);

    oled_send_cmd(0x40);

    oled_send_cmd(0x8D);
    oled_send_cmd(0x14);

    oled_send_cmd(0x20);
    oled_send_cmd(0x02);

    oled_send_cmd(0xA1);
    oled_send_cmd(0xC8);

    oled_send_cmd(0xDA);
    oled_send_cmd(0x12);

    oled_send_cmd(0x81);
    oled_send_cmd(0xCF);

    oled_send_cmd(0xD9);
    oled_send_cmd(0xF1);

    oled_send_cmd(0xDB);
    oled_send_cmd(0x40);

    oled_send_cmd(0xA4);
    oled_send_cmd(0xA6);

    /*
     * Clear RAM.
     */
    oled_send_cmd(0xAF);

    display_clear();

    /*
     * IMPORTANT:
     *
     * DO NOT show startup screen.
     *
     * Turn display OFF after clearing.
     */
    oled_send_cmd(0xAE);
}


/*
 * =============================================================================
 * TIME
 * =============================================================================
 */

void display_show_time(
    uint8_t hour,
    uint8_t minute
)
{
    char buf[6];

    /*
     * If clock has not been synchronized yet,
     * show --:--
     */
    if ((hour > 23) ||
        (minute > 59))
    {
        buf[0] = '-';
        buf[1] = '-';
        buf[2] = ':';
        buf[3] = '-';
        buf[4] = '-';
        buf[5] = '\0';
    }
    else
    {
        /*
         * HH:MM
         */
        buf[0] =
            '0' + (hour / 10);

        buf[1] =
            '0' + (hour % 10);

        buf[2] = ':';

        buf[3] =
            '0' + (minute / 10);

        buf[4] =
            '0' + (minute % 10);

        buf[5] = '\0';
    }

    /*
     * Turn OLED ON.
     */
    oled_send_cmd(0xAF);

    display_clear();

    /*
     * Center HH:MM / --:--
     *
     * 5 characters x 6 pixels = 30 pixels
     */
    display_draw_string(
        3,
        49,
        buf
    );
}


/*
 * =============================================================================
 * BATTERY
 * =============================================================================
 */

void display_show_battery(
    uint8_t percentage
)
{
    char buf[16];

    if (percentage > 100)
    {
        percentage = 100;
    }

    if (percentage == 100)
    {
        buf[0] = 'B';
        buf[1] = 'A';
        buf[2] = 'T';
        buf[3] = ':';
        buf[4] = ' ';
        buf[5] = '1';
        buf[6] = '0';
        buf[7] = '0';
        buf[8] = '%';
        buf[9] = '\0';
    }
    else if (percentage >= 10)
    {
        buf[0] = 'B';
        buf[1] = 'A';
        buf[2] = 'T';
        buf[3] = ':';
        buf[4] = ' ';
        buf[5] =
            '0' + (percentage / 10);
        buf[6] =
            '0' + (percentage % 10);
        buf[7] = '%';
        buf[8] = '\0';
    }
    else
    {
        buf[0] = 'B';
        buf[1] = 'A';
        buf[2] = 'T';
        buf[3] = ':';
        buf[4] = ' ';
        buf[5] =
            '0' + percentage;
        buf[6] = '%';
        buf[7] = '\0';
    }

    /*
     * Turn OLED ON.
     */
    oled_send_cmd(0xAF);

    display_clear();

    /*
     * Calculate string length.
     */
    uint8_t str_len = 0;

    while (buf[str_len])
    {
        str_len++;
    }

    /*
     * 6 pixels per character.
     */
    uint8_t total_px =
        (uint8_t)(str_len * 6);

    uint8_t start_col =
        (total_px < 128)
        ? (uint8_t)((128 - total_px) / 2)
        : 0;

    display_draw_string(
        3,
        start_col,
        buf
    );
}


/*
 * =============================================================================
 * HEART RATE
 * =============================================================================
 *
 * The existing function name is retained so your
 * user_peripheral.c does not need to change.
 *
 * =============================================================================
 */

void display_show_steps(
    uint16_t count
)
{
    char count_str[16];
    uint8_t len = 0;

    /*
     * Convert number to ASCII with BPM suffix.
     */
    if (count == 0)
    {
        count_str[0] = '-';
        count_str[1] = '-';
        count_str[2] = ' ';
        count_str[3] = 'B';
        count_str[4] = 'P';
        count_str[5] = 'M';
        count_str[6] = '\0';
        len = 6;
    }
    else
    {
        char temp[8];
        uint8_t temp_len = 0;
        uint16_t val = count;

        while ((val > 0) &&
               (temp_len < sizeof(temp)))
        {
            temp[temp_len++] =
                '0' + (val % 10);

            val /= 10;
        }

        /*
         * Reverse digits.
         */
        while (temp_len > 0)
        {
            count_str[len++] =
                temp[--temp_len];
        }

        count_str[len++] = ' ';
        count_str[len++] = 'B';
        count_str[len++] = 'P';
        count_str[len++] = 'M';
        count_str[len] = '\0';
    }

    /*
     * Turn OLED ON.
     */
    oled_send_cmd(0xAF);

    display_clear();

    /*
     * Heart Rate title (centered: 10 chars * 6 px = 60 px, (128 - 60) / 2 = 34).
     */
    display_draw_string(
        2,
        34,
        "Heart Rate"
    );

    /*
     * Center heart-rate number with BPM.
     */
    uint8_t num_px =
        (uint8_t)(len * 6);

    uint8_t start_col =
        (num_px < 128)
        ? (uint8_t)((128 - num_px) / 2)
        : 0;

    display_draw_string(
        4,
        start_col,
        count_str
    );
}


/*
 * =============================================================================
 * ADXL362 STATUS
 * =============================================================================
 */

void display_show_accel_status(
    uint8_t connected
)
{
    oled_send_cmd(0xAF);

    display_clear();

    if (connected)
    {
        display_draw_string(
            2,
            31,
            "ADXL362"
        );

        display_draw_string(
            4,
            43,
            "OK"
        );
    }
    else
    {
        display_draw_string(
            2,
            31,
            "ADXL362"
        );

        display_draw_string(
            4,
            37,
            "FAIL"
        );
    }
}


/*
 * =============================================================================
 * ADXL362 ID DISPLAY
 * =============================================================================
 */

void display_show_accel_ids(
    uint8_t devid_ad,
    uint8_t devid_mst,
    uint8_t partid
)
{
    char buf[20];

    oled_send_cmd(0xAF);

    display_clear();

    display_draw_string(
        0,
        25,
        "ADXL362 ID"
    );

    display_draw_string(
        2,
        10,
        "AD:"
    );

    buf[0] =
        "0123456789ABCDEF"[
            (devid_ad >> 4) & 0x0F
        ];

    buf[1] =
        "0123456789ABCDEF"[
            devid_ad & 0x0F
        ];

    buf[2] = '\0';

    display_draw_string(
        2,
        35,
        buf
    );

    display_draw_string(
        4,
        10,
        "MS:"
    );

    buf[0] =
        "0123456789ABCDEF"[
            (devid_mst >> 4) & 0x0F
        ];

    buf[1] =
        "0123456789ABCDEF"[
            devid_mst & 0x0F
        ];

    buf[2] = '\0';

    display_draw_string(
        4,
        35,
        buf
    );

    display_draw_string(
        6,
        10,
        "PT:"
    );

    buf[0] =
        "0123456789ABCDEF"[
            (partid >> 4) & 0x0F
        ];

    buf[1] =
        "0123456789ABCDEF"[
            partid & 0x0F
        ];

    buf[2] = '\0';

    display_draw_string(
        6,
        35,
        buf
    );
}


/*
 * =============================================================================
 * ADXL362 X/Y/Z DISPLAY
 * =============================================================================
 */

static void display_accel_int_to_string(
    int16_t value,
    char *buffer
)
{
    char temp[8];

    uint8_t index = 0;

    uint8_t output_index = 0;

    uint16_t magnitude;


    /*
     * Negative value.
     */
    if (value < 0)
    {
        buffer[output_index++] = '-';

        magnitude =
            (uint16_t)(-(int32_t)value);
    }
    else
    {
        magnitude =
            (uint16_t)value;
    }


    /*
     * Zero.
     */
    if (magnitude == 0)
    {
        buffer[output_index++] = '0';

        buffer[output_index] = '\0';

        return;
    }


    /*
     * Generate digits backwards.
     */
    while (magnitude > 0)
    {
        temp[index++] =
            (char)('0' + (magnitude % 10));

        magnitude =
            magnitude / 10;
    }


    /*
     * Reverse digits.
     */
    while (index > 0)
    {
        index--;

        buffer[output_index++] =
            temp[index];
    }

    buffer[output_index] = '\0';
}


/*
 * =============================================================================
 * ADXL362 X/Y/Z
 * =============================================================================
 */

void display_show_accel_xyz(
    int16_t x,
    int16_t y,
    int16_t z
)
{
    char x_buf[8];

    char y_buf[8];

    char z_buf[8];

    char x_line[12];

    char y_line[12];

    char z_line[12];

    uint8_t x_len = 0;

    uint8_t y_len = 0;

    uint8_t z_len = 0;

    uint8_t x_col;

    uint8_t y_col;

    uint8_t z_col;


    display_accel_int_to_string(
        x,
        x_buf
    );

    display_accel_int_to_string(
        y,
        y_buf
    );

    display_accel_int_to_string(
        z,
        z_buf
    );


    x_line[0] = 'X';
    x_line[1] = ':';

    while (x_buf[x_len] &&
           x_len < 8)
    {
        x_line[x_len + 2] =
            x_buf[x_len];

        x_len++;
    }

    x_line[x_len + 2] = '\0';


    y_line[0] = 'Y';
    y_line[1] = ':';

    while (y_buf[y_len] &&
           y_len < 8)
    {
        y_line[y_len + 2] =
            y_buf[y_len];

        y_len++;
    }

    y_line[y_len + 2] = '\0';


    z_line[0] = 'Z';
    z_line[1] = ':';

    while (z_buf[z_len] &&
           z_len < 8)
    {
        z_line[z_len + 2] =
            z_buf[z_len];

        z_len++;
    }

    z_line[z_len + 2] = '\0';


    x_col =
        (uint8_t)(
            (128 - ((x_len + 2) * 6)) / 2
        );

    y_col =
        (uint8_t)(
            (128 - ((y_len + 2) * 6)) / 2
        );

    z_col =
        (uint8_t)(
            (128 - ((z_len + 2) * 6)) / 2
        );


    oled_send_cmd(0xAF);

    display_clear();


    display_draw_string(
        0,
        49,
        "ACCEL"
    );


    display_draw_string(
        2,
        x_col,
        x_line
    );


    display_draw_string(
        4,
        y_col,
        y_line
    );


    display_draw_string(
        6,
        z_col,
        z_line
    );
}


/*
 * =============================================================================
 * FRAMEBUFFER CLEAR
 * =============================================================================
 */

static void oled_buffer_clear(void)
{
    uint16_t i;

    for (i = 0;
         i < (SSD1306_WIDTH * SSD1306_PAGES);
         i++)
    {
        oled_buffer[i] = 0x00;
    }
}


/*
 * =============================================================================
 * FRAMEBUFFER CHARACTER
 * =============================================================================
 */

static void oled_buffer_draw_char(
    uint8_t page,
    uint8_t col,
    char c
)
{
    uint8_t char_idx;

    uint8_t i;


    /*
     * Convert lowercase to uppercase.
     */
    if (c >= 'a' && c <= 'z')
    {
        c -= 32;
    }


    /*
     * Unsupported character.
     */
    if (c < 0x20 || c > 0x5A)
    {
        c = ' ';
    }


    char_idx =
        (uint8_t)(c - 0x20);


    /*
     * Prevent writing outside OLED.
     */
    if (page >= SSD1306_PAGES)
    {
        return;
    }


    if (col >= SSD1306_WIDTH)
    {
        return;
    }


    /*
     * Draw 5x7 character into framebuffer.
     */
    for (i = 0; i < 5; i++)
    {
        if ((col + i) < SSD1306_WIDTH)
        {
            oled_buffer[
                (page * SSD1306_WIDTH) +
                col +
                i
            ] =
                font_5x7[char_idx][i];
        }
    }


    /*
     * Character spacing.
     */
    if ((col + 5) < SSD1306_WIDTH)
    {
        oled_buffer[
            (page * SSD1306_WIDTH) +
            col +
            5
        ] = 0x00;
    }
}


/*
 * =============================================================================
 * FRAMEBUFFER STRING
 * =============================================================================
 */

static void oled_buffer_draw_string(
    uint8_t page,
    uint8_t col,
    const char *str
)
{
    if (str == NULL)
    {
        return;
    }


    while (*str)
    {
        if (col > (SSD1306_WIDTH - 6))
        {
            break;
        }


        oled_buffer_draw_char(
            page,
            col,
            *str++
        );


        col += 6;
    }
}


/*
 * =============================================================================
 * FRAMEBUFFER FLUSH
 * =============================================================================
 *
 * Sends the complete RAM framebuffer to the SSD1306.
 *
 * Unlike the old implementation, the physical display is NOT
 * cleared first.
 *
 * =============================================================================
 */

static void oled_buffer_flush(void)
{
    uint8_t page;

    uint16_t index;


    for (page = 0;
         page < SSD1306_PAGES;
         page++)
    {
        /*
         * Keep watchdog alive.
         */
        SetWord16(
            WATCHDOG_REG,
            0xFF
        );


        oled_set_cursor(
            page,
            0
        );


        i2c_start();


        i2c_write_byte(0x78);


        /*
         * Data stream.
         */
        i2c_write_byte(0x40);


        /*
         * Send complete page.
         */
        for (index = 0;
             index < SSD1306_WIDTH;
             index++)
        {
            i2c_write_byte(
                oled_buffer[
                    (page * SSD1306_WIDTH) +
                    index
                ]
            );
        }


        i2c_stop();
    }
}


/*
 * =============================================================================
 * STRING LENGTH
 * =============================================================================
 */

static uint8_t display_string_length(
    const char *str
)
{
    uint8_t length = 0;


    if (str == NULL)
    {
        return 0;
    }


    while (str[length] != '\0')
    {
        length++;


        /*
         * Prevent an accidental runaway.
         */
        if (length >= 127)
        {
            break;
        }
    }


    return length;
}


/*
 * =============================================================================
 * FRAMEBUFFER CENTERED STRING
 * =============================================================================
 */

static void oled_buffer_draw_centered_string(
    uint8_t page,
    const char *str
)
{
    uint8_t length;

    uint8_t total_pixels;

    uint8_t start_col;


    if (str == NULL)
    {
        return;
    }


    length =
        display_string_length(str);


    /*
     * Each character = 5 pixels + 1 pixel spacing.
     */
    total_pixels =
        (uint8_t)(length * 6);


    if (total_pixels >= 128)
    {
        start_col = 0;
    }
    else
    {
        start_col =
            (uint8_t)((128 - total_pixels) / 2);
    }


    oled_buffer_draw_string(
        page,
        start_col,
        str
    );
}


/*
 * =============================================================================
 * FRAMEBUFFER ACCEL LINE
 * =============================================================================
 */

static void oled_buffer_draw_accel_line(
    uint8_t page,
    char axis,
    const char *value
)
{
    char line[16];

    uint8_t index = 0;

    uint8_t length;

    uint8_t total_pixels;

    uint8_t start_col;


    /*
     * Build:
     *
     * X:123
     * Y:-45
     * Z:1024
     */
    line[index++] =
        axis;

    line[index++] =
        ':';


    if (value != NULL)
    {
        uint8_t value_index = 0;


        while ((value[value_index] != '\0') &&
               (index < (sizeof(line) - 1)))
        {
            line[index] =
                value[value_index];

            index++;

            value_index++;
        }
    }


    line[index] =
        '\0';


    /*
     * Center complete line.
     */
    length =
        display_string_length(line);


    total_pixels =
        (uint8_t)(length * 6);


    if (total_pixels >= 128)
    {
        start_col = 0;
    }
    else
    {
        start_col =
            (uint8_t)((128 - total_pixels) / 2);
    }


    oled_buffer_draw_string(
        page,
        start_col,
        line
    );
}


/*
 * =============================================================================
 * ACCEL + STEPS + ACTIVITY
 * =============================================================================
 *
 * Shows:
 *
 *              ACCEL
 *
 *              X:123
 *              Y:-45
 *              Z:1024
 *
 *              STEPS:12
 *
 *              WALK
 *
 * Activity:
 *
 *      REST
 *      WALK
 *      ACTIVE
 *
 * The values are the raw ADXL362 values returned by
 * adxl362_read_xyz().
 *
 * The screen is constructed in the framebuffer first.
 * The completed frame is then transferred to the OLED.
 *
 * This prevents the old:
 *
 *      CLEAR -> DRAW X -> DRAW Y -> DRAW Z...
 *
 * sequence from being visible to the user.
 *
 * =============================================================================
 */

static void display_uint_to_string(
    uint32_t value,
    char *buffer
)
{
    char temp[12];

    uint8_t index = 0;

    uint8_t output_index = 0;


    /*
     * Zero.
     */
    if (value == 0)
    {
        buffer[0] = '0';

        buffer[1] = '\0';

        return;
    }


    /*
     * Generate digits backwards.
     */
    while ((value > 0) &&
           (index < sizeof(temp)))
    {
        temp[index++] =
            (char)(
                '0' +
                (value % 10UL)
            );

        value =
            value / 10UL;
    }


    /*
     * Reverse digits.
     */
    while (index > 0)
    {
        index--;

        buffer[output_index++] =
            temp[index];
    }


    buffer[output_index] =
        '\0';
}


void display_show_accel_data(
    int16_t x,
    int16_t y,
    int16_t z,
    uint32_t steps,
    uint8_t activity
)
{
    char x_buffer[12];

    char y_buffer[12];

    char z_buffer[12];

    char steps_buffer[12];

    char steps_line[20];

    const char *activity_string;


    /*
     * Convert X/Y/Z.
     */
    display_accel_int_to_string(
        x,
        x_buffer
    );

    display_accel_int_to_string(
        y,
        y_buffer
    );

    display_accel_int_to_string(
        z,
        z_buffer
    );


    /*
     * Convert steps.
     */
    display_uint_to_string(
        steps,
        steps_buffer
    );


    /*
     * Select activity text.
     */
    if (activity ==
        SLEEP_TRACKER_ACTIVITY_WALK)
    {
        activity_string =
            "WALK";
    }
    else if (activity ==
             SLEEP_TRACKER_ACTIVITY_MOVE)
    {
        activity_string =
            "JUST MOVE";
    }
    else
    {
        activity_string =
            "SLEEP";
    }


    /*
     * Build:
     *
     * STEPS: 123
     */
    steps_line[0] = 'S';
    steps_line[1] = 'T';
    steps_line[2] = 'E';
    steps_line[3] = 'P';
    steps_line[4] = 'S';
    steps_line[5] = ':';
    steps_line[6] = ' ';


    {
        uint8_t i = 0;


        while ((steps_buffer[i] != '\0') &&
               (i < 11))
        {
            steps_line[7 + i] =
                steps_buffer[i];

            i++;
        }


        steps_line[7 + i] =
            '\0';
    }


    /*
     * -------------------------------------------------------------------------
     * Build the complete next frame in RAM.
     * -------------------------------------------------------------------------
     */

    oled_buffer_clear();


    /*
     * -------------------------------------------------------------------------
     * Title
     * -------------------------------------------------------------------------
     */

    oled_buffer_draw_centered_string(
        0,
        "ACCEL"
    );


    /*
     * -------------------------------------------------------------------------
     * X
     * -------------------------------------------------------------------------
     */

    oled_buffer_draw_accel_line(
        1,
        'X',
        x_buffer
    );


    /*
     * -------------------------------------------------------------------------
     * Y
     * -------------------------------------------------------------------------
     */

    oled_buffer_draw_accel_line(
        2,
        'Y',
        y_buffer
    );


    /*
     * -------------------------------------------------------------------------
     * Z
     * -------------------------------------------------------------------------
     */

    oled_buffer_draw_accel_line(
        3,
        'Z',
        z_buffer
    );


    /*
     * -------------------------------------------------------------------------
     * Steps
     * -------------------------------------------------------------------------
     */

    oled_buffer_draw_centered_string(
        5,
        steps_line
    );


    /*
     * -------------------------------------------------------------------------
     * Activity
     * -------------------------------------------------------------------------
     */

    oled_buffer_draw_centered_string(
        7,
        activity_string
    );


    /*
     * -------------------------------------------------------------------------
     * Send the completed frame to the OLED.
     *
     * IMPORTANT:
     *
     * There is NO display_clear() here.
     *
     * The OLED goes directly from the old frame
     * to the new frame.
     * -------------------------------------------------------------------------
     */

    oled_buffer_flush();
}
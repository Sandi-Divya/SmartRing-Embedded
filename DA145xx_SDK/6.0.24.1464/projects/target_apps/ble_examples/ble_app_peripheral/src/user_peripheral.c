
/**
 ****************************************************************************************
 *
 * @file user_peripheral.c
 *
 * @brief Peripheral application implementation.
 *
 * Smart Ring:
 *      - Battery telemetry
 *      - Heart-rate telemetry
 *      - Manual software clock
 *      - Sequential OLED control
 *      - ADXL362 accelerometer test
 *      - Sleep / activity tracking
 *
 * OLED SEQUENCE:
 *
 *      BOOT
 *        |
 *        v
 *      OLED OFF
 *
 *      LONG PRESS
 *        |
 *        v
 *      TIME
 *
 *      SHORT TAP
 *        |
 *        v
 *      BATTERY
 *
 *      SHORT TAP
 *        |
 *        v
 *      HEART RATE
 *
 *      SHORT TAP
 *        |
 *        v
 *      ACCELEROMETER
 *
 *      SHORT TAP
 *        |
 *        v
 *      OLED OFF
 *
 *      LONG PRESS
 *        |
 *        v
 *      TIME
 *
 ****************************************************************************************
 */

#include "rwip_config.h"

#include <stdint.h>
#include <string.h>

#include "gpio.h"
#include "app.h"
#include "app_api.h"
#include "arch.h"
#include "arch_api.h"
#include "ke_msg.h"
#include "ke_task.h"
#include "ke_timer.h"

#include "gapc_task.h"
#include "gattc_task.h"

#include "custs1.h"
#include "custs1_task.h"
#include "user_custs1_def.h"

#include "user_periph_setup.h"
#include "user_peripheral.h"

#include "adc.h"
#include "display.h"
#include "wkupct_quadec.h"

#include "sleep_tracker.h"
#include "adxl362.h"

#include "ke_env.h"

#define GET_TIMESTAMP()   (SysTick->VAL)


/*
 * ================================================================
 * LATENCY TEST GLOBALS
 * ================================================================
 *
 * Kept for future latency measurement implementation.
 *
 * IMPORTANT:
 * lld_evt_time_get() was removed because it is not exposed/
 * linked in this DA14585 SDK build.
 *
 * ================================================================
 */

volatile uint32_t latency_touch_timestamp = 0;
volatile uint32_t latency_test = 0;


/*
 ****************************************************************************************
 * DEFINITIONS
 ****************************************************************************************
 */

#define LONG_PRESS_TIME             100
#define TOUCH_RELEASE_CHECK_TIME      1
#define TOUCH_REARM_DELAY            10


/*
 ****************************************************************************************
 * TELEMETRY TIMING
 ****************************************************************************************
 */

#define BATTERY_POLL_TIME             18000
#define HR_POLL_TIME                  1500


/*
 ****************************************************************************************
 * BATTERY FIRST-READ DELAY
 ****************************************************************************************
 */

#define BATTERY_FIRST_READ_DELAY      500


/*
 ****************************************************************************************
 * SLEEP TRACKER TIMING
 ****************************************************************************************
 *
 * app_easy_timer() uses 10 ms timer units.
 *
 * 10 = 100 ms
 *
 ****************************************************************************************
 */

#define SLEEP_TRACKER_POLL_TIME       10


/*
 ****************************************************************************************
 * ACCELEROMETER DISPLAY TIMING
 ****************************************************************************************
 *
 * 10 = 100 ms
 *
 * The accelerometer OLED screen is refreshed
 * continuously while DISPLAY_SEQUENCE_ACCEL
 * is active.
 *
 ****************************************************************************************
 */

#define ACCEL_DISPLAY_POLL_TIME       10


/*
 ****************************************************************************************
 * DISPLAY SEQUENCE
 ****************************************************************************************
 */

#define DISPLAY_SEQUENCE_TIME         0
#define DISPLAY_SEQUENCE_BATTERY      1
#define DISPLAY_SEQUENCE_HR           2
#define DISPLAY_SEQUENCE_ACCEL        3

/*
 * TIME -> BATTERY -> HR -> ACCEL -> OFF
 */
uint8_t display_sequence =
    DISPLAY_SEQUENCE_TIME;


/*
 ****************************************************************************************
 * GLOBALS
 ****************************************************************************************
 */

/* BLE connection */
uint8_t app_connection_idx =
    GAP_INVALID_CONIDX;


/*
 ****************************************************************************************
 * BATTERY
 ****************************************************************************************
 */

uint8_t current_batt_lvl =
    0;

timer_hnd app_batt_poll_timer =
    EASY_TIMER_INVALID_TIMER;

static uint8_t last_sent_batt_lvl =
    255;


/*
 ****************************************************************************************
 * HEART RATE
 ****************************************************************************************
 */

uint8_t current_hr_value =
    155;

timer_hnd app_hr_poll_timer =
    EASY_TIMER_INVALID_TIMER;


/*
 ****************************************************************************************
 * CONNECTION PARAMETER UPDATE
 ****************************************************************************************
 */

static timer_hnd app_param_update_request_timer =
    EASY_TIMER_INVALID_TIMER;


/*
 ****************************************************************************************
 * SOFTWARE CLOCK
 ****************************************************************************************
 */

uint8_t manual_clock_hour =
    0;

uint8_t manual_clock_minute =
    0;

uint8_t clock_time_valid =
    0;

timer_hnd app_clock_timer =
    EASY_TIMER_INVALID_TIMER;


/*
 ****************************************************************************************
 * SLEEP / ACTIVITY TRACKER
 ****************************************************************************************
 */

timer_hnd app_sleep_tracker_timer =
    EASY_TIMER_INVALID_TIMER;


/*
 ****************************************************************************************
 * ACCELEROMETER DISPLAY
 ****************************************************************************************
 */

timer_hnd app_accel_display_timer =
    EASY_TIMER_INVALID_TIMER;


/*
 ****************************************************************************************
 * TOUCH STATE
 ****************************************************************************************
 */

uint8_t touch_press_active =
    0;

uint8_t long_press_detected =
    0;

uint8_t touch_long_press_lock =
    0;

timer_hnd app_long_press_timer =
    EASY_TIMER_INVALID_TIMER;

timer_hnd app_touch_release_timer =
    EASY_TIMER_INVALID_TIMER;

timer_hnd app_touch_rearm_timer =
    EASY_TIMER_INVALID_TIMER;


/*
 ****************************************************************************************
 * DISPLAY STATE
 ****************************************************************************************
 */

uint8_t display_is_on =
    0;


/*
 ****************************************************************************************
 * FORWARD DECLARATIONS
 ****************************************************************************************
 */

/* Battery */
static void app_batt_poll_timer_cb(void);
static void start_battery_polling(void);
static void stop_battery_polling(void);


/* Heart rate */
static void app_hr_poll_timer_cb(void);
static void start_hr_polling(void);
static void stop_hr_polling(void);


/* Connection parameters */
static void param_update_request_timer_cb(void);


/* Manual clock */
static void app_clock_timer_cb(void);
static void start_clock(void);
static void stop_clock(void);


/* Sleep / activity tracker */
static void app_sleep_tracker_timer_cb(void);
static void start_sleep_tracker(void);
static void stop_sleep_tracker(void);


/* Accelerometer display */
static void app_accel_display_timer_cb(void);
static void start_accel_display(void);
static void stop_accel_display(void);


/* Touch */
static void touch_button_press_cb(void);
static void touch_button_init(void);
static void long_press_timer_cb(void);
static void touch_release_timer_cb(void);
static void touch_rearm_timer_cb(void);


/* Accelerometer */
static void accelerometer_test(void);
static void accelerometer_verify_device(void);


/*
 ****************************************************************************************
 * BATTERY
 ****************************************************************************************
 */

uint8_t read_battery_level_percentage(void)
{
    uint16_t adc_value;


    adc_value =
        adc_get_vbat_sample(false);


    if (adc_value <= 1200)
        return 0;


    if (adc_value >= 1700)
        return 100;


    return (uint8_t)(
        ((uint32_t)(adc_value - 1200) * 100) / 500
    );
}


/*
 * Battery UUID:
 *
 * 15005991-b131-3396-014c-664c9867b917
 *
 * Handle:
 *
 * SVC1_IDX_ADC_VAL_1_VAL
 *
 * KEEP UNCHANGED.
 */
void app_batt_send_telemetry_ntf(uint8_t batt_lvl)
{
    struct custs1_val_set_req *set_req;
    struct custs1_val_ntf_ind_req *ntf_req;


    if (batt_lvl > 100)
        batt_lvl = 100;


    current_batt_lvl =
        batt_lvl;


    if (app_connection_idx ==
        GAP_INVALID_CONIDX)
        return;


    if (app_connection_idx >=
        BLE_CONNECTION_MAX)
        return;


    if (app_env[app_connection_idx].conidx ==
        GAP_INVALID_CONIDX)
        return;


    /*
     ****************************************************************************************
     * UPDATE GATT DATABASE
     ****************************************************************************************
     */

    set_req =
        KE_MSG_ALLOC_DYN(
            CUSTS1_VAL_SET_REQ,
            prf_get_task_from_id(TASK_ID_CUSTS1),
            TASK_APP,
            custs1_val_set_req,
            1
        );


    if (set_req == NULL)
        return;


    set_req->handle =
        SVC1_IDX_ADC_VAL_1_VAL;

    set_req->length =
        1;

    set_req->value[0] =
        batt_lvl;


    ke_msg_send(set_req);


    /*
     ****************************************************************************************
     * BATTERY DUPLICATE PROTECTION
     ****************************************************************************************
     */

    if (batt_lvl == last_sent_batt_lvl)
    {
        return;
    }


    /*
     ****************************************************************************************
     * SEND BATTERY NOTIFICATION
     ****************************************************************************************
     */

    ntf_req =
        KE_MSG_ALLOC_DYN(
            CUSTS1_VAL_NTF_REQ,
            prf_get_task_from_id(TASK_ID_CUSTS1),
            TASK_APP,
            custs1_val_ntf_ind_req,
            1
        );


    if (ntf_req == NULL)
        return;


    ntf_req->conidx =
        app_connection_idx;

    ntf_req->notification =
        true;

    ntf_req->handle =
        SVC1_IDX_ADC_VAL_1_VAL;

    ntf_req->length =
        1;

    ntf_req->value[0] =
        batt_lvl;


    /*
     * NOTE:
     * lld_evt_time_get() was removed because the symbol is not
     * available in this DA14585 SDK build.
     *
     * latency_test remains available for a future supported
     * timestamp implementation.
     */


    ke_msg_send(ntf_req);


    last_sent_batt_lvl =
        batt_lvl;
}


/*
 ****************************************************************************************
 * BATTERY POLLING
 ****************************************************************************************
 */

static void app_batt_poll_timer_cb(void)
{
    uint8_t batt;


    if (app_connection_idx ==
        GAP_INVALID_CONIDX)
    {
        app_batt_poll_timer =
            EASY_TIMER_INVALID_TIMER;

        return;
    }


    batt =
        read_battery_level_percentage();


    if (batt > 100)
        batt = 100;


    current_batt_lvl =
        batt;


    app_batt_send_telemetry_ntf(
        batt
    );


    if (app_connection_idx !=
        GAP_INVALID_CONIDX)
    {
        app_batt_poll_timer =
            app_easy_timer(
                BATTERY_POLL_TIME,
                app_batt_poll_timer_cb
            );
    }
}


/*
 ****************************************************************************************
 * START BATTERY POLLING
 ****************************************************************************************
 */

static void start_battery_polling(void)
{
    stop_battery_polling();


    if (app_connection_idx ==
        GAP_INVALID_CONIDX)
        return;


    last_sent_batt_lvl =
        255;


    app_batt_poll_timer =
        app_easy_timer(
            BATTERY_FIRST_READ_DELAY,
            app_batt_poll_timer_cb
        );
}


/*
 ****************************************************************************************
 * STOP BATTERY POLLING
 ****************************************************************************************
 */

static void stop_battery_polling(void)
{
    if (app_batt_poll_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_batt_poll_timer
        );

        app_batt_poll_timer =
            EASY_TIMER_INVALID_TIMER;
    }
}


/*
 ****************************************************************************************
 * HEART RATE
 ****************************************************************************************
 */

void app_hr_send_telemetry_ntf(uint8_t hr)
{
    struct custs1_val_set_req *set_req;
    struct custs1_val_ntf_ind_req *ntf_req;


    if (app_connection_idx ==
        GAP_INVALID_CONIDX)
        return;


    if (app_connection_idx >=
        BLE_CONNECTION_MAX)
        return;


    if (app_env[app_connection_idx].conidx ==
        GAP_INVALID_CONIDX)
        return;


    /*
     ****************************************************************************************
     * UPDATE GATT DATABASE
     ****************************************************************************************
     */

    set_req =
        KE_MSG_ALLOC_DYN(
            CUSTS1_VAL_SET_REQ,
            prf_get_task_from_id(TASK_ID_CUSTS1),
            TASK_APP,
            custs1_val_set_req,
            1
        );


    if (set_req == NULL)
        return;


    set_req->handle =
        SVC3_IDX_HR_VAL_VAL;

    set_req->length =
        1;

    set_req->value[0] =
        hr;


    ke_msg_send(set_req);


    /*
     ****************************************************************************************
     * SEND HR NOTIFICATION
     ****************************************************************************************
     */

    ntf_req =
        KE_MSG_ALLOC_DYN(
            CUSTS1_VAL_NTF_REQ,
            prf_get_task_from_id(TASK_ID_CUSTS1),
            TASK_APP,
            custs1_val_ntf_ind_req,
            1
        );


    if (ntf_req == NULL)
        return;


    ntf_req->conidx =
        app_connection_idx;

    ntf_req->notification =
        true;

    ntf_req->handle =
        SVC3_IDX_HR_VAL_VAL;

    ntf_req->length =
        1;

    ntf_req->value[0] =
        hr;


    ke_msg_send(ntf_req);
}


/*
 ****************************************************************************************
 * HEART RATE POLLING
 ****************************************************************************************
 */

static void app_hr_poll_timer_cb(void)
{
    if (app_connection_idx ==
        GAP_INVALID_CONIDX)
    {
        app_hr_poll_timer =
            EASY_TIMER_INVALID_TIMER;

        return;
    }


    current_hr_value =
        155;


    app_hr_send_telemetry_ntf(
        current_hr_value
    );


    if (app_connection_idx !=
        GAP_INVALID_CONIDX)
    {
        app_hr_poll_timer =
            app_easy_timer(
                HR_POLL_TIME,
                app_hr_poll_timer_cb
            );
    }
}


/*
 ****************************************************************************************
 * START HEART RATE POLLING
 ****************************************************************************************
 */

static void start_hr_polling(void)
{
    stop_hr_polling();


    if (app_connection_idx ==
        GAP_INVALID_CONIDX)
        return;


    app_hr_poll_timer =
        app_easy_timer(
            HR_POLL_TIME,
            app_hr_poll_timer_cb
        );
}


/*
 ****************************************************************************************
 * STOP HEART RATE POLLING
 ****************************************************************************************
 */

static void stop_hr_polling(void)
{
    if (app_hr_poll_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_hr_poll_timer
        );

        app_hr_poll_timer =
            EASY_TIMER_INVALID_TIMER;
    }
}


/*
 ****************************************************************************************
 * CLOCK
 ****************************************************************************************
 */

static void app_clock_timer_cb(void)
{
    if (clock_time_valid == 0)
    {
        app_clock_timer =
            EASY_TIMER_INVALID_TIMER;

        return;
    }


    manual_clock_minute++;


    if (manual_clock_minute >= 60)
    {
        manual_clock_minute = 0;

        manual_clock_hour++;


        if (manual_clock_hour >= 24)
        {
            manual_clock_hour = 0;
        }
    }


    if ((display_is_on != 0) &&
        (display_sequence == DISPLAY_SEQUENCE_TIME))
    {
        display_show_time(
            manual_clock_hour,
            manual_clock_minute
        );
    }


    app_clock_timer =
        app_easy_timer(
            6000,
            app_clock_timer_cb
        );
}


/*
 ****************************************************************************************
 * STOP CLOCK
 ****************************************************************************************
 */

static void stop_clock(void)
{
    if (app_clock_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_clock_timer
        );

        app_clock_timer =
            EASY_TIMER_INVALID_TIMER;
    }
}


/*
 ****************************************************************************************
 * START CLOCK
 ****************************************************************************************
 */

static void start_clock(void)
{
    stop_clock();


    manual_clock_hour =
        0;

    manual_clock_minute =
        0;

    clock_time_valid =
        0;
}


/*
 ****************************************************************************************
 * SET CLOCK FROM PHONE
 ****************************************************************************************
 */

void app_clock_set_time(
                    uint8_t hour,
                    uint8_t minute)
{
    if (hour >= 24)
        return;


    if (minute >= 60)
        return;


    manual_clock_hour =
        hour;

    manual_clock_minute =
        minute;

    clock_time_valid =
        1;


    stop_clock();


    app_clock_timer =
        app_easy_timer(
            6000,
            app_clock_timer_cb
        );


    if ((display_is_on != 0) &&
        (display_sequence == DISPLAY_SEQUENCE_TIME))
    {
        display_show_time(
            manual_clock_hour,
            manual_clock_minute
        );
    }
}


/*
 ****************************************************************************************
 * SLEEP / ACTIVITY TRACKER
 ****************************************************************************************
 */

static void app_sleep_tracker_timer_cb(void)
{
    /*
     * Read the ADXL362 and update the
     * step / sleep tracker.
     */
    sleep_tracker_update();


    /*
     * Continue sampling every 100 ms.
     */
    app_sleep_tracker_timer =
        app_easy_timer(
            SLEEP_TRACKER_POLL_TIME,
            app_sleep_tracker_timer_cb
        );
}


/*
 ****************************************************************************************
 * START SLEEP TRACKER
 ****************************************************************************************
 */

static void start_sleep_tracker(void)
{
    stop_sleep_tracker();


    app_sleep_tracker_timer =
        app_easy_timer(
            SLEEP_TRACKER_POLL_TIME,
            app_sleep_tracker_timer_cb
        );
}


/*
 ****************************************************************************************
 * STOP SLEEP TRACKER
 ****************************************************************************************
 */

static void stop_sleep_tracker(void)
{
    if (app_sleep_tracker_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_sleep_tracker_timer
        );

        app_sleep_tracker_timer =
            EASY_TIMER_INVALID_TIMER;
    }
}


/*
 ****************************************************************************************
 * ACCELEROMETER DISPLAY REFRESH
 ****************************************************************************************
 */

static void app_accel_display_timer_cb(void)
{
    /*
     * Only refresh the OLED while the
     * accelerometer screen is active.
     */
    if ((display_is_on != 0) &&
        (display_sequence == DISPLAY_SEQUENCE_ACCEL))
    {
        accelerometer_test();


        app_accel_display_timer =
            app_easy_timer(
                ACCEL_DISPLAY_POLL_TIME,
                app_accel_display_timer_cb
            );
    }
    else
    {
        app_accel_display_timer =
            EASY_TIMER_INVALID_TIMER;
    }
}


/*
 ****************************************************************************************
 * START ACCELEROMETER DISPLAY
 ****************************************************************************************
 */

static void start_accel_display(void)
{
    stop_accel_display();


    if ((display_is_on == 0) ||
        (display_sequence != DISPLAY_SEQUENCE_ACCEL))
    {
        return;
    }


    app_accel_display_timer =
        app_easy_timer(
            ACCEL_DISPLAY_POLL_TIME,
            app_accel_display_timer_cb
        );
}


/*
 ****************************************************************************************
 * STOP ACCELEROMETER DISPLAY
 ****************************************************************************************
 */

static void stop_accel_display(void)
{
    if (app_accel_display_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_accel_display_timer
        );

        app_accel_display_timer =
            EASY_TIMER_INVALID_TIMER;
    }
}


/*
 ****************************************************************************************
 * LONG PRESS TIMER
 ****************************************************************************************
 */

static void long_press_timer_cb(void)
{
    app_long_press_timer =
        EASY_TIMER_INVALID_TIMER;


    if (GPIO_GetPinStatus(
            GPIO_PORT_1,
            GPIO_PIN_3) != 0)
    {
        return;
    }


    long_press_detected =
        1;

    touch_long_press_lock =
        1;


    if (display_is_on == 0)
    {
        display_is_on =
            1;

        display_sequence =
            DISPLAY_SEQUENCE_TIME;


        if (clock_time_valid)
        {
            display_show_time(
                manual_clock_hour,
                manual_clock_minute
            );
        }
        else
        {
            display_show_time(
                255,
                255
            );
        }
    }
}


/*
 ****************************************************************************************
 * TOUCH RELEASE TIMER
 ****************************************************************************************
 */

static void touch_release_timer_cb(void)
{
    if (GPIO_GetPinStatus(
            GPIO_PORT_1,
            GPIO_PIN_3) == 0)
    {
        app_touch_release_timer =
            app_easy_timer(
                TOUCH_RELEASE_CHECK_TIME,
                touch_release_timer_cb
            );

        return;
    }


    app_touch_release_timer =
        EASY_TIMER_INVALID_TIMER;


    touch_press_active =
        0;


    if (app_long_press_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_long_press_timer
        );

        app_long_press_timer =
            EASY_TIMER_INVALID_TIMER;
    }


    if (touch_long_press_lock)
    {
        touch_long_press_lock =
            0;

        long_press_detected =
            0;


        if (app_touch_rearm_timer !=
            EASY_TIMER_INVALID_TIMER)
        {
            app_easy_timer_cancel(
                app_touch_rearm_timer
            );

            app_touch_rearm_timer =
                EASY_TIMER_INVALID_TIMER;
        }


        app_touch_rearm_timer =
            app_easy_timer(
                TOUCH_REARM_DELAY,
                touch_rearm_timer_cb
            );

        return;
    }


    if (display_is_on == 0)
    {
        if (app_touch_rearm_timer !=
            EASY_TIMER_INVALID_TIMER)
        {
            app_easy_timer_cancel(
                app_touch_rearm_timer
            );

            app_touch_rearm_timer =
                EASY_TIMER_INVALID_TIMER;
        }


        app_touch_rearm_timer =
            app_easy_timer(
                TOUCH_REARM_DELAY,
                touch_rearm_timer_cb
            );

        return;
    }


    /*
     * ================================================================
     * TIME -> BATTERY
     * ================================================================
     */

    if (display_sequence ==
        DISPLAY_SEQUENCE_TIME)
    {
        current_batt_lvl =
            read_battery_level_percentage();


        if (current_batt_lvl > 100)
            current_batt_lvl =
                100;


        display_show_battery(
            current_batt_lvl
        );

	
				latency_test = (latency_touch_timestamp - GET_TIMESTAMP()) / 16000;
        GPIO_SetActive(GPIO_PORT_1, GPIO_PIN_0); // LED blink = measurement done
				
        app_batt_send_telemetry_ntf(
            current_batt_lvl
        );


        display_sequence =
            DISPLAY_SEQUENCE_BATTERY;
    }


    /*
     * ================================================================
     * BATTERY -> HEART RATE
     * ================================================================
     */

    else if (display_sequence ==
             DISPLAY_SEQUENCE_BATTERY)
    {
        display_show_steps(
            current_hr_value
        );


        display_sequence =
            DISPLAY_SEQUENCE_HR;
    }


    /*
     * ================================================================
     * HEART RATE -> ACCELEROMETER
     * ================================================================
     */

    else if (display_sequence ==
             DISPLAY_SEQUENCE_HR)
    {
        /*
         * DO NOT call sleep_tracker_update()
         * here.
         *
         * The tracker already has its own
         * 100 ms timer.
         */

        display_sequence =
            DISPLAY_SEQUENCE_ACCEL;


        /*
         * Draw the first X/Y/Z values
         * immediately.
         */
        accelerometer_test();


        /*
         * Start continuous OLED refresh.
         */
        start_accel_display();
    }


    /*
     * ================================================================
     * ACCELEROMETER -> DISPLAY OFF
     * ================================================================
     */

    else if (display_sequence ==
             DISPLAY_SEQUENCE_ACCEL)
    {
        /*
         * Stop continuous accelerometer
         * OLED refresh before turning
         * the display off.
         */
        stop_accel_display();


        display_clear();


        display_is_on =
            0;


        display_sequence =
            DISPLAY_SEQUENCE_TIME;
    }


    /*
     * ================================================================
     * REARM TOUCH
     * ================================================================
     */

    if (app_touch_rearm_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_touch_rearm_timer
        );

        app_touch_rearm_timer =
            EASY_TIMER_INVALID_TIMER;
    }


    app_touch_rearm_timer =
        app_easy_timer(
            TOUCH_REARM_DELAY,
            touch_rearm_timer_cb
        );
}


/*
 ****************************************************************************************
 * TOUCH RE-ARM
 ****************************************************************************************
 */

static void touch_rearm_timer_cb(void)
{
    app_touch_rearm_timer =
        EASY_TIMER_INVALID_TIMER;


    if (GPIO_GetPinStatus(
            GPIO_PORT_1,
            GPIO_PIN_3) == 0)
    {
        app_touch_rearm_timer =
            app_easy_timer(
                TOUCH_REARM_DELAY,
                touch_rearm_timer_cb
            );

        return;
    }


    touch_press_active =
        0;

    long_press_detected =
        0;

    touch_long_press_lock =
        0;


    wkupct_register_callback(
        touch_button_press_cb
    );


    wkupct_enable_irq(
        WKUPCT_PIN_SELECT(
            GPIO_PORT_1,
            GPIO_PIN_3
        ),

        WKUPCT_PIN_POLARITY(
            GPIO_PORT_1,
            GPIO_PIN_3,
            WKUPCT_PIN_POLARITY_LOW
        ),

        1,
        40
    );
}


/*
 ****************************************************************************************
 * TOUCH PRESS CALLBACK
 ****************************************************************************************
 */

static void touch_button_press_cb(void)
{
		latency_touch_timestamp = GET_TIMESTAMP();
	
    if (touch_press_active)
        return;


    if (touch_long_press_lock)
        return;


    /*
     * Latency timestamp capture temporarily disabled.
     *
     * lld_evt_time_get() is not available in this SDK build.
     *
     * latency_touch_timestamp remains available for a future
     * supported timing implementation.
     */


    touch_press_active =
        1;

    long_press_detected =
        0;


    GPIO_SetActive(
        GPIO_PORT_1,
        GPIO_PIN_0
    );


    wkupct_disable_irq();


    if (app_long_press_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_long_press_timer
        );

        app_long_press_timer =
            EASY_TIMER_INVALID_TIMER;
    }


    app_long_press_timer =
        app_easy_timer(
            LONG_PRESS_TIME,
            long_press_timer_cb
        );


    if (app_touch_release_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_touch_release_timer
        );

        app_touch_release_timer =
            EASY_TIMER_INVALID_TIMER;
    }


    app_touch_release_timer =
        app_easy_timer(
            TOUCH_RELEASE_CHECK_TIME,
            touch_release_timer_cb
        );
}


/*
 ****************************************************************************************
 * TOUCH INITIALIZATION
 ****************************************************************************************
 */

static void touch_button_init(void)
{
    touch_press_active =
        0;

    long_press_detected =
        0;

    touch_long_press_lock =
        0;


    app_long_press_timer =
        EASY_TIMER_INVALID_TIMER;

    app_touch_release_timer =
        EASY_TIMER_INVALID_TIMER;

    app_touch_rearm_timer =
        EASY_TIMER_INVALID_TIMER;


    display_is_on =
        0;

    display_sequence =
        DISPLAY_SEQUENCE_TIME;


    display_clear();


    wkupct_register_callback(
        touch_button_press_cb
    );


    wkupct_enable_irq(
        WKUPCT_PIN_SELECT(
            GPIO_PORT_1,
            GPIO_PIN_3
        ),

        WKUPCT_PIN_POLARITY(
            GPIO_PORT_1,
            GPIO_PIN_3,
            WKUPCT_PIN_POLARITY_LOW
        ),

        1,
        40
    );
}


/*
 ****************************************************************************************
 * CONNECTION PARAMETER UPDATE
 ****************************************************************************************
 */

static void param_update_request_timer_cb(void)
{
    struct gapc_param_update_cmd *cmd;


    if (app_connection_idx ==
        GAP_INVALID_CONIDX)
    {
        app_param_update_request_timer =
            EASY_TIMER_INVALID_TIMER;

        return;
    }


    cmd =
        KE_MSG_ALLOC(
            GAPC_PARAM_UPDATE_CMD,
            KE_BUILD_ID(
                TASK_GAPC,
                app_connection_idx
            ),
            TASK_APP,
            gapc_param_update_cmd
        );


    if (cmd == NULL)
        return;


    cmd->operation =
        GAPC_UPDATE_PARAMS;

    /*
     * Aligned with user_connection_param_conf
     * (7.5 ms .. 15 ms connection interval, 2 s supervision timeout).
     *
     * intv_min/intv_max are in 1.25 ms double slots.
     * time_out is in 10 ms timer units.
     */
    cmd->intv_min =
        US_TO_DOUBLESLOTS(7500);

    cmd->intv_max =
        US_TO_DOUBLESLOTS(15000);

    cmd->latency =
        0;

    cmd->time_out =
        MS_TO_TIMERUNITS(2000);

    ke_msg_send(cmd);


    app_param_update_request_timer =
        EASY_TIMER_INVALID_TIMER;
}


/*
 ****************************************************************************************
 * SLAVE PREFERRED CONNECTION PARAMETERS
 ****************************************************************************************
 *
 * Advertised in the GAP "slave preferred connection parameters"
 * characteristic so the central is told to connect with a
 * responsive 7.5 ms .. 15 ms interval.
 */

void user_app_get_dev_slv_pref_params(struct gap_slv_pref *slv_params)
{
    if (slv_params == NULL)
        return;


    slv_params->con_intv_min =
        US_TO_DOUBLESLOTS(7500);

    slv_params->con_intv_max =
        US_TO_DOUBLESLOTS(15000);

    slv_params->slave_latency =
        0;

    slv_params->conn_timeout =
        MS_TO_TIMERUNITS(2000);
}


/*
 ****************************************************************************************
 * INTEGER TO STRING
 ****************************************************************************************
 */

static void accel_int_to_string(
                    int16_t value,
                    char *buffer)
{
    char temp[8];

    uint8_t index =
        0;

    uint8_t output_index =
        0;

    uint16_t magnitude;


    if (value < 0)
    {
        buffer[output_index++] =
            '-';

        magnitude =
            (uint16_t)(-(int32_t)value);
    }
    else
    {
        magnitude =
            (uint16_t)value;
    }


    if (magnitude == 0)
    {
        buffer[output_index++] =
            '0';

        buffer[output_index] =
            '\0';

        return;
    }


    while (magnitude > 0)
    {
        temp[index++] =
            (char)('0' + (magnitude % 10));

        magnitude =
            magnitude / 10;
    }


    while (index > 0)
    {
        index--;

        buffer[output_index++] =
            temp[index];
    }


    buffer[output_index] =
        '\0';
}


/*
 ****************************************************************************************
 * ACCELEROMETER TEST
 ****************************************************************************************
 * =============================================================================
 * ACCELEROMETER 10-SAMPLE ANALYSIS & DISPLAY
 * =============================================================================
 *
 * Gathers 10 samples from the ADXL362 accelerometer, analyzes whether the motion
 * is:
 *      - SLEEP
 *      - JUST MOVE
 *      - WALK
 * and then prints the classification, step count, and accelerometer readings into
 * the OLED display.
 * =============================================================================
 */

void accel_sample_and_display_state(void)
{
    int16_t x_samples[ACCEL_WINDOW_SIZE];
    int16_t y_samples[ACCEL_WINDOW_SIZE];
    int16_t z_samples[ACCEL_WINDOW_SIZE];
    uint8_t i;

    /*
     * Gather 10 samples from ADXL362
     */
    for (i = 0; i < ACCEL_WINDOW_SIZE; i++)
    {
        adxl362_read_xyz(
            &x_samples[i],
            &y_samples[i],
            &z_samples[i]
        );

        SetWord16(WATCHDOG_REG, 0xFF);

        /*
         * Short delay ~10 ms between samples
         */
        for (volatile uint32_t d = 0; d < 16000; d++)
        {
            __NOP();
        }
    }

    /*
     * 10-sample analysis: SLEEP, JUST MOVE, or WALK
     */
    uint8_t state =
        sleep_tracker_analyze_10_samples(
            x_samples,
            y_samples,
            z_samples
        );

    uint32_t steps =
        sleep_tracker_get_steps();

    /*
     * Print result to OLED display
     */
    display_show_accel_data(
        x_samples[ACCEL_WINDOW_SIZE - 1],
        y_samples[ACCEL_WINDOW_SIZE - 1],
        z_samples[ACCEL_WINDOW_SIZE - 1],
        steps,
        state
    );
}


/*
 * =============================================================================
 * ACCELEROMETER SCREEN DISPLAY REFRESH
 * =============================================================================
 *
 * Uses the running 10-sample window from sleep_tracker to display live state:
 * SLEEP, JUST MOVE, or WALK on OLED with zero flickering.
 * =============================================================================
 */

static void accelerometer_test(void)
{
    int16_t x;
    int16_t y;
    int16_t z;

    /*
     * Update 10-sample tracker and classify motion
     */
    sleep_tracker_update();
    sleep_tracker_get_latest_xyz(&x, &y, &z);

    uint8_t activity =
        sleep_tracker_get_activity_level();

    uint32_t steps =
        sleep_tracker_get_steps();

    display_show_accel_data(
        x,
        y,
        z,
        steps,
        activity
    );
}


/*
 ****************************************************************************************
 * APPLICATION INITIALIZATION
 ****************************************************************************************
 */

void user_app_init(void)
{
    app_connection_idx =
        GAP_INVALID_CONIDX;


    app_batt_poll_timer =
        EASY_TIMER_INVALID_TIMER;

    app_hr_poll_timer =
        EASY_TIMER_INVALID_TIMER;

    app_param_update_request_timer =
        EASY_TIMER_INVALID_TIMER;

    app_clock_timer =
        EASY_TIMER_INVALID_TIMER;

    app_sleep_tracker_timer =
        EASY_TIMER_INVALID_TIMER;

    app_accel_display_timer =
        EASY_TIMER_INVALID_TIMER;

    app_long_press_timer =
        EASY_TIMER_INVALID_TIMER;

    app_touch_release_timer =
        EASY_TIMER_INVALID_TIMER;

    app_touch_rearm_timer =
        EASY_TIMER_INVALID_TIMER;


    current_batt_lvl =
        read_battery_level_percentage();

    last_sent_batt_lvl =
        255;


    current_hr_value =
        155;


    manual_clock_hour =
        0;

    manual_clock_minute =
        0;

    clock_time_valid =
        0;


    touch_press_active =
        0;

    long_press_detected =
        0;

    touch_long_press_lock =
        0;


    display_is_on =
        0;

    display_sequence =
        DISPLAY_SEQUENCE_TIME;


    default_app_on_init();


    start_clock();


    touch_button_init();


    /*
     ****************************************************************************************
     * INITIALIZE SLEEP TRACKER
     ****************************************************************************************
     */

    sleep_tracker_init();


    /*
     ****************************************************************************************
     * INITIALIZE ADXL362
     ****************************************************************************************
     */

    adxl362_init();


    /*
     ****************************************************************************************
     * START SLEEP / ACTIVITY TRACKER
     ****************************************************************************************
     *
     * ADXL362 is sampled every 100 ms.
     *
     ****************************************************************************************
     */

    start_sleep_tracker();


    /*
     ****************************************************************************************
     * DO NOT DRAW ACCELEROMETER HERE
     ****************************************************************************************
     *
     * OLED must remain OFF after boot.
     *
     ****************************************************************************************
     */
}


/*
 ****************************************************************************************
 * ADVERTISING START
 ****************************************************************************************
 */

void user_app_adv_start(void)
{
    app_easy_gap_undirected_advertise_start();
}


/*
 ****************************************************************************************
 * CONNECTION
 ****************************************************************************************
 */

void user_app_connection(
                    uint8_t connection_idx,
                    struct gapc_connection_req_ind const *param)
{
    if (connection_idx >=
        BLE_CONNECTION_MAX)
        return;


    app_connection_idx =
        connection_idx;


    default_app_on_connection(
        connection_idx,
        param
    );


    start_battery_polling();


    start_hr_polling();


    /*
     * The sleep tracker was already started
     * during initialization.
     *
     * Do not start another tracker timer here.
     */


    if (app_param_update_request_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_param_update_request_timer
        );

        app_param_update_request_timer =
            EASY_TIMER_INVALID_TIMER;
    }


    app_param_update_request_timer =
        app_easy_timer(
            100,
            param_update_request_timer_cb
        );
}


/*
 ****************************************************************************************
 * DISCONNECT
 ****************************************************************************************
 */

void user_app_disconnect(
                    struct gapc_disconnect_ind const *param)
{
    stop_battery_polling();

    stop_hr_polling();

    stop_accel_display();


    last_sent_batt_lvl =
        255;


    if (app_param_update_request_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_param_update_request_timer
        );

        app_param_update_request_timer =
            EASY_TIMER_INVALID_TIMER;
    }


    if (app_long_press_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_long_press_timer
        );

        app_long_press_timer =
            EASY_TIMER_INVALID_TIMER;
    }


    if (app_touch_release_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_touch_release_timer
        );

        app_touch_release_timer =
            EASY_TIMER_INVALID_TIMER;
    }


    if (app_touch_rearm_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_touch_rearm_timer
        );

        app_touch_rearm_timer =
            EASY_TIMER_INVALID_TIMER;
    }


    touch_press_active =
        0;

    long_press_detected =
        0;

    touch_long_press_lock =
        0;


    app_connection_idx =
        GAP_INVALID_CONIDX;


    default_app_on_disconnect(param);
}


/*
 ****************************************************************************************
 * ADVERTISING COMPLETE
 ****************************************************************************************
 */

void user_app_adv_undirect_complete(
                    uint8_t status)
{
    if (status ==
        GAP_ERR_CANCELED)
    {
        user_app_adv_start();
    }
}


/*
 ****************************************************************************************
 * ADVERTISING COMPLETE COMPATIBILITY CALLBACK
 ****************************************************************************************
 */

void app_advertise_complete(
                    const uint8_t status)
{
    (void)status;
}


/*
 ****************************************************************************************
 * REST OF MESSAGE HANDLER
 ****************************************************************************************
 */

void user_catch_rest_hndl(
                            ke_msg_id_t const msgid,
                            void const *param,
                            const ke_task_id_t dest_id,
                            const ke_task_id_t src_id)
{
    switch (msgid)
    {
        /*
         * ================================================================
         * CLOCK WRITE
         * ================================================================
         */

        case CUSTS1_VAL_WRITE_IND:
        {
					  latency_touch_timestamp = GET_TIMESTAMP();
            struct custs1_val_write_ind const *msg_param;


            msg_param =
                (struct custs1_val_write_ind const *)param;


            if (msg_param == NULL)
                break;


            GPIO_SetActive(
                GPIO_LED_PORT,
                GPIO_LED_PIN
            );


            /*
             * ONLY accept writes to the Clock
             * characteristic.
             */

            if ((msg_param->handle ==
                 SVC3_IDX_CLOCK_VAL_VAL) &&
                (msg_param->length >= 2))
            {
                uint8_t hour;
                uint8_t minute;


                hour =
                    msg_param->value[0];

                minute =
                    msg_param->value[1];


                if ((hour < 24) &&
                    (minute < 60))
                {
                    app_clock_set_time(
                        hour,
                        minute
                    );
									
									  latency_test = (latency_touch_timestamp - GET_TIMESTAMP()) / 16;
                }
            }
        }
        break;


        /*
         * ================================================================
         * GATT EVENT CONFIRMATION
         * ================================================================
         */

        case GATTC_EVENT_REQ_IND:
        {
            struct gattc_event_ind const *ind;
            struct gattc_event_cfm *cfm;


            ind =
                (const struct gattc_event_ind *)param;


            if (ind == NULL)
                return;


            cfm =
                KE_MSG_ALLOC(
                    GATTC_EVENT_CFM,
                    src_id,
                    dest_id,
                    gattc_event_cfm
                );


            if (cfm == NULL)
                return;


            cfm->handle =
                ind->handle;


            ke_msg_send(cfm);
        }
        break;


        default:
            break;
    }
}


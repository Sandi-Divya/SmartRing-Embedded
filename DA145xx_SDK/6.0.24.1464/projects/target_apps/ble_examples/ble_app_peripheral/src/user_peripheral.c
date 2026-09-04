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

extern void app_clock_set_time(uint8_t hour,uint8_t minute);


/*
 ****************************************************************************************
 * DEFINITIONS
 ****************************************************************************************
 */

/*
 * app_easy_timer() uses 10 ms units.
 *
 * 100 x 10 ms = 1000 ms = 1 second.
 */
#define LONG_PRESS_TIME               100

/*
 * Check button state every 10 ms.
 */
#define TOUCH_RELEASE_CHECK_TIME      1

/*
 * Wait 100 ms after release before enabling
 * the wake-up interrupt again.
 */
#define TOUCH_REARM_DELAY             10


/*
 ****************************************************************************************
 * TELEMETRY TIMING
 ****************************************************************************************
 *
 * app_easy_timer() unit = 10 ms.
 *
 * Battery:
 *
 *      18000 x 10 ms
 *      = 180000 ms
 *      = 180 seconds
 *      = 3 minutes
 *
 * Heart rate:
 *
 *      1500 x 10 ms
 *      = 15000 ms
 *      = 15 seconds
 *
 ****************************************************************************************
 */

#define BATTERY_POLL_TIME             18000
#define HR_POLL_TIME                  1500


/*
 ****************************************************************************************
 * BATTERY FIRST-READ DELAY
 ****************************************************************************************
 *
 * After BLE connection, wait 1 second before sending
 * the first battery notification.
 *
 * This gives the mobile application time to:
 *
 *      1. Discover services
 *      2. Discover characteristics
 *      3. Enable notifications
 *
 * 100 x 10 ms = 1 second
 *
 ****************************************************************************************
 */

#define BATTERY_FIRST_READ_DELAY      500


/*
 ****************************************************************************************
 * DISPLAY SEQUENCE
 ****************************************************************************************
 */

#define DISPLAY_SEQUENCE_TIME         0
#define DISPLAY_SEQUENCE_BATTERY      1
#define DISPLAY_SEQUENCE_HR            2


/*
 * Current display position:
 *
 * TIME -> BATTERY -> HR -> OFF
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


/*
 * Last battery percentage actually sent
 * through BLE notification.
 *
 * 255 means:
 *
 *      No value has been sent yet.
 *
 * This ensures the first battery value after
 * connection is always sent.
 */
static uint8_t last_sent_batt_lvl =
    255;


/*
 ****************************************************************************************
 * HEART RATE
 ****************************************************************************************
 */

uint8_t current_hr_value =
    75;

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
 *
 * clock_time_valid:
 *
 *      0 = Time has never been received from phone
 *          OLED should show --:--
 *
 *      1 = Valid time received from phone
 *          OLED can show the current time
 *
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
 * TOUCH STATE
 ****************************************************************************************
 */

/*
 * 1 = a press is currently being processed.
 * 0 = no active press.
 */
uint8_t touch_press_active =
    0;


/*
 * 1 = current press became a long press.
 * 0 = current press was a short press.
 */
uint8_t long_press_detected =
    0;


/*
 * 1 = long press was confirmed.
 *
 * While this is active, the release can NEVER
 * become a short tap.
 */
uint8_t touch_long_press_lock =
    0;


/*
 * Long press timer.
 */
timer_hnd app_long_press_timer =
    EASY_TIMER_INVALID_TIMER;


/*
 * Release monitoring timer.
 */
timer_hnd app_touch_release_timer =
    EASY_TIMER_INVALID_TIMER;


/*
 * Delayed wake-up re-arm timer.
 */
timer_hnd app_touch_rearm_timer =
    EASY_TIMER_INVALID_TIMER;


/*
 ****************************************************************************************
 * DISPLAY STATE
 ****************************************************************************************
 *
 *      0 = OLED OFF
 *      1 = OLED ON
 *
 * OLED starts OFF.
 *
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


/* Touch */
static void touch_button_press_cb(void);
static void touch_button_init(void);
static void long_press_timer_cb(void);
static void touch_release_timer_cb(void);
static void touch_rearm_timer_cb(void);


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
     *
     * If the battery percentage has not changed,
     * don't send another BLE notification.
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


    ke_msg_send(ntf_req);


    /*
     * Remember the battery value that was
     * actually sent.
     */
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


    /*
     * Stop if disconnected.
     */
    if (app_connection_idx ==
        GAP_INVALID_CONIDX)
    {
        app_batt_poll_timer =
            EASY_TIMER_INVALID_TIMER;

        return;
    }


    /*
     ****************************************************************************************
     * READ BATTERY ADC
     ****************************************************************************************
     */

    batt =
        read_battery_level_percentage();


    if (batt > 100)
        batt = 100;


    current_batt_lvl =
        batt;


    /*
     ****************************************************************************************
     * SEND BATTERY
     ****************************************************************************************
     *
     * The first connection starts with:
     *
     *      last_sent_batt_lvl = 255
     *
     * Therefore the first battery value is
     * ALWAYS sent.
     *
     * After that, notification is only sent
     * when the battery percentage changes.
     */

    app_batt_send_telemetry_ntf(
        batt
    );


    /*
     ****************************************************************************************
     * NEXT BATTERY CHECK
     ****************************************************************************************
     *
     * 18000 x 10 ms = 3 minutes.
     */

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
    /*
     * Cancel any old battery timer.
     */
    stop_battery_polling();


    /*
     * No connection = nothing to do.
     */
    if (app_connection_idx ==
        GAP_INVALID_CONIDX)
        return;


    /*
     ****************************************************************************************
     * FORCE FIRST BATTERY NOTIFICATION
     ****************************************************************************************
     *
     * 255 means no battery value has been
     * sent for this connection.
     */

    last_sent_batt_lvl =
        255;


    /*
     ****************************************************************************************
     * FIRST BATTERY READ
     ****************************************************************************************
     *
     * Wait 1 second after connection.
     *
     * 100 x 10 ms = 1 second.
     *
     * This is important because the phone needs
     * time to discover the characteristic and
     * enable notifications.
     */

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
    /*
     ****************************************************************************************
     * STOP IF DISCONNECTED
     ****************************************************************************************
     */

    if (app_connection_idx ==
        GAP_INVALID_CONIDX)
    {
        app_hr_poll_timer =
            EASY_TIMER_INVALID_TIMER;

        return;
    }


    /*
     ****************************************************************************************
     * TEMPORARY TEST VALUE
     ****************************************************************************************
     *
     * Keep 75 for now.
     *
     * Later replace ONLY this value with
     * the actual HR sensor reading.
     *
     * Do not change the 15-second timer.
     */

    current_hr_value =
        75;


    /*
     ****************************************************************************************
     * SEND HR
     ****************************************************************************************
     */

    app_hr_send_telemetry_ntf(
        current_hr_value
    );


    /*
     ****************************************************************************************
     * NEXT HR UPDATE
     ****************************************************************************************
     *
     * 1500 x 10 ms = 15 seconds.
     */

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
    /*
     * Cancel any previous HR timer.
     */
    stop_hr_polling();


    /*
     * No connection = nothing to do.
     */
    if (app_connection_idx ==
        GAP_INVALID_CONIDX)
        return;


    /*
     ****************************************************************************************
     * FIRST HR READING
     ****************************************************************************************
     *
     * Do NOT send HR immediately.
     *
     * The HR sensor needs time to settle.
     *
     * First HR sensor update:
     *
     *      +15 seconds
     *
     * Then every 15 seconds.
     */

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


static void app_clock_timer_cb(void)
{
    /*
     * Do nothing until the phone has supplied
     * the first valid time.
     */
    if (clock_time_valid == 0)
    {
        app_clock_timer =
            EASY_TIMER_INVALID_TIMER;

        return;
    }

    /*
     * Advance one minute.
     */
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

    /*
     * 6000 x 10 ms = 60 seconds.
     */
    app_clock_timer =
        app_easy_timer(
            6000,
            app_clock_timer_cb
        );
}


static void stop_clock(void)
{
    if (app_clock_timer != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_clock_timer);
        app_clock_timer = EASY_TIMER_INVALID_TIMER;
    }
}



static void start_clock(void)
{
    stop_clock();

    /*
     * Do NOT create a fake starting time.
     *
     * Until the phone sends the first time,
     * clock_time_valid remains 0.
     */
    manual_clock_hour = 0;
    manual_clock_minute = 0;

    clock_time_valid = 0;

    /*
     * Do not start the minute counter yet.
     *
     * It will start when the phone sends
     * the first valid time.
     */
}


/*
 ****************************************************************************************
 * SET CLOCK FROM PHONE
 ****************************************************************************************
 *
 * hour   = 0 - 23
 * minute = 0 - 59
 *
 * This is called when the mobile application
 * writes the current phone time to the Clock
 * characteristic.
 ****************************************************************************************
 */

void app_clock_set_time(
                    uint8_t hour,
                    uint8_t minute)
{
    /*
     * Reject invalid time.
     */
    if (hour >= 24)
        return;

    if (minute >= 60)
        return;

    /*
     * Store the new time.
     */
    manual_clock_hour =
        hour;

    manual_clock_minute =
        minute;

    /*
     * Time is now valid.
     */
    clock_time_valid =
        1;

    /*
     * Restart the minute timer from the
     * newly received phone time.
     */
    stop_clock();

    app_clock_timer =
        app_easy_timer(
            6000,
            app_clock_timer_cb
        );
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


    /*
     * If the pin is no longer LOW,
     * the user released before 1 second.
     */
    if (GPIO_GetPinStatus(
            GPIO_PORT_1,
            GPIO_PIN_3) != 0)
    {
        return;
    }


    /*
     ****************************************************************************************
     * LONG PRESS CONFIRMED
     ****************************************************************************************
     */

    long_press_detected =
        1;


    touch_long_press_lock =
        1;


    /*
     ****************************************************************************************
     * OLED OFF -> OLED ON + TIME
     ****************************************************************************************
     */

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


    /*
     * If OLED is already ON, do nothing.
     */
}


/*
 ****************************************************************************************
 * TOUCH RELEASE TIMER
 ****************************************************************************************
 */

static void touch_release_timer_cb(void)
{
    /*
     * Button is still LOW.
     *
     * Keep waiting.
     */
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


    /*
     ****************************************************************************************
     * BUTTON RELEASED
     ****************************************************************************************
     */

    app_touch_release_timer =
        EASY_TIMER_INVALID_TIMER;


    touch_press_active =
        0;


    /*
     * Cancel long press timer.
     */
    if (app_long_press_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_long_press_timer
        );

        app_long_press_timer =
            EASY_TIMER_INVALID_TIMER;
    }


    /*
     ****************************************************************************************
     * LONG PRESS RELEASE
     ****************************************************************************************
     */

    if (touch_long_press_lock)
    {
        touch_long_press_lock =
            0;


        long_press_detected =
            0;


        /*
         * Wait before rearming.
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


        return;
    }


    /*
     ****************************************************************************************
     * SHORT TAP
     ****************************************************************************************
     */

    /*
     * OLED OFF:
     *
     * Short tap does nothing.
     */
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
     ****************************************************************************************
     * TIME -> BATTERY
     ****************************************************************************************
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


        /*
         * Update GATT database and notify only
         * if the battery value changed.
         */
        app_batt_send_telemetry_ntf(
            current_batt_lvl
        );


        display_sequence =
            DISPLAY_SEQUENCE_BATTERY;
    }


    /*
     ****************************************************************************************
     * BATTERY -> HEART RATE
     ****************************************************************************************
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
     ****************************************************************************************
     * HEART RATE -> OLED OFF
     ****************************************************************************************
     */

    else if (display_sequence ==
             DISPLAY_SEQUENCE_HR)
    {
        display_clear();


        display_is_on =
            0;


        display_sequence =
            DISPLAY_SEQUENCE_TIME;
    }


    /*
     * Delay wake-up rearming.
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


    /*
     * Never enable wake-up while P1_3
     * is still LOW.
     */
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


    /*
     * Pin is HIGH.
     *
     * Safe to accept another press.
     */
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
    /*
     * Ignore repeated interrupt events.
     */
    if (touch_press_active)
        return;


    /*
     * Ignore while long-press lock is active.
     */
    if (touch_long_press_lock)
        return;


    /*
     * Mark this press active immediately.
     */
    touch_press_active =
        1;


    long_press_detected =
        0;


    /*
     * Diagnostic LED.
     */
    GPIO_SetActive(
        GPIO_PORT_1,
        GPIO_PIN_0
    );


    /*
     ****************************************************************************************
     * DISABLE WAKE-UP INTERRUPT
     ****************************************************************************************
     */

    wkupct_disable_irq();


    /*
     ****************************************************************************************
     * START LONG PRESS TIMER
     ****************************************************************************************
     */

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


    /*
     ****************************************************************************************
     * START RELEASE MONITORING
     ****************************************************************************************
     */

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


    /*
     ****************************************************************************************
     * OLED STARTS OFF
     ****************************************************************************************
     */

    display_is_on =
        0;


    display_sequence =
        DISPLAY_SEQUENCE_TIME;


    display_clear();


    /*
     ****************************************************************************************
     * ENABLE TOUCH
     ****************************************************************************************
     */

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


    cmd->intv_min =
        10;


    cmd->intv_max =
        20;


    cmd->latency =
        0;


    cmd->time_out =
        200;


    ke_msg_send(cmd);


    app_param_update_request_timer =
        EASY_TIMER_INVALID_TIMER;
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


    app_long_press_timer =
        EASY_TIMER_INVALID_TIMER;


    app_touch_release_timer =
        EASY_TIMER_INVALID_TIMER;


    app_touch_rearm_timer =
        EASY_TIMER_INVALID_TIMER;


    /*
     * Read initial battery.
     */
    current_batt_lvl =
        read_battery_level_percentage();


    /*
     * No battery notification has been sent yet.
     */
    last_sent_batt_lvl =
        255;


    /*
     * Temporary HR test value.
     */
    current_hr_value =
        75;


    manual_clock_hour = 0;

		manual_clock_minute = 0;

		clock_time_valid = 0;


    touch_press_active =
        0;


    long_press_detected =
        0;


    touch_long_press_lock =
        0;


    /*
     * OLED OFF at boot.
     */
    display_is_on =
        0;


    display_sequence =
        DISPLAY_SEQUENCE_TIME;


    /*
     * Existing SDK initialization.
     */
    default_app_on_init();


    /*
     * Start manual clock.
     */
    start_clock();


    /*
     * Initialize OLED/touch.
     */
    touch_button_init();
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


    /*
     ****************************************************************************************
     * BATTERY TELEMETRY
     ****************************************************************************************
     *
     * First battery reading:
     *
     *      1 second after connection
     *
     * Then:
     *
     *      every 3 minutes
     *
     * Notification is only sent when the
     * battery percentage changes.
     */

    start_battery_polling();


    /*
     ****************************************************************************************
     * HEART RATE TELEMETRY
     ****************************************************************************************
     *
     * First HR reading:
     *
     *      15 seconds after connection
     *
     * Then:
     *
     *      every 15 seconds
     *
     * This gives the HR sensor time to settle.
     */

    start_hr_polling();


    /*
     ****************************************************************************************
     * CONNECTION PARAMETER UPDATE
     ****************************************************************************************
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
    /*
     ****************************************************************************************
     * STOP TELEMETRY TIMERS
     ****************************************************************************************
     */

    stop_battery_polling();

    stop_hr_polling();


    /*
     * Force first battery value to be sent
     * on the next connection.
     */
    last_sent_batt_lvl =
        255;


    /*
     ****************************************************************************************
     * CANCEL CONNECTION PARAMETER TIMER
     ****************************************************************************************
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


    /*
     ****************************************************************************************
     * CANCEL LONG PRESS TIMER
     ****************************************************************************************
     */

    if (app_long_press_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_long_press_timer
        );

        app_long_press_timer =
            EASY_TIMER_INVALID_TIMER;
    }


    /*
     ****************************************************************************************
     * CANCEL RELEASE TIMER
     ****************************************************************************************
     */

    if (app_touch_release_timer !=
        EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(
            app_touch_release_timer
        );

        app_touch_release_timer =
            EASY_TIMER_INVALID_TIMER;
    }


    /*
     ****************************************************************************************
     * CANCEL DELAYED RE-ARM TIMER
     ****************************************************************************************
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


    touch_press_active =
        0;


    long_press_detected =
        0;


    touch_long_press_lock =
        0;


    app_connection_idx =
        GAP_INVALID_CONIDX;


    /*
     * DO NOT stop clock.
     *
     * DO NOT stop touch detection.
     */
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


				case CUSTS1_VAL_WRITE_IND:
				{
						struct custs1_val_write_ind const *msg_param;

						msg_param =
								(struct custs1_val_write_ind const *)param;

						if (msg_param == NULL)
								break;

						/*
						 * ============================================================
						 * DIAGNOSTIC LED
						 *
						 * If this LED turns ON when nRF Connect sends a write,
						 * we know the BLE write reached this handler.
						 * ============================================================
						 */
						GPIO_SetActive(
								GPIO_LED_PORT,
								GPIO_LED_PIN
						);

						/*
						 * ============================================================
						 * CLOCK
						 * ============================================================
						 */
						if ((msg_param->handle ==
								 SVC3_IDX_CLOCK_VAL_VAL) &&
								(msg_param->length >= 2))
						{
								uint8_t hour =
										msg_param->value[0];

								uint8_t minute =
										msg_param->value[1];

								if ((hour < 24) &&
										(minute < 60))
								{
										app_clock_set_time(
												hour,
												minute
										);
								}
						}
				}
				break;



        case GATTC_EVENT_REQ_IND:
        {
            struct gattc_event_ind const *ind;
            struct gattc_event_cfm *cfm;


            ind =
                (struct gattc_event_ind const *)param;


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
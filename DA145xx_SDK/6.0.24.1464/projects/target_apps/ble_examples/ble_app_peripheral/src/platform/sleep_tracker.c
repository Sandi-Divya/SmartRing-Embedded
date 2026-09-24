
/**
 ****************************************************************************************
 *
 * @file sleep_tracker.c
 * @brief Motion, step and sleep tracking using ADXL362.
 *
 ****************************************************************************************
 */

#include "sleep_tracker.h"
#include "adxl362.h"
#include <stdint.h>


/*
 * =============================================================================
 * TRACKER TIMING
 * =============================================================================
 *
 * user_peripheral.c calls sleep_tracker_update() every 100 ms.
 *
 */

#define SLEEP_TRACKER_UPDATE_PERIOD_MS     100


/*
 * =============================================================================
 * MOTION THRESHOLDS
 * =============================================================================
 *
 * The ADXL362 raw values are used directly.
 *
 * motion =
 *      abs(X - previous X)
 *    + abs(Y - previous Y)
 *    + abs(Z - previous Z)
 *
 * These values are intentionally moderate so normal movement can be detected.
 *
 */

#define SLEEP_MOTION_THRESHOLD             15
#define SLEEP_RANGE_THRESHOLD              30

#define MOTION_THRESHOLD                   25

#define STEP_PEAK_THRESHOLD                180

#define STEP_RELEASE_THRESHOLD             60

/*
 * Minimum time between detected steps.
 *
 * 300 ms means a maximum of about 3.3 detected steps/second.
 */

#define STEP_MIN_INTERVAL_MS               300


/*
 * =============================================================================
 * SLEEP DETECTION
 * =============================================================================
 *
 * Five minutes without significant movement is considered sleep.
 *
 */

#define SLEEP_INACTIVITY_TIME_MS           (5UL * 60UL * 1000UL)


/*
 * =============================================================================
 * INTERNAL STATE
 * =============================================================================
 */

static uint32_t step_count = 0;

static uint32_t tracker_time_ms = 0;

static uint32_t last_step_time_ms = 0;

static uint32_t inactivity_time_ms = 0;

static uint32_t sleep_start_time_ms = 0;

static uint32_t sleep_duration_ms = 0;

static uint8_t sleeping = 0;

static uint8_t activity_level =
    SLEEP_TRACKER_ACTIVITY_REST;


/*
 * Previous accelerometer sample.
 */

static int16_t previous_x = 0;
static int16_t previous_y = 0;
static int16_t previous_z = 0;


/*
 * First sample flag.
 */

static uint8_t first_sample = 1;


/*
 * Peak detector state.
 *
 * 0 = waiting for movement peak
 * 1 = peak detected, waiting for signal to fall
 */

static uint8_t step_peak_detected = 0;


/*
 * 10-sample sliding window for accelerometer analysis.
 */
static int16_t window_x[ACCEL_WINDOW_SIZE];
static int16_t window_y[ACCEL_WINDOW_SIZE];
static int16_t window_z[ACCEL_WINDOW_SIZE];
static uint8_t window_sample_count = 0;


/*
 * =============================================================================
 * ABSOLUTE DIFFERENCE
 * =============================================================================
 */

static uint32_t absolute_difference(
    int16_t a,
    int16_t b
)
{
    int32_t difference;

    difference =
        (int32_t)a - (int32_t)b;

    if (difference < 0)
    {
        difference = -difference;
    }

    return (uint32_t)difference;
}


/*
 * =============================================================================
 * INITIALIZE
 * =============================================================================
 */

void sleep_tracker_init(void)
{
    step_count = 0;

    tracker_time_ms = 0;

    last_step_time_ms = 0;

    inactivity_time_ms = 0;

    sleep_start_time_ms = 0;

    sleep_duration_ms = 0;

    sleeping = 0;

    activity_level =
        SLEEP_TRACKER_ACTIVITY_REST;

    previous_x = 0;
    previous_y = 0;
    previous_z = 0;

    first_sample = 1;

    step_peak_detected = 0;

    window_sample_count = 0;
    for (uint8_t i = 0; i < ACCEL_WINDOW_SIZE; i++)
    {
        window_x[i] = 0;
        window_y[i] = 0;
        window_z[i] = 0;
    }
}


/*
 * =============================================================================
 * ANALYZE 10 ACCEL SAMPLES
 *
 * Analyzes a 10-sample window (~1 second at 100 ms) and classifies into:
 *   - SLEEP_TRACKER_ACTIVITY_SLEEP: virtually no movement (range < 80, deltas < 40)
 *   - SLEEP_TRACKER_ACTIVITY_WALK : sustained rhythmic movement with step peaks
 *   - SLEEP_TRACKER_ACTIVITY_MOVE : movement detected, but not a sustained walk
 * =============================================================================
 */

uint8_t sleep_tracker_analyze_10_samples(
    const int16_t x[ACCEL_WINDOW_SIZE],
    const int16_t y[ACCEL_WINDOW_SIZE],
    const int16_t z[ACCEL_WINDOW_SIZE]
)
{
    uint8_t i;
    int16_t min_x = x[0], max_x = x[0];
    int16_t min_y = y[0], max_y = y[0];
    int16_t min_z = z[0], max_z = z[0];
    uint32_t total_delta = 0;
    uint32_t max_delta = 0;
    uint8_t active_deltas = 0;
    uint8_t walk_peaks = 0;

    for (i = 0; i < ACCEL_WINDOW_SIZE; i++)
    {
        if (x[i] < min_x) min_x = x[i];
        if (x[i] > max_x) max_x = x[i];
        if (y[i] < min_y) min_y = y[i];
        if (y[i] > max_y) max_y = y[i];
        if (z[i] < min_z) min_z = z[i];
        if (z[i] > max_z) max_z = z[i];

        if (i > 0)
        {
            uint32_t d = absolute_difference(x[i], x[i - 1]) +
                         absolute_difference(y[i], y[i - 1]) +
                         absolute_difference(z[i], z[i - 1]);
            total_delta += d;
            if (d > max_delta)
            {
                max_delta = d;
            }
            if (d >= MOTION_THRESHOLD)
            {
                active_deltas++;
            }
            if (d >= STEP_PEAK_THRESHOLD)
            {
                walk_peaks++;
            }
        }
    }

    uint32_t total_range = (uint32_t)(max_x - min_x) +
                           (uint32_t)(max_y - min_y) +
                           (uint32_t)(max_z - min_z);

    /*
     * 1. SLEEP:
     *    Almost no movement across the whole 10-sample window.
     *    Total range is within stationary sensor noise (< 30) and max delta < 15.
     */
    if ((total_range < SLEEP_RANGE_THRESHOLD) && (max_delta < SLEEP_MOTION_THRESHOLD))
    {
        return SLEEP_TRACKER_ACTIVITY_SLEEP;
    }

    /*
     * 2. WALK:
     *    Rhythmic, sustained high-amplitude movement.
     *    At least one peak reaches STEP_PEAK_THRESHOLD (or large total range >= 250),
     *    AND multiple samples (>= 3) show active movement (sustained cadence).
     */
    if (((walk_peaks > 0) || (total_range >= 250)) && (active_deltas >= 3))
    {
        return SLEEP_TRACKER_ACTIVITY_WALK;
    }

    /*
     * 3. JUST A MOVE ("MOVE"):
     *    Movement is detected above sleep threshold, but it lacks the sustained
     *    or high-amplitude periodicity of walking (isolated shift, twitch, or slow movement).
     */
    return SLEEP_TRACKER_ACTIVITY_MOVE;
}


/*
 * =============================================================================
 * GET LATEST RAW ACCEL READING
 * =============================================================================
 */

void sleep_tracker_get_latest_xyz(
    int16_t *x,
    int16_t *y,
    int16_t *z
)
{
    if (x != 0) *x = previous_x;
    if (y != 0) *y = previous_y;
    if (z != 0) *z = previous_z;
}


/*
 * =============================================================================
 * UPDATE TRACKER
 * =============================================================================
 */

void sleep_tracker_update(void)
{
    int16_t x;
    int16_t y;
    int16_t z;

    uint32_t motion_x;
    uint32_t motion_y;
    uint32_t motion_z;

    uint32_t motion;


    /*
     * Read accelerometer.
     */

    adxl362_read_xyz(
        &x,
        &y,
        &z
    );


    /*
     * First sample establishes the reference.
     */

    if (first_sample)
    {
        previous_x = x;
        previous_y = y;
        previous_z = z;

        first_sample = 0;

        for (uint8_t i = 0; i < ACCEL_WINDOW_SIZE; i++)
        {
            window_x[i] = x;
            window_y[i] = y;
            window_z[i] = z;
        }
        window_sample_count = 1;

        return;
    }


    /*
     * Shift window and store newest sample in 10-sample buffer.
     */
    for (uint8_t i = 0; i < (ACCEL_WINDOW_SIZE - 1); i++)
    {
        window_x[i] = window_x[i + 1];
        window_y[i] = window_y[i + 1];
        window_z[i] = window_z[i + 1];
    }
    window_x[ACCEL_WINDOW_SIZE - 1] = x;
    window_y[ACCEL_WINDOW_SIZE - 1] = y;
    window_z[ACCEL_WINDOW_SIZE - 1] = z;

    if (window_sample_count < ACCEL_WINDOW_SIZE)
    {
        window_sample_count++;
    }


    /*
     * Calculate movement between samples.
     */

    motion_x =
        absolute_difference(
            x,
            previous_x
        );

    motion_y =
        absolute_difference(
            y,
            previous_y
        );

    motion_z =
        absolute_difference(
            z,
            previous_z
        );


    /*
     * Total movement.
     */

    motion =
        motion_x +
        motion_y +
        motion_z;


    /*
     * Save current sample.
     */

    previous_x = x;
    previous_y = y;
    previous_z = z;


    /*
     * Advance tracker time.
     */

    tracker_time_ms +=
        SLEEP_TRACKER_UPDATE_PERIOD_MS;


    /*
     * =============================================================================
     * 10-SAMPLE ACTIVITY CLASSIFICATION
     * =============================================================================
     *
     * Analyzes the 10 samples to determine whether it is:
     * SLEEP, JUST MOVE, or WALK.
     */

    activity_level =
        sleep_tracker_analyze_10_samples(
            window_x,
            window_y,
            window_z
        );


    /*
     * =============================================================================
     * STEP DETECTION
     * =============================================================================
     *
     * A step requires:
     *
     * 1. Movement reaches STEP_PEAK_THRESHOLD.
     * 2. Minimum time has passed since previous step.
     * 3. The movement must subsequently fall below the release threshold.
     *
     */

    if (!step_peak_detected)
    {
        if ((motion >= STEP_PEAK_THRESHOLD) &&
            ((tracker_time_ms - last_step_time_ms) >=
             STEP_MIN_INTERVAL_MS))
        {
            step_count++;

            last_step_time_ms =
                tracker_time_ms;

            step_peak_detected = 1;

            activity_level =
                SLEEP_TRACKER_ACTIVITY_WALK;
        }
    }
    else
    {
        /*
         * Wait for movement to fall before allowing
         * another step.
         */

        if (motion <= STEP_RELEASE_THRESHOLD)
        {
            step_peak_detected = 0;
        }
    }


    /*
     * =============================================================================
     * SLEEP / INACTIVITY TRACKING
     * =============================================================================
     */

    if (motion >= MOTION_THRESHOLD)
    {
        /*
         * Movement detected.
         */

        inactivity_time_ms = 0;


        /*
         * If currently sleeping, wake up.
         */

        if (sleeping)
        {
            sleeping = 0;

            sleep_duration_ms +=
                tracker_time_ms -
                sleep_start_time_ms;
        }
    }
    else
    {
        /*
         * No significant movement.
         */

        inactivity_time_ms +=
            SLEEP_TRACKER_UPDATE_PERIOD_MS;


        /*
         * Enter sleep after five minutes.
         */

        if ((!sleeping) &&
            (inactivity_time_ms >=
             SLEEP_INACTIVITY_TIME_MS))
        {
            sleeping = 1;

            sleep_start_time_ms =
                tracker_time_ms -
                SLEEP_INACTIVITY_TIME_MS;
        }
    }


    /*
     * =============================================================================
     * SLEEP STATE OVERRIDES ACTIVITY
     * =============================================================================
     */

    if (sleeping)
    {
        sleep_duration_ms =
            tracker_time_ms -
            sleep_start_time_ms;

        activity_level =
            SLEEP_TRACKER_ACTIVITY_SLEEP;
    }
}


/*
 * =============================================================================
 * GET STEP COUNT
 * =============================================================================
 */

uint32_t sleep_tracker_get_steps(void)
{
    return step_count;
}


/*
 * =============================================================================
 * RESET STEPS
 * =============================================================================
 */

void sleep_tracker_reset_steps(void)
{
    step_count = 0;

    step_peak_detected = 0;

    last_step_time_ms =
        tracker_time_ms;
}


/*
 * =============================================================================
 * GET SLEEP MINUTES
 * =============================================================================
 */

uint32_t sleep_tracker_get_sleep_minutes(void)
{
    return sleep_duration_ms /
           60000UL;
}


/*
 * =============================================================================
 * GET SLEEP STATE
 * =============================================================================
 */

uint8_t sleep_tracker_is_sleeping(void)
{
    return sleeping;
}


/*
 * =============================================================================
 * GET ACTIVITY
 * =============================================================================
 */

uint8_t sleep_tracker_get_activity_level(void)
{
    return activity_level;
}


/*
 * =============================================================================
 * RESET SLEEP
 * =============================================================================
 */

void sleep_tracker_reset_sleep(void)
{
    inactivity_time_ms = 0;

    sleep_start_time_ms = 0;

    sleep_duration_ms = 0;

    sleeping = 0;

    activity_level =
        SLEEP_TRACKER_ACTIVITY_SLEEP;
}


/*
 * =============================================================================
 * GET ACTIVITY STRING
 * =============================================================================
 */

const char *sleep_tracker_get_activity_string(void)
{
    if (activity_level == SLEEP_TRACKER_ACTIVITY_WALK)
    {
        return "WALK";
    }
    else if (activity_level == SLEEP_TRACKER_ACTIVITY_MOVE)
    {
        return "JUST MOVE";
    }
    else
    {
        return "SLEEP";
    }
}


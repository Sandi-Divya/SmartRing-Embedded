#ifndef SLEEP_TRACKER_H
#define SLEEP_TRACKER_H

#include <stdint.h>

/*
 * =============================================================================
 * ACTIVITY LEVELS
 * =============================================================================
 */

#define SLEEP_TRACKER_ACTIVITY_SLEEP     0
#define SLEEP_TRACKER_ACTIVITY_MOVE      1
#define SLEEP_TRACKER_ACTIVITY_WALK      2

/* Compatibility aliases */
#define SLEEP_TRACKER_ACTIVITY_REST      SLEEP_TRACKER_ACTIVITY_SLEEP
#define SLEEP_TRACKER_ACTIVITY_ACTIVE    SLEEP_TRACKER_ACTIVITY_MOVE

#define ACCEL_WINDOW_SIZE                10


/*
 * =============================================================================
 * INITIALIZATION
 * =============================================================================
 */

void sleep_tracker_init(void);


/*
 * =============================================================================
 * MAIN UPDATE
 *
 * Call this periodically.
 * The current implementation expects approximately 100 ms between calls.
 * =============================================================================
 */

void sleep_tracker_update(void);

void sleep_tracker_get_latest_xyz(int16_t *x, int16_t *y, int16_t *z);


/*
 * =============================================================================
 * STEP INFORMATION
 * =============================================================================
 */

uint32_t sleep_tracker_get_steps(void);

void sleep_tracker_reset_steps(void);


/*
 * =============================================================================
 * SLEEP INFORMATION
 * =============================================================================
 */

uint32_t sleep_tracker_get_sleep_minutes(void);

uint8_t sleep_tracker_is_sleeping(void);

void sleep_tracker_reset_sleep(void);


/*
 * =============================================================================
 * ACTIVITY INFORMATION
 * =============================================================================
 */

uint8_t sleep_tracker_get_activity_level(void);

/*
 * =============================================================================
 * 10-SAMPLE ANALYSIS
 * =============================================================================
 */

uint8_t sleep_tracker_analyze_10_samples(
    const int16_t x[ACCEL_WINDOW_SIZE],
    const int16_t y[ACCEL_WINDOW_SIZE],
    const int16_t z[ACCEL_WINDOW_SIZE]
);

const char *sleep_tracker_get_activity_string(void);

#endif /* SLEEP_TRACKER_H */

/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.c
 *
 * @brief Custom Profile 1 implementation.
 *
 ****************************************************************************************
 */

#include "rwip_config.h"

#include <stdint.h>
#include <string.h>

#include "gpio.h"
#include "app.h"
#include "app_api.h"
#include "prf_utils.h"

#include "custs1.h"
#include "custs1_task.h"

#include "user_custs1_def.h"
#include "user_custs1_impl.h"

#include "user_peripheral.h"
#include "user_periph_setup.h"


/*
 * GLOBALS FROM user_peripheral.c
 ****************************************************************************************
 */

extern uint8_t current_batt_lvl;
extern uint8_t current_hr_value;


/*
 ****************************************************************************************
 * EXTERNAL FUNCTIONS
 ****************************************************************************************
 */

extern uint8_t read_battery_level_percentage(void);

extern void app_batt_send_telemetry_ntf(uint8_t batt_lvl);

extern void app_hr_send_telemetry_ntf(uint8_t hr);

extern void app_clock_set_time(uint8_t hour, uint8_t minute);


/*
 ****************************************************************************************
 * BATTERY
 ****************************************************************************************
 */

static void battery_command(void)
{
    uint8_t batt;

    batt = read_battery_level_percentage();

    if (batt > 100)
        batt = 100;

    current_batt_lvl = batt;

    /*
     * Send exactly like the working battery implementation.
     *
     * One raw byte:
     *      85 -> 0x55
     *      100 -> 0x64
     */
    app_batt_send_telemetry_ntf(batt);
}


/*
 ****************************************************************************************
 * SERVICE 1 CONTROL POINT
 *
 * Battery is NOT triggered by 'B', 'b' or 0x01.
 * Battery is automatic telemetry.
 ****************************************************************************************
 */

void user_svc1_ctrl_wr_ind_handler(ke_msg_id_t const msgid,
                                   struct custs1_val_write_ind const *param,
                                   const ke_task_id_t dest_id,
                                   const ke_task_id_t src_id)
{
    (void)msgid;
    (void)dest_id;
    (void)src_id;

    if ((param == NULL) || (param->length == 0))
        return;

    /*
     * No battery command here.
     *
     * Future control commands can be added here.
     */
}


/*
 ****************************************************************************************
 * LED CHARACTERISTIC
 ****************************************************************************************
 */

void user_svc1_led_wr_ind_handler(ke_msg_id_t const msgid,
                                  struct custs1_val_write_ind const *param,
                                  const ke_task_id_t dest_id,
                                  const ke_task_id_t src_id)
{
    (void)msgid;
    (void)dest_id;
    (void)src_id;

    if ((param == NULL) || (param->length == 0))
        return;

    if ((param->value[0] == 0x01) ||
        (param->value[0] == '1'))
    {
        GPIO_SetActive(GPIO_LED_PORT, GPIO_LED_PIN);
    }
    else if ((param->value[0] == 0x00) ||
             (param->value[0] == '0'))
    {
        GPIO_SetInactive(GPIO_LED_PORT, GPIO_LED_PIN);
    }
}


/*
 ****************************************************************************************
 * BATTERY CCCD
 ****************************************************************************************
 */

void user_svc1_adc_val_1_cfg_ind_handler(
                                ke_msg_id_t const msgid,
                                struct custs1_val_write_ind const *param,
                                const ke_task_id_t dest_id,
                                const ke_task_id_t src_id)
{
    (void)msgid;
    (void)param;
    (void)dest_id;
    (void)src_id;

    /*
     * CUSTS1 handles the CCCD.
     */
}


/*
 ****************************************************************************************
 * BUTTON CCCD
 ****************************************************************************************
 */

void user_svc1_button_cfg_ind_handler(
                                ke_msg_id_t const msgid,
                                struct custs1_val_write_ind const *param,
                                const ke_task_id_t dest_id,
                                const ke_task_id_t src_id)
{
    (void)msgid;
    (void)param;
    (void)dest_id;
    (void)src_id;

    /*
     * CUSTS1 handles the CCCD.
     */
}


/*
 ****************************************************************************************
 * LONG VALUE CCCD
 ****************************************************************************************
 */

void user_svc1_long_val_cfg_ind_handler(
                                ke_msg_id_t const msgid,
                                struct custs1_val_write_ind const *param,
                                const ke_task_id_t dest_id,
                                const ke_task_id_t src_id)
{
    (void)msgid;
    (void)param;
    (void)dest_id;
    (void)src_id;

    /*
     * CUSTS1 handles the CCCD.
     */
}


/*
 ****************************************************************************************
 * LONG VALUE WRITE
 ****************************************************************************************
 */

void user_svc1_long_val_wr_ind_handler(
                                ke_msg_id_t const msgid,
                                struct custs1_val_write_ind const *param,
                                const ke_task_id_t dest_id,
                                const ke_task_id_t src_id)
{
    (void)msgid;
    (void)dest_id;
    (void)src_id;

    if ((param == NULL) || (param->length == 0))
        return;

    /*
     * Value has already been accepted by CUSTS1.
     */
}


/*
 ****************************************************************************************
 * LONG VALUE ATTRIBUTE INFORMATION
 ****************************************************************************************
 */

void user_svc1_long_val_att_info_req_handler(
                                ke_msg_id_t const msgid,
                                struct custs1_att_info_req const *param,
                                const ke_task_id_t dest_id,
                                const ke_task_id_t src_id)
{
    struct custs1_att_info_rsp *rsp;

    (void)msgid;
    (void)dest_id;

    if (param == NULL)
        return;

    rsp = KE_MSG_ALLOC(CUSTS1_ATT_INFO_RSP,
                       src_id,
                       dest_id,
                       custs1_att_info_rsp);

    rsp->att_idx = param->att_idx;
    rsp->length = DEF_SVC1_LONG_VALUE_CHAR_LEN;
    rsp->status = ATT_ERR_NO_ERROR;

    KE_MSG_SEND(rsp);
}


/*
 ****************************************************************************************
 * OTHER ATTRIBUTE INFORMATION
 ****************************************************************************************
 */

void user_svc1_rest_att_info_req_handler(
                                ke_msg_id_t const msgid,
                                struct custs1_att_info_req const *param,
                                const ke_task_id_t dest_id,
                                const ke_task_id_t src_id)
{
    struct custs1_att_info_rsp *rsp;

    (void)msgid;
    (void)dest_id;

    if (param == NULL)
        return;

    rsp = KE_MSG_ALLOC(CUSTS1_ATT_INFO_RSP,
                       src_id,
                       dest_id,
                       custs1_att_info_rsp);

    rsp->att_idx = param->att_idx;

    /*
     * Normal DB-backed attributes.
     */
    rsp->length = 0;
    rsp->status = ATT_ERR_NO_ERROR;

    KE_MSG_SEND(rsp);
}


/*
 ****************************************************************************************
 * SERVICE 3 NON-DATABASE READ
 ****************************************************************************************
 */

void user_svc3_read_non_db_val_handler(
                                ke_msg_id_t const msgid,
                                struct custs1_value_req_ind const *param,
                                const ke_task_id_t dest_id,
                                const ke_task_id_t src_id)
{
    struct custs1_value_req_rsp *rsp;

    (void)msgid;

    if (param == NULL)
        return;

    rsp = KE_MSG_ALLOC_DYN(CUSTS1_VALUE_REQ_RSP,
                           src_id,
                           dest_id,
                           custs1_value_req_rsp,
                           1);

    rsp->conidx = param->conidx;
    rsp->att_idx = param->att_idx;

    /*
     * One-byte HR/application-managed value.
     *
     * This is only used for READ.
     * Notifications are sent using app_hr_send_telemetry_ntf().
     */
    rsp->length = 1;

    rsp->value[0] = current_hr_value;

    rsp->status = ATT_ERR_NO_ERROR;

    KE_MSG_SEND(rsp);
}


/*
 ****************************************************************************************
 * BATTERY UPDATE
 *
 * Same mechanism that is already working in the app.
 ****************************************************************************************
 */

void user_update_telemetry_value(uint8_t batt_lvl)
{
    if (batt_lvl > 100)
        batt_lvl = 100;

    current_batt_lvl = batt_lvl;

    app_batt_send_telemetry_ntf(batt_lvl);
}


/*
 ****************************************************************************************
 * HEART RATE UPDATE
 *
 * Same mechanism as battery.
 *
 * HR is sent as ONE RAW BYTE.
 *
 * Example:
 *
 *      75 BPM = 0x4B
 *      80 BPM = 0x50
 *      100 BPM = 0x64
 ****************************************************************************************
 */

void user_update_heart_rate_value(uint8_t hr)
{
    current_hr_value = hr;

    /*
     * This function sends the value through:
     *
     * SVC3_IDX_HR_VAL_VAL
     *
     * UUID:
     * 16005991-b131-3396-014c-664c9867b919
     */
    app_hr_send_telemetry_ntf(hr);
}


/*
 ****************************************************************************************
 * BATTERY TIMER CALLBACK
 ****************************************************************************************
 */

void app_adcval1_timer_cb_handler(void)
{
    uint8_t batt;

    batt = read_battery_level_percentage();

    if (batt > 100)
        batt = 100;

    current_batt_lvl = batt;

    app_batt_send_telemetry_ntf(batt);
}


/*
 ****************************************************************************************
 * HEART RATE TIMER CALLBACK
 *
 * TEST VALUE
 *
 * This sends 75 BPM exactly like the battery notification.
 *
 * If user_peripheral.c already has an HR polling timer,
 * that timer can be used instead and this callback does not
 * need to be called.
 ****************************************************************************************
 */

void app_hr_timer_cb_handler(void)
{
    uint8_t hr;

    /*
     * Temporary test HR.
     *
     * 75 decimal = 0x4B
     */
    hr = 75;

    current_hr_value = hr;

    app_hr_send_telemetry_ntf(hr);
}

/*
 ****************************************************************************************
 * CLOCK WRITE
 *
 * Phone sends exactly 2 bytes:
 *
 *      byte 0 = hour   (0 - 23)
 *      byte 1 = minute (0 - 59)
 *
 * Example:
 *
 *      14:35
 *
 *      [0x0E, 0x23]
 *
 ****************************************************************************************
 */

void user_svc3_clock_wr_ind_handler(
                                ke_msg_id_t const msgid,
                                struct custs1_val_write_ind const *param,
                                const ke_task_id_t dest_id,
                                const ke_task_id_t src_id)
{
    (void)msgid;
    (void)dest_id;
    (void)src_id;

    if (param == NULL)
        return;

    /*
     * Clock requires hour + minute.
     */
    if (param->length < 2)
        return;

    /*
     * Validate hour.
     */
    if (param->value[0] >= 24)
        return;

    /*
     * Validate minute.
     */
    if (param->value[1] >= 60)
        return;

    /*
     ****************************************************************************************
     * SET CLOCK
     ****************************************************************************************
     */

    app_clock_set_time(
        param->value[0],
        param->value[1]
    );
}
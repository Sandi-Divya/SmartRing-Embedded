/**
 ****************************************************************************************
 * @file user_peripheral.c
 * @brief Peripheral BLE Application with Direct OLED Control and Double-Tap Battery UI.
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <string.h>
#include "rwip_config.h"             // RivetWave IP kernel, stack, and memory profile config
#include "gattc_task.h"              // GATT Client task interface for event confirmations
#include "gap.h"                     // Generic Access Profile roles, flags, and macros
#include "app_easy_timer.h"          // SDK non-blocking software timer interface (10ms ticks)
#include "user_peripheral.h"         // Peripheral application definitions
#include "user_custs1_impl.h"        // Custom profile GATT event handlers
#include "user_custs1_def.h"         // Custom database indices (SVC1_IDX_...)
#include "co_bt.h"                   // Bluetooth Core constants and error definitions
#include "gpio.h"                    // Low-level GPIO register interface
#include "display.h"                 // OLED display driver interface
#include "wkupct_quadec.h"           // Dialog Wake-Up Controller & Debounce engine
#include "adc.h"                     // DA14585 General Purpose ADC main driver interface
#include "prf_utils.h"               // Profile utility functions (prf_get_task_from_id)
#include "custs1_task.h"             // Custom profile 1 task interface definitions

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct mnf_specific_data_ad_structure
{
    uint8_t ad_structure_size;                         // Total payload size minus size byte
    uint8_t ad_structure_type;                         // GAP AD Type: 0xFF (Manufacturer Specific)
    uint8_t company_id[APP_AD_MSD_COMPANY_ID_LEN];     // 2-byte assigned Bluetooth Company ID
    uint8_t proprietary_data[APP_AD_MSD_DATA_LEN];     // Dynamic runtime telemetry payload
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint8_t app_connection_idx                      __SECTION_ZERO("retention_mem_area0");
timer_hnd app_adv_data_update_timer_used        __SECTION_ZERO("retention_mem_area0");
timer_hnd app_param_update_request_timer_used   __SECTION_ZERO("retention_mem_area0");
timer_hnd app_led_off_timer_used                __SECTION_ZERO("retention_mem_area0");

// Touch & Multi-Tap State Variables
uint8_t touch_count                             __SECTION_ZERO("retention_mem_area0");
timer_hnd app_touch_clear_timer                 __SECTION_ZERO("retention_mem_area0");
uint8_t tap_click_count                         __SECTION_ZERO("retention_mem_area0");
timer_hnd app_tap_eval_timer                    __SECTION_ZERO("retention_mem_area0");

// Battery Monitoring Variables
uint8_t current_batt_lvl                        __SECTION_ZERO("retention_mem_area0");
timer_hnd app_batt_poll_timer                   __SECTION_ZERO("retention_mem_area0");

struct mnf_specific_data_ad_structure mnf_data  __SECTION_ZERO("retention_mem_area0");
uint8_t mnf_data_index                          __SECTION_ZERO("retention_mem_area0");

uint8_t stored_adv_data_len                     __SECTION_ZERO("retention_mem_area0");
uint8_t stored_scan_rsp_data_len                __SECTION_ZERO("retention_mem_area0");
uint8_t stored_adv_data[ADV_DATA_LEN]           __SECTION_ZERO("retention_mem_area0");
uint8_t stored_scan_rsp_data[SCAN_RSP_DATA_LEN] __SECTION_ZERO("retention_mem_area0");

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static void led_auto_off_timer_cb(void)
{
    GPIO_SetInactive(GPIO_PORT_1, GPIO_PIN_0);
    app_led_off_timer_used = EASY_TIMER_INVALID_TIMER;
}

static void touch_clear_cb(void)
{
    display_clear();
    app_touch_clear_timer = EASY_TIMER_INVALID_TIMER;
}

uint8_t read_battery_level_percentage(void)
{
    uint32_t adc_sample = adc_get_vbat_sample(false);

    if (adc_sample <= 1200)
    {
        return 0;
    }
    if (adc_sample >= 1700)
    {
        return 100;
    }

    return (uint8_t)(((adc_sample - 1200) * 100) / (1700 - 1200));
}

void app_batt_send_telemetry_ntf(uint8_t batt_lvl)
{
    if (app_connection_idx == GAP_INVALID_CONIDX || app_env[app_connection_idx].conidx == GAP_INVALID_CONIDX)
    {
        return;
    }

    // 1. Update the local GATT database attribute value so explicit reads from nRF Connect return the latest battery level
    struct custs1_val_set_req *req_db = KE_MSG_ALLOC(CUSTS1_VAL_SET_REQ,
                                                     prf_get_task_from_id(TASK_ID_CUSTS1),
                                                     TASK_APP,
                                                     custs1_val_set_req);
    req_db->handle = SVC1_IDX_ADC_VAL_1_VAL;
    req_db->length = sizeof(uint8_t);
    req_db->value[0] = batt_lvl;
    KE_MSG_SEND(req_db);

    // 2. Allocate kernel notification request message to push live telemetry
    struct custs1_val_ntf_ind_req *req_ntf = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                              prf_get_task_from_id(TASK_ID_CUSTS1),
                                                              TASK_APP,
                                                              custs1_val_ntf_ind_req,
                                                              sizeof(uint8_t));

    req_ntf->handle = SVC1_IDX_ADC_VAL_1_VAL; // Route to ADC characteristic in database
    req_ntf->length = sizeof(uint8_t);
    req_ntf->value[0] = batt_lvl;
    req_ntf->conidx = app_env[app_connection_idx].conidx;
    req_ntf->notification = true;           // Set true for unacknowledged notification

    KE_MSG_SEND(req_ntf);
}

static void batt_poll_timer_cb(void)
{
    current_batt_lvl = read_battery_level_percentage();
    app_batt_send_telemetry_ntf(current_batt_lvl);
    app_batt_poll_timer = app_easy_timer(500, batt_poll_timer_cb);
}

static void tap_eval_timer_cb(void)
{
    // Increment local step counter (supports continuous counting up to 65535)
    touch_count++;

    // Render 2-line Step UI ("STEPS" on Page 1, count on Page 4)
    display_show_steps(touch_count);

    // Keep display visible for 3 seconds (300 * 10ms = 3000ms)
    if (app_touch_clear_timer != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_touch_clear_timer);
    }
    app_touch_clear_timer = app_easy_timer(300, touch_clear_cb);

    tap_click_count = 0;
    app_tap_eval_timer = EASY_TIMER_INVALID_TIMER;
}

static void touch_button_press_cb(void)
{
    // 1. Diagnostic LED Pulse on P1_0 (1 second)
    GPIO_SetActive(GPIO_PORT_1, GPIO_PIN_0);
    if (app_led_off_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_led_off_timer_used);
    }
    app_led_off_timer_used = app_easy_timer(100, led_auto_off_timer_cb);

    // 2. Multi-Tap Evaluation Logic
    tap_click_count++;

    if (tap_click_count == 1)
    {
        // 1st Tap: Start 350ms window to catch a double tap
        app_tap_eval_timer = app_easy_timer(35, tap_eval_timer_cb);
    }
    else if (tap_click_count >= 2)
    {
        // DOUBLE TAP CONFIRMED: Cancel single-tap timer
        if (app_tap_eval_timer != EASY_TIMER_INVALID_TIMER)
        {
            app_easy_timer_cancel(app_tap_eval_timer);
            app_tap_eval_timer = EASY_TIMER_INVALID_TIMER;
        }
        tap_click_count = 0;

        // Sample battery voltage and display "BAT: XX%"
        current_batt_lvl = read_battery_level_percentage();
        display_show_battery(current_batt_lvl);

        // Send telemetry notification over BLE
        app_batt_send_telemetry_ntf(current_batt_lvl);

        // Keep Battery Percentage visible for EXACTLY 3 SECONDS (300 * 10ms = 3000ms)
        if (app_touch_clear_timer != EASY_TIMER_INVALID_TIMER)
        {
            app_easy_timer_cancel(app_touch_clear_timer);
        }
        app_touch_clear_timer = app_easy_timer(300, touch_clear_cb);
    }

    // 3. Re-arm Wake-Up Controller on P1_3
    wkupct_register_callback(touch_button_press_cb);
    wkupct_enable_irq(
        WKUPCT_PIN_SELECT(GPIO_PORT_1, GPIO_PIN_3),
        WKUPCT_PIN_POLARITY(GPIO_PORT_1, GPIO_PIN_3, WKUPCT_PIN_POLARITY_LOW),
        1,
        40
    );
}

static void touch_button_init(void)
{
    touch_count = 0;
    tap_click_count = 0;
    app_tap_eval_timer = EASY_TIMER_INVALID_TIMER;
    app_touch_clear_timer = EASY_TIMER_INVALID_TIMER;

    wkupct_register_callback(touch_button_press_cb);
    wkupct_enable_irq(
        WKUPCT_PIN_SELECT(GPIO_PORT_1, GPIO_PIN_3),
        WKUPCT_PIN_POLARITY(GPIO_PORT_1, GPIO_PIN_3, WKUPCT_PIN_POLARITY_LOW),
        1,
        40
    );
}

static void mnf_data_init(void)
{
    mnf_data.ad_structure_size = sizeof(struct mnf_specific_data_ad_structure) - sizeof(uint8_t);
    mnf_data.ad_structure_type = GAP_AD_TYPE_MANU_SPECIFIC_DATA;
    mnf_data.company_id[0] = APP_AD_MSD_COMPANY_ID & 0xFF;
    mnf_data.company_id[1] = (APP_AD_MSD_COMPANY_ID >> 8) & 0xFF;
    mnf_data.proprietary_data[0] = 0;
    mnf_data.proprietary_data[1] = 0;
}

static void mnf_data_update(void)
{
    uint16_t data;

    data = mnf_data.proprietary_data[0] | (mnf_data.proprietary_data[1] << 8);
    data += 1;
    mnf_data.proprietary_data[0] = data & 0xFF;
    mnf_data.proprietary_data[1] = (data >> 8) & 0xFF;

    if (data == 0xFFFF) {
         mnf_data.proprietary_data[0] = 0;
         mnf_data.proprietary_data[1] = 0;
    }
}

static void app_add_ad_struct(struct gapm_start_advertise_cmd *cmd, void *ad_struct_data, uint8_t ad_struct_len, uint8_t adv_connectable)
{
    uint8_t adv_data_max_size = (adv_connectable) ? (ADV_DATA_LEN - 3) : (ADV_DATA_LEN);

    if ((adv_data_max_size - cmd->info.host.adv_data_len) >= ad_struct_len)
    {
        memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len], ad_struct_data, ad_struct_len);
        cmd->info.host.adv_data_len += ad_struct_len;
        mnf_data_index = cmd->info.host.adv_data_len - sizeof(struct mnf_specific_data_ad_structure);
    }
    else if ((SCAN_RSP_DATA_LEN - cmd->info.host.scan_rsp_data_len) >= ad_struct_len)
    {
        memcpy(&cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len], ad_struct_data, ad_struct_len);
        cmd->info.host.scan_rsp_data_len += ad_struct_len;
        mnf_data_index = cmd->info.host.scan_rsp_data_len - sizeof(struct mnf_specific_data_ad_structure);
        mnf_data_index |= 0x80;
    }
    else
    {
        ASSERT_WARNING(0);
    }

    stored_adv_data_len = cmd->info.host.adv_data_len;
    memcpy(stored_adv_data, cmd->info.host.adv_data, stored_adv_data_len);
    stored_scan_rsp_data_len = cmd->info.host.scan_rsp_data_len;
    memcpy(stored_scan_rsp_data, cmd->info.host.scan_rsp_data, stored_scan_rsp_data_len);
}

static void adv_data_update_timer_cb(void)
{
    uint8_t *mnf_data_storage = (mnf_data_index & 0x80) ? stored_scan_rsp_data : stored_adv_data;

    mnf_data_update();
    memcpy(mnf_data_storage + (mnf_data_index & 0x7F), &mnf_data, sizeof(struct mnf_specific_data_ad_structure));
    app_easy_gap_update_adv_data(stored_adv_data, stored_adv_data_len, stored_scan_rsp_data, stored_scan_rsp_data_len);
    
    app_adv_data_update_timer_used = app_easy_timer(APP_ADV_DATA_UPDATE_TO, adv_data_update_timer_cb);
}

static void param_update_request_timer_cb(void)
{
    app_easy_gap_param_update_start(app_connection_idx);
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
}

void user_app_init(void)
{
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    app_led_off_timer_used = EASY_TIMER_INVALID_TIMER;
    app_batt_poll_timer = EASY_TIMER_INVALID_TIMER;
    mnf_data_init();

    memcpy(stored_adv_data, USER_ADVERTISE_DATA, USER_ADVERTISE_DATA_LEN);
    stored_adv_data_len = USER_ADVERTISE_DATA_LEN;
    memcpy(stored_scan_rsp_data, USER_ADVERTISE_SCAN_RESPONSE_DATA, USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN);
    stored_scan_rsp_data_len = USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN;

    touch_button_init();

    current_batt_lvl = read_battery_level_percentage();
    app_batt_poll_timer = app_easy_timer(500, batt_poll_timer_cb);

    default_app_on_init();
}

void user_app_adv_start(void)
{
    app_adv_data_update_timer_used = app_easy_timer(APP_ADV_DATA_UPDATE_TO, adv_data_update_timer_cb);

    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();

    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);
    app_easy_gap_undirected_advertise_start();
}

void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;
        app_easy_timer_cancel(app_adv_data_update_timer_used);

        if ((param->con_interval < user_connection_param_conf.intv_min) ||
            (param->con_interval > user_connection_param_conf.intv_max) ||
            (param->con_latency != user_connection_param_conf.latency) ||
            (param->sup_to != user_connection_param_conf.time_out))
        {
            app_param_update_request_timer_used = app_easy_timer(APP_PARAM_UPDATE_REQUEST_TO, param_update_request_timer_cb);
        }
    }
    else
    {
        user_app_adv_start();
    }

    default_app_on_connection(connection_idx, param);
}

void user_app_adv_undirect_complete(uint8_t status)
{
    if (status == GAP_ERR_CANCELED)
    {
        user_app_adv_start();
    }
}

void user_app_disconnect(struct gapc_disconnect_ind const *param)
{
    if (app_param_update_request_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_param_update_request_timer_used);
        app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    }

    if (app_led_off_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_led_off_timer_used);
        app_led_off_timer_used = EASY_TIMER_INVALID_TIMER;
    }

    if (app_tap_eval_timer != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_tap_eval_timer);
        app_tap_eval_timer = EASY_TIMER_INVALID_TIMER;
    }

    if (app_touch_clear_timer != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_touch_clear_timer);
        app_touch_clear_timer = EASY_TIMER_INVALID_TIMER;
    }

    tap_click_count = 0;

    GPIO_SetInactive(GPIO_PORT_1, GPIO_PIN_0);
    display_clear();

    mnf_data_update();
    user_app_adv_start();
}

/**
 * @brief Master Custom Profile Kernel Message Router.
 * (Preserves working BLE write display logic exactly as originally provided).
 */
void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msgid)
    {
        case CUSTS1_VAL_WRITE_IND:
        {
            struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

            // 1. Diagnostic LED Pulse on P1_0 (1 second)
            GPIO_SetActive(GPIO_PORT_1, GPIO_PIN_0);
            if (app_led_off_timer_used != EASY_TIMER_INVALID_TIMER)
            {
                app_easy_timer_cancel(app_led_off_timer_used);
            }
            app_led_off_timer_used = app_easy_timer(100, led_auto_off_timer_cb);

            // 2. Parse Incoming BLE Command
            if (msg_param->length > 0)
            {
                uint8_t first_byte = msg_param->value[0];

                // Command Check: If phone writes 'B', 'b', or raw byte 0x01 -> Show Battery & Notify
                if (first_byte == 'B' || first_byte == 'b' || first_byte == 0x01)
                {
                    current_batt_lvl = read_battery_level_percentage();
                    display_show_battery(current_batt_lvl);
                    app_batt_send_telemetry_ntf(current_batt_lvl);

                    // Hold display for 3 seconds (300 * 10ms = 3000ms)
                    if (app_touch_clear_timer != EASY_TIMER_INVALID_TIMER)
                    {
                        app_easy_timer_cancel(app_touch_clear_timer);
                    }
                    app_touch_clear_timer = app_easy_timer(300, touch_clear_cb);
                }
                else
                {
                    // Standard text / character display logic ("BLE WRITE: ...")
                    char text_buf[21];
                    memset(text_buf, 0, sizeof(text_buf));

                    uint16_t copy_len = (msg_param->length > 20) ? 20 : msg_param->length;
                    memcpy(text_buf, msg_param->value, copy_len);
                    text_buf[copy_len] = '\0';

                    display_show_ble_write(text_buf);

                    if (app_touch_clear_timer != EASY_TIMER_INVALID_TIMER)
                    {
                        app_easy_timer_cancel(app_touch_clear_timer);
                    }
                    app_touch_clear_timer = app_easy_timer(300, touch_clear_cb);
                }
            }
        } break;

        case CUSTS1_VAL_NTF_CFM:
        case CUSTS1_VAL_IND_CFM:
        case GAPC_PARAM_UPDATED_IND:
            break;

        case CUSTS1_ATT_INFO_REQ:
        {
            struct custs1_att_info_req const *msg_param = (struct custs1_att_info_req const *)param;
            if (msg_param->att_idx == SVC1_IDX_LONG_VALUE_VAL)
            {
                user_svc1_long_val_att_info_req_handler(msgid, msg_param, dest_id, src_id);
            }
            else
            {
                user_svc1_rest_att_info_req_handler(msgid, msg_param, dest_id, src_id);
            }
        } break;

        case CUSTS1_VALUE_REQ_IND:
        {
            struct custs1_value_req_ind const *msg_param = (struct custs1_value_req_ind const *) param;
            if (msg_param->att_idx == SVC3_IDX_READ_4_VAL)
            {
                user_svc3_read_non_db_val_handler(msgid, msg_param, dest_id, src_id);
            }
            else
            {
                struct custs1_value_req_rsp *rsp = KE_MSG_ALLOC(CUSTS1_VALUE_REQ_RSP,
                                                                src_id,
                                                                dest_id,
                                                                custs1_value_req_rsp);
                rsp->conidx  = app_env[msg_param->conidx].conidx;
                rsp->att_idx = msg_param->att_idx;
                rsp->length = 0;
                rsp->status  = ATT_ERR_APP_ERROR;
                KE_MSG_SEND(rsp);
            }
        } break;

        case GATTC_EVENT_REQ_IND:
        {
            struct gattc_event_ind const *ind = (struct gattc_event_ind const *) param;
            struct gattc_event_cfm *cfm = KE_MSG_ALLOC(GATTC_EVENT_CFM, src_id, dest_id, gattc_event_cfm);
            cfm->handle = ind->handle;
            KE_MSG_SEND(cfm);
        } break;

        default:
            break;
    }
}
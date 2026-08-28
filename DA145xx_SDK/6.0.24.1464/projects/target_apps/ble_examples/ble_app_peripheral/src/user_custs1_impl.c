/**
 ****************************************************************************************
 * @file user_custs1_impl.c
 * @brief Custom Profile 1 (Custs1) GATT Handlers with Multi-Byte Dynamic String Display.
 ****************************************************************************************
 */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "gpio.h"
#include "app_api.h"
#include "app.h"
#include "prf_utils.h"
#include "custs1.h"
#include "custs1_task.h"
#include "user_custs1_def.h"
#include "user_custs1_impl.h"
#include "user_peripheral.h"
#include "user_periph_setup.h"
#include "display.h"

ke_msg_id_t timer_used        __SECTION_ZERO("retention_mem_area0");
uint16_t indication_counter   __SECTION_ZERO("retention_mem_area0");
uint16_t non_db_val_counter   __SECTION_ZERO("retention_mem_area0");
timer_hnd led_pulse_timer     __SECTION_ZERO("retention_mem_area0");

static void led_off_timer_cb(void)
{
    GPIO_SetInactive(GPIO_PORT_1, GPIO_PIN_0);
    led_pulse_timer = EASY_TIMER_INVALID_TIMER;
}

/**
 * @brief Displays single digits, letters, or multi-letter strings centered on the OLED.
 */
static void handle_display_write(const uint8_t *val_ptr, uint16_t length)
{
    // 1. Diagnostic LED Pulse on P1_0 (1 second)
    GPIO_SetActive(GPIO_PORT_1, GPIO_PIN_0);
    if (led_pulse_timer != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(led_pulse_timer);
    }
    led_pulse_timer = app_easy_timer(100, led_off_timer_cb);

    if (length == 0 || val_ptr == NULL) return;

    char text_buf[21];
    memset(text_buf, 0, sizeof(text_buf));

    // Handle single binary byte (0x00 to 0x09)
    if (length == 1 && val_ptr[0] <= 9)
    {
        text_buf[0] = '0' + val_ptr[0];
        text_buf[1] = '\0';
    }
    else
    {
        uint16_t copy_len = (length > 20) ? 20 : length;
        memcpy(text_buf, val_ptr, copy_len);
        text_buf[copy_len] = '\0';
    }

    // 2. Clear screen
    display_clear();

    // 3. Compute dynamic centering offset (each character is 6px wide)
    uint8_t str_len = (uint8_t)strlen(text_buf);
    uint8_t total_px = (uint8_t)(str_len * 6);
    uint8_t start_col = (total_px < 128) ? (uint8_t)((128 - total_px) / 2) : 0;

    // 4. Render the string centered on Page 3
    display_draw_string(3, start_col, text_buf);
}

void user_svc1_ctrl_wr_ind_handler(ke_msg_id_t const msgid,
                                   struct custs1_val_write_ind const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    (void)msgid; (void)dest_id; (void)src_id;
    handle_display_write(param->value, param->length);
}

void user_svc1_led_wr_ind_handler(ke_msg_id_t const msgid,
                                  struct custs1_val_write_ind const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    (void)msgid; (void)dest_id; (void)src_id;
    handle_display_write(param->value, param->length);
}

void user_svc1_long_val_wr_ind_handler(ke_msg_id_t const msgid,
                                       struct custs1_val_write_ind const *param,
                                       const ke_task_id_t dest_id,
                                       const ke_task_id_t src_id)
{
    (void)msgid; (void)dest_id; (void)src_id;
    handle_display_write(param->value, param->length);
}

/**
 * @brief Responds to GATT size verification requests for Long Value writes.
 */
void user_svc1_long_val_att_info_req_handler(ke_msg_id_t const msgid,
                                             const struct custs1_att_info_req *param,
                                             const ke_task_id_t dest_id,
                                             const ke_task_id_t src_id)
{
    struct custs1_att_info_rsp *rsp = KE_MSG_ALLOC(CUSTS1_ATT_INFO_RSP,
                                                   src_id,
                                                   dest_id,
                                                   custs1_att_info_rsp);

    rsp->conidx  = app_env[param->conidx].conidx;
    rsp->att_idx = param->att_idx;
    rsp->length  = DEF_SVC1_LONG_VALUE_CHAR_LEN;
    rsp->status  = ATT_ERR_NO_ERROR;

    KE_MSG_SEND(rsp);
}

/**
 * @brief Responds to GATT size verification requests for Control Point & LED State writes.
 */
void user_svc1_rest_att_info_req_handler(ke_msg_id_t const msgid,
                                         const struct custs1_att_info_req *param,
                                         const ke_task_id_t dest_id,
                                         const ke_task_id_t src_id)
{
    struct custs1_att_info_rsp *rsp = KE_MSG_ALLOC(CUSTS1_ATT_INFO_RSP,
                                                   src_id,
                                                   dest_id,
                                                   custs1_att_info_rsp);

    rsp->conidx  = app_env[param->conidx].conidx;
    rsp->att_idx = param->att_idx;
    rsp->status  = ATT_ERR_NO_ERROR;

    switch (param->att_idx)
    {
        case SVC1_IDX_CONTROL_POINT_VAL:
            rsp->length = DEF_SVC1_CTRL_POINT_CHAR_LEN;
            break;

        case SVC1_IDX_LED_STATE_VAL:
            rsp->length = DEF_SVC1_LED_STATE_CHAR_LEN;
            break;

        case SVC1_IDX_LONG_VALUE_VAL:
            rsp->length = DEF_SVC1_LONG_VALUE_CHAR_LEN;
            break;

        default:
            rsp->length = 0;
            break;
    }

    KE_MSG_SEND(rsp);
}

void user_svc1_adc_val_1_cfg_ind_handler(ke_msg_id_t const msgid, struct custs1_val_write_ind const *param, const ke_task_id_t dest_id, const ke_task_id_t src_id) { (void)msgid; (void)dest_id; (void)src_id; (void)param; }
void user_svc1_long_val_cfg_ind_handler(ke_msg_id_t const msgid, struct custs1_val_write_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id) { (void)msgid; (void)dest_id; (void)src_id; (void)param; }
void user_svc1_long_val_ntf_cfm_handler(ke_msg_id_t const msgid, struct custs1_val_write_ind const *param, const ke_task_id_t dest_id, const ke_task_id_t src_id) { (void)msgid; (void)param; (void)dest_id; (void)src_id; }
void user_svc1_adc_val_1_ntf_cfm_handler(ke_msg_id_t const msgid, struct custs1_val_write_ind const *param, const ke_task_id_t dest_id, const ke_task_id_t src_id) { (void)msgid; (void)param; (void)dest_id; (void)src_id; }
void user_svc1_button_cfg_ind_handler(ke_msg_id_t const msgid, struct custs1_val_write_ind const *param, const ke_task_id_t dest_id, const ke_task_id_t src_id) { (void)msgid; (void)param; (void)dest_id; (void)src_id; }
void user_svc1_button_ntf_cfm_handler(ke_msg_id_t const msgid, struct custs1_val_write_ind const *param, const ke_task_id_t dest_id, const ke_task_id_t src_id) { (void)msgid; (void)param; (void)dest_id; (void)src_id; }
void user_svc1_indicateable_cfg_ind_handler(ke_msg_id_t const msgid, struct custs1_val_write_ind const *param, const ke_task_id_t dest_id, const ke_task_id_t src_id) { (void)msgid; (void)param; (void)dest_id; (void)src_id; }
void user_svc1_indicateable_ind_cfm_handler(ke_msg_id_t const msgid, struct custs1_val_write_ind const *param, const ke_task_id_t dest_id, const ke_task_id_t src_id) { (void)msgid; (void)param; (void)dest_id; (void)src_id; }
void user_svc3_read_non_db_val_handler(ke_msg_id_t const msgid, const struct custs1_value_req_ind *param, const ke_task_id_t dest_id, const ke_task_id_t src_id) { (void)msgid; (void)param; (void)dest_id; (void)src_id; }
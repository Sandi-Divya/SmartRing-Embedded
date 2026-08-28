/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.c
 *
 * @brief All in one project Custom1 Server implementation source code.
 *
 * Copyright (C) 2015-2025 Renesas Electronics Corporation and/or its affiliates.
 * All rights reserved. Confidential Information.
 *
 * This software ("Software") is supplied by Renesas Electronics Corporation and/or its
 * affiliates ("Renesas"). Renesas grants you a personal, non-exclusive, non-transferable,
 * revocable, non-sub-licensable right and license to use the Software, solely if used in
 * or together with Renesas products. You may make copies of this Software, provided this
 * copyright notice and disclaimer ("Notice") is included in all such copies. Renesas
 * reserves the right to change or discontinue the Software at any time without notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS". RENESAS DISCLAIMS ALL WARRANTIES OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. TO THE
 * MAXIMUM EXTENT PERMITTED UNDER LAW, IN NO EVENT SHALL RENESAS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE, EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES. USE OF THIS SOFTWARE MAY BE SUBJECT TO TERMS AND CONDITIONS CONTAINED IN
 * AN ADDITIONAL AGREEMENT BETWEEN YOU AND RENESAS. IN CASE OF CONFLICT BETWEEN THE TERMS
 * OF THIS NOTICE AND ANY SUCH ADDITIONAL LICENSE AGREEMENT, THE TERMS OF THE AGREEMENT
 * SHALL TAKE PRECEDENCE. BY CONTINUING TO USE THIS SOFTWARE, YOU AGREE TO THE TERMS OF
 * THIS NOTICE.IF YOU DO NOT AGREE TO THESE TERMS, YOU ARE NOT PERMITTED TO USE THIS
 * SOFTWARE.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "adc.h"
#include "app.h"
#include "app_api.h"
#include "gpio.h"
#include "timer0_2.h"
#include "timer0.h"
#include "user_all_in_one.h"
#include "user_custs1_def.h"
#include "user_custs1_impl.h"
#include "user_periph_setup.h"
#include "wkupct_quadec.h"
#include "battery.h"

struct app_proj_env_tag user_app_env __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY


/*
 * DEFINES
 ****************************************************************************************
 */
#define APP_PWM_ON      1000
#define APP_PWM_HIGH    500
#define APP_PWM_LOW     200

#define APP_NOTES_NUM   26

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

ke_msg_id_t timer_used      __SECTION_ZERO("retention_mem_area0"); // @RETENTION MEMORY
uint16_t indication_counter __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
const uint16_t notes[APP_NOTES_NUM] =
{
    1046,987,767,932,328,880,830,609,783,991,739,989,698,456,659,255,622,254,587,554,365,523,251,493,466,440
};

/*
 * LOCAL FUNCTIONS DECLARATION
 ****************************************************************************************
*/

static void user_app_pwm_callback_function(void);
static void user_app_enable_pwm(void);
static void user_app_disable_pwm(void);
static void user_app_set_button_event(uint8_t);
static void user_app_disable_button(void);
static void user_app_disable_button(void);
static void user_app_button_press_cb(void);
static void user_app_get_adcval2_val(void);

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void user_svc1_ctrl_wr_ind_handler(ke_msg_id_t const msgid,
                                      struct custs1_val_write_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    uint8_t val = 0;
    memcpy(&val, &param->value[0], param->length);

    if (val == CUSTS1_CP_CMD_ADC_VAL1_ENABLE)
    {
        timer_used = app_easy_timer(APP_PERIPHERAL_CTRL_TIMER_DELAY, user_app_adcval1_timer_cb_handler);
    }
    else if (val == CUSTS1_CP_CMD_ADC_VAL1_DISABLE)
    {
        if (timer_used != EASY_TIMER_INVALID_TIMER)
        {
            app_easy_timer_cancel(timer_used);
            timer_used = EASY_TIMER_INVALID_TIMER;
        }
    }
    else if (val == CUSTS1_CP_CMD_PWM_ENABLE)
    {
        user_app_enable_pwm();
    }
    else if (val == CUSTS1_CP_CMD_PWM_DISABLE)
    {
        user_app_disable_pwm();
    }
    else if (val == CUSTS1_CP_CMD_ADC_VAL_2_ENABLE)
    {
        user_app_env.custs1_adcval2_enabled = 1;
        user_app_get_adcval2_val();
    }
    else if (val == CUSTS1_CP_CMD_ADC_VAL_2_DISABLE)
    {
        user_app_env.custs1_adcval2_enabled = 0;
    }
}

void user_svc1_led_wr_ind_handler(ke_msg_id_t const msgid,
                                     struct custs1_val_write_ind const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    uint8_t val = 0;
    memcpy(&val, &param->value[0], param->length);

    if (val == CUSTS1_LED_ON)
    {
        GPIO_SetActive(GPIO_LED_PORT, GPIO_LED_PIN);
        arch_force_active_mode();
    }
    else if (val == CUSTS1_LED_OFF)
    {
        GPIO_SetInactive(GPIO_LED_PORT, GPIO_LED_PIN);
        arch_restore_sleep_mode();
    }
}

void user_svc1_long_val_cfg_ind_handler(ke_msg_id_t const msgid,
                                           struct custs1_val_write_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    // Generate indication when the central subscribes to it
    if (param->value[0])
    {
        uint8_t conidx = KE_IDX_GET(src_id);
        
        struct custs1_val_ind_req* req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_IND_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ind_req,
                                                          sizeof(indication_counter));

        req->conidx = app_env[conidx].conidx;
        req->handle = SVC1_IDX_INDICATEABLE_VAL;
        req->length = sizeof(indication_counter);
        req->value[0] = (indication_counter >> 8) & 0xFF;
        req->value[1] = indication_counter & 0xFF;

        indication_counter++;

        KE_MSG_SEND(req);
    }
}

void user_svc1_long_val_wr_ind_handler(ke_msg_id_t const msgid,
                                          struct custs1_val_write_ind const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
}

void user_svc1_long_val_ntf_cfm_handler(ke_msg_id_t const msgid,
                                           struct custs1_val_write_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
}

void user_svc1_adc_val_1_cfg_ind_handler(ke_msg_id_t const msgid,
                                            struct custs1_val_write_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
}

void user_svc1_adc_val_1_ntf_cfm_handler(ke_msg_id_t const msgid,
                                            struct custs1_val_write_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
}

void user_svc1_button_cfg_ind_handler(ke_msg_id_t const msgid,
                                         struct custs1_val_write_ind const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
}

void user_svc1_button_ntf_cfm_handler(ke_msg_id_t const msgid,
                                         struct custs1_val_write_ind const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
}

void user_svc1_indicateable_cfg_ind_handler(ke_msg_id_t const msgid,
                                               struct custs1_val_write_ind const *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id)
{
}

void user_svc1_indicateable_ind_cfm_handler(ke_msg_id_t const msgid,
                                               struct custs1_val_write_ind const *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id)
{
}

void user_svc1_long_val_att_info_req_handler(ke_msg_id_t const msgid,
                                                struct custs1_att_info_req const *param,
                                                ke_task_id_t const dest_id,
                                                ke_task_id_t const src_id)
{
    struct custs1_att_info_rsp *rsp = KE_MSG_ALLOC(CUSTS1_ATT_INFO_RSP,
                                                   src_id,
                                                   dest_id,
                                                   custs1_att_info_rsp);
    // Provide the connection index.
    rsp->conidx  = app_env[param->conidx].conidx;
    // Provide the attribute index.
    rsp->att_idx = param->att_idx;
    // Provide the current length of the attribute.
    rsp->length  = 0;
    // Provide the ATT error code.
    rsp->status  = ATT_ERR_NO_ERROR;

    KE_MSG_SEND(rsp);
}

void user_svc1_rest_att_info_req_handler(ke_msg_id_t const msgid,
                                            struct custs1_att_info_req const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    struct custs1_att_info_rsp *rsp = KE_MSG_ALLOC(CUSTS1_ATT_INFO_RSP,
                                                   src_id,
                                                   dest_id,
                                                   custs1_att_info_rsp);
    // Provide the connection index.
    rsp->conidx  = app_env[param->conidx].conidx;
    // Provide the attribute index.
    rsp->att_idx = param->att_idx;
    // Force current length to zero.
    rsp->length  = 0;
    // Provide the ATT error code.
    rsp->status  = ATT_ERR_WRITE_NOT_PERMITTED;

    KE_MSG_SEND(rsp);
}

void user_app_adcval1_timer_cb_handler()
{
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_ADC_VAL_1_CHAR_LEN);

    // ADC value to be sampled
    static uint16_t sample      __SECTION_ZERO("retention_mem_area0");
    sample = (sample <= 0xffff) ? (sample + 1) : 0;

    req->conidx = app_env->conidx;
    req->notification = true;
    req->handle = SVC1_IDX_ADC_VAL_1_VAL;
    req->length = DEF_SVC1_ADC_VAL_1_CHAR_LEN;
    memcpy(req->value, &sample, DEF_SVC1_ADC_VAL_1_CHAR_LEN);

    KE_MSG_SEND(req);

    if (ke_state_get(TASK_APP) == APP_CONNECTED)
    {
        // Set it once again until Stop command is received in Control Characteristic
        timer_used = app_easy_timer(APP_PERIPHERAL_CTRL_TIMER_DELAY, user_app_adcval1_timer_cb_handler);
    }
}

/**
 ****************************************************************************************
 * @brief Disable pwm timer
 ****************************************************************************************
*/
static void user_app_disable_pwm(void)
{
    if(user_app_env.custs1_pwm_enabled)
    {
        user_app_env.custs1_pwm_enabled = 0;

        arch_restore_sleep_mode();

        timer0_stop();

        // Disable the Timer0/Timer2 input clock
        timer0_2_clk_disable();
    }
}

/**
 ****************************************************************************************
 * @brief Timer callback function. Update PWM settings
 ****************************************************************************************
*/
static void user_app_pwm_callback_function(void)
{
    static uint8_t change_delay     __SECTION_ZERO("retention_mem_area0");
    static uint8_t notes_idx        __SECTION_ZERO("retention_mem_area0");

    if (change_delay == 10)
    {
        change_delay = 0;
        timer0_set_pwm_on_counter(0xFFFF);
        timer0_set_pwm_high_counter(notes[notes_idx]/3 * 2);
        timer0_set_pwm_low_counter(notes[notes_idx]/3);
        notes_idx = (notes_idx + 1) % (APP_NOTES_NUM -1);
    }
    change_delay++;
}

/**
 ****************************************************************************************
 * @brief Enable pwm timer
 ****************************************************************************************
*/
static void user_app_enable_pwm(void)
{
    if(!user_app_env.custs1_pwm_enabled)
    {
        tim0_2_clk_div_config_t clk_div_config =
        {
            .clk_div = TIM0_2_CLK_DIV_8
        };

        user_app_env.custs1_pwm_enabled = 1;

        // Disable sleep mode
        arch_force_active_mode();

        // Enable the Timer0/Timer2 input clock
        timer0_2_clk_enable();
        // Set the Timer0/Timer2 input clock division factor to 8, so 16 MHz / 8 = 2 MHz input clock
        timer0_2_clk_div_set(&clk_div_config);
        // Init timer 0
        timer0_init(TIM0_CLK_FAST, PWM_MODE_ONE, TIM0_CLK_NO_DIV);
        // Set timing parameters
        timer0_set(APP_PWM_ON, APP_PWM_HIGH, APP_PWM_LOW);
        // Register IRQ callback
        timer0_register_callback(user_app_pwm_callback_function);
        // Enable IRQ
        timer0_enable_irq();
        // Start timer
        timer0_start();
    }
}

/**
 ****************************************************************************************
 * @brief Set button event configuration
 ****************************************************************************************
*/
static void user_app_set_button_event(uint8_t next_event)
{

    wkupct_register_callback(user_app_button_press_cb);

    wkupct_enable_irq(WKUPCT_PIN_SELECT(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN),
                      WKUPCT_PIN_POLARITY(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, next_event), // polarity
                      1, // 1 event
                      10); // debouncing time = 10 ms

}

/**
 ****************************************************************************************
 * @brief Disable button action.
 ****************************************************************************************
*/
static void user_app_disable_button(void)
{
    wkupct_disable_irq();
}

/**
 ****************************************************************************************
 * @brief Callback function for button action. Update button state characteristic.
 ****************************************************************************************
*/
static void user_app_button_press_cb(void)
{
    uint8_t next_btn_event = 0;

    // Read button state
    if(GPIO_GetPinStatus( GPIO_BUTTON_PORT, GPIO_BUTTON_PIN ))
    {
        user_app_env.custs1_btn_state = CUSTS1_BTN_STATE_RELEASED;
        next_btn_event = WKUPCT_PIN_POLARITY_LOW;
    }
    else
    {
        user_app_env.custs1_btn_state = CUSTS1_BTN_STATE_PRESSED;
        next_btn_event = WKUPCT_PIN_POLARITY_HIGH;
    }

    // Update button characteristic
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_BUTTON_STATE_CHAR_LEN);

    req->conidx = app_env->conidx;
    req->notification = true;
    req->handle = SVC1_IDX_BUTTON_STATE_VAL;
    req->length = DEF_SVC1_BUTTON_STATE_CHAR_LEN;
    req->value[0] = user_app_env.custs1_btn_state;

    KE_MSG_SEND(req);
    // Configure next button event
    user_app_set_button_event(next_btn_event);
}

/**
 ****************************************************************************************
 * @brief Read ADC val and update ADCVAL2 characteristic
 ****************************************************************************************
*/
static void user_app_get_adcval2_val(void)
{
    if(user_app_env.custs1_adcval2_enabled)
    {
        uint16_t adc_sample;

        adc_offset_calibrate(ADC_INPUT_MODE_SINGLE_ENDED);
#if defined (__DA14531__)
        adc_sample = (uint16_t) battery_get_voltage(BATT_CR2032);
#else
        adc_sample = (uint16_t) adc_get_vbat_sample(false);
#endif

        struct custs1_val_set_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_SET_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_set_req,
                                                          DEF_SVC1_ADC_VAL_2_CHAR_LEN);

       req->conidx = app_env->conidx;
       req->handle = SVC1_IDX_ADC_VAL_2_VAL;
       req->length = DEF_SVC1_ADC_VAL_2_CHAR_LEN;
       memcpy(req->value, &adc_sample, DEF_SVC1_ADC_VAL_1_CHAR_LEN);

       KE_MSG_SEND(req);
    }
}

/**
 ****************************************************************************************
 * @brief Enable peripherals used by application.
 ****************************************************************************************
*/
void user_app_enable_periphs(void)
{
    // Update button state characteristic
    user_app_button_press_cb();
    // Get initial ADC value if enabled
    user_app_get_adcval2_val();
}

/**
 ****************************************************************************************
 * @brief Disable peripherals used by application.
 ****************************************************************************************
*/
void user_app_disable_periphs(void)
{
    user_app_disable_pwm();
    user_app_disable_button();
}


/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.c
 * @brief Custom Profile 1 GATT handlers.
 *
 * Battery:
 *      Service 1
 *      Characteristic: SVC1_IDX_ADC_VAL_1_VAL
 *
 *      Value format:
 *          1 byte
 *          0 - 100 (% battery)
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <string.h>
#include <stdint.h>

#include "rwip_config.h"
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
 * EXTERNAL FUNCTIONS
 ****************************************************************************************
 *
 * These functions are implemented in user_peripheral.c
 *
 */

extern uint8_t read_battery_level_percentage(void);
extern void app_batt_send_telemetry_ntf(uint8_t batt_lvl);

/*
 * EXTERNAL VARIABLES
 ****************************************************************************************
 */

extern uint8_t current_batt_lvl;


/*
 * PRIVATE HELPER FUNCTIONS
 ****************************************************************************************
 */

/**
 * @brief Process a BLE write to the custom profile.
 *
 * The application currently uses the Control Point characteristic
 * as the main command characteristic.
 *
 * Supported battery commands:
 *
 *      'B'     -> show battery
 *      'b'     -> show battery
 *      0x01    -> show battery
 *
 */
static void battery_command(void)
{
    /*
     * Read the real battery level from DA14585 ADC.
     */
    current_batt_lvl = read_battery_level_percentage();

    /*
     * Send the battery value through the battery characteristic.
     *
     * This updates:
     *
     *      SVC1_IDX_ADC_VAL_1_VAL
     *
     * and sends a BLE notification if the phone has enabled
     * notifications for that characteristic.
     */
    app_batt_send_telemetry_ntf(current_batt_lvl);
}


/*
 * SERVICE 1 WRITE HANDLERS
 ****************************************************************************************
 */

/**
 * @brief Handle writes to Service 1 Control Point.
 *
 * This function is intentionally kept simple.
 */
void user_svc1_ctrl_wr_ind_handler(ke_msg_id_t const msgid,
                                   struct custs1_val_write_ind const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t first_byte;

    if (param == NULL)
    {
        return;
    }

    if (param->length == 0)
    {
        return;
    }

    first_byte = param->value[0];

    /*
     * Battery command.
     */
    if ((first_byte == 'B') ||
        (first_byte == 'b') ||
        (first_byte == 0x01))
    {
        battery_command();
        return;
    }

    /*
     * Other commands can be added here later.
     */
}


/**
 * @brief Handle writes to LED State characteristic.
 *
 * 0x01 / '1' -> LED ON
 * 0x00 / '0' -> LED OFF
 */
void user_svc1_led_wr_ind_handler(ke_msg_id_t const msgid,
                                  struct custs1_val_write_ind const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    if (param == NULL)
    {
        return;
    }

    if (param->length == 0)
    {
        return;
    }

    /*
     * LED ON
     */
    if ((param->value[0] == 0x01) ||
        (param->value[0] == '1'))
    {
        GPIO_SetActive(GPIO_LED_PORT, GPIO_LED_PIN);
    }

    /*
     * LED OFF
     */
    else if ((param->value[0] == 0x00) ||
             (param->value[0] == '0'))
    {
        GPIO_SetInactive(GPIO_LED_PORT, GPIO_LED_PIN);
    }
}


/*
 * SERVICE 1 ATTENTION / ATTRIBUTE INFO HANDLERS
 ****************************************************************************************
 */

/**
 * @brief Attribute information handler for the Long Value characteristic.
 *
 * This is required by the CUSTS1 framework when accessing the long-value
 * characteristic.
 */
void user_svc1_long_val_att_info_req_handler(ke_msg_id_t const msgid,
                                              struct custs1_att_info_req const *param,
                                              ke_task_id_t const dest_id,
                                              ke_task_id_t const src_id)
{
    struct custs1_att_info_rsp *rsp;

    rsp = KE_MSG_ALLOC(CUSTS1_ATT_INFO_RSP,
                       src_id,
                       dest_id,
                       custs1_att_info_rsp);

    rsp->conidx  = param->conidx;
    rsp->att_idx = param->att_idx;

    /*
     * Allow the configured long-value length.
     */
    rsp->length = DEF_SVC1_LONG_VALUE_CHAR_LEN;

    /*
     * No error.
     */
    rsp->status = ATT_ERR_NO_ERROR;

    KE_MSG_SEND(rsp);
}


/**
 * @brief Generic attribute information handler.
 */
void user_svc1_rest_att_info_req_handler(ke_msg_id_t const msgid,
                                          struct custs1_att_info_req const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    struct custs1_att_info_rsp *rsp;

    rsp = KE_MSG_ALLOC(CUSTS1_ATT_INFO_RSP,
                       src_id,
                       dest_id,
                       custs1_att_info_rsp);

    rsp->conidx  = param->conidx;
    rsp->att_idx = param->att_idx;

    /*
     * Use the maximum attribute length from the database.
     */
    rsp->length = 0;

    rsp->status = ATT_ERR_NO_ERROR;

    KE_MSG_SEND(rsp);
}


/*
 * SERVICE 3 READ HANDLER
 ****************************************************************************************
 */

/**
 * @brief Dynamic read handler for Service 3 Read 4.
 */
void user_svc3_read_non_db_val_handler(ke_msg_id_t const msgid,
                                       struct custs1_value_req_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    struct custs1_value_req_rsp *rsp;

    rsp = KE_MSG_ALLOC_DYN(CUSTS1_VALUE_REQ_RSP,
                           src_id,
                           dest_id,
                           custs1_value_req_rsp,
                           1);

    rsp->conidx  = param->conidx;
    rsp->att_idx = param->att_idx;

    /*
     * No dynamic value currently provided.
     */
    rsp->length = 0;
    rsp->status = ATT_ERR_NO_ERROR;

    KE_MSG_SEND(rsp);
}


/*
 * OPTIONAL SERVICE HANDLERS
 ****************************************************************************************
 */

/**
 * @brief Handle Service 1 long-value writes.
 *
 * Currently we simply accept the write.
 */
void user_svc1_long_val_wr_ind_handler(ke_msg_id_t const msgid,
                                       struct custs1_val_write_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    if (param == NULL)
    {
        return;
    }

    /*
     * Long-value processing can be added here later.
     */
}


/*
 * SERVICE 2 HANDLERS
 ****************************************************************************************
 */

void user_svc2_write_1_wr_ind_handler(ke_msg_id_t const msgid,
                                      struct custs1_val_write_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    /*
     * Reserved for Service 2 Write 1.
     */
}


void user_svc2_write_2_wr_ind_handler(ke_msg_id_t const msgid,
                                      struct custs1_val_write_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    /*
     * Reserved for Service 2 Write 2.
     */
}


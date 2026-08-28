/**
 ****************************************************************************************
 *
 * @file app_suotar_task.c
 *
 * @brief SUOTA receiver application Message Handlers.
 *
 * Copyright (C) 2013-2025 Renesas Electronics Corporation and/or its affiliates.
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

/**
 ****************************************************************************************
 * @addtogroup APPTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"               // SW configuration

#if (BLE_APP_PRESENT)

#if (BLE_SUOTA_RECEIVER)

#include "app_suotar.h"
#include "app_suotar_task.h"
#include "app_task.h"                  // Application Task API
#include "gpio.h"
#include "app_entry_point.h"
#include "app.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if SUOTAR_SECURE_LOCK_FEATURE
static bool suotar_lock_check(void)
{
    if(suota_state.suota_secure_lock) {
        // Send a SUOTA locked error
        suotar_send_status_update_req((uint8_t) SUOTAR_LOCKED_ERR);
        
        // Return true
        return true;
    }
    
    return false;
}
#endif // SUOTAR_SECURE_LOCK_FEATURE

/**
 ****************************************************************************************
 * @brief Handles patch data indication from SUOTA receiver profile.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int suotar_patch_data_ind_handler(ke_msg_id_t const msgid,
                                         struct suotar_patch_data_ind const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
#if SUOTAR_SECURE_LOCK_FEATURE
    if(suotar_lock_check()) {
        return KE_MSG_CONSUMED;
    }
#endif // SUOTAR_SECURE_LOCK_FEATURE

    if( suota_state.mem_dev < SUOTAR_MEM_INVAL_DEV )
    {
        if (param->char_code)
        {
            if( suota_state.new_patch_len )
            {
                if( SUOTAR_IS_FOR_IMAGE( suota_state.mem_dev ) )
                {
                    //---------------------------- Handle SUOTAR image payload -----------------------
                    if( (suota_state.suota_block_idx + param->len) <= SUOTA_OVERALL_PD_SIZE)
                    {
                        memcpy(&suota_all_pd[suota_state.suota_block_idx], param->pd, param->len );
                        suota_state.suota_block_idx += param->len;

                        if( suota_state.new_patch_len == suota_state.suota_block_idx )
                        {
                            app_suotar_img_hdlr();
                        }

                        if( suota_state.suota_block_idx > suota_state.new_patch_len  )
                        {
                            // Received patch len not equal to PATCH_LEN char value
                            suotar_send_status_update_req((uint8_t) SUOTAR_PATCH_LEN_ERR);
                        }
                    }
                    else
                    {
                        suotar_send_status_update_req((uint8_t) SUOTAR_INT_MEM_ERR);
                    }
                }
                else
                {
                    suotar_send_status_update_req((uint8_t) SUOTAR_INVAL_MEM_TYPE);
                }
            }
            else
            {
                // Inavild PATCH_LEN char value
                suotar_send_status_update_req((uint8_t) SUOTAR_PATCH_LEN_ERR);
            }
        }
    }
    else
    {
        suotar_send_status_update_req((uint8_t) SUOTAR_INVAL_MEM_TYPE);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles memory device indication from SUOTA receiver profile.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int suotar_patch_mem_dev_ind_handler(ke_msg_id_t const msgid,
                                            struct suotar_patch_mem_dev_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    uint32_t mem_info = 0;
#if SUOTAR_SECURE_LOCK_FEATURE
    uint8_t mem_dev_cmd = (uint8_t) ((param->mem_dev & SUOTAR_READ_MEM_DEV_TYPE) >> 24);
    
    // The SUOTAR_REBOOT command should be allowed when SUOTA is locked
    if(mem_dev_cmd != SUOTAR_REBOOT) {
        if(suotar_lock_check()) {
            return KE_MSG_CONSUMED;
        }
    }
#endif // SUOTAR_SECURE_LOCK_FEATURE

    if (param->char_code)
    {
        app_suotar_read_mem(param->mem_dev, &mem_info);

        // write memory info to db
        // Inform SUOTAR task. Allocate the  memory info updadte indication
        suotar_send_mem_info_update_req(mem_info);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles GPIO mapping indication from SUOTA receiver profile.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int suotar_gpio_map_ind_handler(ke_msg_id_t const msgid,
                                       struct suotar_gpio_map_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    uint32_t mem_info;

#if SUOTAR_SECURE_LOCK_FEATURE
    if(suotar_lock_check()) {
        return KE_MSG_CONSUMED;
    }
#endif // SUOTAR_SECURE_LOCK_FEATURE

    if (param->char_code)
    {
         suota_state.gpio_map = (uint32_t)param->gpio_map;

        // Setting GPIO map is only valid for external devices
        if( (suota_state.mem_dev == SUOTAR_MEM_I2C_EEPROM) ||
            (suota_state.mem_dev == SUOTAR_MEM_SPI_FLASH)     )
        {
            mem_info = 0;
            suotar_send_mem_info_update_req(mem_info);
        }
        else if( (suota_state.mem_dev == SUOTAR_IMG_I2C_EEPROM)  ||
                 (suota_state.mem_dev == SUOTAR_IMG_SPI_FLASH )     )
        {
            // return ACTIVE_IMAGE info

        }
        else
        {
            suotar_send_status_update_req((uint8_t) SUOTAR_INVAL_MEM_TYPE);
        }
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles Patch Length indication from SUOTA receiver profile.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int suotar_patch_len_ind_handler(ke_msg_id_t const msgid,
                                        struct suotar_patch_len_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
#if SUOTAR_SECURE_LOCK_FEATURE
    if(suotar_lock_check()) {
        return KE_MSG_CONSUMED;
    }
#endif // SUOTAR_SECURE_LOCK_FEATURE

    if (param->char_code)
    {   // Patchs are send in words (usually 4 bytes). So overall length in bytes should be multiple of 4.
        if( param->len == 0 )
        {
            // Inavid PATCH_LEN char value
            suotar_send_status_update_req((uint8_t) SUOTAR_PATCH_LEN_ERR);
        }
        else
        {
            // Check if the memory device has been set correctly
            if( SUOTAR_IS_VALID_MEM_DEV (suota_state.mem_dev) )
            {
                if( SUOTAR_IS_FOR_IMAGE( suota_state.mem_dev ) )
                {
                    // SUOTAR is configured to recive image, check SYSRAM buffer size to determine if the block can be stored
                    if( param->len > SUOTA_OVERALL_PD_SIZE )
                    {
                        // Not enough memory for new patch. Update status.
                        suotar_send_status_update_req((uint8_t) SUOTAR_INT_MEM_ERR);
                    }
                    else
                    {
                        suota_state.new_patch_len = param->len;
                    }
                }
                else
                {
                    suotar_send_status_update_req((uint8_t) SUOTAR_INVAL_MEM_TYPE);
                }
            }
        }
    }

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

static const struct ke_msg_handler app_suotar_process_handlers[]=
{
    {SUOTAR_PATCH_MEM_DEV_IND,              (ke_msg_func_t)suotar_patch_mem_dev_ind_handler},
    {SUOTAR_GPIO_MAP_IND,                   (ke_msg_func_t)suotar_gpio_map_ind_handler},
    {SUOTAR_PATCH_LEN_IND,                  (ke_msg_func_t)suotar_patch_len_ind_handler},
    {SUOTAR_PATCH_DATA_IND,                 (ke_msg_func_t)suotar_patch_data_ind_handler},
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_suotar_process_handler(ke_msg_id_t const msgid,
                                                       void const *param,
                                                       ke_task_id_t const dest_id,
                                                       ke_task_id_t const src_id,
                                                       enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, app_suotar_process_handlers,
                                         sizeof(app_suotar_process_handlers) / sizeof(struct ke_msg_handler));
}

#endif //BLE_SUOTA_RECEIVER

#endif //(BLE_APP_PRESENT)

/// @} APPTASK

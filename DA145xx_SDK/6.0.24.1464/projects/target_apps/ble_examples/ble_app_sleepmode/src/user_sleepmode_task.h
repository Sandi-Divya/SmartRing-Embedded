/**
 ****************************************************************************************
 *
 * @file user_sleepmode_task.h
 *
 * @brief Sleep mode project header file.
 *
 * Copyright (C) 2012-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _USER_SLEEPMODE_TASK_H_
#define _USER_SLEEPMODE_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include "custs1_task.h"
#include "ke_msg.h"
#include <stdint.h>

/*
 * DEFINES
 ****************************************************************************************
 */

enum
{
    CUSTS1_BTN_STATE_RELEASED = 0,
    CUSTS1_BTN_STATE_PRESSED,
};

enum
{
    CUSTS1_CP_CMD_PWM_DISABLE = 0,
    CUSTS1_CP_CMD_PWM_ENABLE,
    CUSTS1_CP_CMD_ADC_VAL_2_DISABLE,
    CUSTS1_CP_CMD_ADC_VAL_2_ENABLE,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct app_proj_env_tag
{
    uint8_t custs1_pwm_enabled;
    uint8_t custs1_adcval2_enabled;
    uint8_t custs1_btn_state;
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Control point write indication handler.
 * @param[in] msgid    Id of the message received.
 * @param[in] param    Pointer to the parameters of the message.
 * @param[in] dest_id  ID of the receiving task instance.
 * @param[in] src_id   ID of the sending task instance.
 ****************************************************************************************
 */
void user_svc1_ctrl_wr_ind_handler(ke_msg_id_t const msgid,
                                   struct custs1_val_write_ind const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Enable peripherals used by application.
 ****************************************************************************************
 */
void user_app_enable_periphs(void);


/**
 ****************************************************************************************
 * @brief Disable peripherals used by application.
 ****************************************************************************************
 */
void user_app_disable_periphs(void);

/// @} APP

#endif // _USER_SLEEPMODE_TASK_H_


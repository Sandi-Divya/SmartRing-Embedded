/**
 ****************************************************************************************
 *
 * @file mainloop_callbacks.h
 *
 * @brief Implementation of mainloop callbacks used by the production test application. 
 *
 * Copyright (C) 2017-2025 Renesas Electronics Corporation and/or its affiliates.
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

#include "arch.h"
#include "llm.h"
#include "gpio.h"
#include "uart.h"
#include "arch_api.h"
#include "dialog_prod.h"
#include "dialog_commands.h"
#include "hci.h"
#include "system_library.h"
#include "mainloop_callbacks.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

arch_main_loop_callback_ret_t app_on_ble_powered(void)
{
    arch_main_loop_callback_ret_t ret = GOTO_SLEEP;
    static int cnt2sleep=100;

    do {
        if(test_state == STATE_START_TX)
        {
            if(llm_le_env.elt == NULL)
            {
                test_state=STATE_IDLE;
                set_state_start_tx();
            }
        }

        if((test_tx_packet_nr >= text_tx_nr_of_packets) &&  (test_state == STATE_START_TX) )
        {
            test_tx_packet_nr=0;
            send_tx_cmd_cmp_event();
            set_state_stop();
            //btw.state is STATE_IDLE after set_state_stop()
        }

        rwip_schedule();
    } while(0);

    if(arch_get_sleep_mode() == 0)
    {
        ret = KEEP_POWERED;
        cnt2sleep = 100;
    }
    else
    {
        if(cnt2sleep-- > 0)
        {
            ret = KEEP_POWERED;
        }
    }

    return ret;
}


arch_main_loop_callback_ret_t app_on_full_power(void)
{
    return GOTO_SLEEP;
}


void app_going_to_sleep(sleep_mode_t sleep_mode)
{
}


void app_resume_from_sleep(void)
{
    arch_disable_sleep();
}

void send_tx_cmd_cmp_event(void)
{
    void *event = ke_msg_alloc(HCI_CMD_CMP_EVENT , 0, HCI_TX_TEST_CMD_OPCODE, 0);
    hci_send_2_host(event);
}

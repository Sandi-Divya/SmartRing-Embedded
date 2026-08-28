/**
 ****************************************************************************************
 *
 * @file hci_vs.c
 *
 * @brief HCI vendor specific command handlers project source code.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include "hci_vs.h"
#include "hci_int.h"
#include "hci.h"
#include "co_utils.h"
#include "dialog_commands.h"
#include "user_commands.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define HCI_DIALOG_CMD_NUM (HCI_VS_LAST_DIALOG_CMD_OPCODE - HCI_VS_FIRST_DIALOG_CMD_OPCODE)
#define HCI_USER_CMD_NUM   (HCI_VS_LAST_USER_CMD_OPCODE - HCI_VS_FIRST_USER_CMD_OPCODE)
#define TOTAL_HCI_COMMANDS (HCI_RW_CMD_NUM + HCI_DIALOG_CMD_NUM + HCI_USER_CMD_NUM)

/*
 * GLOBAL VARIABLES DECLARATION
 ****************************************************************************************
 */

// ROM variable
extern const struct hci_cmd_desc_tab_ref rom_hci_cmd_desc_root_tab[];
//ROM init data for hci_cmd_desc_root_tab
extern struct hci_cmd_desc_tab_ref hci_cmd_desc_root_tab[];
// RW HCI commands
extern const struct hci_cmd_desc_tag hci_cmd_desc_tab_vs[HCI_RW_CMD_NUM];

/*
 * LOCAL VARIABLES DECLARATION
 ****************************************************************************************
 */

struct hci_cmd_desc_tag hci_ram_cmd_desc_tab_vs[TOTAL_HCI_COMMANDS] __SECTION_ZERO("retention_mem_area0");

/**
 ****************************************************************************************
 * @brief Handles the reception of the vs hci commands.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
*/
int hci_vs_handle_rest_commands(ke_task_id_t const dest_id, 
                                void const *param, 
                                ke_task_id_t const src_id)
{
    int return_status = KE_MSG_CONSUMED;
    
    for(int i = 0; i < user_commands_handler_num; i++)
    {
        // Check if opcode matches
        if(user_commands_handler_tab[i].id == src_id)
        {
            // Check if there is a handler function
            if(user_commands_handler_tab[i].func != NULL)
            {
                // Call handler for user command
                return_status = user_commands_handler_tab[i].func(src_id, param, dest_id, src_id);
                return return_status;
            }
            break;
        }
    }

    for(int i = 0; i < dialog_commands_handler_num; i++)
    {
        // Check if opcode matches
        if(dialog_commands_handler_tab[i].id == src_id)
        {
            // Check if there is a handler function
            if(dialog_commands_handler_tab[i].func != NULL)
            {
                // Call handler for Dialog command
                return_status = dialog_commands_handler_tab[i].func(src_id, param, dest_id, src_id);
                return return_status;
            }
            break;
        }
    }

    return return_status;
}

void hci_vs_initialize_commands_tab(void)
{
    //copy all hci vs rom commands to hci_ram_cmd_desc_tab_vs table
    memcpy(hci_ram_cmd_desc_tab_vs, hci_cmd_desc_tab_vs, sizeof(hci_cmd_desc_tab_vs));    

    //copy all hci vs user commands to hci_ram_cmd_desc_tab_vs table
    for (int i = 0; i < user_commands_num; i++)
    {
        hci_ram_cmd_desc_tab_vs[ARRAY_LEN(hci_cmd_desc_tab_vs) + i] = hci_cmd_desc_tab_user_vs[i];    
    }

    //copy all hci vs dialog commands to hci_ram_cmd_desc_tab_vs table
    for (int i = 0; i < dialog_commands_num; i++)
    {      
        hci_ram_cmd_desc_tab_vs[ARRAY_LEN(hci_cmd_desc_tab_vs) + user_commands_num + i] = hci_cmd_desc_tab_dialog_vs[i];
    }

    //change pointer points to hci_ram_cmd_desc_tab_vs table
    hci_cmd_desc_root_tab[5].cmd_desc_tab = hci_ram_cmd_desc_tab_vs;                      
    hci_cmd_desc_root_tab[5].nb_cmds      = ARRAY_LEN(hci_ram_cmd_desc_tab_vs);   
}

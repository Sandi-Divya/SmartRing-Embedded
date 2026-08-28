/**
 ****************************************************************************************
 *
 * @file dialog_commands.h
 *
 * @brief Dialog_commands header file.
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

#ifndef DIALOG_COMMANDS_H_
#define DIALOG_COMMANDS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "hci_int.h"
#include "ke_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define HCI_VS_FIRST_DIALOG_CMD_OPCODE    0xFE00

//Dialog HCI commands range from 0xFE00 to 0XFEFF
enum
{
    HCI_VS1_DIALOG_CMD_OPCODE = HCI_VS_FIRST_DIALOG_CMD_OPCODE,
    HCI_VS_LAST_DIALOG_CMD_OPCODE           // DO NOT MOVE. Must always be last and opcodes linear (00,01,02 ...)
};

//HCI Debug dialog vs1 command parameters - vendor specific
struct hci_vs1_dialog_cmd
{
    //VS specific data
    uint8_t    inputData;
};

//HCI Debug dialog vs1 complete event parameters - vendor specific
struct hci_vs1_dialog_cmd_cmp_evt
{
    //Status of the command reception
    uint8_t status;
    //type
    uint8_t returnData;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

// Number of Dialog handlers
extern const uint8_t dialog_commands_handler_num;
// Number of Dialog commands
extern const uint8_t dialog_commands_num;
// Dialog HCI commands
extern const struct hci_cmd_desc_tag hci_cmd_desc_tab_dialog_vs[];
// Dialog HCI commands handlers
extern const struct ke_msg_handler dialog_commands_handler_tab[];

#endif // DIALOG_COMMANDS_H_

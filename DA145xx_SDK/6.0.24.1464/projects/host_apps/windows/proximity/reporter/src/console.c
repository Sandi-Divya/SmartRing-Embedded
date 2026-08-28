/**
 ****************************************************************************************
 *
 * @file console.c
 *
 * @brief Basic console user interface of the host application.
 *
 * Copyright (C) 2012-2023 Renesas Electronics Corporation and/or its affiliates.
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

#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include "queue.h"
#include "console.h"
#include "app.h"
#include "proxr.h"

void ConsoleSendScan(void)
{
    console_msg *pmsg = (console_msg *) malloc(sizeof(console_msg));
    assert(pmsg);

    pmsg->type = CONSOLE_DEV_DISC_CMD;
    
    WaitForSingleObject(ConsoleQueueSem, INFINITE);
    EnQueue(&ConsoleQueue, pmsg);
    ReleaseMutex(ConsoleQueueSem);
}

void ConsoleSendConnnect(int indx)
{
    console_msg *pmsg = (console_msg *) malloc(sizeof(console_msg));
    assert(pmsg);

    pmsg->type = CONSOLE_CONNECT_CMD;
    pmsg->val = indx;

    WaitForSingleObject(ConsoleQueueSem, INFINITE);
    EnQueue(&ConsoleQueue, pmsg);
    ReleaseMutex(ConsoleQueueSem);
}

void ConsoleSendDisconnnect(void)
{
    console_msg *pmsg = (console_msg *) malloc(sizeof(console_msg));
    assert(pmsg);

    pmsg->type = CONSOLE_DISCONNECT_CMD;
    
    WaitForSingleObject(ConsoleQueueSem, INFINITE);
    EnQueue(&ConsoleQueue, pmsg);
    ReleaseMutex(ConsoleQueueSem);
}

void ConsoleRead(unsigned char type)
{
    console_msg *pmsg = (console_msg *) malloc(sizeof(console_msg));
    assert(pmsg);

    pmsg->type = type;

    WaitForSingleObject(ConsoleQueueSem, INFINITE);
    EnQueue(&ConsoleQueue, pmsg);
    ReleaseMutex(ConsoleQueueSem);
}

void ConsoleWrite(unsigned char type, unsigned char val)
{
    console_msg *pmsg = (console_msg *) malloc(sizeof(console_msg));
    assert(pmsg);

    pmsg->type = type;
    pmsg->val = val;

    WaitForSingleObject(ConsoleQueueSem, INFINITE);
    EnQueue(&ConsoleQueue, pmsg);
    ReleaseMutex(ConsoleQueueSem);
}

void ConsoleSendExit(void)
{
    console_msg *pmsg = (console_msg *) malloc(sizeof(console_msg));
    assert(pmsg);

    pmsg->type = CONSOLE_EXIT_CMD;
    
    WaitForSingleObject(ConsoleQueueSem, INFINITE);
    EnQueue(&ConsoleQueue, pmsg);
    ReleaseMutex(ConsoleQueueSem);
}

void HandleKeyEvent(int Key)
{
    switch(Key)
    {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            ConsoleSendConnnect(Key - 0x31);
            break;
        case 'S':
            ConsoleSendScan();
            break;
        case 'A':
            ConsoleRead(CONSOLE_RD_LLV_CMD);
            break;
        case 'B':
            ConsoleRead(CONSOLE_RD_TXP_CMD);
            break;
        case 'C':
            ConsoleWrite(CONSOLE_WR_IAS_CMD, PROXR_ALERT_HIGH);
            break;
        case 'D':
            ConsoleWrite(CONSOLE_WR_IAS_CMD, PROXR_ALERT_MILD);
            break;
        case 'E':
            ConsoleWrite(CONSOLE_WR_IAS_CMD, PROXR_ALERT_NONE);
            break;
        case 'F':
            ConsoleWrite(CONSOLE_WR_LLV_CMD, PROXR_ALERT_NONE);
            break;
        case 'G':
            ConsoleWrite(CONSOLE_WR_LLV_CMD, PROXR_ALERT_MILD);
            break;
        case 'H':
            ConsoleWrite(CONSOLE_WR_LLV_CMD, PROXR_ALERT_HIGH);
            break;
        case 'I':
            ConsoleSendDisconnnect();
            break;
        case 0x1B:
            ConsoleSendExit();
            break;
        default:
            break;
    }
        
}

void ConsoleTitle(void)
{
    printf ("\t\t####################################################\n");
    printf ("\t\t#    DA14585 Proximity Reporter demo application   #\n");
    printf ("\t\t####################################################\n\n");
}

void ConsoleScan(void)
{
    ConsoleTitle();

    printf ("\t\t\t\tScanning... Please wait \n\n");    

    ConsolePrintScanList();
}

void ConsoleIdle(void)
{
    ConsoleTitle();
}

void ConsolePrintScanList(void)
{
    int index, i;

    printf ("No. \tbd_addr \t\tName \t\tRssi\n");

    for (index=0; index<MAX_SCAN_DEVICES; index++)
    {
        if (app_env.devices[index].free == false)
        {
            printf("%d\t", index+1);
            for (i=0; i<BD_ADDR_LEN-1; i++)
                printf ("%02x:", app_env.devices[index].adv_addr.addr[BD_ADDR_LEN - 1 - i]);
            printf ("%02x", app_env.devices[index].adv_addr.addr[0]);

            printf ("\t%s\t%d\n", app_env.devices[index].data, app_env.devices[index].rssi);
        }
    }

}

void ConsoleConnected(int full)
{
    int i;

    ConsoleTitle();

    printf ("\t\t\t Connected to Device \n\n");


    printf ("BDA: ");
    for (i=0; i<BD_ADDR_LEN-1; i++)
        printf ("%02x:", app_env.proxr_device.device.adv_addr.addr[BD_ADDR_LEN - 1 - i]);
    printf ("%02x\t", app_env.proxr_device.device.adv_addr.addr[0]);

    printf ("Bonded: ");
    if (app_env.proxr_device.bonded)
        printf ("YES\n");
    else
        printf ("NO\n");

    /* For Future Use
    if (app_env.proxr_device.rssi != 0xFF)
            printf ("RSSI: %02d\n\n", app_env.proxr_device.rssi);
        else
            printf ("RSSI: \n\n");
    */

    if (full)
    {


        if (app_env.proxr_device.llv != 0xFF)
            printf ("Link Loss Alert Lvl: %02d\t\t", app_env.proxr_device.llv);
        else
            printf ("Link Loss Alert Lvl: \t\t");

        if (app_env.proxr_device.txp != 0xFF)
            printf ("Tx Power Lvl: %02d\n\n", app_env.proxr_device.txp);
        else
            printf ("Tx Power Lvl:  \n\n");
    
    }
}

VOID ConsoleProc(PVOID unused)
{
    
   int Key = 0xFF;

   while(StopConsoleTask == FALSE)
   {
        Key = _getch();  // Get keypress 
        Key = toupper(Key);
        HandleKeyEvent(Key);
   }

   StopConsoleTask = FALSE;   // To indicate that the task has stopped
   ExitThread(0);
}

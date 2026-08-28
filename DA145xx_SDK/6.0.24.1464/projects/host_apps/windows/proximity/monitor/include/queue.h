/**
 ****************************************************************************************
 *
 * @file queue.h
 *
 * @brief Header file for queues and threads definitions.
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

#ifndef QUEUE_H_
#define QUEUE_H_

#include <conio.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <stddef.h>     // standard definition


#include "stdtypes.h"


// Queue stuff.
struct QueueStorage {
    struct QueueStorage *Next;
    void *Data;
};

typedef struct {
    struct QueueStorage *First,*Last;
} QueueRecord;

typedef struct {
    unsigned char  bLength;
    unsigned char  bData[1];
} QueueElement;


// Used to stop the tasks.
extern BOOL StopConsoleTask, StopRxTask;

extern HANDLE ConsoleQueueSem;    // mutex semaphore to protect console event queue
extern HANDLE UARTRxQueueSem;     // mutex semaphore to protect uart rx queue

extern HANDLE Rx232Id, ConsoleTaskId;  // Thread handles

extern QueueRecord ConsoleQueue, UARTRxQueue; //Queues UARTRx -> Main thread /  Console -> Main thread

/*
 ****************************************************************************************
 * @brief Initialize the UART RX thread and the console key handling thread.
 ****************************************************************************************
*/
void InitTasks(void);

/*
 ****************************************************************************************
 * @brief Adds an item to the queue
 *
 *  @param[in] rec    Queue.
 *  @param[in] vdata  Pointer to the item to be added.
 ****************************************************************************************
*/
void EnQueue(QueueRecord *rec,void *vdata);

/*
 ****************************************************************************************
 * @brief Removes an item from the queue
 *
 *  @param[in] rec  Queue.
 *
 * @return pointer to the item that was removed
 ****************************************************************************************
*/
void *DeQueue(QueueRecord *rec);

#endif //QUEUE_H_

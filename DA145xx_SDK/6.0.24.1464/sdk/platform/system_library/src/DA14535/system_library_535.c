/**
 ****************************************************************************************
 *
 * @file system_library_535.c
 *
 * @brief System library source file.
 *
 * Copyright (C) 2022-2023 Renesas Electronics Corporation and/or its affiliates.
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

#if defined (__DA14535__)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <string.h>
#include "compiler.h"
#include "system_library.h"
#include "datasheet.h"
#include "arch.h"
#include "lld_evt.h"
#include "ke_task.h"
#include "ke_mem.h"
#include "gapc_task.h"

/*
 * DEFINES
 *****************************************************************************************
 */

#if defined (CFG_SKIP_SLAVE_LATENCY)
#define USE_SKIP_SLAVE_LATENCY                          (1)
#else
#define USE_SKIP_SLAVE_LATENCY                          (0)
#endif

#if defined (CFG_USE_HEAP_LOG)
#define USE_HEAP_LOG                                    (1)
#else
#define USE_HEAP_LOG                                    (0)
#endif

#if (USE_HEAP_LOG) || (USE_SKIP_SLAVE_LATENCY)
#define USE_FUNCTION_PATCH                              (1)
#else
#define USE_FUNCTION_PATCH                              (0)
#endif


#if defined (CFG_USE_DATA_PATCH_1)
#define USE_DATA_PATCH_1                                (1)
#define DATA_PATCH_1_MASK                               (0x00100000)
#else
#define USE_DATA_PATCH_1                                (0)
#define DATA_PATCH_1_MASK                               (0)
#endif

#if (CFG_USE_DATA_PATCH_2)
#define USE_DATA_PATCH_2                                (1)
#define DATA_PATCH_2_MASK                               (0x00200000)
#else
#define USE_DATA_PATCH_2                                (0)
#define DATA_PATCH_2_MASK                               (0)
#endif

#if (USE_HEAP_LOG)
    #if (!DEVELOPMENT_DEBUG)
        #error "USE_HEAP_LOG must not be set when building for production (DEVELOPMENT_DEBUG is 0)"
    #else
        #warning "USE_HEAP_LOG is set!"
    #endif
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

void PATCHED_ke_free(void* mem_ptr);
void *PATCHED_ke_malloc(uint32_t size, uint8_t type);
void PATCHED_lld_evt_schedule_next(struct ea_elt_tag *elt);
int PATCHED_gapc_param_update_cmd_handler(ke_msg_id_t const msgid, struct  gapc_param_update_cmd *param,
                                          ke_task_id_t const dest_id, ke_task_id_t const src_id);

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint16_t heap_log                                   __SECTION_ZERO("retention_mem_area0");
uint8_t skip_slave_latency_mask                     __SECTION_ZERO("retention_mem_area0");

// Custom message handlers
const struct custom_msg_handler my_custom_msg_handlers[] =
{
#if (USE_SKIP_SLAVE_LATENCY) && (!BLE_APP_PRESENT)
    { TASK_GAPC, GAPC_PARAM_UPDATE_CMD, (ke_msg_func_t)PATCHED_gapc_param_update_cmd_handler},
#endif
    { 0, 0, NULL}   //it must end with func = NULL so that loop can terminate correctly
};

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

#if (USE_FUNCTION_PATCH)
/*
* Up to 20 patch entries for ROM functions.
* Array that holds the ROM functions to be patched.
*/
static const void * const patch_orig_table[] = {
#if (USE_SKIP_SLAVE_LATENCY)
    (void *) lld_evt_schedule_next,             // original function 1
     0, // reserved
#endif

#if (USE_HEAP_LOG)
    (void *) ke_free,                           // original function 2
    0, // reserved

    (void *) ke_malloc,                         // original function 3
    0, // reserved
#endif

//    (void *) NULL,        // original function 4
//    0, // reserved

//    (void *) NULL,        // original function 5
//    0, // reserved

//    (void *) NULL,        // original function 6
//    0, // reserved

//    (void *) NULL,        // original function 7
//    0, // reserved

//    (void *) NULL,        // original function 8
//    0, // reserved

//    (void *) NULL,        // original function 9
//    0, // reserved

//    (void *) NULL,        // original function 10
//    0, // reserved

//    (void *) NULL,        // original function 11
//    0, // reserved

//    (void *) NULL,        // original function 12
//    0, // reserved

//    (void *) NULL,        // original function 13
//    0, // reserved

//    (void *) NULL,        // original function 14
//    0, // reserved

//    (void *) NULL,        // original function 15
//    0, // reserved

//    (void *) NULL,        // original function 16
//    0, // reserved

//    (void *) NULL,        // original function 17
//    0, // reserved

//    (void *) NULL,        // original function 18
//    0, // reserved

//    (void *) NULL,        // original function 19
//    0, // reserved

//    (void *) NULL,        // original function 20
//    0, // reserved
};

// 20 function patches available
static const void * const patch_new_table[] = {
#if (USE_SKIP_SLAVE_LATENCY)
    (void *) PATCHED_lld_evt_schedule_next,
#endif
#if (USE_HEAP_LOG)
    (void *) PATCHED_ke_free,
    (void *) PATCHED_ke_malloc,
#endif
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
//    (void *) NULL,
};
#endif // (USE_FUNCTION_PATCH)

void patch_func(void)
{
    // Enable heap log
    heap_log = USE_HEAP_LOG;

    uint32_t bitmap = 0x00000000;

#if (USE_FUNCTION_PATCH)
#ifdef __IS_SDK6_COMPILER_GCC__
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
    #pragma GCC diagnostic ignored "-Warray-bounds"
#endif
    memcpy((uint32_t *)PATCH_ADDR0_REG, patch_orig_table, sizeof(patch_orig_table));
    memcpy((uint32_t *)0x07fc00c0, patch_new_table, sizeof(patch_new_table)); // Function patches start at 0x07fc00c0
#ifdef __IS_SDK6_COMPILER_GCC__
#pragma GCC diagnostic pop
#endif
    bitmap = 0xFFFFFFFF >> (32 - ARRAY_LEN(patch_new_table));
#endif

#if (USE_DATA_PATCH_1)
    patch_data1();
#endif

#if (USE_DATA_PATCH_2)
    patch_data2();
#endif

    // Enable HW patches
    SetWord32(PATCH_VALID_REG, bitmap | DATA_PATCH_1_MASK | DATA_PATCH_2_MASK);
}
#endif // DA14535

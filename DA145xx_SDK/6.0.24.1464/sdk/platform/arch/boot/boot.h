/**
 ****************************************************************************************
 *
 * @file boot.h
 *
 * @brief Declarations of the linker-defined symbols for various C compilers.
 *
 * Copyright (C) 2018-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _BOOT_H_
#define _BOOT_H_

#include <stdint.h>

/*
 * ARM C Compiler 4/5 (armcc)
 */
#if defined ( __CC_ARM )

    extern const uint32_t Image$$ER_IROM3$$Base[];
    #define CODE_AREA_BASE      ((uint32_t)Image$$ER_IROM3$$Base)

    extern const uint32_t Image$$ER_IROM3$$Length[];
    #define CODE_AREA_LENGTH    ((uint32_t)Image$$ER_IROM3$$Length)

    extern const uint32_t Image$$RET_DATA_UNINIT$$Base[];
    #define RET_MEM_BASE        ((uint32_t)Image$$RET_DATA_UNINIT$$Base)

    extern const uint32_t heap_mem_area_not_ret$$Base[];
    #define NON_RET_HEAP_BASE   ((uint32_t)heap_mem_area_not_ret$$Base)

    extern const uint32_t heap_mem_area_not_ret$$Length[];
    #define NON_RET_HEAP_LENGTH ((uint32_t)heap_mem_area_not_ret$$Length)

    extern const uint32_t heap_db_area$$Base[];
    #define DB_HEAP_BASE   ((uint32_t)heap_db_area$$Base)

    extern const uint32_t heap_db_area$$Length[];
    #define DB_HEAP_LENGTH ((uint32_t)heap_db_area$$Length)

/*
 * ARM C Compiler above 6.10.1 (armclang)
 */
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6100100)

    extern int Image$$ER_IROM3$$Base;
    #define CODE_AREA_BASE      (uintptr_t)&Image$$ER_IROM3$$Base

    extern int Image$$ER_IROM3$$Length;
    #define CODE_AREA_LENGTH    (uintptr_t)&Image$$ER_IROM3$$Length

    extern int Image$$RET_DATA_UNINIT$$Base;
    #define RET_MEM_BASE        (uintptr_t)&Image$$RET_DATA_UNINIT$$Base

    extern int Image$$ER_NZI$$Base;
    #define NON_RET_HEAP_BASE   (uintptr_t)&Image$$ER_NZI$$Base

    extern int Image$$ER_NZI$$Length;
    #define NON_RET_HEAP_LENGTH (uintptr_t)&Image$$ER_NZI$$Length

    extern int Image$$RET_HEAP_DB$$Base;
    #define DB_HEAP_BASE   (uintptr_t)&Image$$RET_HEAP_DB$$Base

    extern int Image$$RET_HEAP_DB$$Length;
    #define DB_HEAP_LENGTH (uintptr_t)&Image$$RET_HEAP_DB$$Length

/*
 * GNU C Compiler
 */
#elif defined ( __GNUC__ )

    extern const uint32_t __ER_IROM3_BASE__[];
    #define CODE_AREA_BASE      ((uint32_t)__ER_IROM3_BASE__)

    extern const uint32_t __code_area_end__[];
    #define CODE_AREA_END       ((uint32_t)__code_area_end__)
    #define CODE_AREA_LENGTH    (CODE_AREA_END - CODE_AREA_BASE)

    extern const uint32_t __retention_mem_area_uninit_start__[];
    #define RET_MEM_BASE        ((uint32_t)__retention_mem_area_uninit_start__)

    extern const uint32_t __heap_mem_area_not_ret_start__[];
    #define NON_RET_HEAP_BASE   ((uint32_t)__heap_mem_area_not_ret_start__)

    extern const uint32_t __heap_mem_area_not_ret_end__[];
    #define NON_RET_HEAP_END   ((uint32_t)__heap_mem_area_not_ret_end__)
    #define NON_RET_HEAP_LENGTH (NON_RET_HEAP_END - NON_RET_HEAP_BASE)

    extern const uint32_t __db_heap_start__[];
    #define DB_HEAP_BASE       ((uint32_t)__db_heap_start__)

    extern const uint32_t __db_heap_end__[];
    #define DB_HEAP_END        ((uint32_t)__db_heap_end__)
    #define DB_HEAP_LENGTH     (DB_HEAP_END - DB_HEAP_BASE)

/*
 * IAR C Compiler
 */
#elif defined ( __ICCARM__)

    extern const uint32_t __ER_IROM3_BASE__[];
    #define CODE_AREA_BASE      ((uint32_t)__ER_IROM3_BASE__)

    extern const uint32_t __ER_IROM3_LENGTH__[];
    #define CODE_AREA_LENGTH    ((uint32_t)__ER_IROM3_LENGTH__)

    #pragma section = "retention_mem_area_uninit"
    #define RET_MEM_BASE        ((uint32_t)(__section_begin("retention_mem_area_uninit")))

    #pragma section = "heap_mem_area_not_ret"
    #define NON_RET_HEAP_BASE   ((uint32_t)(__section_begin("heap_mem_area_not_ret")))

    #define NON_RET_HEAP_END    ((uint32_t)(__section_end("heap_mem_area_not_ret")))

    #define NON_RET_HEAP_LENGTH (NON_RET_HEAP_END - NON_RET_HEAP_BASE)

    #pragma section = "heap_db_area"
    #define DB_HEAP_BASE        ((uint32_t)(__section_begin("heap_db_area")))

    #define DB_HEAP_END         ((uint32_t)(__section_end("heap_db_area")))

    #define DB_HEAP_LENGTH (DB_HEAP_END - DB_HEAP_BASE)

#endif

#endif // _BOOT_H_

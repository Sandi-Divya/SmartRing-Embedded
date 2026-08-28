/**
 ****************************************************************************************
 *
 * @file compiler.h
 *
 * @brief Definitions of C compiler specific directives.
 *
 * Copyright (C) 2018-2024 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _COMPILER_H_
#define _COMPILER_H_

#include "cmsis_compiler.h"

/*
 * Extra definitions depending on the used compiler.
 */

/*
 * ARM C Compiler 4/5 (armcc)
 */
#if   defined ( __CC_ARM )

    #undef __IS_SDK6_COMPILER_GCC__    // In armclang, __GNUC__ is defined to 1

    /// define the BLE IRQ handler attribute for this compiler
    #define __BLEIRQ

    /// define size of an empty array (used to declare structure with an array size not defined)
    #define __ARRAY_EMPTY

    /// Put a variable in a zero-initialised memory section
    #define __SECTION_ZERO(sec_name) __attribute__((section(sec_name), zero_init))

    /// Put a variable in a given memory section
    #define __SECTION(sec_name) __attribute__((section(sec_name)))

/*
 * ARM C Compiler above 6.10.1 (armclang)
 */
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6100100)

    #undef __IS_SDK6_COMPILER_GCC__    // In armclang, __GNUC__ is defined to 1

    /// define the BLE IRQ handler attribute for this compiler
    #define __BLEIRQ

    /// define size of an empty array (used to declare structure with an array size not defined)
    #define __ARRAY_EMPTY

    /// Put a variable in a zero-initialised memory section
    #define __SECTION_ZERO(sec_name) __attribute__((section(".bss."sec_name)))

    /// Put a variable in a given memory section
    #define __SECTION(sec_name) __attribute__((section(sec_name)))

/*
 * Clang compiler front end
 */
#elif defined(__clang__)

    #define __IS_SDK6_COMPILER_GCC__    // In armclang, __GNUC__ is defined to 1

    /// define the BLE IRQ handler attribute for this compiler
    #define __BLEIRQ

    /// define size of an empty array (used to declare structure with an array size not defined)
    #define __ARRAY_EMPTY   0

    /// Put a variable in a zero-initialised memory section
    /*
     * No zero_init attribute support in GCC. The initialization is being done
     * by the low level initialisation code. Make sure your section is added
     * in '.zero.table' inside the linker script.
     */
    #define __SECTION_ZERO(sec_name) __attribute__((section(sec_name)))

    /// Put a variable in a given memory section
    #define __SECTION(sec_name) __attribute__((section(sec_name)))
/*
 * GNU C Compiler
 */
#elif defined ( __GNUC__ )

    #define __IS_SDK6_COMPILER_GCC__    // In armclang, __GNUC__ is defined to 1

    /// define the BLE IRQ handler attribute for this compiler
    #define __BLEIRQ

    /// define size of an empty array (used to declare structure with an array size not defined)
    #define __ARRAY_EMPTY   0

    /// Put a variable in a zero-initialised memory section
    /*
     * No zero_init attribute support in GCC. The initialization is being done
     * by the low level initialisation code. Make sure your section is added
     * in '.zero.table' inside the linker script.
     */
    #define __SECTION_ZERO(sec_name) __attribute__((section(sec_name)))

    /// Put a variable in a given memory section
    #define __SECTION(sec_name) __attribute__((section(sec_name)))

/*
 * IAR C Compiler
 */
#elif defined ( __ICCARM__ )

    #undef __IS_SDK6_COMPILER_GCC__    // In armclang, __GNUC__ is defined to 1

    /// define the BLE IRQ handler attribute for this compiler
    #define __BLEIRQ

    /// define size of an empty array (used to declare structure with an array size not defined)
    #define __ARRAY_EMPTY

    /// Put a variable in a zero-initialised memory section
    #define __SECTION_ZERO(sec_name) __attribute__((section(sec_name)))

    /// Put a variable in a given memory section
    #define __SECTION(sec_name) __attribute__((section(sec_name)))

#else
    #warning "Unsupported C compiler used."

#endif

#endif // _COMPILER_H_

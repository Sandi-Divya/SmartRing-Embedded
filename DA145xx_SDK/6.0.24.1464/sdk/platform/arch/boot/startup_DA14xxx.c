/******************************************************************************
 * @file     startup_DA14xxx.c
 * @brief    CMSIS-Core(M) Device Startup File for
 *           Device <Device>
 * @version  V1.0.0
 * @date     20. January 2021
 ******************************************************************************/
/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2019-2024 Modified by Renesas Electronics Corporation and/or its affiliates.
 */

#if defined (__DA14535__)
#include "system_DA14535.h"
#elif defined (__DA14531__)
#include "system_DA14531.h"
#else
#include "system_DA14585_586.h"
#endif

#include "datasheet.h"
#include "compiler.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef enum 
{
    NVIC_ISER,
    NVIC_IP_bs,
    SCB_ICSR    = 9,
    SCB_SCR     = 10,
    SCB_CCR     = 11,
    SCB_SHP_1   = 12,
    SCB_SHP_2   = 13
}strd_ISER_idx;

typedef enum 
{
    MSP_IDX,
    PSP_IDX,
    CONTROL_IDX
}strd_MSP_idx;

/*
 * DEFINITIONS
 ****************************************************************************************
 */
#define HIB_MAGIC_WORD      0xA5

/*---------------------------------------------------------------------------
  External References
 *---------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;

extern __NO_RETURN void __PROGRAM_START(void);

/*---------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Default_Handler(void);
void __NO_RETURN Reset_Handler  (void);

/*---------------------------------------------------------------------------
  Stack configuration
 *---------------------------------------------------------------------------*/
#ifndef STACK_TOP_IS_THE_RAM_END
    #ifndef STACK_SIZE
        #if defined(__DA14531__)
            #define STACK_SIZE (0x700)
        #else
            #define STACK_SIZE (0x600)
        #endif
    #endif

    __USED uint8_t stack_var[STACK_SIZE]            __SECTION_ZERO("stack");
#endif

/*---------------------------------------------------------------------------
 Startup code section definition used for Keil builds
*---------------------------------------------------------------------------*/
#ifdef __IS_SDK6_COMPILER_GCC__
#define __STARTUP_CODE_SECTION
#else
#define __STARTUP_CODE_SECTION                      __SECTION("startup_code_DA14xxx")
#endif

#if defined(__IS_SDK6_COMPILER_GCC__) || defined (__ICCARM__)
/*---------------------------------------------------------------------------
  Patch table configuration
*---------------------------------------------------------------------------*/
#if !defined(__NON_BLE_EXAMPLE__)
#if !defined (PATCH_TABLE_AREA_SZ)
#define PATCH_TABLE_AREA_SZ (0x50) // 20 entries x 4 bytes per entry
#endif

__USED uint8_t patch_table_var[PATCH_TABLE_AREA_SZ] __SECTION_ZERO("patch_table");
#endif  // __NON_BLE_EXAMPLE__
#endif  // defined(__IS_SDK6_COMPILER_GCC__) || defined (__ICCARM__)

/*---------------------------------------------------------------------------
  Hibernation configuration
 *---------------------------------------------------------------------------*/

#ifdef CFG_STATEFUL_HIBERNATION
    // Added reserve space for the ARM core register dump
    // Reserve space for stacking, in case an exception occurs while restoring the state
    __USED uint32_t safe_stack_state[8]         __SECTION_ZERO("stateful_hibernation"); // 32 bytes

    // Reserve space for interrupt mask register (PRIMASK)
    __USED uint32_t stored_PRIMASK              __SECTION_ZERO("stateful_hibernation"); // 4 bytes
    
    // Reserve space for R4, R5, R6, R7, R8, R9, R10, R11, R12, R14
    __USED uint32_t stored_R4[10]               __SECTION_ZERO("stateful_hibernation"); // 40 bytes
    
    // Reserve space for MSP and PSP and control
    __USED uint32_t stored_MSP[3]               __SECTION_ZERO("stateful_hibernation"); // 12 bytes
    
    // Reserve space for NVIC->ISER, NVIC->IPR and SCB->SCR
    __USED uint32_t stored_ISER[17]             __SECTION_ZERO("stateful_hibernation"); // 68 bytes
    
    __USED uint32_t hiber_magic_word            __SECTION_ZERO("stateful_hibernation"); // 4 bytes

    __USED void restore_system_state(void);
#endif

/*---------------------------------------------------------------------------
  Exception / Interrupt Handler
 *---------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler            (void) __attribute__ ((weak));
void HardFault_Handler      (void) __attribute__ ((weak));
void MemManage_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void SecureFault_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));

void BLE_WAKEUP_LP_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void rwble_isr              (void) __attribute__ ((weak, alias("Default_Handler")));
void UART_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void KEYBRD_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void BLE_RF_DIAG_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void RFCAL_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO4_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void SWTIM_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void WKUP_QUADEC_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
#if defined(__DA14531__)
void SWTIM1_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void XTAL32M_RDY_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
#else
void PCM_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void SRCIN_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void SRCOUT_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
#endif
void RESERVED21_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void RESERVED22_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void RESERVED23_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

typedef void(*VECTOR_TABLE_Type)(void);

#if defined(__ICCARM__) && !defined(__VECTOR_TABLE_ATTRIBUTE)
#define __VECTOR_TABLE_ATTRIBUTE  @".intvec"
#endif

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
extern const VECTOR_TABLE_Type __VECTOR_TABLE[40];
const VECTOR_TABLE_Type __VECTOR_TABLE[40] __VECTOR_TABLE_ATTRIBUTE = {
    (VECTOR_TABLE_Type)(&__INITIAL_SP),  /*     Initial Stack Pointer */
    Reset_Handler,                       /*     Reset Handler         */
    NMI_Handler,                         /* -14 NMI Handler           */
    HardFault_Handler,                   /* -13 Hard Fault Handler    */
    MemManage_Handler,                   /* -12 MPU Fault Handler     */
    BusFault_Handler,                    /* -11 Bus Fault Handler     */
    UsageFault_Handler,                  /* -10 Usage Fault Handler   */
    SecureFault_Handler,                 /*  -9 Secure Fault Handler  */
    0,                                   /*     Reserved              */
    0,                                   /*     Reserved              */
    0,                                   /*     Reserved              */
    SVC_Handler,                         /*  -5 SVCall Handler        */
    0,                                   /*  -4 Debug Monitor Handler */
    0,                                   /*     Reserved              */
    PendSV_Handler,                      /*  -2 PendSV Handler        */
    SysTick_Handler,                     /*  -1 SysTick Handler       */
    /* Interrupts */
    BLE_WAKEUP_LP_Handler,               /* 0 BLE WakeUp LP Handler   */
    rwble_isr,                           /* 1 rwble Handler           */
    UART_Handler,                        /* 2 UART Handler            */
    UART2_Handler,                       /* 3 UART2 Handler           */
    I2C_Handler,                         /* 4 I2C Handler             */
    SPI_Handler,                         /* 5 SPI Handler             */
    ADC_Handler,                         /* 6 ADC Handler             */
    KEYBRD_Handler,                      /* 7 KEYBRD Handler          */
    BLE_RF_DIAG_Handler,                 /* 8 BLE RF Diag Handler     */
    RFCAL_Handler,                       /* 9 RFCAL Handler           */
    GPIO0_Handler,                       /* 10 GPIO0 Handler          */
    GPIO1_Handler,                       /* 11 GPIO1 Handler          */
    GPIO2_Handler,                       /* 12 GPIO2 Handler          */
    GPIO3_Handler,                       /* 13 GPIO3 Handler          */
    GPIO4_Handler,                       /* 14 GPIO4 Handler          */
    SWTIM_Handler,                       /* 15 SWTIM Handler          */
    WKUP_QUADEC_Handler,                 /* 16 WKUP QUADEC Handler    */
#if defined(__DA14531__)
    SWTIM1_Handler,                      /* 17 SWTIM1 Handler         */
    RTC_Handler,                         /* 18 RTC Handler            */
    DMA_Handler,                         /* 19 DMA Handler            */
    XTAL32M_RDY_Handler,                 /* 20 XTAL32M RDY Handler    */
#else
    PCM_Handler,                         /* 17 PCM Handler            */
    SRCIN_Handler,                       /* 18 SRCIN Handler          */
    SRCOUT_Handler,                      /* 19 SRCOUT Handler         */
    DMA_Handler,                         /* 20 DMA Handler            */
#endif
    RESERVED21_Handler,                  /* 21 RESERVED 21 Handler    */
    RESERVED22_Handler,                  /* 22 RESERVED 22 Handler    */
    RESERVED23_Handler,                  /* 23 RESERVED 23 Handler    */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*---------------------------------------------------------------------------
  Reset Handler called on controller reset
 *---------------------------------------------------------------------------*/
__STARTUP_CODE_SECTION __NO_RETURN void Reset_Handler(void)
{
    __set_PSP((uint32_t)(&__INITIAL_SP));

#ifdef CFG_STATEFUL_HIBERNATION
    if ((GetBits16(HIBERN_CTRL_REG, HIBERNATION_ENABLE) == 1) &&
        hiber_magic_word == HIB_MAGIC_WORD)
            restore_system_state();

    hiber_magic_word = 0;
#endif  // CFG_STATEFUL_HIBERNATION

    SystemInit();                    /* CMSIS System Initialization */
    __PROGRAM_START();               /* Enter PreMain (C library entry point) */
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

#ifdef CFG_STATEFUL_HIBERNATION
/*---------------------------------------------------------------------------
  Store Core Register
 *---------------------------------------------------------------------------*/
// Assembly function call used instead of C call to avoid LTO-related optimizations 
// (asm-level return from hibernation not possible due to implicit function inline - 
// no return address available)
void _store_core_registers(void)
{
    __asm volatile ("bl  store_core_registers \n");
}

__STATIC_FORCEINLINE __USED void store_GPR(void)
{
    __asm volatile (
        // store R4-R12, LR
        "ldr     r3, =stored_R4 \n"
        "stm     r3!, {r4-r7} \n"           // save R4-R7
        "mov     R4, R8 \n"
        "mov     R5, R9 \n"
        "mov     R6, R10 \n"
        "mov     R7, R11 \n"
        "stm     r3!, {r4-r7} \n"           // save R8-R11

        "mov     R4, R12 \n"
        "mov     R5, R14 \n"
        "stm     r3!, {r4-r5} \n"           // save R12, LR (R14)
    );
}

__STATIC_FORCEINLINE __USED void restore_GPR(void)
{
    __asm volatile (
        "ldr     r1, =stored_R4 \n"
        "movs    r2, #0x10 \n"
        "add     r1, r1, r2 \n"
        "ldm     r1!, {r4-r7} \n" // get R8-R11
        "mov     r8, r4 \n"
        "mov     r9, r5 \n"
        "mov     r10, r6 \n"
        "mov     r11, r7 \n"
        "ldr     r1, =stored_R4 \n"
        "ldm     r1!, {r4-r7} \n" // get R4-R7

        "ldr     r1, =stored_R4 \n"
        "movs    r2, #0x20 \n"
        "add     r1, r1, r2 \n"
        "ldm     r1!, {r4-r5} \n" // get R12 and LR
        "mov     r12, r4 \n"
        "mov     r14, r5 \n"
        );
}

__USED void store_core_registers(void)
{
    store_GPR();

    // Store MSP, PSP, CONTROL
    stored_MSP[MSP_IDX] = __get_MSP();
    stored_MSP[PSP_IDX] = __get_PSP();
    stored_MSP[CONTROL_IDX] = __get_CONTROL();

    stored_ISER[NVIC_ISER] = NVIC->ISER[0U];    // Store NVIC->ISER[0]
    NVIC->ICER[0] = 0xFFFFFFFFUL;               // Disable all interrupts in NVIC
    NVIC->ICPR[0] = 0xFFFFFFFFUL;               // Clear all pending interrupts

    // Save values of NVIC->IP[0..7] - for DA14531 we only have 8
    // (since we have 21 IRQs)
    // They fit in 8 32-bit registers
    memcpy(&stored_ISER[NVIC_IP_bs], (uint32_t*)NVIC->IP, 8*sizeof(uint32_t));
    stored_ISER[SCB_ICSR] = SCB->ICSR;
    memcpy(&stored_ISER[SCB_SCR], (uint32_t*)&SCB->SCR, 4*sizeof(uint32_t));

    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);

    hiber_magic_word = HIB_MAGIC_WORD;
    
    __NOP();
    __NOP();
    __NOP();
    
    __WFI();

    // we reach this point if device didn't actually go to stateful
    // hibernation

    SCB->SCR &= (~SCB_SCR_SLEEPDEEP_Msk);    // Restore SCR.SLEEPDEEP

    restore_system_state();
}

/*---------------------------------------------------------------------------
  Restore Core Register
 *---------------------------------------------------------------------------*/
__NO_RETURN void restore_system_state(void)
{
    __USED extern void arch_hibernation_restore(void);

    // disable interrupts, final state will be set when PRIMASK is restored
    __disable_irq();

    // set SP to a safe value, we have reserved some space for this case
    __set_MSP((uint32_t)&safe_stack_state);

    // restore values of NVIC->ISER, NVIC->PRIORITY{0,1,2},
    // SCB->SCR, SCB->SHR[0] (SHPR2) and SCB->SHR[1] (SHPR3)
    NVIC->ISER[0U] = stored_ISER[NVIC_ISER];
    memcpy((uint32_t*)NVIC->IP, &stored_ISER[NVIC_IP_bs], 8*sizeof(uint32_t));
    memcpy((uint32_t*)&SCB->SCR, &stored_ISER[SCB_SCR], 4*sizeof(uint32_t));

    // restore MSP, PSP, CONTROL
    __set_MSP(stored_MSP[MSP_IDX]);
    __set_PSP(stored_MSP[PSP_IDX]); 
    __set_CONTROL(stored_MSP[CONTROL_IDX]);

    arch_hibernation_restore();

    // Restore R4-R12, LR, 10 32-bit registers in total
    restore_GPR();

    // Restore PRIMASK
    __set_PRIMASK(stored_PRIMASK);

    __asm volatile ("bx     lr \n");
    
#if (!DEVELOPMENT_DEBUG)
    // We should never reach at this point
    // restore_system_state() branches to lr and restores operation 
    ASSERT_ERROR(0);
#else
    // while loop to satisfy the compiler for the __NO_RETURN
    wdg_reload(1);
    wdg_resume();
    while(1);
#endif
}
#endif

/*---------------------------------------------------------------------------
  Hard Fault Handler
 *---------------------------------------------------------------------------*/
extern void HardFault_HandlerC(unsigned long *hardfault_args);
__STARTUP_CODE_SECTION void HardFault_Handler(void)
{
    __asm volatile (
        " movs    r0, #4 \n"
        " mov     r1, lr \n"
        " tst     r0, r1 \n"
        " beq     HardFault_stacking_used_MSP \n"
        " mrs     r0, psp \n"
        " ldr     r1, =HardFault_HandlerC \n"
        " bx      r1 \n"
        "HardFault_stacking_used_MSP: \n"
        " mrs     r0, msp \n"
        " ldr     r1, =HardFault_HandlerC \n"
        " bx      r1 \n"
    );
}

/*---------------------------------------------------------------------------
  NMI Handler
 *---------------------------------------------------------------------------*/
extern void NMI_HandlerC(unsigned long *hardfault_args);
__STARTUP_CODE_SECTION void NMI_Handler(void)
{
    __asm volatile (
        " movs    r0, #4 \n"
        " mov     r1, lr \n"
        " tst     r0, r1 \n"
        " beq     NMI_stacking_used_MSP \n"
        " mrs     r0, psp \n"
        " ldr     r1, =NMI_HandlerC \n"
        " bx      r1 \n"
        "NMI_stacking_used_MSP: \n"
        " mrs     r0, msp \n"
        " ldr     r1, =NMI_HandlerC \n"
        " bx      r1 \n"
    );
}

/*---------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *---------------------------------------------------------------------------*/
__STARTUP_CODE_SECTION void Default_Handler(void)
{
    while(1);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic pop
#endif

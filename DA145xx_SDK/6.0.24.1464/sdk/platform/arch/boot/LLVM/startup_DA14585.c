/******************************************************************************
 * @file     startup_<Device>.c
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
 */

#include "system_DA14585_586.h"
#include "datasheet.h"

/*---------------------------------------------------------------------------
  External References
 *---------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
extern uint32_t __STACK_SEAL;
#endif

extern __NO_RETURN void __PROGRAM_START(void);

/*---------------------------------------------------------------------------
  Internal References
 *---------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler  (void);
__NO_RETURN void Default_Handler(void);

/*---------------------------------------------------------------------------
  Stack configuration
 *---------------------------------------------------------------------------*/
#if !defined (STACK_SIZE)
#if defined(__NON_BLE_EXAMPLE__)
#define STACK_SIZE         (0x600)
#else
#define STACK_SIZE         (0x700)
#endif
#endif

__USED uint8_t stack[STACK_SIZE] __attribute__((section(".stack")));

/*---------------------------------------------------------------------------
  Patch table configuration
 *---------------------------------------------------------------------------*/
#if !defined(__NON_BLE_EXAMPLE__)
#if !defined (PATCH_TABLE_AREA_SZ)
#define PATCH_TABLE_AREA_SZ (0x50) // 20 entries x 4 bytes per entry
#endif

__USED uint8_t patch_table[PATCH_TABLE_AREA_SZ] __attribute__((section(".patch_table")));
#endif


/* ToDo: Add Cortex exception handler according the used Cortex-Core */
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
void SWTIM1_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void XTAL32M_RDY_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
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

extern const VECTOR_TABLE_Type __interrupt_vector[40];
       const VECTOR_TABLE_Type __interrupt_vector[40] __VECTOR_TABLE_ATTRIBUTE = {
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
  SWTIM1_Handler,                      /* 17 SWTIM1 Handler         */
  RTC_Handler,                         /* 18 RTC Handler            */
  DMA_Handler,                         /* 19 DMA Handler            */
  XTAL32M_RDY_Handler,                 /* XTAL32M RDY Handler       */
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
__NO_RETURN void Reset_Handler(void)
{
  __set_PSP((uint32_t)(&__INITIAL_SP));


/* ToDo: Add stack sealing for Armv8-M based processors */
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  __TZ_set_STACKSEAL_S((uint32_t *)(&__STACK_SEAL));
#endif

  SystemInit();                    /* CMSIS System Initialization */
  __PROGRAM_START();               /* Enter PreMain (C library entry point) */
}


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

/*---------------------------------------------------------------------------
  Hard Fault Handler
 *---------------------------------------------------------------------------*/
extern void HardFault_HandlerC(unsigned long *hardfault_args);

void HardFault_Handler(void)
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

void NMI_Handler(void)
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
void Default_Handler(void)
{
  while(1);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#endif

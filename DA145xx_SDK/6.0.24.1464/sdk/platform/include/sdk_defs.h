/**
 ****************************************************************************************
 * @addtogroup SDK Definitions
 * @{
 * @addtogroup SDK_DEFS
 * @brief SDK definitions
 * @{
 *
 * @file sdk_defs.h (parsed by the Keil scatter file as well)
 *
 * Copyright (C) 2023-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _SDK_DEFS_H_
#define _SDK_DEFS_H_

/*
 * DEFINES
 ****************************************************************************************
 */

#define ALIGN4_UP(len)        ((((len) + 3) / 4) * 4)

/*
 * The size of the SDK flash programmmer stack. The flash programmer start stack address
 * is the end address of the RAM. The stack size defines the end address of the unitialized
 * RAM area which can be used by the TRNG mechanism.
 */
#define SDK_FLASH_PROGRAMMER_STACK_SIZE                    (ALIGN4_UP(0x600))

/*
 * The size of the available unitialized RAM provided by the SDK to the TRNG mechanism.
 * The start address of the aforementioned RAM area must reside always after the BLE BASE
 * address.
 */
#define SDK_TRNG_RAM_SIZE                                  (ALIGN4_UP(0xA00))

/*
 * The effective bootloader load image size, defined for DA14531.
 * The effective bootloader load image size for DA14535 will be based on
 * the DA14531 value.
 *
 * For UART Booter effective bootloader load image size is:
 * SDK_SEC_BOOTLOADER_LOAD_IMAGE_SIZE_INIT_VALUE - SDK_SEC_BOOTLOADER_COPY_BASE_ADDRESS
 */
#define SDK_SEC_BOOTLOADER_LOAD_IMAGE_SIZE_INIT_VALUE      (0x9400)

/*
 * The bootloader ECC patch table offset from the SDK_RAM_1_BASE_ADDR
 */
#define SDK_SEC_BL_ECC_PATCH_TABLE_OFFSET       (0xC0)

/*
 * The bootloader ECC patch table size (2 patch functions)
 */
#define SDK_SEC_BL_ECC_PATCH_TABLE_SIZE         (0x08)

#if defined (__DA14535__)

// The base addresses of the 2 RAM blocks
#define SDK_RAM_1_BASE_ADDR                     (0x07FC0000) // RAM1 block = 32KB (0x8000)
#define SDK_RAM_2_BASE_ADDR                     (0x07FC8000) // RAM2 block = 32KB (0x8000)

#define SDK_RAM_END_ADDR                        (SDK_RAM_2_BASE_ADDR + 0x8000)

#define SDK_RAM_UNINIT_END_ADDRESS              (SDK_RAM_END_ADDR - SDK_FLASH_PROGRAMMER_STACK_SIZE)
#define SDK_RAM_UNINIT_START_ADDRESS            (SDK_RAM_UNINIT_END_ADDRESS - SDK_TRNG_RAM_SIZE)
#define SDK_RAM_UNINIT_SIZE_IN_BYTES            (SDK_TRNG_RAM_SIZE)

#define SDK_SEC_BOOTLOADER_BASE_LR_ADDRESS      (SDK_RAM_1_BASE_ADDR + SDK_SEC_BL_ECC_PATCH_TABLE_OFFSET + SDK_SEC_BL_ECC_PATCH_TABLE_SIZE)

#define SDK_SEC_BOOTLOADER_LOAD_IMAGE_SIZE      (SDK_SEC_BOOTLOADER_LOAD_IMAGE_SIZE_INIT_VALUE + 0x4000) // 0x4000 is 16K (the difference between DA14535 and DA14531 RAM end addresses)
#define SDK_SEC_BOOTLOADER_ER_2_START_ADDRESS   (SDK_SEC_BOOTLOADER_BASE_LR_ADDRESS + SDK_SEC_BOOTLOADER_LOAD_IMAGE_SIZE)

#define SDK_SEC_BOOTLOADER_COPY_BASE_ADDRESS     (0x300)

#elif defined (__DA14531__)

// The base addresses of the 3 RAM blocks
#define SDK_RAM_1_BASE_ADDR                     (0x07FC0000) // RAM1 block = 16KB (0x4000)
#define SDK_RAM_2_BASE_ADDR                     (0x07FC4000) // RAM2 block = 12KB (0x3000)
#define SDK_RAM_3_BASE_ADDR                     (0x07FC7000) // RAM3 block = 20KB (0x5000)

#define SDK_RAM_END_ADDR                        (SDK_RAM_3_BASE_ADDR + 0x5000)

#define SDK_RAM_UNINIT_END_ADDRESS              (SDK_RAM_END_ADDR - SDK_FLASH_PROGRAMMER_STACK_SIZE)
#define SDK_RAM_UNINIT_START_ADDRESS            (SDK_RAM_UNINIT_END_ADDRESS - SDK_TRNG_RAM_SIZE)
#define SDK_RAM_UNINIT_SIZE_IN_BYTES            (SDK_TRNG_RAM_SIZE)

#define SDK_SEC_BOOTLOADER_BASE_LR_ADDRESS      (SDK_RAM_1_BASE_ADDR + SDK_SEC_BL_ECC_PATCH_TABLE_OFFSET + SDK_SEC_BL_ECC_PATCH_TABLE_SIZE)

#define SDK_SEC_BOOTLOADER_LOAD_IMAGE_SIZE      (SDK_SEC_BOOTLOADER_LOAD_IMAGE_SIZE_INIT_VALUE)
#define SDK_SEC_BOOTLOADER_ER_2_START_ADDRESS   (SDK_SEC_BOOTLOADER_BASE_LR_ADDRESS + SDK_SEC_BOOTLOADER_LOAD_IMAGE_SIZE)

#define SDK_SEC_BOOTLOADER_COPY_BASE_ADDRESS     (0x300)

#else

// The base addresses of the 4 RAM blocks
#define SDK_RAM_1_BASE_ADDR                     (0x07FC0000) // RAM1 block = 32KB (0x8000)
#define SDK_RAM_2_BASE_ADDR                     (0x07FC8000) // RAM2 block = 16KB (0x4000)
#define SDK_RAM_3_BASE_ADDR                     (0x07FCC000) // RAM3 block = 16KB (0x4000)
#define SDK_RAM_4_BASE_ADDR                     (0x07FD0000) // RAM4 block = 32KB (0x8000)

#define SDK_RAM_END_ADDR                        (SDK_RAM_4_BASE_ADDR + 0x8000)

#define SDK_SEC_BOOTLOADER_BASE_LR_ADDRESS      (SDK_RAM_1_BASE_ADDR + SDK_SEC_BL_ECC_PATCH_TABLE_OFFSET + SDK_SEC_BL_ECC_PATCH_TABLE_SIZE)

#define SDK_SEC_BOOTLOADER_LOAD_IMAGE_SIZE      (SDK_SEC_BOOTLOADER_LOAD_IMAGE_SIZE_INIT_VALUE + 0xC000) // 0xC000 is 48K (the difference between DA14585/586 and DA14531 RAM end addresses)
#define SDK_SEC_BOOTLOADER_ER_2_START_ADDRESS   (SDK_SEC_BOOTLOADER_BASE_LR_ADDRESS + SDK_SEC_BOOTLOADER_LOAD_IMAGE_SIZE)

#define SDK_SEC_BOOTLOADER_COPY_BASE_ADDRESS     (0x300)

#endif

#endif // _SDK_DEFS_H_

/// @}
/// @}

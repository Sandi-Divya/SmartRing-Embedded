/**
 ****************************************************************************************
 *
 * @file da14585_config_basic.h
 *
 * @brief Basic compile configuration file.
 *
 * Copyright (C) 2015-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _DA14585_CONFIG_BASIC_H_
#define _DA14585_CONFIG_BASIC_H_

/****************************************************************************************************************/
/* Encrypted image support                                                                                      */
/****************************************************************************************************************/
#define AES_ENCRYPTED_IMAGE_SUPPORTED     (0)

/****************************************************************************************************************/
/* ECC signed image support                                                                                     */
/* Set to 1 to enable secure boot from with signed images                                                       */
/****************************************************************************************************************/
#define SECURE_BOOT_ECC_IMAGE_SUPPORT     (0)

/****************************************************************************************************************/
/* 2-wire UART support                                                                                          */
/****************************************************************************************************************/
#define UART_SUPPORTED

/****************************************************************************************************************/
/* SPI flash memory support                                                                                     */
/****************************************************************************************************************/
#define SPI_FLASH_SUPPORTED

/****************************************************************************************************************/
/* I2C EEPROM memory support                                                                                    */
/****************************************************************************************************************/
#undef I2C_EEPROM_SUPPORTED

/****************************************************************************************************************/
/* Conditional compilation flags - please do not change                                                         */
/****************************************************************************************************************/
#if defined (SPI_FLASH_SUPPORTED) && defined (I2C_EEPROM_SUPPORTED)
#error "Both SPI_FLASH_SUPPORTED and I2C_EEPROM_SUPPORTED configuration flags are defined."
#endif

#ifndef I2C_EEPROM_SUPPORTED
    #define __EXCLUDE_I2C_HANDLER__
#endif

#ifndef UART_SUPPORTED
    #ifndef ONE_WIRE_UART_SUPPORTED
        #define __EXCLUDE_UART1_HANDLER__
        #define __EXCLUDE_UART2_HANDLER__
        #define __EXCLUDE_SYSTICK_HANDLER__
    #endif
#endif

/****************************************************************************************************************/
/* Enable extra wait time after release from Ultra Deep Power-Down (UDPD). Mainly required for some of the      */
/* Renesas SPI flash memories which support the UDPD command.                                                   */
/****************************************************************************************************************/
#undef CFG_SPI_FLASH_ADESTO_UDPD

/****************************************************************************************************************/
/* Enables/Disables the DMA Support for the SPI interface                                                       */
/****************************************************************************************************************/
#define CFG_SPI_DMA_SUPPORT

/****************************************************************************************************************/
/* Control the memory protection feature of the SPI flash chip through its Status Register 1.                   */
/****************************************************************************************************************/
#undef CFG_SPI_FLASH_MEM_PROTECT_USING_STATUS_REG1

#endif // _DA14585_CONFIG_BASIC_H_

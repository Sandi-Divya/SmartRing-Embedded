/**
 ****************************************************************************************
 *
 * @file da14531_config_basic.h
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

#ifndef _DA14531_CONFIG_BASIC_H_
#define _DA14531_CONFIG_BASIC_H_

/****************************************************************************************************************/
/* Notify the SDK about the fixed power mode (currently used only for Bypass):                                  */
/*     - CFG_POWER_MODE_BYPASS = Bypass mode                                                                    */
/****************************************************************************************************************/
#undef CFG_POWER_MODE_BYPASS

/****************************************************************************************************************/
/* Max buffer size used for the communication with the external tool over UART                                  */
/****************************************************************************************************************/
#define ALLOWED_DATA_UART       (0x08000)

/****************************************************************************************************************/
/* Max buffer size used for the communication with the external tool over SWD                                   */
/****************************************************************************************************************/
#define ALLOWED_DATA_SWD        (0x04000)

/****************************************************************************************************************/
/* Enables the SPI flash memory extensions feature. When enabled, the DA14531 controls, through a dedicated     */
/* GPIO, the activation/deactivation of the SPI flash memory device. This feature is mainly used when the       */
/* DA14531 is in boost mode and the SPI flash memory is supplied by VBAT_H (VBAT_L is the input voltage and the */
/* DCDC converter generates a programmable output voltage at VBAT_H). In boost mode the DCDC converter has a    */
/* limit of 50uA in VBAT_H during the DA14531 power-up. Due to this limitation some SPI flash memories may fail */
/* to start-up for certain values of VBAT_L. Therefore, their power voltage must be externally controlled using */
/* the SPI flash memory extensions feature. Refer to SDK6 documentation for further details.                    */
/****************************************************************************************************************/
#undef CFG_SPI_FLASH_EXTENSIONS

/****************************************************************************************************************/
/* The GPIO pin may be directly connected to a capacitor. If the pin status changes, the capacitor may be       */
/* discharged abruptly through the GPIO pin. If the macro is defined, the pull-down resistor will be enabled    */
/* first before enabling the pin push-pull operation.                                                           */
/* Note: The feature is used in the CFG_SPI_FLASH_EXTENSIONS context when the dedicated pin is connected        */
/* directly to the SPI flash memory voltage input.                                                              */
/****************************************************************************************************************/
#define CFG_SPI_FLASH_CAP_SLOW_DISCHARGE

/****************************************************************************************************************/
/* The GPIO pin may be directly connected to a capacitor. If the pin status changes abruptly, the GPIO pin may  */
/* be stressed since a large current may flow through the pin to charge the capacitor. Therefore, initially the */
/* internal pull-up resistor will be enabled.                                                                   */
/* Note: The feature is used in the CFG_SPI_FLASH_EXTENSIONS context when the dedicated pin is connected        */
/* directly to the SPI flash memory voltage input.                                                              */
/****************************************************************************************************************/
#define CFG_SPI_FLASH_CAP_SLOW_CHARGE

/****************************************************************************************************************/
/* Enable extra wait time after release from Ultra Deep Power-Down (UDPD). Mainly required for some of the      */
/* Renesas SPI flash memories which support the UDPD command.                                                   */
/****************************************************************************************************************/
#undef CFG_SPI_FLASH_ADESTO_UDPD

/****************************************************************************************************************/
/* Control the memory protection feature of the SPI flash chip through its Status Register 1.                   */
/****************************************************************************************************************/
#define CFG_SPI_FLASH_MEM_PROTECT_USING_STATUS_REG1

/****************************************************************************************************************/
/* Enable the secure programming feature of the AES keys and IVs.                                               */
/* Users should also define their secret unwrapping operation and key                                           */
/****************************************************************************************************************/
#define SECURE_AES_KEYS_IV_PROGRAMMER               (1)

#endif // _DA14531_CONFIG_BASIC_H_

/**
 ****************************************************************************************
 *
 * @file spi_hddr.h
 *
 * @brief SPIHDDR inline functions.
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

#ifndef _SPI_HDDR_H_
#define _SPI_HDDR_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>
#include "datasheet.h"
#include "user_periph_setup.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#define SPI_CS_DATA_REG                     (P0_DATA_REG + (SPIHDDR_CS_PORT << 5) )
#define SPI_CS_SET_DATA_REG                 (SPI_CS_DATA_REG + 2)
#define SPI_CS_RESET_DATA_REG               (SPI_CS_DATA_REG + 4)

#define SPI_DREADY_DATA_REG                 (P0_DATA_REG + (SPIHDDR_DREADY_PORT << 5) )
#define SPI_DREADY_SET_DATA_REG             (SPI_DREADY_DATA_REG + 2)
#define SPI_DREADY_RESET_DATA_REG           (SPI_DREADY_DATA_REG + 4)

/**
 ****************************************************************************************
 * @brief  Poll CS pin. 
 * @return CS pin level.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t spi_hddr_cs_getf(void)
{
    return GetBits16(SPI_CS_DATA_REG, 1 << SPIHDDR_CS_PIN);
}

/**
 ****************************************************************************************
 * @brief  Poll DR pin. 
 * @return DR pin level.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t spi_hddr_dready_getf(void)
{
    return GetBits16(SPI_DREADY_DATA_REG, 1 << SPIHDDR_DREADY_PIN);
}

/**
 ****************************************************************************************
 * @brief Asserts DREADY signal. 
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_hddr_dready_high(void)
{
    SetWord16(SPI_DREADY_SET_DATA_REG, 1 << SPIHDDR_DREADY_PIN);
}

/**
 ****************************************************************************************
 * @brief De-asserts DREADY signal. 
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_hddr_dready_low(void)
{
    SetWord16(SPI_DREADY_RESET_DATA_REG, 1 << SPIHDDR_DREADY_PIN);
}

#endif //_SPI_HDDR_H_

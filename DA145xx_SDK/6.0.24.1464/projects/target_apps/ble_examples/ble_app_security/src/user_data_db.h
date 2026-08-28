/**
 ****************************************************************************************
 *
 * @file user_data_db.h
 *
 * @brief User Data Database application header file.
 *
 * Copyright (C) 2024-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _USER_DATA_DB_H_
#define _USER_DATA_DB_H_

/*
 * DEFINES
 ****************************************************************************************
 */
#if defined (CFG_SPI_FLASH_ENABLE)
    #define USER_CFG_APP_DATA_DB_USE_SPI_FLASH
#elif defined (CFG_I2C_EEPROM_ENABLE)
    #define USER_CFG_APP_DATA_DB_USE_I2C_EEPROM
#endif

// SPI FLASH and I2C EEPROM data offset
#ifndef USER_CFG_APP_DATA_OFFSET
#if defined (USER_CFG_APP_DATA_DB_USE_SPI_FLASH)
    #define APP_USER_DB_DATA_OFFSET     (0x1F000)
#elif defined (USER_CFG_USER_DB_USE_I2C_EEPROM)
    #define APP_USER_DB_DATA_OFFSET     (0x10000)
#endif
#else
    #define APP_USER_DB_DATA_OFFSET     (USER_CFG_BOND_DB_DATA_OFFSET)
#endif // USER_CFG_USER_DATA_OFFSET

/// @name Headers used to mark the bond data in memory
///@{
/** Start header */
#define USER_DATA_DB_HEADER_START       (0x5678)
/** End header */
#define USER_DATA_DB_HEADER_END         (0x8765)
///@}

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */
#define ADC_VBAT_HIGH_VALS                      5

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
 
struct user_data_db
{
    uint16_t data_counter[ADC_VBAT_HIGH_VALS];
    uint16_t data[ADC_VBAT_HIGH_VALS];
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Create the data user storage and fetch data if any.
 ****************************************************************************************
 */
void user_storage_init(void);

/**
 ****************************************************************************************
 * @brief Erase all user data instances in the user storage area in NVM.
 * @note The function erases also the data kept in RAM.
 ****************************************************************************************
 */
bool user_app_usdb_erase(void);

/**
 ****************************************************************************************
 * @brief Store user data in NVM.
 * @param[in] the value that should be stored in NVM.
 ****************************************************************************************
 */
void user_storage_save(uint16_t value);

/**
 ****************************************************************************************
 * @brief Load the latest user data from NVM.
 ****************************************************************************************
 */
void user_storage_load(void);
/// @} APP

#endif // _USER_DATA_DB_H_
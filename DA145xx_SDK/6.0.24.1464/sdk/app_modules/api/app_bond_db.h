/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Bond_DB Bond Database
 * @brief Application Bond Database API
 * @{
 *
 * @file app_bond_db.h
 *
 * @brief Bond database header file.
 *
 * Copyright (C) 2012-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _APP_BOND_DB_H_
#define _APP_BOND_DB_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_APP_SEC)
#include "co_bt.h"
#include "gapc_task.h"
#include "gap.h"
#include "app_utils.h"
#include "app_security.h"
#include "user_periph_setup.h"
#include "user_config.h"
#include "gpio.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#if defined (CFG_SPI_FLASH_ENABLE)
    #define USER_CFG_APP_BOND_DB_USE_SPI_FLASH
#elif defined (CFG_I2C_EEPROM_ENABLE)
    #define USER_CFG_APP_BOND_DB_USE_I2C_EEPROM
#endif

// SPI FLASH and I2C EEPROM data offset
#ifndef USER_CFG_BOND_DB_DATA_OFFSET
#if defined (USER_CFG_APP_BOND_DB_USE_SPI_FLASH)
    #define APP_BOND_DB_DATA_OFFSET     (0x1E000)
#elif defined (USER_CFG_APP_BOND_DB_USE_I2C_EEPROM)
    #define APP_BOND_DB_DATA_OFFSET     (0x8000)
#endif
#else
    #define APP_BOND_DB_DATA_OFFSET     (USER_CFG_BOND_DB_DATA_OFFSET)
#endif // USER_CFG_BOND_DB_DATA_OFFSET

// Bond data storage size
#ifndef USER_CFG_BOND_DB_ID_SIZE
#define BOND_DATA_STORAGE_ID_SZ                 0x1000
#else
#define BOND_DATA_STORAGE_ID_SZ                 (USER_CFG_BOND_DB_ID_SIZE)
#endif

/// Max number of bonded peers
#ifndef USER_CFG_BOND_DB_MAX_BONDED_PEERS
#define APP_BOND_DB_MAX_BONDED_PEERS    (5)
#else
#define APP_BOND_DB_MAX_BONDED_PEERS    (USER_CFG_BOND_DB_MAX_BONDED_PEERS)
#endif // USER_CFG_BOND_DB_MAX_BONDED_PEERS

/// Database version
#define BOND_DB_VERSION                 (0x0001)

/// @name Headers used to mark the bond data in memory
///@{
/** Start header */
#define BOND_DB_HEADER_START            ((0x1234) + BOND_DB_VERSION)
/** End header */
#define BOND_DB_HEADER_END              ((0x4321) + BOND_DB_VERSION)
///@}

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Do initial fetch of stored bond data.
 ****************************************************************************************
 */
void default_app_bdb_init(void);

/**
 ****************************************************************************************
 * @brief Erase all bonding data instances in the bond storage area in NVM.
 * @note The function erases also the data kept in RAM at the bdb variable.
 ****************************************************************************************
 */
bool default_app_bdb_erase(void);

/**
 ****************************************************************************************
 * @brief Get the number of slots supported in bond database
 * @return number of slots present in bond database
 ****************************************************************************************
 */
uint8_t default_app_bdb_get_size(void);

/**
 ****************************************************************************************
 * @brief Add a new bond data entry to the bond database. Searches the slot to write the
 *        new bond data by using the following criteria
 *        1) If there is a slot with the same IRK or BD address, replace that slot
 *        2) Else if there is an empty slot, store bond data to the first empty slot
 *        3) Else store bond data to the oldest written slot
 * @param[in] data    Pointer to the data to be stored.
 ****************************************************************************************
 */
void default_app_bdb_add_entry(struct app_sec_bond_data_env_tag *data);

/**
 ****************************************************************************************
 * @brief Search the database to find the slot with the bond data that match.
 *        If remove_type is remove_this_entry, then remove this entry and update ext mem.
 *        If remove_type is remove_all_but_this, then remove all other entries and update ext mem.
 *        If remove_type is remove_all, then remove all entries and update ext mem.
 *
 * @param[in] search_type   Indicates the type with which bond database will be searched.
 *                          An entry can be found either by searching by EDIV, BDA, IRK,
 *                          custom type or if its slot is already known.
 * @param[in] remove_type   Indicates the operation that will be performed
 * @param[in] search_param  Pointer to the value that will be matched
 * @param[in] search_param_length  Size of the value that will be matched
 ****************************************************************************************
 */
void default_app_bdb_remove_entry(enum bdb_search_by_type search_type,
                                  enum bdb_remove_type remove_type,
                                  void *search_param, uint8_t search_param_length);

/**
 ****************************************************************************************
 * @brief Search the database to find the slot with the bond data that match.
 * @param[in] search_type   Indicates the type with which bond database will be searched.
 *                          A slot can be matched either by EDIV, BDA, IRK, or custom type.
 * @param[in] search_param  Pointer to the value that will be matched
 * @param[in] search_param_length  Size of the value that will be matched
 * @return Pointer to the bond data if they were found. Otherwise null.
 ****************************************************************************************
 */
const struct app_sec_bond_data_env_tag* default_app_bdb_search_entry(
                                                        enum bdb_search_by_type search_type,
                                                        void *search_param,
                                                        uint8_t search_param_length);

/**
 ****************************************************************************************
 * @brief Get the number of IRKs stored in bond database.
 * @return Number of valid IRKs
 ****************************************************************************************
 */
uint8_t default_app_bdb_get_number_of_stored_irks(void);

/**
 ****************************************************************************************
 * @brief Search the bond database to find the entries which contain an IRK.
 * @param[in] valid_irk_irray    Pointer to the array with the valid IRKs
 * @return Number of valid IRKs
 ****************************************************************************************
 */
uint8_t default_app_bdb_get_stored_irks(struct gap_sec_key *valid_irk_irray);

/**
 ****************************************************************************************
 * @brief Get the device info stored in a specific slot (identity, identity type, peer irk)
 * @param[in] slot         Slot to search in bond database
 * @param[out] dev_info    Device info
 * @return True if device info has been found, else false
 ****************************************************************************************
 */
bool default_app_bdb_get_device_info_from_slot(uint8_t slot,
                                               struct gap_ral_dev_info *dev_info);

#endif // (BLE_APP_SEC)

#endif // _APP_BOND_DB_H_

///@}
///@}

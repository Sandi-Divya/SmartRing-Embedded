/**
 *****************************************************************************************
 *
 * @file app_bond_db.c
 *
 * @brief Bond database code file.
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
 *****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP_BOND_DB
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_APP_SEC)

#include "rwip.h"
#include "app_bond_db.h"
#include "app_easy_storage.h"

#if (SECURE_DATA_STORAGE)
#include "app_easy_crypto.h"
#endif

#if defined (CFG_PRINTF)
#include "arch_console.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */
#define BOND_DB_VALID_ENTRY                     (0xAA)
#define BOND_DB_EMPTY_SLOT                      (0)
#define BOND_DB_SLOT_NOT_FOUND                  (0xFF)
#define BOND_DB_SZ                              (sizeof(struct bond_db))

#define BDB_ENCRYPTED                           true

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
struct bond_db
{
    uint8_t valid_slot[APP_BOND_DB_MAX_BONDED_PEERS];
    uint32_t timestamp_counter;
    uint32_t timestamp[APP_BOND_DB_MAX_BONDED_PEERS];
    struct app_sec_bond_data_env_tag data[APP_BOND_DB_MAX_BONDED_PEERS];
};

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
static struct bond_db bdb                       __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
static storage_env_t bdb_storage                __SECTION_ZERO("retention_mem_area0");

#if (SECURE_DATA_STORAGE)
static uint8_t bdb_enc_key[ENC_DATA_LEN]        __SECTION_ZERO("retention_mem_area0");
static const static_key_t bdb_enc_key_seed =  { {OTP_PARAM_VAL_1, OTP_PARAM_VAL_2, OTP_PARAM_VAL_3, OTP_PARAM_VAL_4}, 16 };
#endif // SECURE_DATA_STORAGE

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/*
 * STATIC FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Store Bond Database to external memory
 * @param[in] scheduler_en  Only used if external memory is Flash
                            True: Enable rwip_scheduler while Flash is being erased
 *                          False: Do not enable rwip_scheduler. Blocking mode
 ****************************************************************************************
 */
__STATIC_INLINE void bond_db_store_ext(void)
{
#if defined (USER_CFG_APP_BOND_DB_USE_SPI_FLASH) || defined (USER_CFG_APP_BOND_DB_USE_I2C_EEPROM)
    app_easy_storage_save((uint8_t*)&bdb, &bdb_storage);
#endif
}

/**
 ****************************************************************************************
 * @brief Store Bond data entry to external memory
 * @param[in] *data  Data to be stored
 * @param[in] *idx   Entry in the database
 ****************************************************************************************
 */
static void bond_db_store_at_idx(struct app_sec_bond_data_env_tag *data, int idx)
{
    bdb.valid_slot[idx] = BOND_DB_VALID_ENTRY;
    // Update the cache
    memcpy(&bdb.data[idx], data, sizeof(struct app_sec_bond_data_env_tag));
    // Store new bond data to external memory
    bond_db_store_ext();
}

/**
 ****************************************************************************************
 * @brief Clear bond data.
 ****************************************************************************************
 */
static void bond_db_clear()
{
    memset((void *)&bdb, 0, sizeof(struct bond_db) ); // zero bond data
    // Store zero bond data to external memory
    bond_db_store_ext();
}

/*
 * EXPOSED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void default_app_bdb_init(void)
{
#if defined (USER_CFG_APP_BOND_DB_USE_SPI_FLASH) || defined (USER_CFG_APP_BOND_DB_USE_I2C_EEPROM)
#if (SECURE_DATA_STORAGE)
    // Generate the key for the bonding database
    bdb_storage.enc_key_size = app_generate_static_key((static_key_t*)&bdb_enc_key_seed, bdb_enc_key, ENC_DATA_LEN);

#if defined (CFG_PRINTF)
    arch_printf("\r\n Key used for encrypted bonding data storage: ");
    for(uint8_t i=0; i < bdb_storage.enc_key_size; i++)
        arch_printf("%02x", bdb_enc_key[i]);
    arch_printf("\n\r");
#endif

#endif
    // Initialize the bond storage enviroment
    bdb_storage.storage_id = STORAGE_ID(BOND_DB_HEADER_START, BOND_DB_HEADER_END);
    bdb_storage.mem_start_addr = APP_BOND_DB_DATA_OFFSET;
    bdb_storage.storage_id_size = BOND_DATA_STORAGE_ID_SZ;
    bdb_storage.instance_size = BOND_DB_SZ;
#if (SECURE_DATA_STORAGE)
    bdb_storage.enc_data = BDB_ENCRYPTED;
    bdb_storage.enc_key = bdb_enc_key;
    bdb_storage.encrypt_func = app_easy_aes_encrypt_data;
    bdb_storage.decrypt_func = app_easy_aes_decrypt_data;
#endif
    
    app_easy_storage_open(&bdb_storage);
    
    if (app_easy_storage_is_empty(&bdb_storage))
        bond_db_clear();
    else
        app_easy_storage_load((uint8_t*)&bdb, &bdb_storage);
#endif // USER_CFG_APP_BOND_DB_USE_SPI_FLASH || USER_CFG_APP_BOND_DB_USE_I2C_EEPROM
}

bool default_app_bdb_erase(void)
{
    memset((void *)&bdb, 0, sizeof(struct bond_db) ); // delete bonding data in RAM
    return app_easy_storage_clear(&bdb_storage);
}

uint8_t default_app_bdb_get_size(void)
{
    return APP_BOND_DB_MAX_BONDED_PEERS;
}

void default_app_bdb_add_entry(struct app_sec_bond_data_env_tag *data)
{
    bool first_empty_slot_found = false;
    uint8_t i = 0;
    uint32_t min_timestamp;
    uint8_t slot_to_write = BOND_DB_SLOT_NOT_FOUND;

    for(i = 0; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
    {
        // If current slot is not valid (is empty)
        if (bdb.valid_slot[i] != BOND_DB_VALID_ENTRY)
        {
            // Check if an empty slot has already been found
            if (first_empty_slot_found == false)
            {
                // First empty slot has been found
                first_empty_slot_found = true;
                // Store empty slot to write
                slot_to_write = i;
            }
        }
        // If current slot is not empty
        else
        {
            // Check if IRK is present in current slot and in new pairing data
            if ((bdb.data[i].valid_keys & *(&data->valid_keys)) & RIRK_PRESENT)
            {
                // Check if stored IRK matches with new IRK
                if (memcmp(&data->rirk.irk, &bdb.data[i].rirk.irk, sizeof(struct gap_sec_key)) == 0)
                {
                    // IRK matches, store this slot to be replaced and exit
                    slot_to_write = i;
                    break;
                }
            }
            // If IRK is not present
            else
            {
                // Check if stored BD address matches with new BD address
                if (memcmp(&data->peer_bdaddr, &bdb.data[i].peer_bdaddr, sizeof(struct gap_bdaddr)) == 0)
                {
                    // BD address matches, store this slot to be replaced and exit
                    slot_to_write = i;
                    break;
                }
            }
        }
    }

    // If there is no available slot, find the least recently written slot to replace
    if (slot_to_write == BOND_DB_SLOT_NOT_FOUND)
    {
        min_timestamp = bdb.timestamp[0];
        slot_to_write = 0;
        for(i = 1; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
        {
            // Find the slot with the oldest (minimum) timestamp
            if (bdb.timestamp[i] < min_timestamp)
            {
                min_timestamp = bdb.timestamp[i];
                slot_to_write = i;
            }
        }
    }

    bdb.timestamp[slot_to_write] = bdb.timestamp_counter++;

    // Store bond db entry
    data->bdb_slot = slot_to_write;
    bond_db_store_at_idx(data, slot_to_write);
}

void default_app_bdb_remove_entry(enum bdb_search_by_type search_type, enum bdb_remove_type remove_type,
                          void *search_param, uint8_t search_param_length)
{
    uint8_t i = 0;
    uint8_t slot_found = BOND_DB_SLOT_NOT_FOUND;

    if (remove_type == REMOVE_ALL)
    {
        bond_db_clear();
        return;
    }

    if (search_type == SEARCH_BY_SLOT_TYPE)
    {
        slot_found = *((uint8_t *)search_param);
    }
    else
    {
        for(i = 0; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
        {
            // Check if EDIVs match
            if ((search_type == SEARCH_BY_EDIV_TYPE) &&
                ((memcmp(&bdb.data[i].ltk.ediv, search_param, search_param_length) == 0)))
            {
                slot_found = i;
                break;
            }
            // Check if BD addresses match
            else if ((search_type == SEARCH_BY_BDA_TYPE) &&
                     ((memcmp(&bdb.data[i].peer_bdaddr.addr, search_param, search_param_length) == 0)))
            {
                slot_found = i;
                break;
            }
            // Check if IRKs match
            else if ((search_type == SEARCH_BY_IRK_TYPE) &&
                     (memcmp(&bdb.data[i].rirk, search_param, search_param_length) == 0))
            {
                slot_found = i;
                break;
            }
            else if (search_type == SEARCH_BY_ID_TYPE)
            {
                // Check if bond_db BD address and given ID address match
                if ((memcmp(&bdb.data[i].rirk.addr.addr, search_param, search_param_length) == 0))
                {
                    slot_found = i;
                    break;
                }
            }
        }
    }

    // Check if a valid slot has been found
    if (slot_found < APP_BOND_DB_MAX_BONDED_PEERS)
    {
        if (remove_type == REMOVE_THIS_ENTRY)
        {
            // Remove entry from cache
            memset((void *)&bdb.data[slot_found], 0, sizeof(struct app_sec_bond_data_env_tag));
            bdb.timestamp[slot_found] = 0;
            bdb.valid_slot[slot_found] = BOND_DB_EMPTY_SLOT;
        }
        else
        {
            // If remove_all_but_this is true, remove all other entries from cache
            for(i = 0; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
            {
                if (i != slot_found)
                {
                    memset((void *)&bdb.data[i], 0, sizeof(struct app_sec_bond_data_env_tag));
                    bdb.timestamp[i] = 0;
                    bdb.valid_slot[i] = BOND_DB_EMPTY_SLOT;
                }
            }
        }

        // Store the updated cache to the external non volatile memory
        bond_db_store_ext();
    }
}

const struct app_sec_bond_data_env_tag* default_app_bdb_search_entry(enum bdb_search_by_type search_type,
                                                             void *search_param,
                                                             uint8_t search_param_length)
{
    struct app_sec_bond_data_env_tag *found_data = NULL;

    for(uint8_t i = 0; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
    {
        // Check if EDIVs match
        if ((search_type == SEARCH_BY_EDIV_TYPE) &&
            ((memcmp(&bdb.data[i].ltk.ediv, search_param, search_param_length) == 0)))
        {
            found_data = &bdb.data[i];
            break;
        }
        // Check if BD addresses match
        else if ((search_type == SEARCH_BY_BDA_TYPE) &&
                 ((memcmp(&bdb.data[i].peer_bdaddr.addr, search_param, search_param_length) == 0)))
        {
            found_data = &bdb.data[i];
            break;
        }
        // Check if IRKs match
        else if ((search_type == SEARCH_BY_IRK_TYPE) &&
                 (memcmp(&bdb.data[i].rirk, search_param, search_param_length) == 0))
        {
            found_data = &bdb.data[i];
            break;
        }
        // Check if IDs match
        else if ((search_type == SEARCH_BY_ID_TYPE) &&
                (memcmp(&bdb.data[i].rirk.addr.addr, search_param, search_param_length) == 0))
        {
            found_data = &bdb.data[i];
            break;
        }
    }

    return found_data;
}

uint8_t default_app_bdb_get_number_of_stored_irks(void)
{
    uint8_t nb_key = 0;

    // Search DB to find the entries which contain an IRK
    for(uint8_t i = 0; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
    {
        // Check if there is an IRK present in the current slot
        if ((bdb.data[i].valid_keys & RIRK_PRESENT) == RIRK_PRESENT)
            nb_key++;
    }

    return nb_key;
}

uint8_t default_app_bdb_get_stored_irks(struct gap_sec_key *valid_irk_irray)
{
    uint8_t nb_key = 0;

    // Search DB to find the entries which contain an IRK
    for(uint8_t i = 0; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
    {
        // Check if there is an IRK present in the current slot
        if ((bdb.data[i].valid_keys & RIRK_PRESENT) == RIRK_PRESENT)
        {
            // Copy IRK to the IRK array
            memcpy(&valid_irk_irray[nb_key], &bdb.data[i].rirk.irk, sizeof(struct gap_sec_key));
            nb_key++;
        }
    }

    return nb_key;
}

bool default_app_bdb_get_device_info_from_slot(uint8_t slot, struct gap_ral_dev_info *dev_info)
{
    // Check if the entry is valid and if there is an IRK
    if ((bdb.valid_slot[slot] == BOND_DB_VALID_ENTRY) && ((bdb.data[slot].valid_keys & RIRK_PRESENT) == RIRK_PRESENT))
    {
        memcpy(dev_info->addr, &bdb.data[slot].rirk.addr.addr.addr, BD_ADDR_LEN*sizeof(uint8_t));
        dev_info->addr_type = bdb.data[slot].rirk.addr.addr_type;
        memcpy(dev_info->peer_irk, &bdb.data[slot].rirk.irk, sizeof(struct gap_sec_key));
        return true;
    }
    return false;
}
#endif // (BLE_APP_SEC)

/// @} APP_BOND_DB

/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Easy_Storage Easy Crypto
 * @brief Storage-related helper functions API
 * @{
 *
 * @file app_easy_storage.h
 *
 * @brief Application storage helper functions header file.
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

#ifndef _APP_EASY_STORAGE_H_
#define _APP_EASY_STORAGE_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#include "user_periph_setup.h" 
#include "user_config.h"
#include "gpio.h"

/*
 * DEFINES
 ****************************************************************************************
 */
#define STORAGE_ID_SIZE         4

#define STORAGE_ID(start, end)                          ((start << 16) | (end))

#define STORAGE_INSTANCE_SZ(size, key_len)              ((!key_len) ? size : ((size % key_len) == 0) ? size : \
                                                        (((size / key_len) + 1) * key_len))

#define STORAGE_SLOTS_2_SZ(slots, size, enc, key_len)   (( slots *  ( ( (!enc) ? size : STORAGE_INSTANCE_SZ(size, key_len) ) + STORAGE_ID_SIZE ) ) )

#if (SECURE_DATA_STORAGE)
typedef void (*crypto_func)(uint8_t* key, uint8_t key_size, uint8_t* in, uint8_t* out, uint16_t size);
#endif

typedef struct
{
    uint32_t            mem_end_addr;       // external memory storage end address
    uint32_t            mem_cur_addr;       // external memory current data address
    bool                storage_empty;      // No data in storage
    bool                is_open;            // indicating flag the storage is initialized
#if (SECURE_DATA_STORAGE)
    uint32_t            instance_enc_size;  // instance encrypted size
#endif // SECURE_DATA_STORAGE
} storage_env_rsv_t;

typedef struct
{
    uint32_t            storage_id;         // every storage instance should have its own id
    uint32_t            mem_start_addr;     // external memory storage start address
    uint32_t            storage_id_size;    // memory available for this id storage space in bytes
    uint32_t            instance_size;      // instance size ex. sizeof(bond_db)
    storage_env_rsv_t   reserved;           // storage internal variables
#if (SECURE_DATA_STORAGE)
    bool                enc_data;           // true if data are encrypted
    uint8_t             *enc_key;           // key used for encryption
    uint8_t             enc_key_size;       // key size of the encryption algorithm
    crypto_func         encrypt_func;       // pointer for encryption function
    crypto_func         decrypt_func;       // pointer for decryption function
#endif
} storage_env_t;

/**
 ****************************************************************************************
 * @brief Initialize the enviroment for the each storage instance
 * @param[in] storage_hnd       Storage Enviroment
 * @return                      True if returned successfuly, false otherwise
 ****************************************************************************************
 */
bool app_easy_storage_open(storage_env_t* storage_hnd);

/**
 ****************************************************************************************
 * @brief Stores data in the current available slot in storage
 * @param[in] data              Pointer to data buffer that should be stored in flash
 * @param[in] storage_env       Storage environment pointer
 * @return                      True if returned successfuly, false otherwise
 ****************************************************************************************
 */
bool app_easy_storage_save(void* data, storage_env_t *storage_env);

/**
 ****************************************************************************************
 * @brief Load the last data instance from storage space to RAM
 * @param[out] data             Pointer to buffer where the data will be loaded in
 * @param[in] storage_env       Storage environment pointer
 * @return                      True if returned successfuly, false otherwise
 ****************************************************************************************
 */
bool app_easy_storage_load(void* data, storage_env_t *storage_env);

/**
 ****************************************************************************************
 * @brief Erases all the data for the specific storage
 * @param[in] storage_env       Pointer to the storage enviroment
 * @return                      True if returned successfuly, false otherwise
 ****************************************************************************************
 */
bool app_easy_storage_is_empty(storage_env_t *storage_env);

/**
 ****************************************************************************************
 * @brief Erases all the data for the specific storage
 * @param[in] storage_env       Pointer to the storage enviroment
 * @return                      True if returned successfuly, false otherwise
 ****************************************************************************************
 */
bool app_easy_storage_clear(storage_env_t *storage_env);

#endif // _APP_EASY_STORAGE_H_

///@}
///@}
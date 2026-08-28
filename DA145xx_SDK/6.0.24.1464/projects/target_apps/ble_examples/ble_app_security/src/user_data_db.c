/**
 ****************************************************************************************
 *
 * @file user_data_db.c
 *
 * @brief User Data Database source code.
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

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "user_data_db.h"
#include "app_easy_storage.h"

#if (SECURE_DATA_STORAGE)
#include "app_easy_crypto.h"
#endif

#if defined (CFG_PRINTF)
#include "arch_console.h"
#endif

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */
#define UDDB_ENCRYPTED                          true
#define STORAGE_DATA_INSTANCES                  3
// Calculate the size of the user data storage based on the slots required
#define USER_DATA_STORAGE_ID_SZ                 STORAGE_SLOTS_2_SZ(STORAGE_DATA_INSTANCES, sizeof(struct user_data_db), UDDB_ENCRYPTED, ENC_DATA_LEN)

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
static storage_env_t usdb_storage               __SECTION_ZERO("retention_mem_area0");
struct user_data_db usdb                        __SECTION_ZERO("retention_mem_area0");

#if (SECURE_DATA_STORAGE)
static uint8_t usdb_enc_key[ENC_DATA_LEN]       __SECTION_ZERO("retention_mem_area0");
static const static_key_t usdb_enc_key_seed =  { {OTP_SEED(1), OTP_SEED(4), OTP_SEED(2), OTP_SEED(5)}, 16 };
#endif // SECURE_DATA_STORAGE

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize user data to zero and save.
 ****************************************************************************************
 */
static void user_db_clear()
{
    memset((void *)&usdb, 0, sizeof(struct user_data_db) ); // zero user data
    // Store zero bond data to external memory
    app_easy_storage_save((uint8_t*)&usdb, &usdb_storage);
}

bool user_app_usdb_erase(void)
{
    memset((void *)&usdb, 0, sizeof(struct user_data_db) ); // zero user data
    return app_easy_storage_clear(&usdb_storage);
}

void user_storage_save(uint16_t value)
{
    uint16_t latest_entry = usdb.data_counter[ADC_VBAT_HIGH_VALS-1];
    // Copy the last 4 values at idx 0
    memcpy(&usdb.data_counter[0], &usdb.data_counter[1], sizeof(uint16_t) * (ADC_VBAT_HIGH_VALS - 1));
    memcpy(&usdb.data[0], &usdb.data[1], sizeof(uint16_t) * (ADC_VBAT_HIGH_VALS - 1));
    // Store the latest value
    usdb.data_counter[ADC_VBAT_HIGH_VALS - 1] = latest_entry + 1;
    usdb.data[ADC_VBAT_HIGH_VALS - 1] = value;
    app_easy_storage_save((uint8_t*)&usdb, &usdb_storage);
}

void user_storage_init(void)
{
#if (SECURE_DATA_STORAGE)
    // Generate the key for the bonding database
    usdb_storage.enc_key_size = app_generate_static_key((static_key_t*)&usdb_enc_key_seed, usdb_enc_key, ENC_DATA_LEN);

#if defined (CFG_PRINTF)
    arch_printf("\r\n Key used for encrypted user data storage: ");
    for(uint8_t i=0; i < usdb_storage.enc_key_size; i++)
        arch_printf("%02x", usdb_enc_key[i]);
    arch_printf("\n\r");
#endif // CFG_PRINTF
#endif // SECURE_DATA_STORAGE

    // Initialize the bond storage enviroment
    usdb_storage.storage_id = STORAGE_ID(USER_DATA_DB_HEADER_START, USER_DATA_DB_HEADER_END);
    usdb_storage.mem_start_addr = APP_USER_DB_DATA_OFFSET;
    usdb_storage.storage_id_size = USER_DATA_STORAGE_ID_SZ;
    usdb_storage.instance_size = sizeof(struct user_data_db);
#if (SECURE_DATA_STORAGE)
    usdb_storage.enc_data = UDDB_ENCRYPTED;
    usdb_storage.enc_key = usdb_enc_key;
    usdb_storage.encrypt_func = app_easy_aes_encrypt_data;
    usdb_storage.decrypt_func = app_easy_aes_decrypt_data;
#endif

    app_easy_storage_open(&usdb_storage);
    app_easy_storage_load((uint8_t*)&usdb, &usdb_storage);
    
    if (app_easy_storage_is_empty(&usdb_storage))
        user_db_clear();
}


/// @} APP
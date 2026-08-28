/**
 ****************************************************************************************
 *
 * @file app_easy_storage.c
 *
 * @brief Application Storage Entry Point
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
 * @addtogroup APP_STORAGE
 * @{
 ****************************************************************************************
 */
 
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "app_easy_storage.h"
#include "rwip.h"
#include "user_callback_config.h"

#if defined (CFG_SPI_FLASH_ENABLE)
    #include "spi.h"
    #include "spi_flash.h"
    #define APP_EASY_STORAGE_USE_SPI_FLASH
#elif defined (CFG_I2C_EEPROM_ENABLE)
    #include "i2c.h"
    #include "i2c_eeprom.h"
    #define APP_EASY_STORAGE_USE_I2C_EEPROM
#endif

#if (SECURE_DATA_STORAGE)
#include "app_easy_crypto.h"
#endif

/*
 * #DEFINES
 ****************************************************************************************
 */
#define HEADER_START            0
#define HEADER_END              1
#define HEADER_SIZE             2

#define ID2HEADER_START(id)     (id >> 16)
#define ID2HEADER_END(id)       (id & 0x0000FFFF)

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if defined (APP_EASY_STORAGE_USE_SPI_FLASH)

/**
 ****************************************************************************************
 * @brief Initialize flash storage interaction
 ****************************************************************************************
 */
static void flash_storage_init(void)
{
   uint8_t dev_id;

   // Release the SPI flash from power down
   spi_flash_release_from_power_down();

   // Try to auto-detect the SPI flash memory
   spi_flash_auto_detect(&dev_id);

   // Disable the SPI flash memory protection (unprotect all sectors)
   spi_flash_configure_memory_protection(SPI_FLASH_MEM_PROT_NONE);
}

/**
 ****************************************************************************************
 * @brief Terminate flash storage interaction
 ****************************************************************************************
 */
static void flash_storage_release(void)
{
    // Power down flash
    spi_flash_power_down();
}

/**
 ****************************************************************************************
 * @brief Erase Flash sectors where data are stored
 * @param[in] scheduler_en  True: Enable rwip_scheduler while Flash is being erased
 *                          False: Do not enable rwip_scheduler. Blocking mode
 * @param[in] size          Size of the data that should be deleted
 * @param[in] flash_offset  Address of data in flash
 * @return ret              Error code or success (ERR_OK)
 ****************************************************************************************
 */
static int8_t erase_flash_sectors(bool scheduler_en, uint16_t size, uint32_t flash_offset)
{
    uint32_t sector_nb;
    uint32_t offset;
    int8_t ret;
    int i;
    uint32_t timeout_cnt;

    // Calculate the starting sector offset
    offset = (flash_offset / SPI_FLASH_SECTOR_SIZE) * SPI_FLASH_SECTOR_SIZE;

    // Calculate the numbers of sectors to erase
    sector_nb = (size / SPI_FLASH_SECTOR_SIZE);
    if (size % SPI_FLASH_SECTOR_SIZE)
        sector_nb++;

    for (i = 0; i < sector_nb; i++)
    {
        if (scheduler_en)
        {
            // Non-Blocking Erase of a Flash sector
            ret = spi_flash_block_erase_no_wait(offset, SPI_FLASH_OP_SE);
            if (ret != SPI_FLASH_ERR_OK)
                return ret;

            timeout_cnt = 0;

            while ((spi_flash_read_status_reg() & SPI_FLASH_SR_BUSY) != 0)
            {
                // Check if BLE is on and not in deep sleep and call rwip_schedule()
                if ((GetBits16(CLK_RADIO_REG, BLE_ENABLE) == 1) &&
                   (GetBits32(BLE_DEEPSLCNTL_REG, DEEP_SLEEP_STAT) == 0))
                {
                    // Assuming that the WDG is not active, timeout will be reached in case of a Flash erase error.
                    // NOTE: In case the WDG is active, the WDG timer will expire (much) earlier than the timeout
                    // is reached and therefore an NMI will be triggered.
                    if (++timeout_cnt > SPI_FLASH_WAIT)
                    {
                        return SPI_FLASH_ERR_TIMEOUT;
                    }
                    rwip_schedule();
                }
            }
        }
        else
        {
            // Blocking Erase of a Flash sector
            ret = spi_flash_block_erase(offset, SPI_FLASH_OP_SE);
            if (ret != SPI_FLASH_ERR_OK)
                return ret;
        }
        offset += SPI_FLASH_SECTOR_SIZE;
    }

    return SPI_FLASH_ERR_OK;
}
#endif // APP_EASY_STORAGE_USE_SPI_FLASH


#if defined (APP_EASY_STORAGE_USE_I2C_EEPROM)
/**
 ****************************************************************************************
 * @brief Initialize EEPROM storage interaction
 ****************************************************************************************
 */
static void eeprom_storage_init(void)
{
    // Initialize I2C for Serial EEPROM
    i2c_eeprom_initialize();
}

/**
 ****************************************************************************************
 * @brief Terminate EEPROM storage interaction
 ****************************************************************************************
 */
static void eeprom_storage_release(void)
{
    i2c_eeprom_release();
}

#define EEPROM_STORAGE_CLEAR_BLOCK_SIZE (16)
#define EEPROM_STORAGE_CLEAR_VAL        (0x00)

/**
 ****************************************************************************************
 * @brief Clears an EEPROM area
 * @param[in] scheduler_en  True: Enable rwip_scheduler while EEPROM is being cleared
 *                          False: Do not enable rwip_scheduler. Blocking mode
 * @param[in] size          Size of the data that should be cleared
 * @param[in] eeprom_offset Address of area to be cleared in EEPROM
 * @return i2c_error_code   I2C status codes
 ****************************************************************************************
 */
static i2c_error_code clear_eeprom_area(bool scheduler_en, uint16_t size, uint32_t eeprom_offset)
{
    uint32_t length2write;
    i2c_error_code ret;
    uint32_t bytes_written;
    uint8_t byte_clear_array[EEPROM_STORAGE_CLEAR_BLOCK_SIZE];

    memset(byte_clear_array, EEPROM_STORAGE_CLEAR_VAL, EEPROM_STORAGE_CLEAR_BLOCK_SIZE);

    while(size) {
        if(size >= EEPROM_STORAGE_CLEAR_BLOCK_SIZE) {
            length2write = EEPROM_STORAGE_CLEAR_BLOCK_SIZE;
        } else {
            length2write = size;
        }
    
        ret = i2c_eeprom_write_data(byte_clear_array, eeprom_offset, length2write, &bytes_written);
        ASSERT_WARNING(bytes_written == length2write);
        if(ret != I2C_NO_ERROR) {
            return ret;
        }

        size -= length2write;

        if (scheduler_en)
        {
            // Check if BLE is on and not in deep sleep and call rwip_schedule()
            if ((GetBits16(CLK_RADIO_REG, BLE_ENABLE) == 1) &&
               (GetBits32(BLE_DEEPSLCNTL_REG, DEEP_SLEEP_STAT) == 0))
            {
                rwip_schedule();
            }
        }
    }
    
    return ret;
}
#endif // APP_EASY_STORAGE_USE_I2C_EEPROM

/**
 ****************************************************************************************
 * @brief Wrapper for communication initialization of the external memory's interface
 ****************************************************************************************
 */
static void external_mem_open(void)
{
#if defined (APP_EASY_STORAGE_USE_SPI_FLASH)
    flash_storage_init();
#elif defined (APP_EASY_STORAGE_USE_I2C_EEPROM)
    eeprom_storage_init();
#endif
}

/**
 ****************************************************************************************
 * @brief Wrapper for writing data to the external memory
 * @param[in] data          Pointer to the data to be written
 * @param[in] address       Starting address of page to be written (must be a multiple
 *                          of SPI Flash page size - if FLASH is used for storage)
 * @param[in] size          Size of the data to be written (can be larger than SPI Flash
 *                          page size)
 * @return                  If successful interaction with memory true, false otherwise
 ****************************************************************************************
 */
static bool external_mem_store(uint8_t *data, uint32_t address, uint32_t size)
{
#if defined (APP_EASY_STORAGE_USE_SPI_FLASH)
    uint32_t actual_size;
    int8_t ret;
    ret = spi_flash_write_data(data, address, size, &actual_size);
    ASSERT_WARNING(actual_size == size);

    if(ret != SPI_FLASH_ERR_OK) {
        return false;
    }
#elif defined (APP_EASY_STORAGE_USE_I2C_EEPROM)
    i2c_error_code ret;
    uint32_t bytes_written;

    ret = i2c_eeprom_write_data(data, address, size, &bytes_written);
    ASSERT_WARNING(bytes_written == size);
    if(ret != I2C_NO_ERROR) {
        return false;
    }
#endif

    return true;
}

/**
 ****************************************************************************************
 * @brief Wrapper for reading data from the external memory
 * @param[in] data          Pointer to the position the read data will be stored
 * @param[in] address       Starting address of data to be read
 * @param[in] size          Size of the data to be read
 * @return                  If successful interaction with memory true, false otherwise
 ****************************************************************************************
 */
static bool external_mem_load(uint8_t *data, uint32_t address, uint32_t size)
{
#if defined (APP_EASY_STORAGE_USE_SPI_FLASH)
    uint32_t actual_size;
    int8_t ret;

    ret = spi_flash_read_data(data, address, size, &actual_size);
    ASSERT_WARNING(actual_size == size);

    if(ret != SPI_FLASH_ERR_OK) {
        return false;
    }
#elif defined (APP_EASY_STORAGE_USE_I2C_EEPROM)
    uint32_t bytes_read;
    i2c_error_code ret;

    ret = i2c_eeprom_read_data(data, address, size, &bytes_read);
    ASSERT_WARNING(bytes_read == size);
    
    if(ret != I2C_NO_ERROR) {
        return false;
    }
#endif

    return true;
}

/**
 ****************************************************************************************
 * @brief Wrapper for communication termination of the external memory's interface
 ****************************************************************************************
 */
static void external_mem_close(void)
{
#if defined (APP_EASY_STORAGE_USE_SPI_FLASH)
    // Power down flash
    flash_storage_release();
#elif defined (APP_EASY_STORAGE_USE_I2C_EEPROM)
    eeprom_storage_release();
#endif
}

/**
 ****************************************************************************************
 * @brief Wrapper for erasing a specific external memory area
 * @param[in] scheduler_en  True: Enable rwip_scheduler while Flash is being erased
 *                          False: Do not enable rwip_scheduler. Blocking mode
 * @param[in] address       Starting address of data to be erased
 * @param[in] size          Size of the data to be erased
 * @return                  True if erase operation was sucesfull, false otherwise
 ****************************************************************************************
 */
static bool external_mem_erase(bool scheduler_en, uint32_t address, uint32_t size)
{
#if defined (APP_EASY_STORAGE_USE_SPI_FLASH)
    int8_t ret;
    ret = erase_flash_sectors(scheduler_en, size, address);

    if(ret != SPI_FLASH_ERR_OK) {
        return false;
    }
#elif defined (APP_EASY_STORAGE_USE_I2C_EEPROM)
    i2c_error_code ret;
    ret = clear_eeprom_area(scheduler_en, size, address);
    
    if(ret != I2C_NO_ERROR) {
        return false;
    }
#endif

    return true;
}

/**
 ****************************************************************************************
 * @brief Checks if storage headers for a specific storage id exist
 * @param[in] id         The id of the storage
 * @param[in] address    The start address of the storage
 * @param[in] inst_size  The storage instance size
 * @return               True if a valid storage has been found, false otherwise
 ****************************************************************************************
 */
static bool storage_headers_exist(uint32_t id, uint32_t address, uint32_t inst_size)
{
    bool ret = false;

    // Read headers from the storage space
    uint8_t header[2][HEADER_SIZE];

    // Open/init the external memory storage
    external_mem_open();

    do {
        // Try to load the start header, if there is an error break immediately
        if(!external_mem_load(header[HEADER_START], address, HEADER_SIZE)) {
            break;
        }

        // Try to load the end header, if there is an error break immediately
        if(!external_mem_load(header[HEADER_END], (address + inst_size - HEADER_SIZE), HEADER_SIZE)) {
            break;
        }

        ret = true;
    } while(0);

    // Close/De-init the external memory storage
    external_mem_close();

    // Check if there is an existing header in the data that was read
    if(co_read16p(header[HEADER_START]) != ID2HEADER_START(id) || 
       co_read16p(header[HEADER_END]) != ID2HEADER_END(id)) 
    {
        ret = false;
    }

    return ret;
}

/**
 ****************************************************************************************
 * @brief Allocates space for a storage in the external memory by writing the storage headers to it
 * @param[in] id         The id of the storage
 * @param[in] address    The start address of the storage
 * @param[in] inst_size  The storage instance size
 * @return               True if storage allocation has completed succesfully, false otherwise
 ****************************************************************************************
 */
static bool storage_area_allocate(uint32_t id, uint32_t address, uint32_t inst_size)
{
    bool ret = false;
    uint16_t header_start = ID2HEADER_START(id);
    uint16_t header_end = ID2HEADER_END(id);

    // Open/init the external memory storage
    external_mem_open();

    do {
        // Try to write the start header, if there is an error break immediately
        if (!external_mem_store((uint8_t*)&header_start, address, HEADER_SIZE)) {
            break;
        }

        // Try to write the end header, if there is an error break immediately
        if (!external_mem_store((uint8_t*)&header_end, (address + inst_size - HEADER_SIZE), HEADER_SIZE)) {
            break;
        }

        ret = true;
    } while(0);

    // Close/De-init the external memory storage
    external_mem_close();

    return ret;
}


/**
 ****************************************************************************************
 * @brief Function used by the app_easy_storage to write storage data in an external memory
 * @param[in] data         A pointer to the data to be written in the storage
 * @param[in] address      The start address of the storage
 * @param[in] size         The size of the storage data to be written
 * @return                 True if storage writing has completed succesfully, false otherwise
 ****************************************************************************************
 */
static bool storage_area_write(uint8_t *data, uint32_t address, uint32_t size)
{
    bool ret;

    // Open/init the external memory storage
    external_mem_open();

    // Write the data in the external memory
    ret = external_mem_store(data, address, size);

    // Close/De-init the external memory storage
    external_mem_close();

    return ret;
}

/**
 ****************************************************************************************
 * @brief Function used by the app_easy_storage to load storage data from an external memory
 * @param[out] data        A pointer where the storage data loaded will be put
 * @param[in] address      The start address of the required data in the external storage memory
 * @param[in] size         The size of the storage data to be read/loaded
 * @return                 True if storage data loading has completed succesfully, false otherwise
 ****************************************************************************************
 */
static bool storage_area_load(uint8_t *data, uint32_t address, uint32_t size)
{
    bool ret;

    // Open/init the external memory storage
    external_mem_open();

    // Write the external memory area
    ret = external_mem_load(data, address, size);

    // Close/De-init the external memory storage
    external_mem_close();

    return ret;
}

/**
 ****************************************************************************************
 * @brief Function used by the app_easy_storage to load storage data from an external memory
 * @param[out] start_addr  The address from which erasing should start
 * @param[in]  size        The size of the area to be erased
 * @return                 True if storage data erasing has completed succesfully, false otherwise
 ****************************************************************************************
 */
static bool storage_area_erase(uint32_t start_addr, uint32_t size)
{
    bool ret;

    // Open/init the external memory storage
    external_mem_open();

    // Erase the external memory area
    ret = external_mem_erase(true, start_addr, size);

    // Close/De-init the external memory storage
    external_mem_close();
    
    return ret;
}

/*************************************************************************************************/
/****************************** APP EASY STORAGE API IMPLEMENTATION ******************************/
/*************************************************************************************************/

bool app_easy_storage_open(storage_env_t* storage_env)
{
    // Check if there are any data in the storage area
    storage_env->reserved.mem_cur_addr = storage_env->mem_start_addr;
    // Calculate the instance size
    uint32_t size = storage_env->instance_size + STORAGE_ID_SIZE;
    
#if (SECURE_DATA_STORAGE)
    if (storage_env->enc_data)
    {
        // Calculate the instance store size
        storage_env->reserved.instance_enc_size = STORAGE_INSTANCE_SZ(storage_env->instance_size, storage_env->enc_key_size);
        size = storage_env->reserved.instance_enc_size + STORAGE_ID_SIZE;
    }
#endif

    storage_env->reserved.mem_end_addr = storage_env->mem_start_addr + storage_env->storage_id_size;

    // Storage is created but considered that contains data
    storage_env->reserved.storage_empty = false;

    // Indicate that this storage enviroment is open
    storage_env->reserved.is_open = true;

    do
    {
        if (!storage_headers_exist(storage_env->storage_id, storage_env->reserved.mem_cur_addr, size))
        {
            if (storage_env->reserved.mem_cur_addr == storage_env->mem_start_addr)
            {
                // Clear the storage area
                return app_easy_storage_clear(storage_env);
            }
            else
                break;
        }
        storage_env->reserved.mem_cur_addr += size;
    }
    while ((storage_env->reserved.mem_cur_addr + size) <= storage_env->reserved.mem_end_addr);

    storage_env->reserved.mem_cur_addr -= size;

    return true;
}

bool app_easy_storage_save(void* data, storage_env_t *storage_env)
{
    bool ret = false;

    if (storage_env->reserved.is_open)
    {
        ret = true;

#if (SECURE_DATA_STORAGE)
        uint32_t size = ((storage_env->enc_data) ? storage_env->reserved.instance_enc_size : storage_env->instance_size) + STORAGE_ID_SIZE;
#else
        uint32_t size = storage_env->instance_size + STORAGE_ID_SIZE;
#endif

        uint8_t *temp_data = data;

        // If the storage is empty dont increment the slot for storing
        if (!storage_env->reserved.storage_empty)
        {
            if(storage_env->reserved.mem_cur_addr + (size << 1) > storage_env->reserved.mem_end_addr)
                ret &= app_easy_storage_clear(storage_env);
            else
                storage_env->reserved.mem_cur_addr += size;
        }

        ret &= storage_area_allocate(storage_env->storage_id, storage_env->reserved.mem_cur_addr, size);

        size -= STORAGE_ID_SIZE;

#if (SECURE_DATA_STORAGE)
        if (storage_env->enc_data)
        {
            // Allocate space for the padded encrypted data
            temp_data = ke_malloc(size, KE_MEM_NON_RETENTION);

            // Clear the temp data buffer area
            memset(temp_data, 0, size);

            // Copy the data to be encrypted(plaintext) in the temp data buffer
            memcpy(temp_data, data, storage_env->instance_size);

            // Perform the encryption
            storage_env->encrypt_func(storage_env->enc_key, storage_env->enc_key_size, temp_data, temp_data, size);
        }
#endif // SECURE_DATA_STORAGE

        ret &= storage_area_write(temp_data, storage_env->reserved.mem_cur_addr + HEADER_SIZE, size);

#if (SECURE_DATA_STORAGE)
        if (storage_env->enc_data)
            ke_free(temp_data);
#endif

        storage_env->reserved.storage_empty = false;
    }

    return ret;
}

bool app_easy_storage_load(void* data, storage_env_t *storage_env)
{
    bool ret = false;

    if (storage_env->reserved.is_open)
    {
#if (SECURE_DATA_STORAGE)
        uint32_t size = ((storage_env->enc_data) ? storage_env->reserved.instance_enc_size : storage_env->instance_size) + STORAGE_ID_SIZE;
#else
        uint32_t size = storage_env->instance_size + STORAGE_ID_SIZE;
#endif

        // Read headers from the storage space
        if(!storage_headers_exist(storage_env->storage_id, storage_env->reserved.mem_cur_addr, size))
            return ret;

        uint8_t *temp_data = data;

        size -= STORAGE_ID_SIZE;
    
#if (SECURE_DATA_STORAGE)
        if (storage_env->enc_data)
            temp_data = ke_malloc(size, KE_MEM_NON_RETENTION);
#endif

        // Load the data. Temp_data is already assigned with the enviroment data pointer
        ret = storage_area_load(temp_data, storage_env->reserved.mem_cur_addr + HEADER_SIZE, size);

#if (SECURE_DATA_STORAGE)
        if (storage_env->enc_data)
        {
            // Decrypt the loaded data
            storage_env->decrypt_func(storage_env->enc_key, storage_env->enc_key_size, temp_data, temp_data, size);

            // Copy the decrypted data to the plaintext data buffer
            memcpy(data, temp_data, storage_env->instance_size);

            // Free the temp data buffer
            ke_free(temp_data);
        }
#endif
    }

    return ret;
}

bool app_easy_storage_is_empty(storage_env_t *storage_env)
{
    return storage_env->reserved.storage_empty;
}

bool app_easy_storage_clear(storage_env_t *storage_env)
{
    if (storage_env->reserved.is_open)
    {
        // Erase the storage area data from the external memory
        if(storage_area_erase(storage_env->mem_start_addr, storage_env->storage_id_size))
        {
            storage_env->reserved.storage_empty = true;
            storage_env->reserved.mem_cur_addr = storage_env->mem_start_addr;
            return true;
        }
    }

    return false;
}

/// @} APP_STORAGE

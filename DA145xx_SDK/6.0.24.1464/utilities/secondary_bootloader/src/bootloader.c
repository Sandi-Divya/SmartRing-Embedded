/**
 ****************************************************************************************
 *
 * @file bootloader.c
 *
 * @brief bootloader application
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "user_periph_setup.h"
#include "uart_booter.h"
#include "i2c_eeprom.h"
#include "bootloader.h"
#include "spi_flash.h"
#include "gpio.h"
#if AES_ENCRYPTED_IMAGE_SUPPORTED
#include "decrypt.h"
#endif
#if SECURE_BOOT_ECC_IMAGE_SUPPORT
#include "ecc_verify.h"
#include "sha2.h"
#endif // SIGNED_IMAGE_SUPPORT_EXT_MEM

#include "key_storage.h"

#if SECURE_BOOT_ECC_IMAGE_SUPPORT
#define SIGNED_IMAGE_SUPPORT_EXT_MEM            (1)
#else
#define SIGNED_IMAGE_SUPPORT_EXT_MEM            (0)
#endif // SIGNED_IMAGE_SUPPORT_EXT_MEM

#if AES_ENCRYPTED_IMAGE_SUPPORTED
#define AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE     (1)
#else
#define AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE     (0)
#endif

// AES and/or ECC Key revokation is only supported if the keys are saved in the OTP and ECC image verification is enabled
// If key revokation is enabled, revokation (writing the revokation flag in the OTP) will happen after the image has been verified

/*
 * TYPEDEFS
 ****************************************************************************************
 */
typedef enum {
    IMAGE_STATUS_FLAG_NONE_VALID =    (0),
    IMAGE_STATUS_FLAG_IMAGE_1_VALID = (1 << 0),
    IMAGE_STATUS_FLAG_IMAGE_2_VALID = (1 << 1),
    IMAGE_STATUS_FLAG_BOTH_VALID =    (IMAGE_STATUS_FLAG_IMAGE_1_VALID | IMAGE_STATUS_FLAG_IMAGE_2_VALID),
} IMAGE_STATUS_FLAG;

typedef struct {
    s_imageHeaderKeyInfo_t keyInfo[2];
} image_key_section_t;
 
/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

#ifdef I2C_EEPROM_SUPPORTED
/// Configuration struct for I2C
static const i2c_cfg_t i2c_cfg = {
    .clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET,
    .clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET,
    .clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET,
    .clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET,
    .restart_en = I2C_RESTART_ENABLE,
    .speed = I2C_SPEED_MODE,
    .mode = I2C_MODE_MASTER,
    .addr_mode = I2C_ADDRESS_MODE,
    .address = I2C_SLAVE_ADDRESS,
    .tx_fifo_level = 1,
    .rx_fifo_level = 1,
};

/// Configuration struct for I2C EEPROM
static const i2c_eeprom_cfg_t i2c_eeprom_cfg = {
    .size = I2C_EEPROM_DEV_SIZE,
    .page_size = I2C_EEPROM_PAGE_SIZE,
    .address_size = I2C_ADDRESS_SIZE,
};
#endif

#if !defined(SUPPORT_AN_B_001) && (defined(SPI_FLASH_SUPPORTED ) || defined(I2C_EEPROM_SUPPORTED))

/**
****************************************************************************************
* @brief Read a block of data from a nonvolatile memory
* @param[in] destination_buffer: buffer to put the data
* @param[in] source_addr: starting position to read the data from spi
* @param[in] len: size of data to read
****************************************************************************************
*/
static int FlashRead(unsigned long destination_buffer, unsigned long source_addr, unsigned long len)
{
#ifdef SPI_FLASH_SUPPORTED
    uint32_t actual_length;

#if defined (CFG_SPI_DMA_SUPPORT)
    spi_flash_read_data_dma((uint8_t*)destination_buffer, source_addr, len, &actual_length);
#else
    spi_flash_read_data((uint8_t*)destination_buffer, source_addr, len, &actual_length);
#endif
    return 0;
#else
    uint32_t bytes_read;
    if (i2c_eeprom_read_data((uint8_t*)destination_buffer, (unsigned long)source_addr, (unsigned long)len, &bytes_read) != I2C_NO_ERROR)
    {
        return bytes_read;
    }
    else
    {
        return 0;
    }
#endif
}

static void FlashInvalidateImageHeader(unsigned long imageHeaderFlagAddress)
{
#ifdef SPI_FLASH_SUPPORTED
    // Erase the sector that the image header resides
    // (alternatively, if the flash supports page erasing, spi_flash_page_erase(imageHeaderFlagAddress) can be used instead)
    spi_flash_block_erase(imageHeaderFlagAddress, SPI_FLASH_OP_SE);
#else
    uint8_t validflagbyte;
    uint32_t bytes_written;
    validflagbyte = 0xFF;
    i2c_eeprom_write_data((uint8_t*)&validflagbyte, imageHeaderFlagAddress, 1, &bytes_written);
#endif
}

/**
 ****************************************************************************************
 * @brief Return the bank index of the active (latest and valid) image
 * @param[in] destination_buffer: buffer to put the data
 * @param[in] source_addr: starting position to read the data from spi
 * @param[in] len: size of data to read
 ****************************************************************************************
 */
static uint8_t findlatest(uint8_t id1, uint8_t id2)
{
    if (id1 == 0xFF && id2 == 0)
    {
        return 2;
    }

    if (id2 == 0xFF && id1 == 0)
    {
        return 1;
    }

    if (id1 >= id2)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

extern uint32_t crc32(uint32_t crc, const void *buf, size_t size);

/**
 ****************************************************************************************
 * @brief Read the image header of an image in the external memory
 * @param[in] imageOffset: the offset of the image inside the external memory
 * @param[out] pImageHeader: a pointer to place the image header that was read
 ****************************************************************************************
 */
void imageHeaderRead(uint32_t imageOffset, s_imageHeader *pImageHeader)
{
    FlashRead((unsigned long )pImageHeader,
              (unsigned long)imageOffset,
              (unsigned long)sizeof(s_imageHeader));
}

/**
 ****************************************************************************************
 * @brief Check if the image header of an image is valid
 * @param[in] iHeader: a pointer to the image header to be checked
 * @return True if the header is valid, false otherwise
 ****************************************************************************************
 */
static bool imageHeaderValidityCheck(s_imageHeader *imgHeader)
{
    if (imgHeader->validflag == STATUS_VALID_IMAGE &&
        imgHeader->signature[0] == IMAGE_HEADER_SIGNATURE1 &&
        imgHeader->signature[1] == IMAGE_HEADER_SIGNATURE2)
    {
        return true;
    }
    
    return false;
}

/**
 ****************************************************************************************
 * @brief Check the images in the external memory and determine which is the active image
 * @param[in] imageAddress1: the starting address of the 1st image
 * @param[in] imageAddress2: the starting address of the 2nd image
 * @param[out] pvalid_images: how many images are valid
 *            (1: 1st image is valid, 2: 2nd image is valid, 3: both images are valid)
 * @param[out] keyInfo: The key information inside each image header will be returned here
 * @return The active image index (0: first image, 1: second image). 
 *         Will return -1 if no valid image was found
 ****************************************************************************************
 */
int8_t imageGetInfo(uint32_t imageAddress1, uint32_t imageAddress2, IMAGE_STATUS_FLAG *pvalid_images, image_key_section_t *keyInfo)
{
    IMAGE_STATUS_FLAG valid_images = IMAGE_STATUS_FLAG_NONE_VALID;
    uint8_t imageId[2];
    uint32_t imgAddress[2] = {imageAddress1, imageAddress2};
    s_imageHeader ImageHeader;
    s_imageHeader *pImageHeader;
    uint8_t i;

    pImageHeader = &ImageHeader;

    if(keyInfo) {
        memset(keyInfo, 0, sizeof(image_key_section_t));
    }

    // Read the image headers of both images to determine validity
    for (i = 0; i < 2; i++) {
        imageHeaderRead(imgAddress[i], pImageHeader);
        if (imageHeaderValidityCheck(pImageHeader))
        {
            valid_images |= (1 << i);
            imageId[i] = pImageHeader->imageid;
            if(keyInfo) {
                memcpy(&keyInfo->keyInfo[i], &pImageHeader->keyInfo, sizeof(s_imageHeaderKeyInfo_t));
            }
        }
    }

    // Return the image validity status
    if(pvalid_images) {
        *pvalid_images = valid_images;
    }

    // If no valid image was found, return -1
    if(valid_images == IMAGE_STATUS_FLAG_NONE_VALID) {
        return -1;
    }
    
    // If only one valid image was found, return the index of that one
    if(valid_images != IMAGE_STATUS_FLAG_BOTH_VALID) {
        return (valid_images == IMAGE_STATUS_FLAG_IMAGE_1_VALID)?0:1;
    }

    // If both the images were valid, return the index of the active/latest image
    return findlatest(imageId[0], imageId[1]) - 1;
}

#if SIGNED_IMAGE_SUPPORT_EXT_MEM
/**
 ****************************************************************************************
 * @brief Calculate the sha256 of an image in an external memory by reading 
 * it step by step in small blocks
 * @param[in] ext_mem_addr: The external memory address where the image is located
 * @param[in]  len: The length of the image
 * @param[out] hash: A pointer to where the calculated sha256 will be put
 ****************************************************************************************
 */
static void sha256_blocks_ext_mem(uint32_t ext_mem_addr, uint32_t len, uint8_t *hash)
{
    uint32_t rlen;
    uint8_t buff[SHA256_BLOCK_SIZE];
    sha256_ctx ctx;

    sha256_init(&ctx);

    while(len) {
        if(len >= SHA256_BLOCK_SIZE) {
            rlen = SHA256_BLOCK_SIZE;
        } else {
            rlen = len;
        }

        FlashRead((unsigned long) buff,
                  (unsigned long) ext_mem_addr,
                  (unsigned long) rlen);

        sha256_update(&ctx, buff, rlen);

        ext_mem_addr += rlen;
        len -= rlen;
    }

    sha256_final(&ctx, hash);
}
#endif // SIGNED_IMAGE_SUPPORT_EXT_MEM

#if SIGNED_IMAGE_SUPPORT_EXT_MEM || AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE
struct storage_key_status {
    uint8_t valid_key[2];
};

/**
 ****************************************************************************************
 * @brief Checks if the OTP keys that the images point to, are valid
 * @param[in]  img_key_info: The image key info as carried by the images
 * @param[out] status: The status of each image key in the OTP storage
 ****************************************************************************************
 */
static void storage_key_check(image_key_section_t *img_key_info, struct storage_key_status *status)
{
    if(status == NULL)
        return;

    for (uint8_t i = 0; i < 2; i++) 
    {
        status->valid_key[i] = 1;
#if SIGNED_IMAGE_SUPPORT_EXT_MEM
        status->valid_key[i] &= key_storage_is_key_index_valid(KEY_TYPE_ECC, img_key_info->keyInfo[i].ecc_key_idx);
        status->valid_key[i] &= !key_storage_otp_is_key_revoked(KEY_TYPE_ECC, img_key_info->keyInfo[i].ecc_key_idx);
#endif // SIGNED_IMAGE_SUPPORT_EXT_MEM

#if AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE
        status->valid_key[i] &= key_storage_is_key_index_valid(KEY_TYPE_AES, img_key_info->keyInfo[i].aes_key_idx);
        status->valid_key[i] &= !key_storage_otp_is_key_revoked(KEY_TYPE_AES, img_key_info->keyInfo[i].aes_key_idx);
#endif // AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE
    }
}
#endif // SIGNED_IMAGE_SUPPORT_EXT_MEM || AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE

/**
 ****************************************************************************************
 * @brief Load the active (latest and valid) image from a non-volatile memory
 * @return Success (0) or Error Code.
 *
 ****************************************************************************************
 */
static int loadActiveImage(void)
{
    s_productHeader *ProductHeader;
    s_imageHeader *ImageHeader;
    uint8_t flashbuffer[sizeof(s_imageHeader)];
    uint32_t imageOffsets[2];
    uint16_t image_header_size = sizeof(s_imageHeader);
    int8_t activeImageIdx = 0;
    IMAGE_STATUS_FLAG images_status;
    uint32_t image_code_size;
    bool bad_image_condition;
    image_key_section_t imageKeyInfo;

#if SIGNED_IMAGE_SUPPORT_EXT_MEM || AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE
    struct storage_key_status otp_keys_status;
    key_storage_key_t *keyp;
    uint8_t i;
#endif // SIGNED_IMAGE_SUPPORT_EXT_MEM || AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE

#if SIGNED_IMAGE_SUPPORT_EXT_MEM
    uint8_t ecc_signature[ECC_SIGNATURE_SIZE];
    uint8_t sha256_digest[SHA256_DIGEST_SIZE];
    key_storage_ecc_key_parts_t ecc_keys[2];
#endif // SIGNED_IMAGE_SUPPORT_EXT_MEM

#if AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE
    key_storage_aes_key_parts_t aes_keys[2];
#endif // AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE

    ProductHeader = (s_productHeader *)flashbuffer;

    // read product header
    FlashRead((unsigned long )ProductHeader,
              (unsigned long)PRODUCT_HEADER_POSITION,
              (unsigned long)sizeof(s_productHeader));
    // verify product header
    if (ProductHeader->signature[0] != PRODUCT_HEADER_SIGNATURE1 ||
        ProductHeader->signature[1] != PRODUCT_HEADER_SIGNATURE2)
    {
        return -1;
    }

    // Keep the image offsets
    imageOffsets[0] = ProductHeader->offset1;
    imageOffsets[1] = ProductHeader->offset2;

    // Get the active image index
    activeImageIdx = imageGetInfo(imageOffsets[0], imageOffsets[1], &images_status, &imageKeyInfo);

    // Return immediately if no valid image has been found
    if(images_status == IMAGE_STATUS_FLAG_NONE_VALID) {
        return -1;
    }

    ImageHeader = (s_imageHeader *)flashbuffer;

#if SIGNED_IMAGE_SUPPORT_EXT_MEM || AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE
    storage_key_check(&imageKeyInfo, &otp_keys_status);
#endif // SIGNED_IMAGE_SUPPORT_EXT_MEM || AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE

    // Load the keys from the OTP now, because afterwards we won't be able to
    for (uint8_t i = 0; i < 2; i++) {
#if SIGNED_IMAGE_SUPPORT_EXT_MEM
        if(otp_keys_status.valid_key[i]) {
            memcpy(&ecc_keys[i], key_storage_load_key_in_cache(KEY_TYPE_ECC, imageKeyInfo.keyInfo[i].ecc_key_idx), sizeof(key_storage_ecc_key_parts_t));
        }
#endif // SIGNED_IMAGE_SUPPORT_EXT_MEM

#if AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE
        if(otp_keys_status.valid_key[i]) {
            memcpy(&aes_keys[i], key_storage_load_key_in_cache(KEY_TYPE_AES, imageKeyInfo.keyInfo[i].aes_key_idx), sizeof(key_storage_aes_key_parts_t));
        }
#endif // AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE
    }

    while (1) {

        do {
            // Assume the currently checked image is bad
            bad_image_condition = true;

            // Get the image header of the current active-marked image
            imageHeaderRead(imageOffsets[activeImageIdx], ImageHeader);

#if SIGNED_IMAGE_SUPPORT_EXT_MEM || AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE
            // Check the OTP keys's status for this image
            if(!otp_keys_status.valid_key[activeImageIdx]) {
                break;
            }
#endif // SIGNED_IMAGE_SUPPORT_EXT_MEM || AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE

#if SIGNED_IMAGE_SUPPORT_EXT_MEM
            image_code_size = ImageHeader->code_size - ECC_SIGNATURE_SIZE;
            
            // Point to the current ecc key record
            keyp = (key_storage_key_t *)&ecc_keys[activeImageIdx];

            // Clear the watchdog here
            SetWord16(WATCHDOG_REG, WATCHDOG_REG_RESET);

            // Get the image header part and code block by block and calculate its sha256
            sha256_blocks_ext_mem((unsigned long) imageOffsets[activeImageIdx] + ECC_HASH_IMG_HEADER_OFFSET, 
                                  (unsigned long) image_header_size + image_code_size - ECC_HASH_IMG_HEADER_OFFSET,
                                  sha256_digest);

            // Read the ecc signature (it is located right after the code)
            FlashRead((unsigned long) ecc_signature,
                      (unsigned long) imageOffsets[activeImageIdx] + image_header_size + image_code_size,
                      (unsigned long) ECC_SIGNATURE_SIZE);

            // Clear the watchdog here
            SetWord16(WATCHDOG_REG, WATCHDOG_REG_RESET);

            // If secure booting is supported, check if the signature of the image header + image is valid
            if(ECC_Verify_Image(sha256_digest, (uint8_t*)keyp->ecc.key, ecc_signature)) {
                // If the image was succesfully verified, we can trust it and check if there were any keys pending for invalidation/revokation and do it now,
                // because afterwards the reading of the OTP will not be possible since the otp driver will be overwritten
                if(imageKeyInfo.keyInfo[activeImageIdx].ecc_rvk_valid_flag == STATUS_VALID_RVK_IDX) {
                    key_storage_otp_revoke(KEY_TYPE_ECC, imageKeyInfo.keyInfo[activeImageIdx].ecc_rvk_idx);
                }
    #if AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE
                if(imageKeyInfo.keyInfo[activeImageIdx].aes_rvk_valid_flag == STATUS_VALID_RVK_IDX) {
                    key_storage_otp_revoke(KEY_TYPE_AES, imageKeyInfo.keyInfo[activeImageIdx].aes_rvk_idx);
                }
    #endif // AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE
            }
            // If the image has not been verified
            else {
                break;
            }
#else
            image_code_size = ImageHeader->code_size;
#endif // SIGNED_IMAGE_SUPPORT_EXT_MEM

            // Read the whole image from the external memory into RAM
            FlashRead(SYSRAM_BASE_ADDRESS,
                      (unsigned long) imageOffsets[activeImageIdx] + image_header_size,
                      (unsigned long) image_code_size);

#if AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE
            // Point to the current AES key record
            keyp = (key_storage_key_t *)&aes_keys[activeImageIdx];

            // Clear the watchdog here
            SetWord16(WATCHDOG_REG, WATCHDOG_REG_RESET);

            // Decrypt the copied image
            Decrypt_Image(image_code_size, keyp->aes.iv, keyp->aes.key, 0);
#endif // AES_SUPPORT_ENCRYPTED_EXT_MEM_IMAGE

#if !SIGNED_IMAGE_SUPPORT_EXT_MEM
            // Check if the copied image has invalid CRC (only check this if secure booting is not enabled)
            // If it does, break to check the other image (if there is any other valid image), and don't waste any more time
            if(ImageHeader->CRC != crc32(0, (uint8_t*)SYSRAM_BASE_ADDRESS, image_code_size)) {
                break;
            }
#endif

            // We reached the end of all sanity checks so the image is good/bootable/safe
            bad_image_condition = false;
        } while(0);

        // If bad/non-bootable/unsafe image flag is true
        if (bad_image_condition)
        {
#if INVALIDATE_BAD_IMAGES
            // Before moving on, invalidate this bad/non-bootable/unsafe image's 'validflag' field
            FlashInvalidateImageHeader(imageOffsets[activeImageIdx] + ((unsigned long)&ImageHeader->validflag - (unsigned long)ImageHeader));
#else
            #warning 'Invalidating bad images is disabled. Consider enabling it for a more secure secondary bootloader'
#endif // INVALIDATE_BAD_IMAGES

            // If both of the image slots had a valid image, try to check if the other one is good/bootable/safe
            if(images_status == IMAGE_STATUS_FLAG_BOTH_VALID) {
                images_status = IMAGE_STATUS_FLAG_NONE_VALID; // invalidate the image status flag so that we don't stay in the loop
                activeImageIdx ^= 1;
            }
            // Else return with error status, since NONE of the 2 images seems to be good/bootable/safe
            else  {
                return -1;
            }
        } else {
            break;
        }
    }

    return 0;
}

#endif // defined(SPI_FLASH_SUPPORTED ) || defined(I2C_EEPROM_SUPPORTED)

#ifdef SPI_FLASH_SUPPORTED

/**
****************************************************************************************
* @brief Initialize the spi flash
****************************************************************************************
**/
static void spi_flash_peripheral_init(spi_cfg_t *spi_cfg, spi_flash_cfg_t *spi_flash_cfg)
{
#if (USE_SPI_BOOT_SPECIFIC_PORT_MAPPING)
    extern sb_conf_t sb_conf;
    GPIO_ConfigurePin(sb_conf.spi_en_port, sb_conf.spi_en_pin, OUTPUT, PID_SPI_EN, true);
    GPIO_ConfigurePin(sb_conf.spi_clk_port, sb_conf.spi_clk_pin, OUTPUT, PID_SPI_CLK, false);
    GPIO_ConfigurePin(sb_conf.spi_mosi_port, sb_conf.spi_mosi_pin, OUTPUT, PID_SPI_DO, false);
    GPIO_ConfigurePin(sb_conf.spi_miso_port, sb_conf.spi_miso_pin, OUTPUT, PID_SPI_DI, false);
#else
    GPIO_ConfigurePin(SPI_EN_PORT, SPI_EN_PIN, OUTPUT, PID_SPI_EN, true);
    GPIO_ConfigurePin(SPI_CLK_PORT, SPI_CLK_PIN, OUTPUT, PID_SPI_CLK, false);
    GPIO_ConfigurePin(SPI_DO_PORT, SPI_DO_PIN, OUTPUT, PID_SPI_DO, false);
    GPIO_ConfigurePin(SPI_DI_PORT, SPI_DI_PIN, INPUT, PID_SPI_DI, false);
#endif

    spi_cfg->spi_ms = SPI_MS_MODE_MASTER;
#if (USE_SPI_SET_DEFAULT_BOOTER_SPI_SETTINGS)
    spi_cfg->spi_cp = SPI_CP_MODE_3;
#else
    spi_cfg->spi_cp = SPI_CP_MODE_0;
#endif
    spi_cfg->spi_speed = SPI_SPEED_MODE_8MHz;
    spi_cfg->spi_wsz = SPI_MODE_8BIT;
    spi_cfg->spi_cs = SPI_CS_0;
    spi_cfg->cs_pad.port = SPI_EN_PORT;
    spi_cfg->cs_pad.pin = SPI_EN_PIN;
#if defined (__DA14531__)
    spi_cfg->spi_capture = spi_cfg->spi_speed < SPI_SPEED_MODE_8MHz ?
                            SPI_MASTER_EDGE_CAPTURE : SPI_MASTER_EDGE_CAPTURE_NEXT;
#endif
#if defined (CFG_SPI_DMA_SUPPORT)
    spi_cfg->send_cb = NULL;
    spi_cfg->receive_cb = NULL;
    spi_cfg->transfer_cb = NULL;
    spi_cfg->spi_dma_channel = SPI_DMA_CHANNEL_01;
    spi_cfg->spi_dma_priority = DMA_PRIO_0;
#endif

    spi_flash_cfg->chip_size = SPI_FLASH_DEV_SIZE;

    spi_flash_enable(spi_cfg, spi_flash_cfg);
}

/**
****************************************************************************************
* @brief  Initialize the flash size
****************************************************************************************
*/
static void spi_flash_size_init(spi_cfg_t *spi_cfg, spi_flash_cfg_t *spi_flash_cfg)
{
    uint8_t dev_id;
    int8_t status;

    status = spi_flash_enable_with_autodetect(spi_cfg, &dev_id);
    if (status == SPI_FLASH_ERR_NOT_DETECTED)
    {
        // The device was not identified. The default parameters are used.
        // Alternatively, an error can be asserted here.
        spi_flash_configure_env(spi_flash_cfg);
    }
}


/**
****************************************************************************************
* @brief Load the active from a SPI flash
* @return Success (0) or Error Code.
*
****************************************************************************************
**/
int spi_loadActiveImage(void)
{
    spi_cfg_t spi_cfg;
    spi_flash_cfg_t spi_flash_cfg;

    // Initiate the SPI interface
    spi_flash_peripheral_init(&spi_cfg, &spi_flash_cfg);

    spi_flash_size_init(&spi_cfg, &spi_flash_cfg);

#ifdef SUPPORT_AN_B_001
    s_bootHeader58x_spi header;
    uint32_t actual_size;
    uint32_t image_size;

    // check for boot header
    spi_flash_read_data_buffer((uint8_t*)&header, 0, sizeof(header), &actual_size);
    if (header.preamble[0] == IMAGE_BOOT_SIGNATURE1 && header.preamble[1] == IMAGE_BOOT_SIGNATURE2)
    {  // it must be disabled if the bootloader runs from the SPI flash
        // Supports also the extended len field of platform 585
        image_size = header.len[0] << 16 | header.len[1] << 8 | header.len[2];
    
        // Read the image from the flash
        spi_flash_read_data_buffer((uint8_t*)SYSRAM_BASE_ADDRESS, AN001_SPI_STARTCODE_POSITION,
                     image_size, &actual_size);
        return 0;
    }
    else
    {
  #if defined(ALLOW_NO_HEADER)
        // Load MAX_CODE_LENGTH_SPI bytes from memory offset 0.
        spi_flash_read_data_buffer((uint8_t*)SYSRAM_BASE_ADDRESS, 0, MAX_CODE_LENGTH_SPI, &actual_size);
        return 0;
  #endif // defined(ALLOW_NO_HEADER)
    }

  #if !defined (ALLOW_NO_HEADER)
    // No AN-B-001 valid image has been found in SPI flash. Return error.
    return -1;
  #endif
#else

#if defined (CFG_SPI_DMA_SUPPORT)
    NVIC_DisableIRQ(DMA_IRQn);
#endif

    return loadActiveImage();
#endif
}
#endif // SPI_FLASH_SUPPORTED


#ifdef I2C_EEPROM_SUPPORTED

/**
****************************************************************************************
* @brief set gpio port function mode for EEPROM interface
*
****************************************************************************************
**/
static void set_pad_eeprom(void)
{
    GPIO_ConfigurePin(I2C_SCL_PORT, I2C_SCL_PIN, INPUT, PID_I2C_SCL, false);
    GPIO_ConfigurePin(I2C_SDA_PORT, I2C_SDA_PIN, INPUT, PID_I2C_SDA, false);
}

/**
****************************************************************************************
* @brief Load the active from an EEPROM flash
* @return Success (0) or Error Code.
*
****************************************************************************************
**/
int eeprom_loadActiveImage(void)
{
    // Set pad functionality
    set_pad_eeprom();

    // Configure I2C EEPROM environment
    i2c_eeprom_configure(&i2c_cfg, &i2c_eeprom_cfg);

    // Initialize I2C
    i2c_eeprom_initialize();

#ifdef SUPPORT_AN_B_001
    int i;
    uint8_t AN001Header[AN001_EEPROM_HEADER_SIZE];
    char *code;
    unsigned long current;
    char crc_code;
    char header_crc_code;
    unsigned long  codesize;
    uint32_t bytes_read;

    // check for AN-B-001 header
    i2c_eeprom_read_data((uint8_t *)&AN001Header, 0, AN001_EEPROM_HEADER_SIZE, &bytes_read);
    if (AN001Header[0] == IMAGE_BOOT_SIGNATURE1 && AN001Header[1] == IMAGE_BOOT_SIGNATURE2)
    {
        //it must be disabled if the bootloader runs from the SPI flash
        codesize = ((s_bootHeader580_i2c*) AN001Header)->len[0] << 8 |
                    ((s_bootHeader580_i2c*) AN001Header)->len[1];
        header_crc_code = ((s_bootHeader580_i2c*) AN001Header)->crc;

        // On 585 header these old length bytes are reserved - check that
        if (codesize == 0)
        {
            codesize = ((s_bootHeader585_i2c*) AN001Header)->len[0] << 16 |
                        ((s_bootHeader585_i2c*) AN001Header)->len[1] << 8 |
                        ((s_bootHeader585_i2c*) AN001Header)->len[2];
            header_crc_code = ((s_bootHeader585_i2c*) AN001Header)->crc;
        }
  
        // Transfer the image into RAM
        i2c_eeprom_read_data((uint8_t*)SYSRAM_BASE_ADDRESS,
                            (unsigned long)AN001_EEPROM_STARTCODE_POSITION,
                            (unsigned long)codesize, &bytes_read);

        crc_code = 0;
        current = 0;

        code = (char*)SYSRAM_BASE_ADDRESS;
        do
        {
            current += 32;               // set to next page (32bytes per page)
            for (i = 0; i < 32; i++)
                    crc_code ^= code[i]; // update CRC
            code += 32;
        } while (current < codesize);    // check if read all code

        if (crc_code == header_crc_code)
            return 0;
    }
    else
    {
  #if defined(ALLOW_NO_HEADER)
        // Load MAX_CODE_LENGTH_I2C bytes from memory offset 0.
        i2c_eeprom_read_data((uint8_t*)SYSRAM_BASE_ADDRESS, 0, MAX_CODE_LENGTH_I2C, &bytes_read);
        return 0;
  #endif // defined(ALLOW_NO_HEADER)
    }

    // No AN-B-001 valid image has been found in I2C EEPROM. Return error.
    return -1;
#else
    return loadActiveImage();
#endif // SUPPORT_AN_B_001
}

#endif // I2C_EEPROM_SUPPORTED


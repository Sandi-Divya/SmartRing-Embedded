/**
 ****************************************************************************************
 *
 * @file image.h
 *
 * @brief Definition of image header structure.
 *
 * Copyright (C) 2014-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef __IMAGE_H
#define __IMAGE_H

#include <stdint.h>

/* single image image header key information */
struct image_header_key_info {
    uint8_t ecc_key_idx;
    uint8_t ecc_rvk_valid_flag;
    uint8_t ecc_rvk_idx;
    uint8_t aes_key_idx;
    uint8_t aes_rvk_valid_flag;
    uint8_t aes_rvk_idx;
};

#define KEY_INFO_RVK_KEY_VALID          (0x55)
#define MAX_KEYS_NUM                    (0x05)

/* header for single image */
struct image_header {
	uint8_t signature[2];
	uint8_t valid_flag;
	uint8_t image_id;
	uint8_t code_size[4];
	uint8_t CRC[4] ;
	uint8_t version[16];
	uint8_t timestamp[4];
	uint8_t flags;
/* ---------------------------- */
	uint8_t encryption_pad;
	struct image_header_key_info key_info;
/* ---------------------------- */
	uint8_t reserved[24];
};

/* single image header flags */
#define IMG_ENCRYPTED		0x01
#define IMG_SIGNED		0x02

/* AN-B-001 header for SPI */
struct an_b_001_spi_header {
	uint8_t preamble[2];
	uint8_t empty[4];
	uint8_t len[2];
};

/* AN-B-001 header for I2C/EEPROM */
struct an_b_001_i2c_header {
	uint8_t preamble[2];
	uint8_t len[2];
	uint8_t crc;
	uint8_t dummy[27];
};

/* product header */
struct product_header {
	uint8_t signature[2];
	uint8_t version[2];
	uint8_t offset1[4];
	uint8_t offset2[4];
	uint8_t bd_address[6];
	uint8_t pad[2];
	uint8_t cfg_offset[4];
};

enum multi_options {
		SPI = 1,
		EEPROM = 2,
		BOOTLOADER = 4,
		CONFIG = 8
};


#endif  /* __IMAGE_H */

/**
 ****************************************************************************************
 *
 * @file key_storage_defs.h
 *
 * @brief Key Storage definitions header file
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

#ifndef KEY_STORAGE_DEFS_H_
#define KEY_STORAGE_DEFS_H_

#include "hw_otpc.h"

#if defined (__DA14531__)
#include "otp_cs.h"
#include "otp_hdr.h"
#endif // defined (__DA14531__)

/*
 * The total AES key size is 16+16 bytes (16 bytes for the IV and 16 bytes for the key)
 */
#define AES_USED_IV_SIZE                                    (16)
#define AES_USED_KEY_SIZE                                   (16)

/*
 * The total ECC public key size is 64 bytes
 */
#define ECC_USED_KEY_SIZE                                   (64)

/*
 * The number of ECC keys
 */
#define KEY_STORAGE_ECC_KEYS_NUM                            (5)

/*
 * The number of AES key pairs(AES IV and AES key)
 */
#define KEY_STORAGE_AES_KEYS_NUM                            (5)

/*
 * Key Storage key revokation size based on the platform
 *  - DA14531: The minimum OTP write size is 4 bytes
 *  - DA14585: The minimum OTP write size is 8 bytes
 */
#define KEY_STORAGE_OTP_RVK_FLAG_SIZE                       (HW_OTP_CELL_SIZE)

/*
 * The total AES key size is 16+16 bytes (16 bytes for the IV and 16 bytes for the key)
 */
#define AES_STORAGE_KEY_SIZE                                (AES_USED_IV_SIZE + AES_USED_KEY_SIZE)

/*
 * The total ECC public key size is 64 bytes
 */
#define ECC_STORAGE_KEY_SIZE                                (ECC_USED_KEY_SIZE)

/*
 * Key Storage total AES key and AES key revokation flags size calculation
 */
#define KEY_STORAGE_OTP_AES_KEY_AREA_SIZE                   (KEY_STORAGE_AES_KEYS_NUM * AES_STORAGE_KEY_SIZE)
#define KEY_STORAGE_OTP_AES_RVK_FLAG_AREA_SIZE              (KEY_STORAGE_AES_KEYS_NUM * KEY_STORAGE_OTP_RVK_FLAG_SIZE)

/*
 * Key Storage total ECC key and ECC key revokation flags size calculation
 */
#define KEY_STORAGE_OTP_ECC_KEY_AREA_SIZE                   (KEY_STORAGE_ECC_KEYS_NUM * ECC_STORAGE_KEY_SIZE)
#define KEY_STORAGE_OTP_ECC_RVK_FLAG_AREA_SIZE              (KEY_STORAGE_ECC_KEYS_NUM * KEY_STORAGE_OTP_RVK_FLAG_SIZE)

/*
 * Key Storage total AES area size calculation
 */
#define KEY_STORAGE_AES_TOTAL_AREA_SIZE                     (KEY_STORAGE_OTP_AES_KEY_AREA_SIZE + KEY_STORAGE_OTP_AES_RVK_FLAG_AREA_SIZE)

/*
 * Key Storage total ECC area size calculation
 */
#define KEY_STORAGE_ECC_TOTAL_AREA_SIZE                     (KEY_STORAGE_OTP_ECC_KEY_AREA_SIZE + KEY_STORAGE_OTP_ECC_RVK_FLAG_AREA_SIZE)

/*
 * Key Storage total size calculation
 */
#define KEY_STORAGE_TOTAL_SIZE                              (KEY_STORAGE_ECC_TOTAL_AREA_SIZE + KEY_STORAGE_AES_TOTAL_AREA_SIZE)

/*
* The storage areas are above the OTP CS area for DA1453x ICs and above the OTP header area for DA1458x ICs
*   +------------------------------+----------------------+-------------------+---------+
*   | ############################ | DA14531 / DA14531-01 | DA14585 / DA14586 | DA14535 |
*   +------------------------------+----------------------+-------------------+---------+
*   | AES Keys and IV storage area | 7F87CC8              | 7F8FBD0           | 7F82CC8 |
*   | ECC keys storage area        | 7F87D68              | 7F8FC70           | 7F82D68 |
*   | AES Revokation area          | 7F87EA8              | 7F8FDB0           | 7F82EA8 |
*   | ECC Revokation area          | 7F87EBC              | 7F8FDD8           | 7F82EBC |
*   +------------------------------+----------------------+-------------------+---------+
*
*
* The AES Keys and IV storage area consists of 5 Key and IV pair entries which are assumed to be placed in the following addresses:
*   +------------------------+----------------------+-------------------+---------+
*   | ###################### | DA14531 / DA14531-01 | DA14585 / DA14586 | DA14535 |
*   +------------------------+----------------------+-------------------+---------+
*   | AES Key and IV Entry 0 | 7F87CC8              | 7F8FBD0           | 7F82CC8 |
*   | AES Key and IV Entry 1 | 7F87CE8              | 7F8FBF0           | 7F82CE8 |
*   | AES Key and IV Entry 2 | 7F87D08              | 7F8FC10           | 7F82D08 |
*   | AES Key and IV Entry 3 | 7F87D28              | 7F8FC30           | 7F82D28 |
*   | AES Key and IV Entry 4 | 7F87D48              | 7F8FC50           | 7F82D48 |
*   +------------------------+----------------------+-------------------+---------+
*
* Each entry consists of a 128-bit AES key and an 128-bit AES IV:
*   +----------------------+-------------------+--------------------+
*   | AES Key and IV Entry | AES Key (16 bytes) | AES IV (16 bytes) |
*   +----------------------+-------------------+--------------------+
*
*
* The ECC Keys storage area consists of 5 64-byte ECC keys:
*   +-----------+----------------------+-------------------+---------+
*   | ######### | DA14531 / DA14531-01 | DA14585 / DA14586 | DA14535 |
*   +-----------+----------------------+-------------------+---------+
*   | ECC key 0 | 7F87D68              | 7F8FC70           | 7F82D68 |
*   | ECC key 1 | 7F87DA8              | 7F8FCB0           | 7F82DA8 |
*   | ECC key 2 | 7F87DE8              | 7F8FCF0           | 7F82DE8 |
*   | ECC key 3 | 7F87E28              | 7F8FD30           | 7F82E28 |
*   | ECC key 4 | 7F87E68              | 7F8FD70           | 7F82E68 |
*   +-----------+----------------------+-------------------+---------+
*
* Each entry consists of 256-bit Ux,Uy ECC keys (64 bytes in total):
*   +---------------+---------------------------+
*   | ECC key entry | ECC Public Key (64 bytes) |
*   +---------------+---------------------------+
*
*
* The AES Revokation area consists of 5 revokation flags that will be written from the bootloader
* if a downloaded signed and verified image requests to revoke a certain AES key set (IV and Key)
* A revoked AES key will not be used for decryption in future decryption procedures
*   +-----------------------+----------------------+-------------------+---------+
*   | ##################### | DA14531 / DA14531-01 | DA14585 / DA14586 | DA14535 |
*   +-----------------------+----------------------+-------------------+---------+
*   | AES Revokation Flag 0 | 7F87EA8              | 7F8FDB0           | 7F82EA8 |
*   | AES Revokation Flag 1 | 7F87EAC              | 7F8FDB8           | 7F82EAC |
*   | AES Revokation Flag 2 | 7F87EB0              | 7F8FDC0           | 7F82EB0 |
*   | AES Revokation Flag 3 | 7F87EB4              | 7F8FDC8           | 7F82EB4 |
*   | AES Revokation Flag 4 | 7F87EB8              | 7F8FDD0           | 7F82EB8 |
*   +-----------------------+----------------------+-------------------+---------+
*
*
* The ECC Revokation area consists of 5 revokation flags that will be written from the bootloader
* if a downloaded signed and verified image requests to revoke a certain ECC key.
* A revoked ECC key will not be used for verification in future verification procedures
*   +-----------------------+----------------------+-------------------+---------+
*   | ##################### | DA14531 / DA14531-01 | DA14585 / DA14586 | DA14535 |
*   +-----------------------+----------------------+-------------------+---------+
*   | ECC Revokation Flag 0 | 7F87EBC              | 7F8FDD8           | 7F82EBC |
*   | ECC Revokation Flag 1 | 7F87EC0              | 7F8FDE0           | 7F82EC0 |
*   | ECC Revokation Flag 2 | 7F87EC4              | 7F8FDE8           | 7F82EC4 |
*   | ECC Revokation Flag 3 | 7F87EC8              | 7F8FDF0           | 7F82EC8 |
*   | ECC Revokation Flag 4 | 7F87ECC              | 7F8FDF8           | 7F82ECC |
*   +-----------------------+----------------------+-------------------+---------+
*/

/*
 * For DA14531, DA14531-01, DA14535, the keys are put right before the OTP CS 
 * For DA14585/DA14586, the keys are put right before the OTP header
 */
#if defined (__DA14531__)
#define KEY_STORAGE_OTP_BASE_ADDR                           (OTP_CS_BASE_ADDR - KEY_STORAGE_TOTAL_SIZE)
#else
#define KEY_STORAGE_OTP_BASE_ADDR                           (OTP_HDR_BASE_ADDR - KEY_STORAGE_TOTAL_SIZE)
#endif

/*
 * Base OTP address calculation for the AES keys (Key and IV pairs)
 */
#define KEY_STORAGE_OTP_AES_KEY_BASE_ADDRESS                (KEY_STORAGE_OTP_BASE_ADDR)

/*
 * Base OTP address calculation for the ECC keys
 */
#define KEY_STORAGE_OTP_ECC_KEY_BASE_ADDRESS                (KEY_STORAGE_OTP_AES_KEY_BASE_ADDRESS + KEY_STORAGE_OTP_AES_KEY_AREA_SIZE)

/*
 * Base OTP address calculation for the AES key pair revokation flags
 */
#define KEY_STORAGE_OTP_AES_KEY_RVK_FLAG_BASE_ADDRESS       (KEY_STORAGE_OTP_ECC_KEY_BASE_ADDRESS + KEY_STORAGE_OTP_ECC_KEY_AREA_SIZE)

/*
 * Base OTP address calculation for the ECC keys revokation flags
 */
#define KEY_STORAGE_OTP_ECC_KEY_RVK_FLAG_BASE_ADDRESS       (KEY_STORAGE_OTP_AES_KEY_RVK_FLAG_BASE_ADDRESS + KEY_STORAGE_OTP_AES_RVK_FLAG_AREA_SIZE)

#endif // KEY_STORAGE_DEFS_H_
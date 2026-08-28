/**
 ****************************************************************************************
 *
 * @file uart_booter.c
 *
 * @brief Firmware download through UART
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
#include "uart_booter.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "uart.h"
#include "systick.h"
#include "bootloader.h"
#if AES_ENCRYPTED_IMAGE_SUPPORTED
#include "decrypt.h"
#endif // AES_ENCRYPTED_IMAGE_SUPPORTED
#if SECURE_BOOT_ECC_IMAGE_SUPPORT
#include "ecc_verify.h"
#include "sha2.h"
#endif // SECURE_BOOT_ECC_IMAGE_SUPPORT

#if SECURE_BOOT_ECC_IMAGE_SUPPORT || AES_ENCRYPTED_IMAGE_SUPPORTED
#include "key_storage.h"
#endif

#if defined (UART_SUPPORTED) || defined (ONE_WIRE_UART_SUPPORTED)

#if AES_ENCRYPTED_IMAGE_SUPPORTED
// By default UART encrypted image support is enabled when AES_ENCRYPTED_IMAGE_SUPPORTED is enabled
#define AES_SUPPORT_ENCRYPTED_UART_IMAGE     (1)
#else
#define AES_SUPPORT_ENCRYPTED_UART_IMAGE     (0)
#endif // AES_ENCRYPTED_IMAGE_SUPPORTED

#if SECURE_BOOT_ECC_IMAGE_SUPPORT
// By default UART signed image support is enabled when SECURE_BOOT_ECC_IMAGE_SUPPORT is enabled
#define SIGNED_IMAGE_SUPPORT_UART            (1)
#else
#define SIGNED_IMAGE_SUPPORT_UART            (0)
#endif // SECURE_BOOT_ECC_IMAGE_SUPPORT


extern uint32_t crc32(uint32_t crc, const void *buf, size_t size);

static volatile bool timeout;
/**
 ****************************************************************************************
 * @brief SysTick timer callback
 ****************************************************************************************
 */
static void systick_callback(void)
{
    timeout = true;
}

static volatile bool received = false;
/**
 ****************************************************************************************
 * @brief callback UART receiver
 ****************************************************************************************
 */
void uart_receive_callback(void)
{
    received = true;
}

/**
 ****************************************************************************************
 * @brief UART error callback
 ****************************************************************************************
 */
void uart_error_callback(uart_t *uart, uint8_t uart_err_status)
{
}

/**
****************************************************************************************
 * @brief Receive byte from UART
 *
 * @param[in] ch    Pointer to the variable that will store the received byte
 *
 * @return 1 on success, 0 on failure.
****************************************************************************************
*/
static int uart_receive_byte(uint8_t *recv_byte)
{
    received = false;
    timeout = false;

#ifdef ONE_WIRE_UART_SUPPORTED
    GPIO_ConfigurePin(UART_ONE_WIRE_TX_RX_PORT, UART_ONE_WIRE_TX_RX_PIN, INPUT, PID_UART1_RX, false);
#endif
    
    systick_register_callback(systick_callback);
    // Timeout set to 60ms to cover all UART baud rates
    // and support up to 60ms RX waiting periods.
    systick_start(60000, true);
    uart_receive(UART1, recv_byte, sizeof(uint8_t), UART_OP_INTR);
    while (!received && !timeout);
    systick_stop();

    if (timeout && !received) {
        return 0;
    }

    return 1;
}

/**
****************************************************************************************
 * @brief Transmit byte to UART
 *
 * @param[in] tx_byte    The byte to send
****************************************************************************************
*/
static void uart_transmit_byte(uint8_t tx_byte)
{
#ifdef ONE_WIRE_UART_SUPPORTED
    // Configure the one-wire UART pin to TX
    GPIO_ConfigurePin(UART_ONE_WIRE_TX_RX_PORT, UART_ONE_WIRE_TX_RX_PIN, OUTPUT, PID_UART1_TX, false);
#endif

    // Write the byte
    uart_write_byte(UART1, tx_byte);

#ifdef ONE_WIRE_UART_SUPPORTED
    // Wait for the UART TX to end
    uart_wait_tx_finish(UART1);
#endif
}

#if AES_SUPPORT_ENCRYPTED_UART_IMAGE || SIGNED_IMAGE_SUPPORT_UART
/**
****************************************************************************************
* @brief Parse the image header and determine if it is valid
* @param[in] pImgHeader: A pointer to the image header to be parsed
* @param[out] raw_img_size: The raw image size as extracted from the image header
* @return Will return 1 if the image header parsing was succesfull:
            - Image header signature is valid and
            - Image header key indexes are valid and not revoked
          Will return 0 otherwise
****************************************************************************************
*/
static uint8_t uartImageHeaderParse(s_imageHeader *pImgHeader, uint32_t *raw_img_size)
{
    uint32_t headerImgSize;
    
#if SIGNED_IMAGE_SUPPORT_UART
    headerImgSize = pImgHeader->code_size - ECC_SIGNATURE_SIZE;
#else
    headerImgSize = pImgHeader->code_size;
#endif
    
    if(pImgHeader->signature[0] == IMAGE_HEADER_SIGNATURE1 &&
        pImgHeader->signature[1] == IMAGE_HEADER_SIGNATURE2)
    {
        // If the calculated image size does not agree with the image header code size field, return with error
        if(raw_img_size) {
            *raw_img_size = headerImgSize;
        }
#if AES_SUPPORT_ENCRYPTED_UART_IMAGE
        if(!key_storage_is_key_index_valid(KEY_TYPE_AES, pImgHeader->keyInfo.aes_key_idx)) {
            return 0;
        }

        if(key_storage_otp_is_key_revoked(KEY_TYPE_AES, pImgHeader->keyInfo.aes_key_idx)) {
            return 0;
        }
#endif // AES_SUPPORT_ENCRYPTED_UART_IMAGE

#if SIGNED_IMAGE_SUPPORT_UART
        if(!key_storage_is_key_index_valid(KEY_TYPE_ECC, pImgHeader->keyInfo.ecc_key_idx)) {
            return 0;
        }

        if(key_storage_otp_is_key_revoked(KEY_TYPE_ECC, pImgHeader->keyInfo.ecc_key_idx)) {
            return 0;
        }
#endif // SIGNED_IMAGE_SUPPORT_UART
        return 1;
    }

    return 0;
}
#endif // AES_SUPPORT_ENCRYPTED_UART_IMAGE || SIGNED_IMAGE_SUPPORT_UART

/**
****************************************************************************************
* @brief download firmware through UART (1-wire or 2-wire)
****************************************************************************************
*/
int FwDownload(void)
{
    int fw_size;
    int total_bin_size;
    int i;
    int temp_i;
    char *code;
    char crc_code;
    uint8_t recv_byte;
    uint32_t actual_code_size;
#if AES_SUPPORT_ENCRYPTED_UART_IMAGE || SIGNED_IMAGE_SUPPORT_UART
    uint32_t raw_img_size;
    uint8_t imgHeaderBuff[sizeof(s_imageHeader)];
    s_imageHeader *pImgHeader = (s_imageHeader *)imgHeaderBuff;
#endif // AES_SUPPORT_ENCRYPTED_UART_IMAGE || SIGNED_IMAGE_SUPPORT_UART

#if SIGNED_IMAGE_SUPPORT_UART
    uint8_t sig_i = 0;
    uint8_t sigbuf[ECC_SIGNATURE_SIZE];
    uint8_t sha256_digest[SHA256_DIGEST_SIZE];
    key_storage_ecc_key_parts_t ecc_pub_key;
    uint32_t signature_start_offset;
#endif

#if AES_SUPPORT_ENCRYPTED_UART_IMAGE
    key_storage_aes_key_parts_t aes_key;
#endif

    uart_transmit_byte(STX);                                // send start TX char

    if (0 == uart_receive_byte(&recv_byte))
    {
        return -1;                                          // receive SOH
    }
    if (recv_byte != SOH)
    {
        return -2;
    }

    if (0 == uart_receive_byte(&recv_byte))
    {
        return -3;                                          // receive FW length LSB
    }
    total_bin_size = recv_byte;

    if (0 == uart_receive_byte(&recv_byte))
    {
        return -4;                                          // receive FW length MSB
    }
    total_bin_size += (recv_byte << 8);

    // Support the extended length protocol
    if (total_bin_size == 0)
    {
        if (0 == uart_receive_byte(&recv_byte))
        {
            return -3;                                      // receive FW length LSB
        }
        total_bin_size = recv_byte;

        if (0 == uart_receive_byte(&recv_byte))
        {
            return -4;                                      // receive FW length MiddleSB
        }
        total_bin_size += (recv_byte << 8);

        if (0 == uart_receive_byte(&recv_byte))
        {
            return -9;                                      // receive FW length MSB
        }
        total_bin_size += (recv_byte << 16);
    }

#if SIGNED_IMAGE_SUPPORT_UART
    fw_size = total_bin_size - ECC_SIGNATURE_SIZE - sizeof(s_imageHeader);
#elif AES_SUPPORT_ENCRYPTED_UART_IMAGE
    fw_size = total_bin_size - sizeof(s_imageHeader);
#else
    fw_size = total_bin_size;
#endif // SIGNED_IMAGE_SUPPORT_UART

    if ((fw_size <= 0) || (fw_size > MAX_CODE_LENGTH))      // check if the program fits into memory
    {
        uart_transmit_byte(NAK);                            // if it does not fit send NACK
        return -5;
    }
    else
    {
        uart_transmit_byte(ACK);                            // if it fits send ACK
    }

    crc_code = 0;
    code = (char*) (SYSRAM_COPY_BASE_ADDRESS);              // set pointer to SYSRAM

    for (i = 0; i < total_bin_size; i++)                    // copy code from UART to RAM
    {
        SetWord16(WATCHDOG_REG, WATCHDOG_REG_RESET);

        if (0 == uart_receive_byte(&recv_byte))
        {
            return -6;                                      // receive code byte
        }

#if SIGNED_IMAGE_SUPPORT_UART || AES_SUPPORT_ENCRYPTED_UART_IMAGE
        // First get the image header
        if(i < sizeof(s_imageHeader)) {
            imgHeaderBuff[i] = recv_byte;
            // If we have read the whole image header
            if((i + 1) == sizeof(s_imageHeader)) {

                // Parse the image header members and determine if the image header is valid
                if(uartImageHeaderParse(pImgHeader, &raw_img_size)) {
#if SIGNED_IMAGE_SUPPORT_UART
                    // Calculate the signature starting offset
                    signature_start_offset = sizeof(s_imageHeader) + raw_img_size;

                    // If the image header parsing is ok, load the ecc key now, since we will not be able to do so afterwards since the otp driver will be overwritten
                    memcpy(&ecc_pub_key, key_storage_load_key_in_cache(KEY_TYPE_ECC, pImgHeader->keyInfo.ecc_key_idx), sizeof(ecc_pub_key));
#endif // SIGNED_IMAGE_SUPPORT_UART

#if AES_SUPPORT_ENCRYPTED_UART_IMAGE
                    // If the image header parsing is ok, load the aes iv and key now, since we will not be able to do so afterwards since the otp driver will be overwritten
                    memcpy(&aes_key, key_storage_load_key_in_cache(KEY_TYPE_AES, pImgHeader->keyInfo.aes_key_idx), sizeof(aes_key));
#endif // AES_SUPPORT_ENCRYPTED_UART_IMAGE

#if SIGNED_IMAGE_SUPPORT_UART
                    // Put also the image header in RAM since we will need part of it for the image verification
                    memcpy(code, imgHeaderBuff, sizeof(imgHeaderBuff));
                    code += sizeof(imgHeaderBuff);
#endif // SIGNED_IMAGE_SUPPORT_UART
                } else {
                    // Return an error if the image header is not valid
                    return -10;
                }
            }
        }
#endif

#if SIGNED_IMAGE_SUPPORT_UART
        // If we reached the signature part, read the signature into a separate buffer
        else if(i >= signature_start_offset && i < signature_start_offset + ECC_SIGNATURE_SIZE) {
            sigbuf[sig_i++] = recv_byte;
        }
#endif // SIGNED_IMAGE_SUPPORT_UART

#if SIGNED_IMAGE_SUPPORT_UART || AES_SUPPORT_ENCRYPTED_UART_IMAGE
        else
#endif
        {
            *code = recv_byte;                              // write to RAM
            code++;                                         // increase RAM pointer
        }

        crc_code ^= recv_byte;                              // update CRC
    }

#if SIGNED_IMAGE_SUPPORT_UART || AES_SUPPORT_ENCRYPTED_UART_IMAGE
    bool uart_ver_dec_ok = false;
    do {
    #if SIGNED_IMAGE_SUPPORT_UART
        SetWord16(WATCHDOG_REG, WATCHDOG_REG_RESET);

        // Calculate the sha256 of the image header part and image code
        sha256((uint8_t*)(SYSRAM_COPY_BASE_ADDRESS + ECC_HASH_IMG_HEADER_OFFSET), raw_img_size + sizeof(s_imageHeader) - ECC_HASH_IMG_HEADER_OFFSET, sha256_digest);

        SetWord16(WATCHDOG_REG, WATCHDOG_REG_RESET);

        // If the image verification fails, break immediately
        if(!ECC_Verify_Image(sha256_digest, (uint8_t*)&ecc_pub_key, sigbuf)) {
            break;
        }

        SetWord16(WATCHDOG_REG, WATCHDOG_REG_RESET);

        // Copy only the part of the image in the COPY_BASE_ADDRESS (we don't need the image header part any more)
        memcpy((void*)SYSRAM_COPY_BASE_ADDRESS, (void*)(SYSRAM_COPY_BASE_ADDRESS + sizeof(s_imageHeader)), fw_size);
    #endif // SIGNED_IMAGE_SUPPORT_UART

    #if AES_SUPPORT_ENCRYPTED_UART_IMAGE
        // Decrypt the image
        Decrypt_Image(raw_img_size, aes_key.iv, aes_key.key, SYSRAM_COPY_BASE_ADDRESS);

      #if !SIGNED_IMAGE_SUPPORT_UART
        // Check if the copied image has invalid CRC (only check this if secure booting is not enabled)
        // If it does, break immediately
        if(pImgHeader->CRC != crc32(0, (uint8_t*)SYSRAM_COPY_BASE_ADDRESS, raw_img_size)) {
            break;
        }
      #endif // SIGNED_IMAGE_SUPPORT_UART
    #endif // AES_SUPPORT_ENCRYPTED_UART_IMAGE

        // Try to put the extra prod test byte that is sent by the tool at the correct place (8-byte aligned end of the image)
        uint8_t prod_test_byte;
        uint8_t byte8_alingment = 0;

        prod_test_byte = *((uint8_t*)(SYSRAM_COPY_BASE_ADDRESS + fw_size - 1));
      #if AES_SUPPORT_ENCRYPTED_UART_IMAGE
        raw_img_size -= pImgHeader->encryption_pad;
      #endif
        byte8_alingment = raw_img_size % 8;
        if (byte8_alingment) {
            raw_img_size = raw_img_size + (8 - byte8_alingment);
        }
        *((uint8_t*)(SYSRAM_COPY_BASE_ADDRESS + raw_img_size)) = prod_test_byte;

        // Re-calculate the fw_size based on the new raw image size value
        fw_size = raw_img_size + 1;
        
        uart_ver_dec_ok = true;
    } while(0);

    // If verification or decryption failed, invalidate the calculated CRC to indicate an error
    if(!uart_ver_dec_ok) 
    {
        crc_code ^= 0xFF;
    }
#endif // SIGNED_IMAGE_SUPPORT_UART || AES_SUPPORT_ENCRYPTED_UART_IMAGE

    uart_transmit_byte(crc_code);                           // send CRC byte

    if (0 == uart_receive_byte(&recv_byte))
    {
        return -7;                                          // receive ACK
    }

    if (recv_byte != ACK)
    {
        return -8;
    }

    return fw_size;
}
#endif // defined (UART_SUPPORTED) || defined (ONE_WIRE_UART_SUPPORTED)

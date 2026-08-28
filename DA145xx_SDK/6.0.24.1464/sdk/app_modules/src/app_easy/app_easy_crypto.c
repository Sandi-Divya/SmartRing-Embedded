/**
 ****************************************************************************************
 *
 * @file app_easy_crypto.c
 *
 * @brief Application crypto Entry Point
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
 * @addtogroup APP_CRYPTO
 * @{
 ****************************************************************************************
 */
 #if defined (CFG_USE_AES) && defined (CFG_AES_DECRYPT)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 #include "user_callback_config.h"
 #include "app_easy_crypto.h"
 #include "aes.h"
 #include "aes_api.h"
 #include "rwip.h"
 
 /*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_easy_aes_crypto_init(void)
{
    aes_init(false, NULL);
}

void app_easy_aes_encrypt_data(uint8_t *key, uint8_t key_len, uint8_t *in, uint8_t *out, uint16_t size)
{
    wdg_reload(WATCHDOG_DEFAULT_PERIOD);
    while(1) {
        rwip_schedule();
        if (aes_operation((uint8_t*)key, key_len, (uint8_t*)in, size, out, size, AES_ENCRYPT, NULL, BLE_SYNC_MASK | BLE_SAFE_MASK) == 0) {
            break;
        }
    }
    wdg_reload(WATCHDOG_DEFAULT_PERIOD);
}

void app_easy_aes_decrypt_data(uint8_t *key, uint8_t key_len, uint8_t *in, uint8_t *out, uint16_t size)
{
    wdg_reload(WATCHDOG_DEFAULT_PERIOD);
    while(1) {
        rwip_schedule();
        if (aes_operation((uint8_t*)key, key_len, (uint8_t*)in, size, out, size, AES_DECRYPT, NULL, BLE_SYNC_MASK | BLE_SAFE_MASK) == 0) {
            break;
        }
    }
    wdg_reload(WATCHDOG_DEFAULT_PERIOD);
}

#endif // SECURE_DATA_STORAGE

/// @} APP_CRYPTO


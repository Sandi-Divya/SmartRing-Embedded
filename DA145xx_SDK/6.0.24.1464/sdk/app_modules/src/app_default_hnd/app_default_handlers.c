/**
 ****************************************************************************************
 *
 * @file app_default_handlers.c
 *
 * @brief Default helper handlers implementing a primitive peripheral.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration
#include "gap.h"
#include "gapc_task.h"
#include "arch.h"
#include "arch_api.h"
#include "co_bt.h"
#include "co_math.h"
#include "app_prf_types.h"
#include "app_prf_perm_types.h"
#include "app.h"
#include "app_callback.h"
#include "app_default_handlers.h"
#include "app_utils.h"
#include "hw_otpc.h"
#include <string.h>

#if (BLE_APP_SEC)
#include "app_easy_security.h"
#include "app_security.h"
#endif // (BLE_APP_SEC)

#include "user_config.h"
#include "user_profiles_config.h"
#include "user_callback_config.h"

#if (BLE_CUSTOM_SERVER)
#include "user_custs_config.h"
#endif

#if (SECURE_DATA_STORAGE)
#include "hash.h"
#endif

#if defined (CFG_PRINTF)
#include "arch_console.h"
#endif

#if (BLE_SUOTA_RECEIVER)
#include "app_suotar.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */

#if (BLE_APP_SEC)
/// Default Passkey that is presented to the user and is entered on the peer device
#define DEFAULT_PASSKEY_TK_VAL          (123456)

/// Default OOB data
#define DEFAULT_SECURITY_OOB_TK_VAL     {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,\
                                         0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}

#endif // (BLE_APP_SEC)

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

extern struct gap_sec_key lirk;
static const static_key_t irk_key_seed =  { {OTP_PARAM_VAL_1, OTP_PARAM_VAL_2, OTP_PARAM_VAL_5, OTP_PARAM_VAL_6}, 16 };

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void default_advertise_operation(void)
{
    if (user_default_hnd_conf.adv_scenario == DEF_ADV_FOREVER)
    {
        app_easy_gap_undirected_advertise_start();
    }
    else if (user_default_hnd_conf.adv_scenario == DEF_ADV_WITH_TIMEOUT)
    {
        app_easy_gap_undirected_advertise_with_timeout_start(user_default_hnd_conf.advertise_period, NULL);
    }
}

void default_app_on_init(void)
{
#if BLE_PROX_REPORTER
    app_proxr_init();
#endif

#if BLE_FINDME_TARGET
    app_findt_init();
#endif

#if BLE_FINDME_LOCATOR
    app_findl_init();
#endif

#if BLE_BATT_SERVER
    app_batt_init();
#endif

#if BLE_DIS_SERVER
    app_dis_init();
#endif

#if BLE_BMS_SERVER
    app_bmss_init();
#endif

#if BLE_BCS_SERVER
    app_bcss_init();
#endif

#if BLE_UDS_SERVER
    app_udss_init();
#endif

#if BLE_CTS_SERVER
    app_ctss_init();
#endif

#if BLE_SUOTA_RECEIVER
    app_suotar_init();
#endif

#if BLE_WSS_SERVER
    app_wsss_init();
#endif

#if BLE_GL_SENSOR
    app_glps_init();
#endif

#if (BLE_LN_SENSOR)
    app_lans_init();
#endif

#if BLE_PAS_SERVER
    app_pasps_init();
#endif

#if BLE_HID_DEVICE
    app_hogpd_init();
#endif

#if BLE_RSC_SENSOR
    app_rscps_init();
#endif

#if BLE_HR_SENSOR
    app_hrps_init();
#endif

#if BLE_TIP_SERVER
    app_tips_init();
#endif

#if BLE_BP_SENSOR
    app_blps_init();
#endif

#if BLE_HT_THERMOM
    app_htpt_init();
#endif 

#if BLE_GCM_SERVER
    app_gcms_init();
#endif

#if BLE_CSC_SENSOR
    app_cscps_init();
#endif

#if BLE_SP_SERVER
    app_scpps_init();
#endif

#if BLE_AN_SERVER
    app_anps_init();
#endif

    // Initialize service access write permissions for all the included profiles
    prf_init_srv_perm();

    // Set sleep mode
    arch_set_sleep_mode(app_default_sleep_mode);
}

void default_app_on_connection(uint8_t conidx, struct gapc_connection_req_ind const *param)
{
    if (app_env[conidx].conidx != GAP_INVALID_CONIDX)
    {
        if (user_default_hnd_conf.adv_scenario == DEF_ADV_WITH_TIMEOUT)
        {
            app_easy_gap_advertise_with_timeout_stop();
        }

        // Enable the created profiles/services
        app_prf_enable(conidx);

        #if (BLE_APP_SEC)
        if (user_default_hnd_conf.security_request_scenario == DEF_SEC_REQ_ON_CONNECT)
        {
            app_easy_security_request(conidx);
        }
        #endif // (BLE_APP_SEC)
    }
    else
    {
       // No connection has been established, restart advertising
       CALLBACK_ARGS_0(user_default_app_operations.default_operation_adv)
    }
}

void default_app_on_disconnect( struct gapc_disconnect_ind const *param ){
    // Restart Advertising
    CALLBACK_ARGS_0(user_default_app_operations.default_operation_adv)
}

void default_app_on_set_dev_config_complete(void)
{
#if !defined (__DA14531_01__) && !defined (__DA14535__)
#if (USER_CFG_ADDRESS_MODE == APP_CFG_CNTL_PRIV_RPA_RAND)
    // Add local device info in RAL. Do not wait for completion event
    struct gap_ral_dev_info dev_info;
    memcpy(&dev_info.addr, &(gapm_env.addr), BD_ADDR_LEN*sizeof(uint8_t));
    dev_info.addr_type = ((GAPM_F_GET(gapm_env.cfg_flags, ADDR_TYPE) == GAPM_CFG_ADDR_PUBLIC) ? ADDR_PUBLIC : ADDR_RAND);
    memset(&dev_info.peer_irk, 0, KEY_LEN * sizeof(uint8_t));
    memcpy(&dev_info.local_irk, lirk.key, KEY_LEN * sizeof(uint8_t));
    app_easy_security_ral_op(APP_ADD_DEV_IN_RAL, &dev_info);
#endif
#if (USER_CFG_ADDRESS_MODE == APP_CFG_CNTL_PRIV_RPA_PUB) || (USER_CFG_ADDRESS_MODE == APP_CFG_CNTL_PRIV_RPA_RAND)
    app_easy_security_ral_sync_with_bdb();
#endif
#endif // does not apply to DA14531-01, DA14535
    // Add the first required service in the database
    if (app_db_init_start_func())
    {
        // No more service to add, start advertising
        CALLBACK_ARGS_0(user_default_app_operations.default_operation_adv)
    }
}

void default_app_on_db_init_complete( void )
{
    CALLBACK_ARGS_0(user_default_app_operations.default_operation_adv)
}

void default_app_on_get_dev_name(struct app_device_name* device_name)
{
    device_name->length = device_info.dev_name.length;
    memcpy(device_name->name, device_info.dev_name.name , device_name->length);
}

void default_app_on_get_dev_appearance(uint16_t* appearance)
{
    *appearance = device_info.appearance;
}

void default_app_on_get_dev_slv_pref_params(struct gap_slv_pref* slv_params)
{
    slv_params->con_intv_min = MS_TO_DOUBLESLOTS(10);
    slv_params->con_intv_max = MS_TO_DOUBLESLOTS(20);
    slv_params->slave_latency = 0;
    slv_params->conn_timeout = MS_TO_TIMERUNITS(1250);
}

void default_app_on_set_dev_info(struct gapc_set_dev_info_req_ind const *param, uint8_t* status)
{
    switch (param->req)
    {
        case GAPC_DEV_NAME:
        {
            device_info.dev_name.length = param->info.name.length;
            memcpy(device_info.dev_name.name, param->info.name.value, device_info.dev_name.length);
            *status = GAP_ERR_NO_ERROR;
        }
        break;
        case GAPC_DEV_APPEARANCE:
        {
            device_info.appearance = param->info.appearance;
            *status = GAP_ERR_NO_ERROR;
        }
        break;
        default:
        {
            *status = GAP_ERR_REJECTED;
        }
        break;
    }
}

void default_app_update_params_request(struct gapc_param_update_req_ind const *param, struct gapc_param_update_cfm *cfm)
{
    // by default, the request is being accepted no matter what the param values are
    cfm->accept = true;
    cfm->ce_len_min = MS_TO_DOUBLESLOTS(0);
    cfm->ce_len_max = MS_TO_DOUBLESLOTS(0);
}

void default_app_generate_static_random_addr(struct bd_addr *addr)
{
    // Check if the static random address is already generated.
    // If it is already generated the two MSB are equal to '1'
    if (!(addr->addr[BD_ADDR_LEN - 1] & GAP_STATIC_ADDR))
    {
        // Generate static random address, 48-bits
        co_write32p(&addr->addr[0], co_rand_word());
        co_write16p(&addr->addr[4], co_rand_hword());

        // The two MSB shall be equal to '1'
        addr->addr[BD_ADDR_LEN - 1] |= GAP_STATIC_ADDR;
    }
}

static void create_bd_addr(uint32_t timestamp, uint32_t tester, struct bd_addr *static_addr)
{
    const uint8_t key[] = { 31, 23, 28,  9,  8, 14, 17, 22, 20,  7,
                            24, 29, 30, 39, 27, 21,  6,  1,  4,  0,
                            35, 42, 41, 10, 18, 37, 11, 34, 40, 26,
                            45, 15, 44, 32, 33,  2, 19, 36, 43,  3,
                            13,  5, 25, 38, 16, 12};

    uint64_t tssid_src = ((uint64_t)tester << 32) | (0x00000000FFFFFFFF & (uint64_t)timestamp);
    uint64_t tssid_dest = 0;
    uint64_t mask = 0x1;

    for (uint8_t i = 0; i < sizeof(key); i++)
    {
        tssid_dest |= (((tssid_src >> i) & mask) << key[i]);
    }

    for (uint8_t i = 0; i < sizeof(static_addr->addr); i++)
    {
        static_addr->addr[i] = (uint8_t)((tssid_dest >> (8 * i)) & 0xFF);
    }
    static_addr->addr[5] |= GAP_STATIC_ADDR;  // The two MSB = '1'
}

void default_app_generate_unique_static_random_addr(struct bd_addr *addr)
{
    uint32_t timestamp;
    uint32_t tester;

    // Initialize OTP controller
    hw_otpc_init();

#if defined (__DA14531__)
    hw_otpc_enter_mode(HW_OTPC_MODE_READ);
#else
    hw_otpc_manual_read_on(false);
#endif

    // Read OTP values
#if defined (__DA14531__)
    timestamp = GetWord32(OTP_HDR_TIMESTAMP_ADDR);
    tester = GetWord32(OTP_HDR_TESTER_ADDR);
#else
    timestamp = GetWord32(OTP_TIMESTAMP_ADDR);
    tester = GetWord32(OTP_SITE_ID_ADDR);
#endif

    hw_otpc_disable();

    create_bd_addr(timestamp, tester, addr);
}

void default_app_on_provide_local_irk(uint8_t* key)
{
    app_generate_static_key((static_key_t*)&irk_key_seed, key, KEY_LEN);

#if defined (CFG_PRINTF)
    // Print IRK for debugging (e.g. when using a sniffer)
    arch_printf(" \r\n IRK: 0x");
    for (int count = 0; count < KEY_LEN; count++)
        arch_printf("%02x", key[15 - count]);
#endif
}
#if (BLE_APP_SEC)

/*
 * SECURITY DEFAULT FUNCTIONS
 ****************************************************************************************
 */

void default_app_on_pairing_request(uint8_t conidx, struct gapc_bond_req_ind const *param)
{
    app_easy_security_send_pairing_rsp(conidx, param);
}

void default_app_on_tk_exch(uint8_t conidx, struct gapc_bond_req_ind const *param)
{
    if (param->data.tk_type == GAP_TK_OOB)
    {
        // By default send hardcoded OOB data
        uint8_t oob_tk[KEY_LEN] = DEFAULT_SECURITY_OOB_TK_VAL;
        // Provide TK to the Host
        app_easy_security_tk_exch(conidx, (uint8_t*) oob_tk, KEY_LEN, true);
    }
    else if (param->data.tk_type == GAP_TK_DISPLAY)
    {
        uint32_t passkey = DEFAULT_PASSKEY_TK_VAL;
        // Provide TK to the Host
        app_easy_security_tk_exch(conidx, (uint8_t*) &passkey, sizeof(passkey), true);
    }
    else if (param->data.tk_type == GAP_TK_KEY_ENTRY)
    {
        uint32_t passkey = DEFAULT_PASSKEY_TK_VAL;
        // Provide TK to the Host
        app_easy_security_tk_exch(conidx, (uint8_t*) &passkey, sizeof(passkey), true);
    }
#if (ENABLE_SMP_SECURE)
    else if (param->data.tk_type == GAP_TK_KEY_CONFIRM)
    {
        // Numeric Comparison - Auto-Confirm
        app_easy_security_tk_exch(conidx, (uint8_t *) &param->tk, sizeof(param->tk), true);
    }
#endif // ENABLE_SMP_SECURE
}

void default_app_on_csrk_exch(uint8_t conidx, struct gapc_bond_req_ind const *param)
{
    // Provide the CSRK to the host
    app_easy_security_csrk_exch(conidx);
}

void default_app_on_ltk_exch(uint8_t conidx, struct gapc_bond_req_ind const *param)
{
    // generate ltk and store it to sec_env
    app_sec_gen_ltk(conidx, param->data.key_size);
    //copy the parameters in the message
    app_easy_security_set_ltk_exch_from_sec_env(conidx);
    //send the message
    app_easy_security_ltk_exch(conidx);
}

void default_app_on_encrypt_req_ind(uint8_t conidx,
                                 struct gapc_encrypt_req_ind const *param)
{
    const struct app_sec_bond_data_env_tag *pbd = NULL;

#if (ENABLE_SMP_SECURE)
    uint8_t bdaddr_type;

    // Check if pairing is in progress
    if (app_env[conidx].pairing_in_progress == false)
    {
        // Check if we have secure connection feature enabled
        if (param->ediv == 0)
        {
            // Check if peer's BD address is public or random static
            bdaddr_type = app_get_address_type(app_env[conidx].peer_addr_type, app_env[conidx].peer_addr);
            if ((bdaddr_type == APP_PUBLIC_ADDR_TYPE) || (bdaddr_type == APP_RANDOM_STATIC_ADDR_TYPE) || (bdaddr_type == APP_ID_ADDR_TYPE))
            {
                if (bdaddr_type == APP_ID_ADDR_TYPE)
                {
                    // Search DB by peer's BD address
                    pbd = app_easy_security_bdb_search_entry(SEARCH_BY_ID_TYPE, (void *) app_env[conidx].peer_addr.addr, BD_ADDR_LEN);
                }
                else
                {
                    // Search DB by peer's BD address
                    pbd = app_easy_security_bdb_search_entry(SEARCH_BY_BDA_TYPE, (void *) app_env[conidx].peer_addr.addr, BD_ADDR_LEN);
                }                // If peer has been found in DB
                if(pbd)
                {
                    // Store device bond data to security environment
                    app_sec_env[conidx] = *pbd;
                    // Accept encryption
                    app_easy_security_accept_encryption(conidx);
                }
                // If peer has not been found in DB
                else
                {
                    // Reject encryption, disconnect
                    app_easy_security_reject_encryption(conidx);
                }
            }
            // Check if peer's BD address is Resolvable Private Address
            else if (bdaddr_type == APP_RANDOM_PRIVATE_RESOLV_ADDR_TYPE)
            {
                // Start BD address resolving procedure
                if(!app_easy_security_resolve_bdaddr(conidx))
                {
                    app_easy_security_reject_encryption(conidx);
                }
            }
            // Check if peer's BD address is non-Resolvable Private Address
            else
            {
                // Reject encryption, disconnect
                app_easy_security_reject_encryption(conidx);
            }
        }
        // We have legacy pairing (no secure connection)
        else
#endif // ENABLE_SMP_SECURE
        {
            pbd = app_easy_security_bdb_search_entry(SEARCH_BY_EDIV_TYPE, (void *) &param->ediv, 2);
            // If peer has been found in DB
            if(pbd)
            {
                // Store device bond data to security environment
                app_sec_env[conidx] = *pbd;
                // Accept encryption
                app_easy_security_accept_encryption(conidx);
            }
            // If peer has not been found in DB
            else
            {
                // Reject encryption, disconnect
                app_easy_security_reject_encryption(conidx);
            }
        }
#if (ENABLE_SMP_SECURE)
    }
    // Devices have not been bonded yet. Pairing phase 3
    else
    {
        uint8_t zeros[RAND_NB_LEN] = {0};
        // Check if we have secure connection feature enabled and
        // LTK has not been created yet
        if ((param->ediv == 0) && ((app_sec_env[conidx].valid_keys & LTK_PRESENT) == 0))
        {
            // Reject encryption, disconnect
            app_easy_security_reject_encryption(conidx);
        }
        else if ((app_sec_env[conidx].valid_keys & LTK_PRESENT) && ((param->ediv != 0) || (memcmp(param->rand_nb.nb, zeros, RAND_NB_LEN))))
        {
            // Reject encryption, disconnect
            app_easy_security_reject_encryption(conidx);
        }
        else
        {
            // Accept encryption to start exchange of keys
            app_easy_security_accept_encryption(conidx);
        }
    }
#endif // ENABLE_SMP_SECURE
}

void default_app_on_pairing_succeeded(uint8_t conidx)
{
    if (app_sec_env[conidx].auth & GAP_AUTH_BOND)
    {
        app_easy_security_bdb_add_entry(&app_sec_env[conidx]);
    }
}

/*
 * PRIVACY DEFAULT FUNCTIONS
 ****************************************************************************************
 */

void default_app_on_addr_solved_ind(uint8_t conidx,
                                    struct gapm_addr_solved_ind const *param)
{
    const struct app_sec_bond_data_env_tag *pbd = NULL;

    // Search DB by peer's IRK
    pbd = app_easy_security_bdb_search_entry(SEARCH_BY_IRK_TYPE, (void *) &param->irk, sizeof(struct gap_sec_key));

    // If peer has been found in DB
    if(pbd)
    {
        // Store device bond data to security environment
        app_sec_env[conidx] = *pbd;
        // Accept encryption
        app_easy_security_accept_encryption(conidx);
    }
    // If peer has not been found in DB
    else
    {
        // Reject encryption, disconnect
        app_easy_security_reject_encryption(conidx);
    }
}

void default_app_on_addr_resolve_failed(uint8_t conidx)
{
    app_easy_security_reject_encryption(conidx);
}

#if !defined (__DA14531_01__) && !defined (__DA14535__)
void default_app_on_ral_cmp_evt(const struct gapm_cmp_evt *param)
{
    switch(param->operation)
    {
        case GAPM_ADD_DEV_IN_RAL:
        {
            // Assert error if RAL is full
            if(param->status == LL_ERR_MEMORY_CAPA_EXCEED)
            {
                ASSERT_ERROR(0);
            }
        }
        break;
        default:
        {
        }
        break;
    }
}
#endif // does not apply to DA14531-01, DA14535

#endif // (BLE_APP_SEC)

#if (BLE_SUOTA_RECEIVER)

///SUOTA default version checking parameters configuration
#ifndef SDK_VERSION_STRING_DELIMITER
#define SDK_VERSION_STRING_DELIMITER      "."
#endif // SDK_VERSION_STRING_DELIMITER

#ifndef SDK_VERSION_STRING_MAX_TOKENS
#define SDK_VERSION_STRING_MAX_TOKENS      4
#endif // SDK_VERSION_STRING_MAX_TOKENS

#ifndef SDK_VERSION_STRING_SIZE
#define SDK_VERSION_STRING_SIZE            (IMAGE_HEADER_VERSION_SIZE)
#endif // SDK_VERSION_STRING_SIZE

typedef struct {
    uint16_t version_nums[SDK_VERSION_STRING_MAX_TOKENS];
} sdk6_version_int_t;

static char * strtok_r_func (char *s, const char *delim, char **save_ptr)
{
    char *end;
    if (s == NULL)
        s = *save_ptr;
    if (*s == '\0')
    {
        *save_ptr = s;
        return NULL;
    }
    /* Scan leading delimiters.  */
    s += strspn (s, delim);
    if (*s == '\0')
    {
        *save_ptr = s;
        return NULL;
    }
    /* Find the end of the token.  */
    end = s + strcspn (s, delim);
    if (*end == '\0')
    {
        *save_ptr = end;
        return s;
    }
    /* Terminate the token and make *SAVE_PTR point past it.  */
    *end = '\0';
    *save_ptr = end + 1;
    return s;
}

/**
 ****************************************************************************************
 * @brief The function parses a string containing the SDK version in ascii format and 
 *        returns an array containing every version number as integer.
 *        The function will also check if the version string has correct format/syntax 
 *        and correct number of integer
 * @param[in]   input The SDK version string to be parsed
 * @param[out]  version A pointer to where the returned integers of the version string
 *              will be returned.
 * @return      Will return 1 if the image was parsed succesfully:
 *              - Has correct syntax (correct number of expected tokens) and
 *              - All the found tokens are numbers
 *              Will return 0 otherwise
 ****************************************************************************************
 */
static uint8_t parse_sdk_version_string(uint8_t *input, sdk6_version_int_t *version)
{
    char input_copy[SDK_VERSION_STRING_SIZE + 1] = {'\0', };
    char *saveptr; // Stores strtok_r state

    memcpy(input_copy, (char*)input, strlen((char*)input));

    // Split string by the delimiter
    char *token = strtok_r_func(input_copy, SDK_VERSION_STRING_DELIMITER, &saveptr);
    int part_count = 0;
    int is_valid = 1;

    // Initialize all output variables to 0
    if(version) {
         memset(version, 0, sizeof(sdk6_version_int_t));
    }

    while (token != NULL) {
        part_count++;

        // Ensure we dont' exceed the SDK_VERSION_STRING_MAX_TOKENS parts (x.y.z.k)
        if(part_count > SDK_VERSION_STRING_MAX_TOKENS) {
            return 0; // Too may tokens, return failure
        }

        // Ensure all parts (v1, v2, v3, ..., vN) are numeric
        for (int i = 0; token[i] != '\0'; i++) {
            if (token[i] < '0' || token[i] > '9') {
                is_valid = 0;
                break;
            }
        }

        if (!is_valid) {
            return 0; // Invalid format: one of the parts is not a number
        }

        // Convert the current token to an integer and assign to the correct variable
        int value = 0;
        for (int i = 0; token[i] != '\0'; i++) {
            value = value * 10 + (token[i] - '0');
        }

        // Store the values in the respective variables
        if(version) {
            version->version_nums[part_count - 1] = value;
        }

        // Get the next token
        token = strtok_r_func(NULL, SDK_VERSION_STRING_DELIMITER, &saveptr);
    }

    // Ensure we have exactly SDK_VERSION_STRING_MAX_TOKENS parts (x.y.z.k)
    if (part_count != SDK_VERSION_STRING_MAX_TOKENS) {
        return 0; // Invalid format
    }

    return 1; // Valid format
}

SUOTAR_IMG_VERSION_CHECK default_suotar_on_version_check(const uint8_t *cur_version, const uint8_t *new_version)
{
    sdk6_version_int_t curr_image_version;
    sdk6_version_int_t new_image_version;

    // Parse the current image version string and get the subversion numbers
    parse_sdk_version_string((uint8_t*)cur_version, &curr_image_version);

    // Parse the incoming candidate image version string, get the subversion numbers and check if the format is valid
    if(!parse_sdk_version_string((uint8_t*)new_version, &new_image_version)) {
         // The incoming image seems to have an invalid version string format so return with error
        return SUOTAR_IMG_VERSION_CHECK_INVALID_FORMAT;
    }

#if SUOTAR_DO_NOT_ALLOW_SAME_IMAGE
    if (!memcmp(&curr_image_version, &new_image_version, sizeof(sdk6_version_int_t))) {
        // Images are the same so return same image error
        return SUOTAR_IMG_VERSION_CHECK_SAME_IMAGE;
    }
#endif

    // Anti-rollback protection checking
    // Check if the first SDK_VERSION_STRING_MAX_TOKENS - 1 numbers of the incoming image are the same or greater
    for ( uint8_t i = 0; i < SDK_VERSION_STRING_MAX_TOKENS; i++) {
        if(curr_image_version.version_nums[i] != new_image_version.version_nums[i]) {
            if(new_image_version.version_nums[i] < curr_image_version.version_nums[i]) {
                // Anti - rollback protection error, incoming image is older than the current one
                return SUOTAR_IMG_VERSION_CHECK_ROLLBACK_ERR;
            } else {
                // Incoming image is greater than the one currently running so return success
                return SUOTAR_IMG_VERSION_CHECK_OK;
            }
        }
    }

    // If all checks are succesfull, return success
    return SUOTAR_IMG_VERSION_CHECK_OK;
}
#endif // (BLE_SUOTA_RECEIVER)

/**
 ****************************************************************************************
 *
 * @file fsp_ext_task.c
 *
 * @brief FSP external processor user application source code.
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

#ifndef _FSP_EXT_TASK_H_
#define _FSP_EXT_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

#ifdef CFG_FSP_EXT_TASK

// Image chip part number strings for all the supported ICs
#define IMAGE_CHIP_PART_NUMBER_DA14531  {'2', '6', '3', '2'}
#define IMAGE_CHIP_PART_NUMBER_DA14535  {'3', '0', '8', '1'}
#define IMAGE_CHIP_PART_NUMBER_DA14585  {'5', '8', '5', 0x00}
#define IMAGE_CHIP_PART_NUMBER_DA14586  {'5', '8', '6', 0x00}

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "app.h"

#define FSP_EXT_IDX_MAX             (1)

/// Possible states of the EXT_HOST_BLE_AUX task
enum
{
    ///Disabled state
    FSP_EXT_STATE_DISABLED,
    
    ///Idle state
    FSP_EXT_STATE_IDLE,

    ///Number of defined states.
    FSP_EXT_STATE_MAX
};

/// Possible statuses of the TASK_FSP_EXT commands
typedef enum {
    FSP_EXT_STATUS_ERROR = -1,
    FSP_EXT_STATUS_SUCCESS = CO_ERROR_NO_ERROR,
    FSP_EXT_STATUS_PARAM_OUT_OF_RANGE = CO_ERROR_PARAM_OUT_OF_MAND_RANGE,
} FSP_EXT_STATUS;

typedef enum {
    FSP_MSG_CUSTOM_DUMMY = KE_FIRST_MSG(TASK_ID_FSP_EXT),
    FSP_MSG_GEN_RAND_REQ,
    FSP_MSG_GEN_RAND_RSP,
    FSP_MSG_GET_FW_VERSION_REQ,
    FSP_MSG_GET_FW_VERSION_RSP,
    FSP_MSG_SET_TX_PWR_REQ,
    FSP_MSG_SET_TX_PWR_RSP,
    FSP_MSG_GET_TX_PWR_REQ,
    FSP_MSG_GET_TX_PWR_RSP,
    
    FSP_MSG_SET_SLEEP_MODE_REQ,
    FSP_MSG_GET_SLEEP_MODE_REQ,
    FSP_MSG_SLEEP_MODE_RSP,
    
    FSP_MSG_SET_BD_ADDR_REQ,
    FSP_MSG_SET_BD_ADDR_RSP,
    
    FSP_MSG_LAST,
} fsp_ext_task_msg_t;

typedef enum {
    FSP_SLEEP_OFF,
    FSP_EXTENDED_SLEEP,
    FSP_DEEP_SLEEP,
    FSP_HIBERNATION
} fsp_sleep_mode_t;

typedef enum {
    FSP_SLEEP_RSP_NO_ERROR,
    FSP_SLEEP_RSP_AIR_OP_ONGOING,
    FSP_SLEEP_RSP_INVALID_PARAM,
} fsp_sleep_rsp_status_t;

/// Parameters of the @ref FSP_MSG_GEN_RAND_REQ message
typedef struct
{
    ///requested random number size
    uint8_t rand_size;
} fsp_ext_gen_rand_req_t;

/// Parameters of the @ref FSP_MSG_GEN_RAND_RSP message
typedef struct
{
    ///random number size
    FSP_EXT_STATUS status;
    uint8_t size;
    uint8_t rand[__ARRAY_EMPTY];
} fsp_ext_gen_rand_rsp_t;

typedef struct {
    uint8_t chip_id[4];
    uint8_t image_chip_id[4];
    uint8_t sdk_major_version;
    uint8_t sdk_minor_version;
    uint8_t sdk_patch_version;
    uint16_t build_number;
    uint8_t sdk_build_extension;
    uint8_t gtl_app_version_number;
    uint16_t glt_app_cfg_flags;
    uint16_t spare_version_info;
} fsp_ext_fw_version_info_t;

typedef struct {
    uint8_t image_tag[8];
    fsp_ext_fw_version_info_t fw_version_info;
} fsp_image_identification_t;

/// Parameters of the @ref FSP_MSG_GET_FW_VERSION_RSP message
typedef struct {
    fsp_ext_fw_version_info_t fw_version_info;
} fsp_ext_get_fw_version_rsp_t;

typedef enum {
    FSP_EXT_RF_SETTING_HIGH,
    FSP_EXT_RF_SETTING_MEDIUM,
    FSP_EXT_RF_SETTING_LOW,
    FSP_EXT_RF_SETTING_NUM,
} FSP_EXT_RF_SETTING;

/// Parameters of the @ref FSP_MSG_SET_TX_PWR_REQ message
typedef struct
{
    int8_t rf_setting; // FSP_EXT_RF_SETTING type value
} fsp_ext_set_tx_pwr_req_t;

/// Parameters of the @ref FSP_MSG_SET_TX_PWR_RSP message
typedef struct gapm_cmp_evt fsp_ext_set_tx_pwr_rsp_t;

/// Parameters of the @ref FSP_MSG_GET_TX_PWR_RSP message
typedef struct {
    int8_t rf_setting; // FSP_EXT_RF_SETTING type value
} fsp_ext_get_tx_pwr_rsp_t;

/// Parameters of the @ref FSP_MSG_SET_SLEEP_MODE_REQ message
typedef struct
{
    fsp_sleep_mode_t        sleep_mode;
#if defined (__DA14531__)
    pd_sys_down_ram_t       ram_123;
#endif
    uint16_t                 wkup_mask;
    uint16_t                 wkup_pol;
} fsp_sleep_mode_req_t;

/// Parameters of the @ref FSP_MSG_SLEEP_MODE_RSP message
typedef struct
{
    fsp_sleep_mode_t        sleep_mode;
    fsp_sleep_rsp_status_t  status;
} fsp_sleep_mode_rsp_t;

/// Parameters of the @ref FSP_MSG_SET_BD_ADDR_REQ message
typedef struct
{
    struct bd_addr          addr;
    uint8_t                 addr_type;
} fsp_set_bd_addr_req_t;

/// Parameters of the @ref FSP_MSG_SET_BD_ADDR_RSP message
typedef struct
{
    struct bd_addr          addr;
    uint8_t                 addr_type;
    FSP_EXT_STATUS          status;
} fsp_set_bd_addr_rsp_t;


typedef void (*fsp_ext_custom_cmd_handler_t)(ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id);
/**
 ****************************************************************************************
 * @brief FSP ext task initialization function
 * @param[in] cb: a callback/hook for user custom commands
 ****************************************************************************************
 */
void fsp_ext_task_init(fsp_ext_custom_cmd_handler_t cb);
#endif // CFG_FSP_EXT_TASK

/// @} APP

#endif // _FSP_EXT_TASK_H_
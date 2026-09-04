#include <stdint.h>
#include "co_utils.h"
#include "prf_types.h"
#include "attm_db_128.h"
#include "user_custs1_def.h"

/*
 * SERVICE 1 UUIDS
 ****************************************************************************************
 */

static const att_svc_desc128_t custs1_svc1 = DEF_SVC1_UUID_128;

static const uint8_t SVC1_CTRL_POINT_UUID_128[ATT_UUID_128_LEN]   = DEF_SVC1_CTRL_POINT_UUID_128;

static const uint8_t SVC1_LED_STATE_UUID_128[ATT_UUID_128_LEN]    = DEF_SVC1_LED_STATE_UUID_128;

static const uint8_t SVC1_ADC_VAL_1_UUID_128[ATT_UUID_128_LEN]    = DEF_SVC1_ADC_VAL_1_UUID_128;

static const uint8_t SVC1_ADC_VAL_2_UUID_128[ATT_UUID_128_LEN]    = DEF_SVC1_ADC_VAL_2_UUID_128;

static const uint8_t SVC1_BUTTON_STATE_UUID_128[ATT_UUID_128_LEN] = DEF_SVC1_BUTTON_STATE_UUID_128;

static const uint8_t SVC1_INDICATEABLE_UUID_128[ATT_UUID_128_LEN] = DEF_SVC1_INDICATEABLE_UUID_128;

static const uint8_t SVC1_LONG_VALUE_UUID_128[ATT_UUID_128_LEN]   = DEF_SVC1_LONG_VALUE_UUID_128;


/*
 * SERVICE 2 UUIDS
 ****************************************************************************************
 */

static const att_svc_desc128_t custs1_svc2 = DEF_SVC2_UUID_128;

static const uint8_t SVC2_WRITE_VAL_1_UUID_128[ATT_UUID_128_LEN] =
    DEF_SVC2_WRITE_VAL_1_UUID_128;

static const uint8_t SVC2_WRITE_VAL_2_UUID_128[ATT_UUID_128_LEN] =
    DEF_SVC2_WRITE_VAL_2_UUID_128;


/*
 * SERVICE 3 UUIDS
 ****************************************************************************************
 */

static const att_svc_desc128_t custs1_svc3 = DEF_SVC3_UUID_128;

static const uint8_t SVC3_READ_VAL_1_UUID_128[ATT_UUID_128_LEN] =
    DEF_SVC3_READ_VAL_1_UUID_128;

static const uint8_t SVC3_READ_VAL_2_UUID_128[ATT_UUID_128_LEN] =
    DEF_SVC3_READ_VAL_2_UUID_128;

static const uint8_t SVC3_READ_VAL_3_UUID_128[ATT_UUID_128_LEN] =
    DEF_SVC3_READ_VAL_3_UUID_128;

static const uint8_t SVC3_READ_VAL_4_UUID_128[ATT_UUID_128_LEN] =
    DEF_SVC3_READ_VAL_4_UUID_128;


/*
 * HEART RATE UUID
 ****************************************************************************************
 */

static const uint8_t SVC3_HR_VAL_UUID_128[ATT_UUID_128_LEN]    = DEF_SVC3_HR_VAL_UUID_128;

/*
 * CLOCK UUID
 ****************************************************************************************
 */

static const uint8_t SVC3_CLOCK_VAL_UUID_128[ATT_UUID_128_LEN] = DEF_SVC3_CLOCK_VAL_UUID_128;


/*
 * ATTRIBUTE SPECIFICATIONS
 ****************************************************************************************
 */

static const uint16_t att_decl_svc       = ATT_DECL_PRIMARY_SERVICE;
static const uint16_t att_decl_char      = ATT_DECL_CHARACTERISTIC;
static const uint16_t att_desc_cfg       = ATT_DESC_CLIENT_CHAR_CFG;
static const uint16_t att_desc_user_desc = ATT_DESC_CHAR_USER_DESCRIPTION;


/*
 * GLOBAL DATABASE INFORMATION
 ****************************************************************************************
 */

const uint8_t custs1_services[] =
{
    SVC1_IDX_SVC,
    SVC2_IDX_SVC,
    SVC3_IDX_SVC,
    CUSTS1_IDX_NB
};

const uint8_t custs1_services_size =
    ARRAY_LEN(custs1_services) - 1;

const uint16_t custs1_att_max_nb =
    CUSTS1_IDX_NB;


/*
 * DATABASE
 ****************************************************************************************
 */

const struct attm_desc_128 custs1_att_db[CUSTS1_IDX_NB] =
{
    /*
     * SERVICE 1
     ****************************************************************************************
     */

    [SVC1_IDX_SVC] =
    {
        (uint8_t*)&att_decl_svc,
        ATT_UUID_128_LEN,
        PERM(RD, ENABLE),
        sizeof(custs1_svc1),
        sizeof(custs1_svc1),
        (uint8_t*)&custs1_svc1
    },

    [SVC1_IDX_CONTROL_POINT_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC1_IDX_CONTROL_POINT_VAL] =
    {
        SVC1_CTRL_POINT_UUID_128,
        ATT_UUID_128_LEN,
        PERM(WR, ENABLE) |
        PERM(WRITE_REQ, ENABLE) |
        PERM(WRITE_COMMAND, ENABLE),
        DEF_SVC1_CTRL_POINT_CHAR_LEN,
        0,
        NULL
    },

    [SVC1_IDX_CONTROL_POINT_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC1_CONTROL_POINT_USER_DESC) - 1,
        sizeof(DEF_SVC1_CONTROL_POINT_USER_DESC) - 1,
        (uint8_t*)DEF_SVC1_CONTROL_POINT_USER_DESC
    },

    [SVC1_IDX_LED_STATE_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC1_IDX_LED_STATE_VAL] =
    {
        SVC1_LED_STATE_UUID_128,
        ATT_UUID_128_LEN,
        PERM(WR, ENABLE) |
        PERM(WRITE_COMMAND, ENABLE) |
        PERM(WRITE_REQ, ENABLE),
        DEF_SVC1_LED_STATE_CHAR_LEN,
        0,
        NULL
    },

    [SVC1_IDX_LED_STATE_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC1_LED_STATE_USER_DESC) - 1,
        sizeof(DEF_SVC1_LED_STATE_USER_DESC) - 1,
        (uint8_t*)DEF_SVC1_LED_STATE_USER_DESC
    },

    /*
     * BATTERY
     * DO NOT CHANGE THIS UUID OR HANDLE.
     */

    [SVC1_IDX_ADC_VAL_1_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC1_IDX_ADC_VAL_1_VAL] =
    {
        SVC1_ADC_VAL_1_UUID_128,
        ATT_UUID_128_LEN,
        PERM(RD, ENABLE) |
        PERM(NTF, ENABLE),
        DEF_SVC1_ADC_VAL_1_CHAR_LEN,
        0,
        NULL
    },

    [SVC1_IDX_ADC_VAL_1_NTF_CFG] =
    {
        (uint8_t*)&att_desc_cfg,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE) |
        PERM(WR, ENABLE) |
        PERM(WRITE_REQ, ENABLE),
        sizeof(uint16_t),
        0,
        NULL
    },

    [SVC1_IDX_ADC_VAL_1_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC1_ADC_VAL_1_USER_DESC) - 1,
        sizeof(DEF_SVC1_ADC_VAL_1_USER_DESC) - 1,
        (uint8_t*)DEF_SVC1_ADC_VAL_1_USER_DESC
    },

    [SVC1_IDX_ADC_VAL_2_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC1_IDX_ADC_VAL_2_VAL] =
    {
        SVC1_ADC_VAL_2_UUID_128,
        ATT_UUID_128_LEN,
        PERM(RD, ENABLE),
        DEF_SVC1_ADC_VAL_2_CHAR_LEN,
        0,
        NULL
    },

    [SVC1_IDX_ADC_VAL_2_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC1_ADC_VAL_2_USER_DESC) - 1,
        sizeof(DEF_SVC1_ADC_VAL_2_USER_DESC) - 1,
        (uint8_t*)DEF_SVC1_ADC_VAL_2_USER_DESC
    },

    [SVC1_IDX_BUTTON_STATE_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC1_IDX_BUTTON_STATE_VAL] =
    {
        SVC1_BUTTON_STATE_UUID_128,
        ATT_UUID_128_LEN,
        PERM(RD, ENABLE) |
        PERM(NTF, ENABLE),
        DEF_SVC1_BUTTON_STATE_CHAR_LEN,
        0,
        NULL
    },

    [SVC1_IDX_BUTTON_STATE_NTF_CFG] =
    {
        (uint8_t*)&att_desc_cfg,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE) |
        PERM(WR, ENABLE) |
        PERM(WRITE_REQ, ENABLE),
        sizeof(uint16_t),
        0,
        NULL
    },

    [SVC1_IDX_BUTTON_STATE_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC1_BUTTON_STATE_USER_DESC) - 1,
        sizeof(DEF_SVC1_BUTTON_STATE_USER_DESC) - 1,
        (uint8_t*)DEF_SVC1_BUTTON_STATE_USER_DESC
    },

    [SVC1_IDX_INDICATEABLE_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC1_IDX_INDICATEABLE_VAL] =
    {
        SVC1_INDICATEABLE_UUID_128,
        ATT_UUID_128_LEN,
        PERM(RD, ENABLE) |
        PERM(IND, ENABLE),
        DEF_SVC1_INDICATEABLE_CHAR_LEN,
        0,
        NULL
    },

    [SVC1_IDX_INDICATEABLE_IND_CFG] =
    {
        (uint8_t*)&att_desc_cfg,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE) |
        PERM(WR, ENABLE) |
        PERM(WRITE_REQ, ENABLE),
        sizeof(uint16_t),
        0,
        NULL
    },

    [SVC1_IDX_INDICATEABLE_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC1_INDICATEABLE_USER_DESC) - 1,
        sizeof(DEF_SVC1_INDICATEABLE_USER_DESC) - 1,
        (uint8_t*)DEF_SVC1_INDICATEABLE_USER_DESC
    },

    [SVC1_IDX_LONG_VALUE_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC1_IDX_LONG_VALUE_VAL] =
    {
        SVC1_LONG_VALUE_UUID_128,
        ATT_UUID_128_LEN,
        PERM(RD, ENABLE) |
        PERM(WR, ENABLE) |
        PERM(NTF, ENABLE) |
        PERM(WRITE_REQ, ENABLE) |
        PERM(WRITE_COMMAND, ENABLE),
        DEF_SVC1_LONG_VALUE_CHAR_LEN,
        0,
        NULL
    },

    [SVC1_IDX_LONG_VALUE_NTF_CFG] =
    {
        (uint8_t*)&att_desc_cfg,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE) |
        PERM(WR, ENABLE) |
        PERM(WRITE_REQ, ENABLE),
        sizeof(uint16_t),
        0,
        NULL
    },

    [SVC1_IDX_LONG_VALUE_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC1_LONG_VALUE_CHAR_USER_DESC) - 1,
        sizeof(DEF_SVC1_LONG_VALUE_CHAR_USER_DESC) - 1,
        (uint8_t*)DEF_SVC1_LONG_VALUE_CHAR_USER_DESC
    },


    /*
     * SERVICE 2
     ****************************************************************************************
     */

    [SVC2_IDX_SVC] =
    {
        (uint8_t*)&att_decl_svc,
        ATT_UUID_128_LEN,
        PERM(RD, ENABLE),
        sizeof(custs1_svc2),
        sizeof(custs1_svc2),
        (uint8_t*)&custs1_svc2
    },

    [SVC2_WRITE_1_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC2_WRITE_1_VAL] =
    {
        SVC2_WRITE_VAL_1_UUID_128,
        ATT_UUID_128_LEN,
        PERM(WR, ENABLE) |
        PERM(WRITE_REQ, ENABLE),
        PERM(RI, ENABLE) | DEF_SVC2_WRITE_VAL_1_CHAR_LEN,
        0,
        NULL
    },

    [SVC2_WRITE_1_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC2_WRITE_VAL_1_USER_DESC) - 1,
        sizeof(DEF_SVC2_WRITE_VAL_1_USER_DESC) - 1,
        (uint8_t*)DEF_SVC2_WRITE_VAL_1_USER_DESC
    },

    [SVC2_WRITE_2_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC2_WRITE_2_VAL] =
    {
        SVC2_WRITE_VAL_2_UUID_128,
        ATT_UUID_128_LEN,
        PERM(WR, ENABLE) |
        PERM(WRITE_COMMAND, ENABLE),
        DEF_SVC2_WRITE_VAL_2_CHAR_LEN,
        0,
        NULL
    },

    [SVC2_WRITE_2_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC2_WRITE_VAL_2_USER_DESC) - 1,
        sizeof(DEF_SVC2_WRITE_VAL_2_USER_DESC) - 1,
        (uint8_t*)DEF_SVC2_WRITE_VAL_2_USER_DESC
    },


    /*
     * SERVICE 3
     ****************************************************************************************
     */

    [SVC3_IDX_SVC] =
    {
        (uint8_t*)&att_decl_svc,
        ATT_UUID_128_LEN,
        PERM(RD, ENABLE),
        sizeof(custs1_svc3),
        sizeof(custs1_svc3),
        (uint8_t*)&custs1_svc3
    },

    [SVC3_IDX_READ_1_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC3_IDX_READ_1_VAL] =
    {
        SVC3_READ_VAL_1_UUID_128,
        ATT_UUID_128_LEN,
        PERM(RD, ENABLE) |
        PERM(NTF, ENABLE),
        DEF_SVC3_READ_VAL_1_CHAR_LEN,
        0,
        NULL
    },

    [SVC3_IDX_READ_1_NTF_CFG] =
    {
        (uint8_t*)&att_desc_cfg,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE) |
        PERM(WR, ENABLE) |
        PERM(WRITE_REQ, ENABLE),
        sizeof(uint16_t),
        0,
        NULL
    },

    [SVC3_IDX_READ_1_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC3_READ_VAL_1_USER_DESC) - 1,
        sizeof(DEF_SVC3_READ_VAL_1_USER_DESC) - 1,
        (uint8_t*)DEF_SVC3_READ_VAL_1_USER_DESC
    },

    [SVC3_IDX_READ_2_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC3_IDX_READ_2_VAL] =
    {
        SVC3_READ_VAL_2_UUID_128,
        ATT_UUID_128_LEN,
        PERM(RD, ENABLE),
        DEF_SVC3_READ_VAL_2_CHAR_LEN,
        0,
        NULL
    },

    [SVC3_IDX_READ_2_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC3_READ_VAL_2_USER_DESC) - 1,
        sizeof(DEF_SVC3_READ_VAL_2_USER_DESC) - 1,
        (uint8_t*)DEF_SVC3_READ_VAL_2_USER_DESC
    },

    [SVC3_IDX_READ_3_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC3_IDX_READ_3_VAL] =
    {
        SVC3_READ_VAL_3_UUID_128,
        ATT_UUID_128_LEN,
        PERM(RD, ENABLE) |
        PERM(IND, ENABLE),
        DEF_SVC3_READ_VAL_3_CHAR_LEN,
        0,
        NULL
    },

    [SVC3_IDX_READ_3_IND_CFG] =
    {
        (uint8_t*)&att_desc_cfg,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE) |
        PERM(WR, ENABLE) |
        PERM(WRITE_REQ, ENABLE),
        sizeof(uint16_t),
        0,
        NULL
    },

    [SVC3_IDX_READ_3_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC3_READ_VAL_3_USER_DESC) - 1,
        sizeof(DEF_SVC3_READ_VAL_3_USER_DESC) - 1,
        (uint8_t*)DEF_SVC3_READ_VAL_3_USER_DESC
    },

    [SVC3_IDX_READ_4_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0, 0, NULL
    },

    [SVC3_IDX_READ_4_VAL] =
    {
        SVC3_READ_VAL_4_UUID_128,
        ATT_UUID_128_LEN,
        PERM(RD, ENABLE),
        PERM(RI, ENABLE) | DEF_SVC3_READ_VAL_4_CHAR_LEN,
        0,
        NULL
    },

    [SVC3_IDX_READ_4_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC3_READ_VAL_4_USER_DESC) - 1,
        sizeof(DEF_SVC3_READ_VAL_4_USER_DESC) - 1,
        (uint8_t*)DEF_SVC3_READ_VAL_4_USER_DESC
    },


    /*
     * HEART RATE
     ****************************************************************************************
     */

    [SVC3_IDX_HR_VAL_CHAR] = {(uint8_t*)&att_decl_char,ATT_UUID_16_LEN,PERM(RD, ENABLE),0,0,NULL},

    [SVC3_IDX_HR_VAL_VAL] =  {SVC3_HR_VAL_UUID_128,ATT_UUID_128_LEN,PERM(RD, ENABLE) | PERM(NTF, ENABLE),DEF_SVC3_HR_VAL_CHAR_LEN,0,NULL},

    [SVC3_IDX_HR_VAL_NTF_CFG] =
    {
        (uint8_t*)&att_desc_cfg,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE) |
        PERM(WR, ENABLE) |
        PERM(WRITE_REQ, ENABLE),
        sizeof(uint16_t),
        0,
        NULL
    },

    [SVC3_IDX_HR_VAL_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC3_HR_VAL_USER_DESC) - 1,
        sizeof(DEF_SVC3_HR_VAL_USER_DESC) - 1,
        (uint8_t*)DEF_SVC3_HR_VAL_USER_DESC
    },
		
		    /*
     * CLOCK
     ****************************************************************************************
     */

    [SVC3_IDX_CLOCK_VAL_CHAR] =
    {
        (uint8_t*)&att_decl_char,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        0,
        0,
        NULL
    },

    [SVC3_IDX_CLOCK_VAL_VAL] =
    {
        SVC3_CLOCK_VAL_UUID_128,
        ATT_UUID_128_LEN,
        PERM(WR, ENABLE) |
        PERM(WRITE_REQ, ENABLE) |
        PERM(WRITE_COMMAND, ENABLE),
        DEF_SVC3_CLOCK_VAL_CHAR_LEN,
        0,
        NULL
    },

    [SVC3_IDX_CLOCK_VAL_USER_DESC] =
    {
        (uint8_t*)&att_desc_user_desc,
        ATT_UUID_16_LEN,
        PERM(RD, ENABLE),
        sizeof(DEF_SVC3_CLOCK_VAL_USER_DESC) - 1,
        sizeof(DEF_SVC3_CLOCK_VAL_USER_DESC) - 1,
        (uint8_t*)DEF_SVC3_CLOCK_VAL_USER_DESC
    }
};
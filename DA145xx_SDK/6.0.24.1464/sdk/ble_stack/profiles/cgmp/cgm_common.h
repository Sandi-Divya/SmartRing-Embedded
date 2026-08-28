/**
 ****************************************************************************************
 *
 * @file cts_common.h
 *
 * @brief Header file - Continuous Glucose Monitoring Profile common types.
 *
 * Copyright (C) 2022-2024 Renesas Electronics Corporation and/or its affiliates.
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


#ifndef _CGM_COMMON_H_
#define _CGM_COMMON_H_

/**
 ****************************************************************************************
 * @addtogroup CGM Continuous Glucose Monitoring Profile
 * @ingroup PROFILE
 * @brief Continuous Glucose Monitoring Profile
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_CGM_SERVER)

#include "prf_types.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// Glucose Measurement max length
#define CGM_MEAS_MAX_LEN                (17)

/// Status value packet max length
#define CGM_STATUS_VAL_MAX_LEN          (6)

/// Record Access Control Point packet max length
#define CGM_REC_ACCESS_CTRL_MAX_LEN     (21)

/// Specific Ops Control Point packet max length
#define CGM_SPEC_OPS_CTRL_MAX_LEN       (16)

/*
 * ENUMS
 ****************************************************************************************
 */
 
//CGM Feature Flags 
enum cgms_feat_flags
{
    CGMS_FEAT_CALIBRATION_SUP                       = 0x000001,
    CGMS_FEAT_PATIENT_HL_ALERT_SUP                  = 0x000002,
    CGMS_FEAT_HYPO_ALERT_SUP                        = 0x000004,
    CGMS_FEAT_HYPER_ALERT_SUP                       = 0x000008,
    CGMS_FEAT_RATE_OF_INCR_DEACR_SUP                = 0x000010,
    CGMS_FEAT_DEVICE_SPEC_ALERT_SUP                 = 0x000020,
    CGMS_FEAT_SENSOR_MALFUNC_DETECT_SUP             = 0x000040,
    CGMS_FEAT_SENSOR_TEMP_HL_SUP                    = 0x000080,
    CGMS_FEAT_SENSOR_RESULT_HL_DETECT_SUP           = 0x000100,
    CGMS_FEAT_LOW_BATTERY_DETECT_SUP                = 0x000200,
    CGMS_FEAT_SENSOR_TYPE_ERR_DETECT_SUP            = 0x000400,
    CGMS_FEAT_GEN_DEV_FAULT_DETECT_SUP              = 0x000800,
    CGMS_FEAT_E2E_CRC_SUP                           = 0x001000,
    CGMS_FEAT_MULTI_BOND_SUP                        = 0x002000,
    CGMS_FEAT_MULTI_SESSION_SUP                     = 0x004000,
    CGMS_FEAT_CGM_TREND_INFO_SUP                    = 0x008000,
    CGMS_FEAT_CGM_QUALITY_SUP                       = 0x010000,
};

enum cgms_status_field_flags
{
    CGMS_STATUS_SESSION_STOPPED                     = 0x000001,
    CGMS_STATUS_DEV_BATT_LOW                        = 0x000002,
    CGMS_STATUS_SENS_INCORR_FOR_DEV                 = 0x000004,
    CGMS_STATUS_SENS_MALFUNC                        = 0x000008,
    CGMS_STATUS_DEV_SPEC_ALERT                      = 0x000010,
    CGMS_STATUS_GEN_DEV_FAULT                       = 0x000020,
    CGMS_STATUS_TIME_SYNCH_REQ                      = 0x000100,
    CGMS_STATUS_CALIB_NOT_ALLOWED                   = 0x000200,
    CGMS_STATUS_CALIB_RECOMMENDED                   = 0x000400,
    CGMS_STATUS_CALIB_REQUIRED                      = 0x000800,
    CGMS_STATUS_SENS_TEMP_TOO_HIGH_FOR_VALID        = 0x001000,
    CGMS_STATUS_SENS_TEMP_TOO_LOW_FOR_VALID         = 0x002000,
    CGMS_STATUS_DEV_SPEC_ALERT2                      = 0x004000,
    CGMS_STATUS_SENS_MALFUNC2                        = 0x008000,
    CGMS_STATUS_SENS_RES_LESS_PATIENT_LOW           = 0x010000,
    CGMS_STATUS_SENS_RES_MORE_PATIENT_HIGH          = 0x020000,
    CGMS_STATUS_SENS_RES_LESS_HYPO_LOW              = 0x040000,
    CGMS_STATUS_SENS_RES_MORE_HYPER_HIGH            = 0x080000,
    CGMS_STATUS_SENS_RATE_OF_INCR_EXCEED            = 0x100000,
    CGMS_STATUS_SENS_RATE_OF_INCR_EXCEED2           = 0x200000,
    CGMS_STATUS_RES_LOWER_THAN_CAN_PROC             = 0x400000,
    CGMS_STATUS_RES_HIGHER_THAN_CAN_PROC            = 0x800000,
};

/// CGM Error Codes
enum cgm_error_code
{
    /// Missing CRC
    CGM_ERR_MISSING_CRC  = (0x80),
    /// Invalid CRC
    CGM_ERR_INVALID_CRC  = (0x81),
};

/// CGM RACP opcodes
enum cgm_racp_op_code
{
    /// Report stored records (Operator: Value from Operator Table)
    CGM_REQ_REP_STRD_RECS = (1),

    /// Delete stored records (Operator: Value from Operator Table)
    CGM_REQ_DEL_STRD_RECS = (2),

    /// Abort operation (Operator: Null 'value of 0x00 from Operator Table')
    CGM_REQ_ABORT_OP = (3),

    /// Report number of stored records (Operator: Value from Operator Table)
    CGM_REQ_REP_NUM_OF_STRD_RECS = (4),

    /// Number of stored records response (Operator: Null 'value of 0x00 from Operator Table')
    CGM_REQ_NUM_OF_STRD_RECS_RSP = (5),

    /// Response Code (Operator: Null 'value of 0x00 from Operator Table')
    CGM_REQ_RSP_CODE = (6),
};

/// CGM access control operator
enum cgm_racp_operator
{
    /// All records
    CGM_OP_ALL_RECS = (1),

    /// Less than or equal to
    CGM_OP_LT_OR_EQ = (2),

    /// Greater than or equal to
    CGM_OP_GT_OR_EQ = (3),

    /// Within range of (inclusive)
    CGM_OP_WITHIN_RANGE_OF = (4),

    /// First record(i.e. oldest record)
    CGM_OP_FIRST_REC = (5),

    /// Last record (i.e. most recent record)
    CGM_OP_LAST_REC = (6),
};

/// CGM access control response code
enum cgm_racp_status
{
    /// Success
    CGM_RSP_SUCCESS = (1),

    /// Op Code not supported
    CGM_RSP_OP_CODE_NOT_SUP = (2),

    /// Invalid Operator
    CGM_RSP_INVALID_OPERATOR = (3),

    /// Operator not supported
    CGM_RSP_OPERATOR_NOT_SUP = (4),

    /// Invalid Operand
    CGM_RSP_INVALID_OPERAND = (5),

    /// No records found
    CGM_RSP_NO_RECS_FOUND = (6),

    /// Abort unsuccessful
    CGM_RSP_ABORT_UNSUCCESSFUL = (7),

    /// Procedure not completed
    CGM_RSP_PROCEDURE_NOT_COMPLETED = (8),

    /// Operand not supported
    CGM_RSP_OPERAND_NOT_SUP = (9),
};

/// Record Access Control filter type
enum cgm_racp_filter
{
    /// Filter using Sequence number
    CGM_FILTER_TIME_OFFSET = (1),
    /// Other values are RFU
};

/// CGM Specific Ops Control Point opcodes
enum cgm_spec_ops_cp_opcode
{
    /// Set CGM communication interval (Operand: uint8_t in minutes)
    CGM_REQ_SET_COMM_INT = (1),

    /// Get CGM Communication Interval (Operand: NULL)
    CGM_REQ_GET_COMM_INT = (2),
    
    /// Communication Interval Response
    CGM_RSP_COMM_INT = (3),

    /// Set Glucose Calibration Value (Operand: As defined from Operator Table)
    CGM_REQ_SET_GLUC_CAL_VAL = (4),

    /// Get Glucose Calibration Value (Operand: uint16_t calibration data record number)
    CGM_REQ_GET_GLUC_CAL_VAL = (5),

    /// Calibration Value Response
    CGM_RSP_CAL_VAL = (6),
    
    /// Set Patient High Alert Level (Operand: sfloat level in mg/dL)
    CGM_REQ_SET_PATIENT_HIGH_ALERT_LVL = (7),

    /// Get Patient High Alert Level (Operand: NULL)
    CGM_REQ_GET_PATIENT_HIGH_ALERT_LVL = (8),
    
    /// Patient High Alert Level Response
    CGM_RSP_PATIENT_HIGH_ALERT_LVL = (9),
    
    /// Set Patient Low Alert Level (Operand: sfloat level in mg/dL)
    CGM_REQ_SET_PATIENT_LOW_ALERT_LVL = (10),

    /// Get Patient Low Alert Level (Operand: NULL)
    CGM_REQ_GET_PATIENT_LOW_ALERT_LVL = (11),

    /// Patient Low Alert Level Response
    CGM_RSP_PATIENT_LOW_ALERT_LVL = (12),
    
    /// Set Hypo Alert Level (Operand: sfloat level in mg/dL)
    CGM_REQ_SET_HYPO_ALERT_LVL = (13),

    /// Get Hypo Alert Level (Operand: NULL)
    CGM_REQ_GET_HYPO_ALERT_LVL = (14),
    
    /// Patient Hypo Alert Level Response
    CGM_RSP_HYPO_ALERT_LVL = (15),
    
    /// Set Hyper Alert Level (Operand: sfloat level in mg/dL)
    CGM_REQ_SET_HYPER_ALERT_LVL = (16),

    /// Get Hyper Alert Level (Operand: NULL)
    CGM_REQ_GET_HYPER_ALERT_LVL = (17),
    
    /// Patient Hyper Alert Level Response
    CGM_RSP_HYPER_ALERT_LVL = (18),
    
    /// Set Rate Of Decrease Alert Level (Operand: sfloat level in mg/dL/min)
    CGM_REQ_SET_RATE_OF_DECR_ALERT_LVL = (19),

    /// Get Rate of Decrease Alert Level (Operand: NULL)
    CGM_REQ_GET_RATE_OF_DECR_ALERT_LVL = (20),
    
    /// Rate of Decrease Alert Level Response
    CGM_RSP_RATE_OF_DECR_ALERT_LVL = (21),
    
    /// Set Rate Of Increase Alert Level (Operand: sfloat level in mg/dL/min)
    CGM_REQ_SET_RATE_OF_INCR_ALERT_LVL = (22),

    /// Get Rate of Increase Alert Level (Operand: NULL)
    CGM_REQ_GET_RATE_OF_INCR_ALERT_LVL = (23),
    
    /// Rate of Increase Alert Level Response
    CGM_RSP_RATE_OF_INCR_ALERT_LVL = (24),
    
    /// Reset Device Specific Alert (Operand: NULL)
    CGM_REQ_RESET_DEV_SPEC_ALERT = (25),
    
    /// Start Session (Operand: NULL)
    CGM_REQ_START_SESSION = (26),

    /// Stop Session (Operand: NULL)
    CGM_REQ_STOP_SESSION = (27),

    /// Operand not supported
    CGM_RSP_RESPONSE = (28),
};


/*
 * STRUCTS
 ****************************************************************************************
 */
/// Sensor Status Annunciation Field
struct sens_stat_annunc_field
{
    uint8_t status;
    uint8_t cal_temp;
    uint8_t warning;
};
 
 /// Glucose measurement
struct cgm_meas
{
    /// Size field
    uint8_t size;
    /// Flags field
    uint8_t flags;
    /// Glucose Concentration field
    prf_sfloat gluc_concentr;
    /// Time offset field
    uint16_t time_offset;
    /// Sensor status annunciation field
    struct sens_stat_annunc_field sens_stat_annunc;
    /// Trend Information Field
    prf_sfloat trend_info;
    /// Quality field
    prf_sfloat quality;
};

struct cgm_session_start_time
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    int8_t time_zone;
    uint8_t dst_time_offset;
};

/// CGM RACP operation filter
struct cgm_filter
{
    /// Function operator
    uint8_t operator;

    /// Filter type, only time offset supported atm
    uint8_t filter_type;

    /// Filter union
    union
    {
        /// Time offset filtering
        struct
        {
            /// Minimum filter value
            uint16_t min;
            /// Maximum filter value
            uint16_t max;
        } time_offset;

    } val;
};

/// CGM RACP Request
struct cgm_racp_req
{
    /// Opcode
    uint8_t op_code;

    /// Operation filter
    struct cgm_filter filter;
};

/// CGM RACP response
struct cgm_racp_rsp
{
    /// Opcode
    uint8_t op_code;

    /// Operator (ignored, always set to null)
    uint8_t operator;

    /// response operand
    union
    {
        /// Number of record (if op_code = GLP_REQ_NUM_OF_STRD_RECS_RSP)
        uint16_t num_of_record;

        /// Command Status (if op_code = GLP_REQ_RSP_CODE)
        struct
        {
            /// Request Op Code
            uint8_t op_code_req;
            /// Response Code value
            uint8_t value;
        } rsp;
    } operand;
};

struct cgm_gluc_calib
{
    // Glucose Concentration of Calibration
    prf_sfloat gluc_conc_of_calib;
    // Calibration Time
    uint16_t calib_time;
    // Calibration Type-Sample Location
    uint8_t calib_type_samp_loc;
    // Next calibration time
    uint16_t next_calib_time;
    // Calibration Data Record Number
    uint16_t calib_data_rec_num;
    // Calibration Status
    uint8_t calib_stat;
};

/// CGM Specific Ops Control Point Request 
struct cgm_spec_ops_cp_req
{
    /// Opcode
    uint8_t op_code;

    /// Response operand
    union
    {
        /// Communication Interval (in mins) 
        uint8_t comm_interv;

        // Calibration value
        struct cgm_gluc_calib calib_value;
        
        // Calibration Data Record Number
        uint16_t calib_data_rec_num;
        
        // Alert Level
        prf_sfloat level; 
    } operand;
};

/// CGM Specific Ops Control Point Response
struct cgm_spec_ops_cp_rsp
{
    /// Opcode
    uint8_t op_code;
    
    union
    {
        /// Communication Interval (in mins)
        uint8_t comm_interv;

        /// Calibration Record
        struct cgm_gluc_calib calib_rec;
        
        // Alert Level
        prf_sfloat level;
        
        struct
        {
            uint8_t opcode;
            uint8_t value;
        } rsp;
    } operand;
};
/// @} cgm_common


#endif /* #if (BLE_CGM_SERVER) */

#endif /* _CGM_COMMON_H_ */

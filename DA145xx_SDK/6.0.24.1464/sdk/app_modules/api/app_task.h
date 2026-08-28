/**
 ****************************************************************************************
 *
 * @file app_task.h
 *
 * @brief Header file - APPTASK.
 *
 * Copyright (C) RivieraWaves 2009-2013
 * Copyright (C) 2017-2019 Modified by Renesas Electronics Corporation and/or its affiliates.
 *
 ****************************************************************************************
 */

#ifndef _APP_TASK_H_
#define _APP_TASK_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT)

#include <stdint.h>         // Standard Integer
#include "ke_msg.h"         // Kernel Message

/*
 * DEFINES
 ****************************************************************************************
 */

/// Number of APP Task Instances
#define APP_IDX_MAX                 (CFG_MAX_CONNECTIONS)

#if (!defined (__DA14531__))
#define __APP_TASK_BUILD_FOR_DA1458X__      (1)
#else
#define __APP_TASK_BUILD_FOR_DA1458X__      (0)
#endif

#if (defined (__DA14531__) && (!defined (__DA14531_01__) && !defined (__DA14535__)))
#define __APP_TASK_BUILD_FOR_DA14531__      (1)
#else
#define __APP_TASK_BUILD_FOR_DA14531__      (0)
#endif

#if (defined (__DA14531__) && defined (__DA14535__))
#define __APP_TASK_BUILD_FOR_DA14535__      (1)
#else
#define __APP_TASK_BUILD_FOR_DA14535__      (0)
#endif

#if (defined (__DA14531__) && defined (__DA14531_01__))
#define __APP_TASK_BUILD_FOR_DA14531_01__   (1)
#else
#define __APP_TASK_BUILD_FOR_DA14531_01__   (0)
#endif

#if (APP_IDX_MAX > 1) || __APP_TASK_BUILD_FOR_DA14531__ || __APP_TASK_BUILD_FOR_DA1458X__ || defined (__EXCLUDE_ROM_APP_TASK__)
#define APP_DEFAULT_HANDLER_VAR         app_default_handler_RAM
#define APP_GAP_PROCESS_HANDLER_FUNC    app_gap_process_handler_RAM
#else
#define APP_DEFAULT_HANDLER_VAR         app_default_handler
#define APP_GAP_PROCESS_HANDLER_FUNC    app_gap_process_handler
#endif
/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// States of APP task
enum APP_STATE
{
    /// Disabled State
    APP_DISABLED,

    /// Database Initialization State
    APP_DB_INIT,

    /// Connectable state
    APP_CONNECTABLE,

    /// Connected state
    APP_CONNECTED,

    /// Number of defined states.
    APP_STATE_MAX
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/// State handlers
extern const struct ke_state_handler APP_DEFAULT_HANDLER_VAR;
/// Array of task instance states
extern ke_state_t app_state[APP_IDX_MAX];

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Returns the current gap event's handled connection index
 * @return The connection index of the current gap event
 ****************************************************************************************
 */
uint8_t app_gap_get_current_handled_connection_idx(void);

/**
 ****************************************************************************************
 * @brief Process handler for the Application GAP messages.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance (probably unused)
 * @param[in] src_id  ID of the sending task instance
 * @param[in] msg_ret Result of the message handler
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
enum process_event_response APP_GAP_PROCESS_HANDLER_FUNC (ke_msg_id_t const msgid,
                                                     void const *param,
                                                     ke_task_id_t const dest_id,
                                                     ke_task_id_t const src_id,
                                                     enum ke_msg_status_tag *msg_ret);

#endif //(BLE_APP_PRESENT)

#endif // _APP_TASK_H_

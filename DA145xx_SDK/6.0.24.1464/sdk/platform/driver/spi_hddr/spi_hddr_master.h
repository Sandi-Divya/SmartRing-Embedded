/**
 ****************************************************************************************
 *
 * @file spihddr_master.h
 *
 * @brief Driver for Master SPIHDDR (SPI Half-Duplex with Data Ready) protocol.
 *
 * Copyright (C) 2023 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _SPI_HDDR_MASTER_
#define _SPI_HDDR_MASTER_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "ble_msg.h"
#include "gpio.h"
#include "spi.h"
#include "systick.h"
#include "app_list.h"

/*
 * DEFINES
 *****************************************************************************************
 */

#define SPI_MASTER_HEADER_TX                                            (0x9E)
#define SPI_MASTER_HEADER_RX                                            (0x61)
#define SPIHDDR_MASTER_CFG_MESSAGES_RECEIVED_IN_SEQUENCE_COUNT_MAX      (5)
#define SPIHDDR_MASTER_CFG_MESSAGES_TRANSMITTED_IN_SEQUENCE_COUNT_MAX   (5)

#define spihddr_master_delay_before_cs_assertion() systick_wait(480)
#define spihddr_master_delay_after_cs_assertion() systick_wait(50)

typedef enum
{
    SPIHDDR_MASTER_STATE_UNKNOWN = 0,
    SPIHDDR_MASTER_STATE_IDLE,
    SPIHDDR_MASTER_STATE_RECEIVING,
    SPIHDDR_MASTER_STATE_TRANSMITTING,
} spihddr_master_state_t;

typedef enum
{
    SPIHDDR_MASTER_EVENT_NONE = 0,
    SPIHDDR_MASTER_EVENT_INCOMING_MESSAGE_NOTIFICATION,
    SPIHDDR_MASTER_EVENT_OUTGOING_MESSAGE_ADDED,
} spihddr_master_event_t;

typedef struct spihddr_master_env
{
    spihddr_master_state_t state;
    uint32_t messages_received_in_sequence_count;
    uint32_t messages_transmitted_in_sequence_count;
} spihddr_master_env_t;

/**
 ****************************************************************************************
 * @brief SPIHDDR Data Ready Pin IRQ Enable
 ****************************************************************************************
 */
void spihddr_dready_pin_irq_enable(void);

/**
 ****************************************************************************************
 * @brief SPIHDDR Data Ready Pin IRQ Disable
 ****************************************************************************************
 */
void spihddr_dready_pin_irq_disable(void);

/**
 ****************************************************************************************
 * @brief SPIHDDR Data Ready IRQ Callback
 ****************************************************************************************
 */
void spihddr_dready_pin_irq_callback(void);

/**
****************************************************************************************
 * @brief Check a SPIHDDR master idle state
 * @return Current state of SPIHDDR interface.
****************************************************************************************
*/
uint8_t spihddr_master_fsm_state_is_idle(void);

/**
****************************************************************************************
 * @brief Initializes the SPI module as master to default values.
****************************************************************************************
*/
void spihddr_master_init(void);

/**
****************************************************************************************
* @brief Received data from Slave over SPIHDDR
 * @brief Send a message over the SPI using the SPI-HDDR protocol
 * @param[in] blemsg    Pointer to message.
****************************************************************************************
*/
void spihddr_master_tx(ble_msg *blemsg);

/**
****************************************************************************************
 * @brief Received data from Slave over SPIHDDR
****************************************************************************************
*/
void spihddr_master_rx(void);

/**
 ****************************************************************************************
 * @brief Registers an SPIHDDR master communication event
 * @param[in] event  Event to handle.
 * @return 0 if event was handled properly, 1 otherwise.
 ****************************************************************************************
 */
uint8_t spihddr_master_event_handler(spihddr_master_event_t event);

#endif //_SPI_HDDR_MASTER_

/**
 ****************************************************************************************
 *
 * @file spi_hddr_slave.h
 *
 * @brief Driver for Slave SPIHDDR (SPI Half-Duplex with Data Ready) protocol.
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

#ifndef _SPI_HDDR_SLAVE_
#define _SPI_HDDR_SLAVE_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include "dma.h"
#include "rwip.h"
#include "user_periph_setup.h"
#include "co_list.h"

#define SPI_MASTER_HEADER_TX                        (0x9E)
#define SPI_MASTER_HEADER_RX                        (0x61)
#define SPIHDDR_RX_MAX_AVAILABLE_BUFFERS            (100)
#define SPIHDDR_TX_MAX_AVAILABLE_BUFFERS            SPIHDDR_RX_MAX_AVAILABLE_BUFFERS
#define SPIHDDR_RX_BUFFER_LENGTH                    (5 * SPIHDDR_RX_MAX_AVAILABLE_BUFFERS)
#define SPIHDDR_TX_BUFFER_LENGTH                    (SPIHDDR_TX_MAX_AVAILABLE_BUFFERS)

/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */

typedef struct
{
    /// List element for chaining in the list
    struct co_list_hdr hdr;

    uint32_t tag;
    uint16_t total_len;
    uint8_t buffer[__ARRAY_EMPTY];
} spihddr_packet_t;

typedef struct
{
    uint8_t opcode;
    uint16_t max_available_buffers;
    uint16_t slave_message_length;
    spihddr_packet_t *packet_to_send;
} spihddr_msg_env_t;

/* TX and RX channel class holding data used for asynchronous read and write data
 * transactions
 */

/// SPI TX RX Channel
struct spihddr_txrxchannel
{
    /// size
    uint32_t size;
    /// buffer pointer
    uint8_t *bufptr;
    /// call back function pointer
    void (*callback) (uint8_t);
};

/// SPI environment structure
struct spihddr_env_tag
{
    /// tx channel
    struct spihddr_txrxchannel tx;
    /// rx channel
    struct spihddr_txrxchannel rx;
};

/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */

typedef enum{
    SPIHDDR_STATE_IDLE,
    SPIHDDR_STATE_READ_REQUESTED_IN_IDLE,
    SPIHDDR_STATE_CS_RECEIVED_MASTER_OPCODE_EXPECTED,
    SPIHDDR_STATE_MASTER_TRANSMISSION,
    SPIHDDR_STATE_MASTER_RECEPTION,
} spihddr_slave_state_t;

typedef enum{
    SPIHDDR_EVENT_NONE,
    SPIHDDR_EVENT_CS_ASSERTION_DETECTED,
    SPIHDDR_EVENT_RX_IRQ_TRIGGERED,
    SPIHDDR_EVENT_CS_DEASSERTION_DETECTED,
    SPIHDDR_EVENT_SLAVE_MESSAGE_ADDED,
} spihddr_slave_event_t;

typedef enum {
    SPIHDDR_ERROR_NO_ERROR,
    SPIHDDR_ERROR_INVALID_OPCODE,
} spihddr_com_error_t;

typedef struct
{
    /// List element for chaining in the list
    struct co_list_hdr hdr;
    spihddr_slave_event_t dma_spi_event;
} dma_spihddr_queued_event_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization function of the SPI module for the SPIHDDR protocol.
 * Initializes SPI module as slave.
 * @return void.
 ****************************************************************************************
 */
void spihddr_slave_init(void);

/**
 ****************************************************************************************
 * @brief Enable SPIHDDR flow.
 *****************************************************************************************
 */
void spihddr_flow_on_func(void);

/**
 ****************************************************************************************
 * @brief Flow off function of the SPIHDDR external interface API.
 * @return 0 if error, 1 otherwise.
 ****************************************************************************************
 */
bool spihddr_flow_off_func(void);

/**
 ****************************************************************************************
 * @brief Read function of the SPIHDDR external interface API.
 * @param[in] *bufptr    Pointer to the memory location the read data will be stored.
 * @param[in] size       Size of the data to be read in bytes.
 * @param[in] *callback  Pointer to the callback function.
 ****************************************************************************************
 */
void spihddr_read_func(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));

/**
 ****************************************************************************************
 * @brief Write function of the SPIHDDR external interface API.
 * @param[in] *bufptr    Pointer to the memory location of the data to be written.
 * @param[in] size       Size of the data to be written in bytes.
 * @param[in] *callback  Pointer to the callback function.
 ****************************************************************************************
 */
void spihddr_write_func(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));

/**
 ****************************************************************************************
 * @brief Exported function for SPI interrupt handler.
 ****************************************************************************************
 */
void SPI_Handler(void);

/**
 ****************************************************************************************
 * @brief The user command function to call periodically when the system is powered
 * @return Power mode.
 ****************************************************************************************
 */
arch_main_loop_callback_ret_t spihddr_on_system_powered(void);

/**
 ****************************************************************************************
 * @brief Validate an SPIHDDR sleep mode
 * @param[in] current_sleep_mode  Current sleep mode.
 * @return Actual sleep mode.
 ****************************************************************************************
 */
sleep_mode_t spihddr_validate_sleep(sleep_mode_t current_sleep_mode);

#endif // _SPI_HDDR_SLAVE_

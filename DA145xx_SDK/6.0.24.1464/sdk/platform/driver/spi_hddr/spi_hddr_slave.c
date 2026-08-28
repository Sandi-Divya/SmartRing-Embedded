/**
 ****************************************************************************************
 *
 * @file spi_hddr_slave.c
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "datasheet.h"
#include "arch.h"
#include "dma.h"
#include "rwip.h"
#include "arch_rom.h"
#include "ke_mem.h"
#include "gtl_env.h"
#include "spi_hddr.h"
#include "spi_hddr_slave.h"
#include "wkupct_quadec.h"
#include "spi.h"

struct co_list spihddr_outgoing_packets_list    __SECTION_ZERO("retention_mem_area0");
struct co_list spihddr_events_list              __SECTION_ZERO("retention_mem_area0");
struct spihddr_env_tag spihddr_env              __SECTION_ZERO("retention_mem_area0");
spihddr_slave_state_t spihddr_state             __SECTION_ZERO("retention_mem_area0");
spihddr_msg_env_t spihddr_msg_env               __SECTION_ZERO("retention_mem_area0");

#if USE_SPIHDDR_CRC
    uint8_t spihddr_rx_buffer_single_message[SPIHDDR_RX_MAX_AVAILABLE_BUFFERS + 2 + 1];
    uint8_t spihddr_tx_buffer_single_message[SPIHDDR_RX_MAX_AVAILABLE_BUFFERS + 2 + 1];
#else
    uint8_t spihddr_rx_buffer_single_message[SPIHDDR_RX_MAX_AVAILABLE_BUFFERS + 2];
    uint8_t spihddr_tx_buffer_single_message[SPIHDDR_RX_MAX_AVAILABLE_BUFFERS + 2];
#endif

uint8_t spihddr_rx_buffer[SPIHDDR_RX_BUFFER_LENGTH];
uint8_t spihddr_tx_buffer[SPIHDDR_TX_BUFFER_LENGTH];

uint8_t spihddr_slave_event_update_in_progress;
uint16_t spihddr_rx_buffer_current_write_index;
uint16_t spihddr_rx_buffer_current_read_index;
uint8_t spihddr_bytes_requested_pending_count;
uint8_t *spihddr_bytes_requested_target_buffer;
uint8_t spihddr_rx_done_pending;

static void spihddr_slave_event_handler(spihddr_slave_event_t event);

/**
 ****************************************************************************************
 * @brief SPIHDDR data receive callback
 * @param[in] *user_data  Pointer to the memory location of the data buffer.
 * @param[in] len         Data length.
 * @return void.
 ****************************************************************************************
 */
static void dma_spi_rx_callback(void *user_data, uint16_t len);

/**
 ****************************************************************************************
 * @brief SPIHDDR data transmit callback
 * @param[in] *user_data  Pointer to the memory location of the data buffer.
 * @param[in] len         Data length.
 * @return void.
 ****************************************************************************************
 */
static void dma_spi_tx_callback(void *user_data, uint16_t len);

/**
 ****************************************************************************************
 * @brief SPIHDDR data receive callback wrapper function
 * @param[in] len  Data length.
 * @return void.
 ****************************************************************************************
 */
static void dma_spi_rx_callback_t(uint16_t len)
{
    dma_spi_rx_callback(NULL, len);
}

/**
 ****************************************************************************************
 * @brief SPIHDDR data transmit callback wrapper function
 * @param[in] len  Data length.
 * @return void.
 ****************************************************************************************
 */
static void dma_spi_tx_callback_t(uint16_t len)
{
    dma_spi_tx_callback(NULL, len);
}

/**
 ****************************************************************************************
 * @brief SPIHDDR Configuration structure
 ****************************************************************************************
 */
const spi_cfg_t spi_cfg_slave =
{
    /// SPI master/slave mode
    .spi_ms = SPI_MS_MODE_SLAVE,

    /// SPI clock mode (CPOL, CPHA)
    .spi_cp = SPI_CP_MODE_3,

    /// SPI master clock frequency
    .spi_speed = SPI_SPEED_MODE_4MHz,

    /// SPI word size (1, 2, 4 Bytes or 9 bits)
    .spi_wsz = SPI_MODE_8BIT,

    /// SPI master CS mode
#if defined (__DA14531__)
    .spi_cs = SPI_CS_NONE,
#else
    .spi_cs = SPI_CS_0,
#endif

    /// SPI Interrupt mode
    .spi_irq = SPI_IRQ_DISABLED,

    /// SPI CS Pad
    .cs_pad = { .port = SPIHDDR_CS_PORT, .pin = SPIHDDR_CS_PIN, },

#if defined (CFG_SPI_DMA_SUPPORT)
    /// SPI send callback
    .send_cb = NULL,

    /// SPI receive callback
    .receive_cb = dma_spi_rx_callback_t,

    /// SPI transfer callback
    .transfer_cb = dma_spi_tx_callback_t,

    /// SPI DMA Channel Pair Configuration
    .spi_dma_channel = SPI_DMA_CHANNEL_01,

    /// SPI DMA Priority Configuration
    .spi_dma_priority = DMA_PRIO_3,

#endif // CFG_SPI_DMA_SUPPORT

    /// SPI Capture at current or next edge.
#if defined (__DA14531__)
    .spi_capture = SPI_MASTER_EDGE_CAPTURE,
#endif
};

/**
 ****************************************************************************************
 * @brief DMA callback setup
 ****************************************************************************************
 */
static void dma_callback_setup(void)
{
    dma_register_callback(DMA_CHANNEL_0, dma_spi_rx_callback, NULL);
    dma_register_callback(DMA_CHANNEL_1, dma_spi_rx_callback, NULL);
}

/**
 ****************************************************************************************
 * @brief DMA RX channel reinitialized
 * @param[in] length       Data length.
 * @param[in] src_address  Source address.
 * @param[in] irq_idx      IRQ index.
 ****************************************************************************************
 */
static void dma_rx_channel_setup_reinitiazed(uint16_t length, uint32_t dest_address, uint32_t irq_idx)
{
    // Make sure DMA channel is disabled first
    SetBits16(DMA0_CTRL_REG, DMA_ON, DMA_STATE_DISABLED);

    SetWord16(DMA0_B_STARTL_REG, dest_address & 0xFFFF);
    SetWord16(DMA0_B_STARTH_REG, dest_address >> 16);
    SetWord16(DMA0_LEN_REG, length - 1);

    // Set IRQ number of transfers
    if (irq_idx > 0)
    {
        // If user explicitly set this number use it
        SetWord16(DMA0_INT_REG, irq_idx - 1);
    }
    else
    {
        // If user passed 0, use transfer length to fire interrupt after transfer ends
        SetWord16(DMA0_INT_REG, length - 1);
    }
}

/**
 ****************************************************************************************
 * @brief DMA TX channel reinitialized
 * @param[in] length       Data length.
 * @param[in] src_address  Source address.
 * @param[in] irq_idx      IRQ index.
 ****************************************************************************************
 */
static void dma_tx_channel_setup_reinitiazed(uint16_t length, uint32_t src_address, uint32_t irq_idx)
{
    // Make sure DMA channel is disabled first
    SetBits16(DMA1_CTRL_REG, DMA_ON, DMA_STATE_DISABLED);

    SetWord16(DMA1_A_STARTL_REG, src_address & 0xFFFF);
    SetWord16(DMA1_A_STARTH_REG, src_address >> 16);
    SetWord16(DMA1_LEN_REG, length - 1);

    // Set IRQ number of transfers
    if (irq_idx > 0)
    {
        // If user explicitly set this number use it
        SetWord16(DMA1_INT_REG, irq_idx - 1);
    }
    else
    {
        // If user passed 0, use transfer length to fire interrupt after transfer ends
        SetWord16(DMA1_INT_REG, length - 1);
    }
}

/**
 ****************************************************************************************
 * @brief Update SPIHDDR Event
 * @param[in] event  Event code.
 ****************************************************************************************
 */
static void spihddr_event_update(spihddr_slave_event_t event)
{
    if (spihddr_slave_event_update_in_progress == true)
    {
        dma_spihddr_queued_event_t *dma_spi_queued_event = (dma_spihddr_queued_event_t*) ke_malloc(sizeof(dma_spihddr_queued_event_t), KE_MEM_KE_MSG);
        dma_spi_queued_event->dma_spi_event = event;
        co_list_push_back(&spihddr_events_list, &dma_spi_queued_event->hdr);
    }
    if (spihddr_slave_event_update_in_progress == true)
    {
        return;
    }

    spihddr_slave_event_update_in_progress = true;

    while (co_list_is_empty(&spihddr_events_list) == false)
    {
        dma_spihddr_queued_event_t *dma_spi_queued_event = (dma_spihddr_queued_event_t*)co_list_pop_front(&spihddr_events_list);
        spihddr_slave_event_handler(dma_spi_queued_event->dma_spi_event);
        ke_free(dma_spi_queued_event);
    }

    spihddr_slave_event_handler(event);
    spihddr_slave_event_update_in_progress = false;
}

/**
 ****************************************************************************************
 * @brief DMA RX callback
 * @param[in] *user_data  Pointer to the memory location of the data to be written.
 * @param[in] len         Size of the data to be written in bytes.
 ****************************************************************************************
 */
static void dma_spi_rx_callback(void *user_data, uint16_t len)
{
    spihddr_event_update(SPIHDDR_EVENT_RX_IRQ_TRIGGERED);
}

/**
 ****************************************************************************************
 * @brief DMA TX callback
 * @param[in] *user_data  Pointer to the memory location of the data to be sent.
 * @param[in] len         Size of the data to be sent in bytes.
 ****************************************************************************************
 */
static void dma_spi_tx_callback(void *user_data, uint16_t len)
{
    ASSERT_ERROR(0);
}

/**
 ****************************************************************************************
 * @brief SPIHDDR communication error handler
 * @param[in] error  Communication error code.
 ****************************************************************************************
 */
static void spihddr_com_error(spihddr_com_error_t error)
{
    ASSERT_ERROR(0);
}

/**
 ****************************************************************************************
 * @brief Initialize spihddr protocol (upon system startup)
 ****************************************************************************************
 */
static void spihddr_lists_initialize(void)
{
    co_list_init(&spihddr_outgoing_packets_list);
    co_list_init(&spihddr_events_list);
}

/**
 ****************************************************************************************
 * @brief Configure the DMA for reception of the opcode from the MASTER
 * @param[in] *target_buffer  Pointer to the memory location of the data to be written.
 ****************************************************************************************
 */
static void spihddr_receive_opcode_from_master(uint8_t *target_buffer)
{
    #if defined (__DA14531__)
        spi_ctrl_reg_setf(SPI_FIFO_RESET);
        spi_initialize(&spi_cfg_slave);
        spi_ctrl_reg_spi_fifo_reset_setf(SPI_BIT_DIS);
        spi_receive(target_buffer, 1, SPI_OP_DMA);
    #else
        SetBits16(SPI_CTRL_REG, SPI_ON, SPI_BIT_DIS);
        dma_rx_channel_setup_reinitiazed(1, (uint32_t)target_buffer, 1);
        dma_channel_start(DMA_CHANNEL_0, DMA_IRQ_STATE_ENABLED); //Channel 0:Rx
        SetBits16(SPI_CTRL_REG, SPI_ON, SPI_BIT_EN);
    #endif
}

/**
 ****************************************************************************************
 * @brief Prepare spihddr for transaction
 ****************************************************************************************
 */
static void spihddr_prepare_for_spi_transaction(void)
{
    spihddr_receive_opcode_from_master(&spihddr_msg_env.opcode);
}
/**
 ****************************************************************************************
 * @brief Reinitialize spihddr protocol (after sleep)
 *        Note: spihddr_lists_initialize() must be called upon system startup
 ****************************************************************************************
 */
static void spihddr_reactivate_after_sleep(void)
{
    spihddr_prepare_for_spi_transaction();
}

/**
 ****************************************************************************************
 * @brief A packet has been sent to the master
 ****************************************************************************************
 */
static void spihddr_packet_sent_to_master_callback(void)
{
    void (*callback) (uint8_t) = NULL;

    dma_channel_stop(DMA_CHANNEL_0); //Channel 0:Rx
    dma_channel_start(DMA_CHANNEL_1, DMA_IRQ_STATE_ENABLED); //Channel 1:Tx

    // Reset TX parameters
    spihddr_env.tx.bufptr = NULL;
    spihddr_env.tx.size = 0;

    // Retrieve callback pointer
    callback = spihddr_env.tx.callback;

    if(callback != NULL)
    {
        // Clear callback pointer
        spihddr_env.tx.callback = NULL;

        // Call handler
        callback(RWIP_EIF_STATUS_OK);
    }
    else
    {
        ASSERT_WARNING(0); //CS deasserted but no payload read by the master
    }
}

/**
 ****************************************************************************************
 * @brief A packet has been received by the master
 ****************************************************************************************
 */
static void spihddr_packet_received_from_master_callback(void)
{
    dma_channel_stop(DMA_CHANNEL_0); //Channel 0:Rx
    dma_channel_stop(DMA_CHANNEL_1); //Channel 1:Tx
}

/**
 ****************************************************************************************
 * @brief Returns true if at least one outging HCI message is pending to be sent to
 *        the master
 * @return 0 if no message is pending, 1 otherwise.
 ****************************************************************************************
 */
static uint8_t spihddr_outgoing_message_pending(void)
{
    return (co_list_is_empty(&spihddr_outgoing_packets_list) == false);
}

/**
 ****************************************************************************************
 * @brief The RX header has been sent by the master. Prepare for transmission to the master
 ****************************************************************************************
 */
static void spihddr_operation_rx_callback(void)
{
    ASSERT_WARNING(spihddr_msg_env.opcode == SPI_MASTER_HEADER_RX);
    memset(&spihddr_tx_buffer, 0, sizeof(spihddr_tx_buffer));
    spihddr_msg_env.slave_message_length = 0;

    if (spihddr_outgoing_message_pending())
    {

        spihddr_msg_env.packet_to_send = (spihddr_packet_t*)co_list_pop_front(&spihddr_outgoing_packets_list);
        memcpy(spihddr_tx_buffer + 2, spihddr_msg_env.packet_to_send->buffer, spihddr_msg_env.packet_to_send->total_len);
        ASSERT_ERROR(spihddr_msg_env.packet_to_send->total_len <= 0xFFFF)
        spihddr_msg_env.slave_message_length = spihddr_msg_env.packet_to_send->total_len;
        ASSERT_WARNING(spihddr_msg_env.slave_message_length > 0);
        ke_free(spihddr_msg_env.packet_to_send);
        spihddr_tx_buffer[1] = spihddr_msg_env.slave_message_length & 0xFF;
        spihddr_tx_buffer[0] = spihddr_msg_env.slave_message_length >> 8;
    }

    #if USE_SPIHDDR_CRC
    uint8_t crc = 0;
    crc ^= SPI_MASTER_HEADER_RX;
    for (uint16_t i=0; i<spihddr_msg_env.slave_message_length + 2; i++)
    {
        crc ^= spihddr_tx_buffer[i];
    }

    spihddr_tx_buffer[spihddr_msg_env.slave_message_length + 2] = crc;
    #endif

    ASSERT_ERROR(sizeof(spihddr_rx_buffer_single_message) >= sizeof(spihddr_tx_buffer));

    #if defined (__DA14531__)
        spi_ctrl_reg_setf(SPI_FIFO_RESET);
        spi_initialize(&spi_cfg_slave);
        spi_ctrl_reg_spi_fifo_reset_setf(SPI_BIT_DIS);
        spi_transfer(spihddr_tx_buffer, spihddr_rx_buffer_single_message, spihddr_msg_env.slave_message_length+3, SPI_OP_DMA);
    #else
        SetBits16(SPI_CTRL_REG, SPI_ON, SPI_BIT_DIS);
        dma_tx_channel_setup_reinitiazed(sizeof(spihddr_tx_buffer), (uint32_t)spihddr_tx_buffer, 0);
        dma_rx_channel_setup_reinitiazed( sizeof(spihddr_rx_buffer_single_message), (uint32_t)spihddr_rx_buffer_single_message, 0);
        dma_channel_start(DMA_CHANNEL_0, DMA_IRQ_STATE_ENABLED); //Channel 0:Rx
        dma_channel_start(DMA_CHANNEL_1, DMA_IRQ_STATE_ENABLED); //Channel 1:Tx
        SetBits16(SPI_CTRL_REG, SPI_ON, SPI_BIT_EN);
    #endif
}

/**
 ****************************************************************************************
 * @brief The TX header has been sent by the master. Prepare for reception of packet
 *        from the master
 ****************************************************************************************
 */
static void spihddr_operation_tx_callback(void)
{
    spihddr_msg_env.max_available_buffers = SPIHDDR_RX_MAX_AVAILABLE_BUFFERS;
    ASSERT_WARNING(spihddr_msg_env.opcode == SPI_MASTER_HEADER_TX);

    memset(&spihddr_tx_buffer_single_message, 0, sizeof(spihddr_tx_buffer_single_message));
    *(uint16_t*)spihddr_tx_buffer_single_message = spihddr_msg_env.max_available_buffers;

    ASSERT_ERROR(sizeof(spihddr_rx_buffer_single_message) >= sizeof(spihddr_tx_buffer_single_message));

    #if defined (__DA14531__)
        spi_ctrl_reg_setf(SPI_FIFO_RESET);
        spi_initialize(&spi_cfg_slave);
        spi_ctrl_reg_spi_fifo_reset_setf(SPI_BIT_DIS);
        spi_transfer(spihddr_tx_buffer_single_message, spihddr_rx_buffer_single_message, sizeof(spihddr_rx_buffer_single_message), SPI_OP_DMA);
    #else
        SetBits16(SPI_CTRL_REG, SPI_ON, SPI_BIT_DIS);
        dma_tx_channel_setup_reinitiazed( sizeof(spihddr_tx_buffer_single_message), (uint32_t)spihddr_tx_buffer_single_message, 0);
        dma_rx_channel_setup_reinitiazed( sizeof(spihddr_rx_buffer_single_message), (uint32_t)spihddr_rx_buffer_single_message, 0);
        dma_channel_start(DMA_CHANNEL_0, DMA_IRQ_STATE_ENABLED); //Channel 0:Rx
        dma_channel_start(DMA_CHANNEL_1, DMA_IRQ_STATE_ENABLED); //Channel 1:Tx
        SetBits16(SPI_CTRL_REG, SPI_ON, SPI_BIT_EN);
    #endif
}

/**
 ****************************************************************************************
 * @brief Gets the count of bytes already received from the master in the rx buffer,
 *        available to be read by the eif read handler
 * @return Current buffer space left in bytes.
 ****************************************************************************************
 */
static uint16_t spihddr_rx_buffer_bytes_received(void)
{
    ASSERT_ERROR(spihddr_rx_buffer_current_write_index >= spihddr_rx_buffer_current_read_index);
    return spihddr_rx_buffer_current_write_index - spihddr_rx_buffer_current_read_index;
}

/**
 ****************************************************************************************
 * @brief Reads as many bytes as available in the rx buffer, up to the {length} requested
 *        Called by the eif read handler
 * @param[in] *target_buffer   Pointer to the memory location of the data to be written.
 * @param[in] length           Size of the data to be written in bytes.
 * @return 1 if reading is flaged as pending, 0 otherwise.
 ****************************************************************************************
 */
static uint16_t spihddr_read_from_rx_buffer(uint8_t *target_buffer, uint16_t length)
{
    ASSERT_ERROR(spihddr_rx_done_pending == false);
    ASSERT_ERROR(target_buffer != NULL);
    ASSERT_ERROR(spihddr_rx_buffer_current_write_index < sizeof(spihddr_rx_buffer));

    if (spihddr_rx_buffer_current_write_index - spihddr_rx_buffer_current_read_index < length)
    {
        // No bytes or not all bytes available to read
        // Reading request is flagged as pending
        // Call to rx_done is not flagged as pending
        spihddr_bytes_requested_pending_count = length;
        spihddr_bytes_requested_target_buffer = target_buffer;
        spihddr_bytes_requested_pending_count = length;
        return 1;
    }
    else
    {
        // As many bytes as available or less are requested to read
        // Reading request is not flagged as pending
        // Call to rx_done is flagged as pending
        memcpy(target_buffer, &spihddr_rx_buffer[spihddr_rx_buffer_current_read_index], length);

        spihddr_rx_buffer_current_read_index += length;
        if (spihddr_rx_buffer_current_write_index == spihddr_rx_buffer_current_read_index)
        {
                spihddr_rx_buffer_current_write_index = 0;
                spihddr_rx_buffer_current_read_index = 0;
        }
        spihddr_bytes_requested_pending_count = 0;
        spihddr_bytes_requested_target_buffer = NULL;
        return 0;
    }
}

/**
 ****************************************************************************************
 * @brief Triggers reading of the remaining bytes in the rx_buffer, pending after a
 *        previous read has completed as partial
 ****************************************************************************************
 */
static void spihddr_read_pending_bytes_from_rx_buffer(void)
{
    if ( (spihddr_bytes_requested_pending_count > 0) && (spihddr_rx_buffer_current_write_index != spihddr_rx_buffer_current_read_index) )
    {
        ASSERT_ERROR(spihddr_bytes_requested_target_buffer != NULL);
        ASSERT_ERROR(spihddr_rx_done_pending == false);
        if (spihddr_read_from_rx_buffer(spihddr_bytes_requested_target_buffer, spihddr_bytes_requested_pending_count) == 0)
        {
            spihddr_rx_done_pending = true;
        }
    }
}

/**
 ****************************************************************************************
 * @brief Returns true if the SPIHDDR is idle (no transaction ongoing)
 * @return 1 if state is idle, 1 otherwise.
 ****************************************************************************************
 */
static uint8_t spihddr_is_idle(void)
{
    return (spihddr_state == SPIHDDR_STATE_IDLE);
}

/**
 ****************************************************************************************
 * @brief Notifies the MASTER that data is available to be sent
 * @return 0 if no message is pending, 1 otherwise.
 ****************************************************************************************
 */
static uint8_t spihddr_notify_master_data_is_available(void)
{
    if (spihddr_outgoing_message_pending())
    {
        if (spihddr_is_idle())
        {
            spihddr_event_update(SPIHDDR_EVENT_SLAVE_MESSAGE_ADDED);
        }
        return 1;
    }
    return 0;
}

/**
 ****************************************************************************************
 * @brief Registers a message to be sent to the master
 * @param[in] *bufptr     Pointer to the memory location of the data buffer.
 * @param[in] total_size  Size of the data buffer in bytes.
 ****************************************************************************************
 */
static void spihddr_register_outgoing_message(uint8_t *bufptr, uint32_t total_size)
{
    spihddr_packet_t *spihddr_packet = (spihddr_packet_t*) ke_malloc(sizeof(spihddr_packet_t) + total_size, KE_MEM_KE_MSG);
    memcpy(&spihddr_packet->buffer[0], bufptr, total_size);
    spihddr_packet->total_len = total_size;
    co_list_push_back(&spihddr_outgoing_packets_list, &spihddr_packet->hdr);
}

/**
 ****************************************************************************************
 * @brief Returns true if at least one spihddr event is pending to be registered
 * @return 0 if list is empty, 1 if not.
 ****************************************************************************************
 */
static uint8_t spihddr_slave_event_update_pending(void)
{
    return (co_list_is_empty(&spihddr_events_list) == false);
}

/**
 ****************************************************************************************
 * @brief Handler for the GPIO IRQ assigned to CS deassertion
 ****************************************************************************************
 */
static void spihddr_cs_deassertion_gpio_isr_handler(void)
{
    GPIO_ResetIRQ(GPIO0_IRQn);
    spihddr_event_update(SPIHDDR_EVENT_CS_DEASSERTION_DETECTED);
}

/**
 ****************************************************************************************
 * @brief Initialize the GPIO IRQ assigned to CS deassertion
 ****************************************************************************************
 */
static void spihddr_cs_deassertion_gpio_isr_init(void)
{
    GPIO_RegisterCallback(GPIO0_IRQn, spihddr_cs_deassertion_gpio_isr_handler);
    GPIO_EnableIRQ(SPIHDDR_CS_PORT, SPIHDDR_CS_PIN, GPIO0_IRQn, false, true, 0);
}

/**
 ****************************************************************************************
 * @brief Handler for the GPIO IRQ assigned to CS assertion
 ****************************************************************************************
 */
static void spihddr_cs_assertion_gpio_isr_handler(void)
{
    GPIO_ResetIRQ(GPIO1_IRQn);
    WKUP_QUADEC_Handler();
    spihddr_event_update(SPIHDDR_EVENT_CS_ASSERTION_DETECTED);
}

/**
 ****************************************************************************************
 * @brief Initialize the GPIO IRQ assigned to CS assertion
 ****************************************************************************************
 */
static void spihddr_cs_assertion_gpio_isr_init(void)
{
    GPIO_RegisterCallback(GPIO1_IRQn, spihddr_cs_assertion_gpio_isr_handler);
    GPIO_EnableIRQ(SPIHDDR_CS_PORT, SPIHDDR_CS_PIN, GPIO1_IRQn, true, true, 0);
}

/**
 ****************************************************************************************
 * @brief Get state of SPIHDDR
 ****************************************************************************************
 */
static spihddr_slave_state_t spihddr_get_state(void)
{
    return spihddr_state;
}

void spihddr_flow_on_func(void)
{
    //Do nothing
}

bool spihddr_flow_off_func(void)
{
    bool ret_val = 1;

    if ( (spihddr_get_state() != SPIHDDR_STATE_IDLE) && (spihddr_get_state() != SPIHDDR_STATE_READ_REQUESTED_IN_IDLE) )
    {
        ret_val = 0;
    }

    if (spihddr_rx_done_pending == true)
    {
        ret_val = 0;
    }

    if (spihddr_rx_buffer_bytes_received() != 0)
    {
        ret_val = 0;
    }

    if (spi_hddr_cs_getf() == 0)
    {
        ret_val = 0;
    }

    if (spihddr_slave_event_update_pending())
    {
        ret_val = 0;
    }

    return ret_val;
}

void spihddr_read_func(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t))
{
    // Sanity check
    ASSERT_ERROR(bufptr != NULL);
    ASSERT_ERROR(size != 0);

    spihddr_env.rx.callback = callback;

    ASSERT_ERROR(spihddr_rx_done_pending == false);
    if (spihddr_read_from_rx_buffer(bufptr, size) == 0)
    {
        spihddr_rx_done_pending = true;
    }
}

void spihddr_write_func(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t))
{
    // Sanity check
    ASSERT_ERROR(bufptr != NULL);
    ASSERT_ERROR(size != 0);
    ASSERT_ERROR(spihddr_env.tx.bufptr == NULL);

    // Prepare TX parameters
    spihddr_env.tx.size = size;
    spihddr_env.tx.bufptr = bufptr;
    spihddr_env.tx.callback = callback;

    spihddr_register_outgoing_message(bufptr, size);
}

void spihddr_slave_init(void)
{
    if (spi_initialize(&spi_cfg_slave) != SPI_STATUS_ERR_OK)
    {
        ASSERT_ERROR(0);
    }

#if !defined (__DA14531__)
    dma_callback_setup();
#endif

    SetBits16(GP_CONTROL_REG, CPU_DMA_BUS_PRIO, 1);  //set to 1: The DMA has the highest priority

    NVIC_SetPriority(DMA_IRQn, 2);

    spihddr_receive_opcode_from_master(&spihddr_msg_env.opcode);

    spihddr_cs_assertion_gpio_isr_init();
    spihddr_cs_deassertion_gpio_isr_init();

    spihddr_reactivate_after_sleep();
}

/**
 ****************************************************************************************
 * @brief Update SPIHDDR state
 * @param[in] spihddr_state_to_set  Target state.
 * @return void.
 ****************************************************************************************
 */
static void spihddr_state_update(spihddr_slave_state_t spihddr_state_to_set)
{
    spihddr_state = spihddr_state_to_set;
}

/**
 ****************************************************************************************
 * @brief Registers an SPIHDDR slave communication event
 * @param[in] event  Event to handle.
 * @return void.
 ****************************************************************************************
 */
static void spihddr_slave_event_handler(spihddr_slave_event_t event)
{
    uint16_t bytes_received_count = GetWord16(DMA0_IDX_REG);
    volatile uint8_t crc = 0;
#if USE_SPIHDDR_CRC
    static uint32_t diag_spihddr_master_tx_crc_error_cnt = 0;
#endif

    switch(spihddr_state)
    {
        case SPIHDDR_STATE_IDLE:
        case SPIHDDR_STATE_READ_REQUESTED_IN_IDLE:
        case SPIHDDR_STATE_CS_RECEIVED_MASTER_OPCODE_EXPECTED:
            switch (event)
            {
                case SPIHDDR_EVENT_CS_ASSERTION_DETECTED:
                    spihddr_state_update(SPIHDDR_STATE_CS_RECEIVED_MASTER_OPCODE_EXPECTED);
                    spi_hddr_dready_high();
                    break;

                case SPIHDDR_EVENT_RX_IRQ_TRIGGERED:
                    switch (spihddr_msg_env.opcode)
                    {
                        case SPI_MASTER_HEADER_TX:
                            spihddr_state_update(SPIHDDR_STATE_MASTER_TRANSMISSION);
                            spihddr_operation_tx_callback();
                            spi_hddr_dready_low();
                            break;

                        case SPI_MASTER_HEADER_RX:
                            spihddr_state_update(SPIHDDR_STATE_MASTER_RECEPTION);
                            spihddr_operation_rx_callback();
                            spi_hddr_dready_low();
                            break;

                        default:
                            spihddr_com_error(SPIHDDR_ERROR_INVALID_OPCODE);
                            break;
                    }
                    break;

                case SPIHDDR_EVENT_CS_DEASSERTION_DETECTED:
                    //#warning "NO REACTION ON CS DEASSERTION!!!"
                    break;

                case SPIHDDR_EVENT_SLAVE_MESSAGE_ADDED:
                    if (spihddr_state == SPIHDDR_STATE_IDLE)
                    {
                        spihddr_state_update(SPIHDDR_STATE_READ_REQUESTED_IN_IDLE);
                        spi_hddr_dready_high();
                    }
                    break;

                default:
                    ASSERT_WARNING(0);
                    break;
            }
            break;

        case SPIHDDR_STATE_MASTER_TRANSMISSION:
            switch (event)
            {
                case SPIHDDR_EVENT_RX_IRQ_TRIGGERED:
                    ASSERT_WARNING(0); //buffer full
                    break;

                case SPIHDDR_EVENT_CS_DEASSERTION_DETECTED:
                    spihddr_state_update(SPIHDDR_STATE_IDLE);
                    ASSERT_ERROR(spihddr_rx_buffer_current_write_index + bytes_received_count < sizeof(spihddr_rx_buffer));

#if USE_SPIHDDR_CRC
                    memcpy(spihddr_rx_buffer + spihddr_rx_buffer_current_write_index, spihddr_rx_buffer_single_message + 2, bytes_received_count - 2 - 1);

                    spihddr_rx_buffer_current_write_index += bytes_received_count - 2 - 1;

                    crc ^= SPI_MASTER_HEADER_TX;
                    crc ^= spihddr_msg_env.max_available_buffers & 0xFF;
                    crc ^= spihddr_msg_env.max_available_buffers >> 8;

                    for (uint32_t i=0; i<bytes_received_count - 1; i++)
                    {
                       crc ^= spihddr_rx_buffer_single_message[i];

                    }
                    if (crc != spihddr_rx_buffer_single_message[bytes_received_count-1])
                    {
                        diag_spihddr_master_tx_crc_error_cnt++;
                    }
                    ASSERT_WARNING(crc == spihddr_rx_buffer_single_message[bytes_received_count-1])
                    memset(spihddr_rx_buffer_single_message, 0, sizeof(spihddr_rx_buffer_single_message));
#else
                    memcpy(spihddr_rx_buffer + spihddr_rx_buffer_current_write_index, spihddr_rx_buffer_single_message + 2, bytes_received_count - 2);
                    memset(spihddr_rx_buffer_single_message, 0, sizeof(spihddr_rx_buffer_single_message));

                    spihddr_rx_buffer_current_write_index += bytes_received_count - 2;
#endif

                    spihddr_packet_received_from_master_callback();
                    spihddr_cs_assertion_gpio_isr_init();
                    spihddr_prepare_for_spi_transaction();
                    ASSERT_ERROR(spihddr_rx_buffer_current_write_index > spihddr_rx_buffer_current_read_index)
                    spi_hddr_dready_low();

                    break;

                case SPIHDDR_EVENT_SLAVE_MESSAGE_ADDED:
                    break;

                default:
                    ASSERT_WARNING(0);
                    break;
            }
            break;

        case SPIHDDR_STATE_MASTER_RECEPTION:
            switch (event)
            {
                case SPIHDDR_EVENT_CS_DEASSERTION_DETECTED:
                    // Tx of message to external host is done
                    spi_hddr_dready_low();
                    spihddr_state_update(SPIHDDR_STATE_IDLE);
                    spihddr_packet_sent_to_master_callback();
                    spihddr_cs_assertion_gpio_isr_init();
                    spihddr_prepare_for_spi_transaction();
                    break;

                case SPIHDDR_EVENT_SLAVE_MESSAGE_ADDED:
                    break;

                default:
                    ASSERT_WARNING(0);
                    break;
            }
            break;

        default:
            ASSERT_WARNING(0);
            break;
    }
}

arch_main_loop_callback_ret_t spihddr_on_system_powered(void)
{
    arch_main_loop_callback_ret_t ret_val = GOTO_SLEEP;

    if (spihddr_notify_master_data_is_available() != 0)
    {
        ret_val = KEEP_POWERED;
    }

    spihddr_read_pending_bytes_from_rx_buffer();

    if (spihddr_rx_done_pending == true)
    {
        spihddr_rx_done_pending = false;
        ASSERT_WARNING(spihddr_env.rx.callback != NULL);
        spihddr_env.rx.callback(RWIP_EIF_STATUS_OK);
        ret_val = KEEP_POWERED;
    }

    if (co_list_is_empty(&spihddr_events_list) == false)
    {
        dma_spihddr_queued_event_t *dma_spi_queued_event = (dma_spihddr_queued_event_t*)co_list_pop_front(&spihddr_events_list);
        spihddr_slave_event_handler(dma_spi_queued_event->dma_spi_event);
        ke_free(dma_spi_queued_event);
        ret_val = KEEP_POWERED;
    }

    if ( spihddr_outgoing_message_pending() ||  spihddr_slave_event_update_pending() )
    {
        ret_val = KEEP_POWERED;
    }

    if (NVIC_GetPendingIRQ(GPIO0_IRQn) != 0)
    {
        ret_val = KEEP_POWERED;
    }

    if (NVIC_GetPendingIRQ(GPIO1_IRQn) != 0)
    {
        ret_val = KEEP_POWERED;
    }

    if (spihddr_is_idle() == false)
    {
        ret_val = KEEP_POWERED;
    }

    return ret_val;
}

sleep_mode_t spihddr_validate_sleep(sleep_mode_t current_sleep_mode)
{
    sleep_mode_t ret_val = current_sleep_mode;

    if (spihddr_rx_done_pending == true)
    {
        ret_val = mode_active;
    }

    if (co_list_is_empty(&spihddr_events_list) == false)
    {
        ret_val = mode_active;
    }

    if ( spihddr_outgoing_message_pending() ||  spihddr_slave_event_update_pending() )
    {
        ret_val = mode_active;
    }

    return ret_val;
}

void SPI_Handler(void)
{
    ASSERT_ERROR(0); //Should never arrive here
}

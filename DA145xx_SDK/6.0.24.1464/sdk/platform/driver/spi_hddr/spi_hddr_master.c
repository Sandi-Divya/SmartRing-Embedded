/**
 ****************************************************************************************
 *
 * @file spihddr_master.c
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gpio.h"
#include <stdlib.h>
#include <stdint.h>
#include "spi_hddr.h"
#include "spi_hddr_master.h"
#include "user_periph_setup.h"
#include "spi.h"
#include "uart.h"
#include "user_app.h"
#include "gtl_eif.h"

app_list_elt_t *app_list_elt;

uint16_t spi_count_of_available_buffers = 0;
uint16_t spi_count_of_available_bytes = 0;
uint32_t read_requests_from_host_not_served_counter = 0;

SPI_Pad_t spihddr_master_CS_Pad;
spihddr_master_env_t spihddr_master_env;

struct app_list spihddr_master_outgoing_blemsg_list;
struct app_list spihddr_master_incoming_blemsg_list;

#if USE_SPIHDDR_CRC
uint32_t diag_spihddr_master_rx_crc_error_cnt __SECTION_ZERO("retention_mem_area0");
#endif

/**
 ****************************************************************************************
 * @brief Configuration of SPIHDDR structure
 ****************************************************************************************
*/
static const spi_cfg_t spi_cfg_master = {
    .spi_ms = SPI_MS_MODE,
    .spi_cp = SPI_CP_MODE,
    .spi_speed = SPI_SPEED_MODE,
    .spi_wsz = SPI_WSZ,
    .spi_cs = SPI_CS,
    .cs_pad.port = SPIHDDR_CS_PORT,
    .cs_pad.pin = SPIHDDR_CS_PIN,
#if defined (__DA14531__)
    .spi_capture = SPI_EDGE_CAPTURE,
#endif
#if defined (CFG_SPI_DMA_SUPPORT)
    .spi_dma_channel = SPI_DMA_CHANNEL_01,
    .spi_dma_priority = DMA_PRIO_0,
#endif
};

void spihddr_dready_pin_irq_enable(void)
{
  NVIC_DisableIRQ(GPIO1_IRQn);

  // set isr callback for button pressed interrupt
  GPIO_RegisterCallback(GPIO1_IRQn, spihddr_dready_pin_irq_callback);

  //Enable the IRQ for the SPIHDDR_DREADY signal (active high signal)
  GPIO_EnableIRQ(SPIHDDR_DREADY_PORT, SPIHDDR_DREADY_PIN, GPIO1_IRQn, false, true, 0);
}

void spihddr_dready_pin_irq_disable(void)
{
  NVIC_DisableIRQ(GPIO1_IRQn);
}

void spihddr_dready_pin_irq_callback(void)
{
    read_requests_from_host_not_served_counter++;
}

uint8_t spihddr_master_fsm_state_is_idle(void)
{
    return (spihddr_master_env.state == SPIHDDR_MASTER_STATE_IDLE);
}

void spihddr_master_init(void)
{
    spihddr_master_CS_Pad.pin = SPIHDDR_CS_PIN;
    spihddr_master_CS_Pad.port = SPIHDDR_CS_PORT;

    spi_cs_high();

    spi_initialize(&spi_cfg_master);

    spihddr_master_env.state = SPIHDDR_MASTER_STATE_IDLE;
}

void spihddr_master_tx(ble_msg *blemsg)
{
#if USE_SPIHDDR_CRC
    uint8_t crc = 0;
#endif
    uint16_t i;

    spihddr_dready_pin_irq_disable();

    spihddr_master_delay_before_cs_assertion();

    spi_cs_low();

    spihddr_master_delay_after_cs_assertion();

    while (spi_hddr_dready_getf() == false);

    spi_access(SPI_MASTER_HEADER_TX);   // send SPI_HEADER_TX

    while (spi_hddr_dready_getf() == true);

    spi_count_of_available_buffers = spi_access(0x00) & 0xFF;
    spi_count_of_available_buffers |= spi_access(0x00) << 8;

    spi_access(0x05);

#if USE_SPIHDDR_CRC
    crc ^= SPI_MASTER_HEADER_TX;
    crc ^= spi_count_of_available_buffers & 0xFF;
    crc ^= spi_count_of_available_buffers >> 8;
    crc ^= 5;
#endif

    // send message over spi
    for (i=0; i<blemsg->ble_hdr.bLength + sizeof(blemsg->ble_hdr); i++)
    {

#if USE_SPIHDDR_CRC
        crc ^= *((uint8_t*)blemsg);
#endif

        spi_access(*(((uint8_t*)blemsg)+i));
    }

#if USE_SPIHDDR_CRC
    spi_access(crc);
#endif

    spihddr_dready_pin_irq_enable();

    spi_cs_high();
}

void spihddr_master_rx(void)
{
    uint8_t rx_buffer[RD_DATA_MAX_LENGTH_IN_BYTES] __attribute__((aligned (4)));
    ble_msg spi_msg;
    uint32_t i;
    volatile uint8_t delay;

#if USE_SPIHDDR_CRC
    uint8_t sent_crc = 0;
    uint8_t calc_crc = 0;
#endif

    memset(rx_buffer, 0x00, sizeof(rx_buffer));
    memset((uint8_t*)&spi_msg, 0x00, sizeof(ble_msg));

    if (spi_hddr_dready_getf() == true)
    {
        spihddr_dready_pin_irq_disable();

        spihddr_master_delay_before_cs_assertion();

        spi_cs_low();

        spihddr_master_delay_after_cs_assertion();

        spi_access(SPI_MASTER_HEADER_RX);   // send SPI_HEADER_RX

        while (spi_hddr_dready_getf() == true);

        //spi_count_of_available_bytes = spi_access(0x00) & 0xFF;   // get avail. buffers L
        //spi_count_of_available_bytes |= spi_access(0x00) << 8;   // get avail. buffers H

        spi_count_of_available_bytes = spi_access(0x00) << 8;   // get avail. buffers H
        spi_count_of_available_bytes |= spi_access(0x00) & 0xFF;   // get avail. buffers L

        rx_buffer[0] = spi_access(0x00);

#if USE_SPIHDDR_CRC
        calc_crc ^= SPI_MASTER_HEADER_RX;
        calc_crc ^= spi_count_of_available_bytes & 0xFF;
        calc_crc ^= spi_count_of_available_bytes >> 8;
        calc_crc ^= rx_buffer[0];
#endif

        switch (rx_buffer[0])
        {
            case GTL_KE_MSG_TYPE:
                for (i=1; i < spi_count_of_available_bytes; i++)
                {
                    rx_buffer[i] = spi_access(0x00);
#if USE_SPIHDDR_CRC
                    calc_crc ^= rx_buffer[i];
#endif
                }

                memcpy((uint8_t*)&spi_msg, &rx_buffer[1], sizeof(hdr_t));

#if USE_SPIHDDR_CRC
                sent_crc = spi_access(0x00);
                ASSERT_WARNING( calc_crc == sent_crc);
                if ( calc_crc != sent_crc)
                {
                    diag_spihddr_master_rx_crc_error_cnt++;
                }
#endif

                spihddr_dready_pin_irq_enable();

                spi_cs_high();
                break;

            case HCI_EVT_MSG_TYPE:
            {
                uint8_t parameter_total_length = 0;

                for (i=1; i<3 + parameter_total_length ; i++)
                {
                    rx_buffer[i] = spi_access(0x00);
#if USE_SPIHDDR_CRC
                    calc_crc ^= rx_buffer[i];
#endif
                    if (i==2)
                    {
                        parameter_total_length = rx_buffer[2];
                    }
                }

                memcpy((uint8_t*)&spi_msg, &rx_buffer[1], 0x08);

#if USE_SPIHDDR_CRC
                sent_crc = spi_access(0x00);
                ASSERT_WARNING( calc_crc == sent_crc);
                if ( calc_crc != sent_crc)
                {
                    diag_spihddr_master_rx_crc_error_cnt++;
                }
#endif

                spihddr_dready_pin_irq_enable();

                spi_cs_high();
                break;
            }

            default:
                ASSERT_ERROR(0);
                break;
        }

        switch (rx_buffer[0])
        {
            case 0x05:
                app_list_elt = malloc(sizeof(app_list_elt_t) + spi_msg.ble_hdr.bLength - sizeof (uint8_t));
                ASSERT_ERROR(app_list_elt != NULL);
                memcpy(&app_list_elt->blemsg, &rx_buffer[1], sizeof(hdr_t) + spi_msg.ble_hdr.bLength);
                app_list_push_back(&spihddr_master_incoming_blemsg_list, &app_list_elt->hdr);
                break;

            case 0x04:
                break;

            default:
                break;
        }
    }
    else
    {
        ASSERT_WARNING(0);
    }
}

uint8_t spihddr_master_event_handler(spihddr_master_event_t event)
{
    int8_t retval = 1;
    switch(event)
    {
        case SPIHDDR_MASTER_EVENT_NONE:
        default:
            break;

        case SPIHDDR_MASTER_EVENT_INCOMING_MESSAGE_NOTIFICATION:
            switch(spihddr_master_env.state)
            {
                case SPIHDDR_MASTER_STATE_UNKNOWN:
                default:
                    ASSERT_ERROR(0);
                    break;

                case SPIHDDR_MASTER_STATE_IDLE:
                    spihddr_master_env.state = SPIHDDR_MASTER_STATE_RECEIVING;
                    spihddr_master_rx();
                    spihddr_master_env.state = SPIHDDR_MASTER_STATE_IDLE;
                    retval = 0;
                    break;

                case SPIHDDR_MASTER_STATE_RECEIVING:
                case SPIHDDR_MASTER_STATE_TRANSMITTING:
                    break;
            }
            break;

        case SPIHDDR_MASTER_EVENT_OUTGOING_MESSAGE_ADDED:
            switch(spihddr_master_env.state)
            {
                case SPIHDDR_MASTER_STATE_UNKNOWN:
                default:
                    ASSERT_ERROR(0);
                    break;

                case SPIHDDR_MASTER_STATE_IDLE:
                    if (!app_list_is_empty(&spihddr_master_outgoing_blemsg_list))
                    {
                        spihddr_master_env.state = SPIHDDR_MASTER_STATE_TRANSMITTING;
                        app_list_elt_t *app_list_elt = (app_list_elt_t*) app_list_pop_front(&spihddr_master_outgoing_blemsg_list);
                        spihddr_master_tx(&app_list_elt->blemsg);
                        free(app_list_elt);
                        spihddr_master_env.state = SPIHDDR_MASTER_STATE_IDLE;
                        retval = 0;
                    }
                    break;

                case SPIHDDR_MASTER_STATE_RECEIVING:
                    break;
                case SPIHDDR_MASTER_STATE_TRANSMITTING:
                    break;
            }
            break;
    }
    return retval;
}

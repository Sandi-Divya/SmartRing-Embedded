/**
 ****************************************************************************************
 * @addtogroup Drivers
 * @{
 * @addtogroup PDM
 * @{
 * @addtogroup PDM_MIC PDM MIC
 * @brief PDM MIC audio driver API
 * @{
 *
 * @file pdm_mic.h
 *
 * @brief PDM MIC audio interface driver.
 *
 * Copyright (C) 2017-2023 Renesas Electronics Corporation and/or its affiliates.
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

#if !defined (__DA14531__)

#if defined (CFG_PDM_DMA_SUPPORT)

#ifndef PDM_MIC_H_
#define PDM_MIC_H_

#include "pdm.h"
#include "dma.h"

/**
 \brief PDM interface sample rates. Possible values: PDM_8000, PDM_11025, PDM_16000, PDM_22050,
 PDM_32000, PDM_44100, PDM_48000, PDM_88200, PDM_96000, PDM_176400 or PDM_192000.
 */
typedef enum {
        PDM_8000 = 8000,
        PDM_11025 = 11025,
        PDM_16000 = 16000,
        PDM_22050 = 22050,
        PDM_32000 = 32000,
        PDM_44100 = 44100,
        PDM_48000 = 48000,
        PDM_88200 = 88200,
        PDM_96000 = 96000,
        PDM_176400 = 176400,
        PDM_192000 = 192000
} SRC_SampleRate_t;

/// Callback type to be associated with PDM mic.
typedef uint32_t * (*pdm_mic_data_available_cb)(uint16_t length);

/// PDM mic configuration struct
typedef struct pdm_mic_setup_s {
    /// Buffer to write data to
    uint32_t *buffer;

    /// Buffer length
    uint16_t buffer_length;

    /// Buffer mode
    bool buffer_circular;

    /// Interrupt threshold
    uint16_t int_thresold;

    /// PDM clock port-pin
    pdm_gpio_t clk_gpio;

    /// PDM data port-pin
    pdm_gpio_t data_gpio;

    /// Sampling rate
    SRC_SampleRate_t sampling_rate;

    /// PDM mic callback
    pdm_mic_data_available_cb   callback;

    /// PDM DMA channel
    DMA_ID dma_channel;
} pdm_mic_setup_t;

/**
 ****************************************************************************************
 * \brief       Start MIC recording
 *
 * \param [in]  config MIC configuration
 ****************************************************************************************
 */
void pdm_mic_start(pdm_mic_setup_t *config);

/**
 ****************************************************************************************
 * \brief       Stop MIC recording
 ****************************************************************************************
 */
void pdm_mic_stop(void);

#endif /* PDM_MIC_H_ */

#endif // CFG_PDM_DMA_SUPPORT

#endif // DA14585, DA14586

///@}
///@}
///@}

/**
 ****************************************************************************************
 * @addtogroup Drivers
 * @{
 * @addtogroup PDM
 * @brief PDM audio driver API
 * @{
 *
 * @file pdm.h
 *
 * @brief PDM audio interface driver.
 *
 * Copyright (C) 2017-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef PDM_H_
#define PDM_H_

#include <stdbool.h>
#include "gpio.h"

/**
 \brief PDM interface mode.
 */
typedef enum {
        /** PDM interface in slave mode, i.e. clocked externally. */
        PDM_MODE_SLAVE = 0,
        /** PDM interface in master mode, i.e. it provides the clock signal. */
        PDM_MODE_MASTER = 1,
} pdm_mode_t;

/**
 \brief PDM signal direction.
 */
typedef enum {
        PDM_DIRECTION_IN = 0, ///< PDM interface in receive mode.
        PDM_DIRECTION_OUT_RIGHT = 1, ///< PDM interface transmits on the right channel.
        PDM_DIRECTION_OUT_LEFT = 2, ///< PDM interface transmits on the left channel.
        PDM_DIRECTION_OUT_STEREO = 3, ///< PDM interface transmits right and left channels.
} pdm_direction_t;

/**
 \brief PDM output direction.
 \note \ref PDM_SRC_DIRECTION_PCM is not currently supported.
 */
typedef enum {
        PDM_SRC_DIRECTION_PCM, ///< The PDM input is directed to the PCM output registers.
        PDM_SRC_DIRECTION_REG, ///< The PDM input is directed to the SRC output registers.
} pdm_src_direction_t;

/**
 \brief PDM interface GPIO structure containing the GPIO port and the GPIO pin.
 */
typedef struct {
        GPIO_PORT port; ///< GPIO port.
        GPIO_PIN pin; ///< GPIO pin.
} pdm_gpio_t;

/**
 \brief PDM interrupt data structure. Used to return PDM data through the PDM callback function.
 Depending on the Direction of the PDM, the SRC in or out interrupt handler will call
 the callback function.
 */
typedef struct {
        /** Value of SRC1_OUT1_REG. 0 if PDM direction is not #PDM_DIRECTION_IN. */
        uint32_t src_out1_value;

        /** Value of SRC1_OUT2_REG. 0 if PDM direction is not #PDM_DIRECTION_IN. */
        uint32_t src_out2_value;

        /** Pointer to SRC1_IN1_REG. NULL if PDM direction is #PDM_DIRECTION_IN. */
        volatile uint32_t *src_in1_reg;

        /** Pointer to SRC1_IN2_REG. NULL if PDM direction is #PDM_DIRECTION_IN. */
        volatile uint32_t *src_in2_reg;
} pdm_src_isr_data_t;

/**
 \brief PDM interface SRC callback
 This function is called by the PDM interface whenever an SRC interrupt is fired.

 \param [in] src_isr_data PDM ISR data.
 */
typedef void (*pdm_src_interrupt_cb)(pdm_src_isr_data_t *src_isr_data);

/**
 * \brief PDM interface configuration.
 */
typedef struct {
        /** PDM clock GPIO pin. */
        pdm_gpio_t clk_gpio;

        /** PDM data GPIO pin. */
        pdm_gpio_t data_gpio;

        /** The mode of the PDM interface. Master or slave. */
        pdm_mode_t mode;

        /** The direction of the PDM interface. */
        pdm_direction_t direction;

        /**
         * The direction of the sample rate converter. For example, when direction is
         * #PDM_DIRECTION_IN and \ref src_direction is #PDM_SRC_DIRECTION_PCM the PDM
         * signal will be passed to the PCM interface. Or when the PDM direction is out and \ref
         * src_direction is #PDM_SRC_DIRECTION_REG the SRC I/O registers are used for PDM
         * input.
         */
        pdm_src_direction_t src_direction;

        /**
         * The sample rate of the sample rate converter (Hz). This is only used when \ref src_direction
         * is #PDM_SRC_DIRECTION_REG.
         */
        unsigned int src_sample_rate;

        /** This is used to bypass the sample rate converter out upsampling filters. */
        bool bypass_out_filter;

        /** This is used to bypass the sample rate converter in upsampling filters. */
        bool bypass_in_filter;

        /** This is used to enable the dithering feature of the sample rate converter. */
        bool enable_dithering;

        /**
         * This is used to set the PDM_DIV field of the PDM_DIV_REG. If this equals 0 the PDM_DIV
         * is set to 8.
         */
        uint8_t pdm_div;

        /** Enable the SRC interrupt for PDM samples. */
        bool enable_interrupt;

        /** The priority of the SRC interrupt. */
        uint8_t interrupt_priority;

        /** Callback function that is called when the SRC interrupt for PDM samples is fired. */
        pdm_src_interrupt_cb callback;
} pdm_config_t;

/**
 ****************************************************************************************
 * @brief Enables the PDM
 * @note The PD_PER domain shall be functional prior to enabling PDM:
 * SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
 * while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
 * @param[in] config The configuration of the PDM
 ****************************************************************************************
 */
void pdm_enable(const pdm_config_t *config);

/**
 ****************************************************************************************
 * @brief Disables the PDM
 ****************************************************************************************
 */ 
void pdm_disable(void);

#endif /* PDM_H_ */

#endif // DA14585, DA14586

///@}
///@}

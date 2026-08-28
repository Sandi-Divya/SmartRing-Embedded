/**
 ****************************************************************************************
 *
 * @file hw_otpc_531.c
 *
 * @brief DA14531 OTP Controller driver source file.
 *
 * Copyright (C) 2018-2025 Renesas Electronics Corporation and/or its affiliates.
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

#if defined (__DA14531__)

#include "hw_otpc.h"
#include "syscntl.h"
#include "arch.h"

/*
 * Local variables
 */

/// DCDC converter reservation status
bool dcdc_reserved       __SECTION_ZERO("retention_mem_area0");

// Required DA14531-01, DA14535 ROM symbols
#if (defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_HW_OTPC_531__)
extern syscntl_dcdc_state_t syscntl_dcdc_state;
extern void hw_otpc_init_ROM(bool *ptr_dcdc_reserved, syscntl_dcdc_state_t *ptr_dcdc_state);
extern void hw_otpc_disable_ROM(bool *ptr_dcdc_reserved, syscntl_dcdc_state_t *ptr_dcdc_state);
extern void hw_otpc_enter_mode_ROM(hw_otpc_mode_t mode);
#endif

#if (!defined (__DA14531_01__) && !defined (__DA14535__)) || defined (__EXCLUDE_ROM_HW_OTPC_531__)
/* Add specific TIM1 settings
 *  TIM1_CC_T_1US value  =  (1000ns * N Mhz / 1000) - 1
 *  TIM1_CC_T_20NS value =  (20ns   * N Mhz / 1000) - 1
 *  TIM1_CC_T_RD value   =  (60ns   * N Mhz / 1000) - 1
 *
 */
static const uint32_t tim1[] = {
     /* 2MHz */
    ( 0x01U << 0 ) |    // write @ 2MHz is not supported
    ( 0x00U << 8 ) |
    ( 0x00U << 12 ) |
    ( 0x09U << 16 ) |
    ( 0x09U << 20 ) |
    ( 0x09U << 24 ),
    /* 4MHz */
    ( 0x03U << 0 ) |
    ( 0x00U << 8 ) |
    ( 0x00U << 12 ) |
    ( 0x09U << 16 ) |
    ( 0x09U << 20 ) |
    ( 0x09U << 24 ),
    /* 8MHz */
    ( 0x07U << 0 ) |
    ( 0x00U << 8 ) |
    ( 0x00U << 12 ) |
    ( 0x09U << 16 ) |
    ( 0x09U << 20 ) |
    ( 0x09U << 24 ),
    /* 16MHz */
    ( 0x0FU << 0 ) |
    ( 0x00U << 8 ) |
#if defined (__DA14535__)
    ( 0x01U << 12 ) |
#elif defined (__DA14531__)
    ( 0x00U << 12 ) |
#endif
    ( 0x09U << 16 ) |
    ( 0x09U << 20 ) |
    ( 0x09U << 24 ),
};

/* TIM2 settings */
static const uint32_t tim2 = {
    /* default*/
    ( 0x09U << 0 ) |
    ( 0x00U << 5 ) |
    ( 0x04U << 8 ) |
    ( 0x04U << 16 ) |
    ( 0x00U << 21 ) |
    ( 0x04U << 24 ) |
    ( 0x01U << 29 ) |
    ( 0x01U << 31 )
};
#endif // __EXCLUDE_ROM_HW_OTPC_531__

/*
 * Forward declarations
 */

/*
 * Inline helpers
 */

/*
 * Assertion macros
 */

/*
 * Make sure that the OTP clock is enabled
 */
#define ASSERT_WARNING_OTP_CLK_ENABLED ASSERT_WARNING(GetBits16(CLK_AMBA_REG, OTP_ENABLE))

/*
 * Make sure that the cell address is valid
 */
#define ASSERT_CELL_OFFSET_VALID(off) ASSERT_WARNING(off < HW_OTP_CELL_NUM)

/*
 * Function definitions
 */

bool hw_otpc_is_dcdc_reserved(void)
{
    return dcdc_reserved;
}

void hw_otpc_clear_dcdc_reserved(void)
{
    dcdc_reserved = false;
}

#if (!defined (__DA14531_01__) && !defined (__DA14535__)) || defined (__EXCLUDE_ROM_HW_OTPC_531__)
/**
 ****************************************************************************************
 * @brief Configures the DCDC converter for OTP actions (read/write). Called only by
 * @p hw_otpc_enter_mode().
 * @param[in] level     The voltage level to be set.
 ****************************************************************************************
 */
#if (!USE_POWER_MODE_BYPASS)
static void otpc_dcdc_cfg(syscntl_dcdc_level_t level)
{
    if (level > syscntl_dcdc_get_level())
    {
        syscntl_dcdc_set_level(level);
    }
    // Check DCDC converter status
    if (!GetBits16(DCDC_CTRL_REG, DCDC_ENABLE))
    {
        // Enable the DCDC converter
        SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 1);

        // Wait for the indication that VBAT_HIGH (boost mode) is OK
        while (!GetBits(ANA_STATUS_REG, DCDC_OK));
    }
}
#endif
#endif // __EXCLUDE_ROM_HW_OTPC_531__

#if (defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_HW_OTPC_531__) && (!USE_POWER_MODE_BYPASS)
void hw_otpc_init(void)
{
    hw_otpc_init_ROM(&dcdc_reserved, &syscntl_dcdc_state);
}
#else
void hw_otpc_init(void)
{
    GLOBAL_INT_DISABLE();

#if (!USE_POWER_MODE_BYPASS)
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
        extern syscntl_dcdc_state_t syscntl_dcdc_state;

        // OTPC has higher priority than GPIOs when it requests the DCDC converter
        dcdc_reserved = true;

        syscntl_dcdc_state.level = syscntl_dcdc_get_level();
        syscntl_dcdc_state.status = GetBits16(DCDC_CTRL_REG, DCDC_ENABLE);
    }
#endif

    /*
     * Enable OTPC clock
     */
    SetBits16(CLK_AMBA_REG, OTP_ENABLE, 0x01);

    /*
     * Set OTPC to deep stand-by mode
     */
    SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_DSTBY);
    hw_otpc_wait_mode_change();

    /*
     * Configure OTPC timings (by defualt 16MHz clock)
     */
    hw_otpc_set_speed(HW_OTPC_CLK_FREQ_16MHz);

    GLOBAL_INT_RESTORE();
}
#endif // __EXCLUDE_ROM_HW_OTPC_531__

#if (defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_HW_OTPC_531__) && (!USE_POWER_MODE_BYPASS)
void hw_otpc_enter_mode(hw_otpc_mode_t mode)
{
    hw_otpc_enter_mode_ROM(mode);
}
#else
void hw_otpc_enter_mode(hw_otpc_mode_t mode)
{
    volatile hw_otpc_mode_t current_mode;

    /*change mode only if new mode is different than the old one*/
    current_mode = (hw_otpc_mode_t) GetBits32(OTPC_MODE_REG, OTPC_MODE_MODE);
    if (mode != current_mode)
    {
#if (!USE_POWER_MODE_BYPASS)
        if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
        {
            switch(mode)
            {
                case HW_OTPC_MODE_READ:
                case HW_OTPC_MODE_AREAD:
                {
                    // In boost mode set OTP voltage for read (must be > 1.62V)
                    otpc_dcdc_cfg(SYSCNTL_DCDC_LEVEL_1V8);
                    break;
                }
                case HW_OTPC_MODE_PVFY:
                case HW_OTPC_MODE_RINI:
                {
#if defined (__DA14535__)
                    // In boost mode set OTP voltage for margin read (must be 2.5V)
                    otpc_dcdc_cfg(SYSCNTL_DCDC_LEVEL_2V5);
#else
                    // In boost mode set OTP voltage for margin read (must be > 1.62V)
                    otpc_dcdc_cfg(SYSCNTL_DCDC_LEVEL_1V8);
#endif
                    break;
                }
                case HW_OTPC_MODE_PROG:
                {
                    // In boost mode set OTP voltage for program (must be > 2.25V)
                    otpc_dcdc_cfg(SYSCNTL_DCDC_LEVEL_2V5);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
#endif
        // Change the operation mode of the OTP controller
        SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, mode);
        hw_otpc_wait_mode_change();
    }
}
#endif // // __EXCLUDE_ROM_HW_OTPC_531__

#if (!defined (__DA14531_01__) && !defined (__DA14535__)) || defined (__EXCLUDE_ROM_HW_OTPC_531__)
void hw_otpc_set_speed(hw_otpc_clk_freq_t clk_speed)
{
    ASSERT_WARNING_OTP_CLK_ENABLED;

    /*
     * Set access speed
     */
    SetWord32(OTPC_TIM1_REG, tim1[(uint8_t) clk_speed]);
    SetWord32(OTPC_TIM2_REG, tim2);
}

bool hw_otpc_word_prog_and_verify(uint32_t wdata, uint32_t cell_offset)
{

    ASSERT_CELL_OFFSET_VALID(cell_offset);

    ASSERT_WARNING_OTP_CLK_ENABLED;

    hw_otpc_word_prog(wdata, cell_offset);

    hw_otpc_enter_mode(HW_OTPC_MODE_PVFY);
    if (wdata != *(uint32_t *)(MEMORY_OTP_BASE + HW_OTP_CELL_SIZE * cell_offset))
    {
        return false;
    }

    hw_otpc_enter_mode(HW_OTPC_MODE_RINI);
    if (wdata != *(uint32_t *)(MEMORY_OTP_BASE + HW_OTP_CELL_SIZE * cell_offset))
    {
        return false;
    }

    return true;
}

uint32_t hw_otpc_word_read(uint32_t cell_offset)
{
    ASSERT_CELL_OFFSET_VALID(cell_offset);

    ASSERT_WARNING_OTP_CLK_ENABLED;

    hw_otpc_enter_mode(HW_OTPC_MODE_READ);
    return *(uint32_t *)(MEMORY_OTP_BASE + HW_OTP_CELL_SIZE * cell_offset);
}

void hw_otpc_prog(uint32_t *p_data, uint32_t cell_offset, uint32_t num_of_words)
{
    uint32_t i;

    ASSERT_WARNING_OTP_CLK_ENABLED;
    ASSERT_CELL_OFFSET_VALID(cell_offset + num_of_words - 1);

    hw_otpc_enter_mode(HW_OTPC_MODE_PROG);

    for (i = 0; i < num_of_words; i++)
    {
        SetWord32(OTPC_PWORD_REG, *p_data++);
        SetWord32(OTPC_PADDR_REG, cell_offset++);
        hw_otpc_wait_while_programming_buffer_is_full();
    }
    hw_otpc_wait_while_busy_programming();
}

static bool hw_otpc_read_verif(uint32_t *w_data, uint32_t cell_offset, uint32_t num_of_words, hw_otpc_mode_t mode)
{
    uint32_t i;

    ASSERT_WARNING_OTP_CLK_ENABLED;

    hw_otpc_enter_mode(mode);

    for (i = 0; i < num_of_words; i++)
    {
        if (*w_data != *(uint32_t *)(MEMORY_OTP_BASE + HW_OTP_CELL_SIZE * cell_offset))
        {
            return false;
        }
        cell_offset++;
        w_data++;
    }
    return true;
}

bool hw_otpc_prog_and_verify(uint32_t *p_data, uint32_t cell_offset, uint32_t num_of_words)
{
    ASSERT_WARNING_OTP_CLK_ENABLED;

    hw_otpc_prog(p_data, cell_offset, num_of_words);

    if (false == hw_otpc_read_verif(p_data, cell_offset, num_of_words, HW_OTPC_MODE_PVFY))
    {
        return false;
    }
    if (false == hw_otpc_read_verif(p_data, cell_offset, num_of_words, HW_OTPC_MODE_RINI))
    {
        return false;
    }
    hw_otpc_enter_mode(HW_OTPC_MODE_PROG);
    return true;
}

void hw_otpc_read(uint32_t *p_data, uint32_t cell_offset, uint32_t num_of_words)
{
    uint32_t i;

    ASSERT_WARNING_OTP_CLK_ENABLED;

    ASSERT_CELL_OFFSET_VALID(cell_offset + num_of_words - 1);

    hw_otpc_enter_mode(HW_OTPC_MODE_READ);

    for (i = 0; i < num_of_words; i++)
    {
        *p_data = *(uint32_t *)(MEMORY_OTP_BASE + HW_OTP_CELL_SIZE * cell_offset);
        p_data++;
        cell_offset++;
    }
}
#endif // __EXCLUDE_ROM_HW_OTPC_531__

#if (defined (__DA14531_01__) || defined (__DA14535__)) && !defined (__EXCLUDE_ROM_HW_OTPC_531__) && (!USE_POWER_MODE_BYPASS)
void hw_otpc_disable(void)
{
    hw_otpc_disable_ROM(&dcdc_reserved, &syscntl_dcdc_state);
}
#else
void hw_otpc_disable(void)
{
    GLOBAL_INT_DISABLE();

    /*
     * Enable OTPC clock
     */
    SetBits16(CLK_AMBA_REG, OTP_ENABLE, 0x01);

     /*
     * set OTPC to stand-by mode
     */
    SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_DSTBY);
    hw_otpc_wait_mode_change();

    SetWord32(OTPC_TIM1_REG, OTPC_TIM1_REG_RESET);
    SetWord32(OTPC_TIM2_REG, OTPC_TIM2_REG_RESET);

    /*
     * Disable OTPC clock
     */
    SetBits16(CLK_AMBA_REG, OTP_ENABLE, 0x00);

#if (!USE_POWER_MODE_BYPASS)
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
        extern syscntl_dcdc_state_t syscntl_dcdc_state;

        dcdc_reserved = false;

        // Reload DCDC converter voltage level value
        syscntl_dcdc_set_level(syscntl_dcdc_state.level);

        if (!syscntl_dcdc_state.status)
        {
            // Disable the DCDC
            SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 0);
        }
    }
#endif

    GLOBAL_INT_RESTORE();
}
#endif // __EXCLUDE_ROM_HW_OTPC_531__

#endif // __DA14531__

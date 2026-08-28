/**
 ****************************************************************************************
 *
 * @file arch_main.c
 *
 * @brief Main loop of the application.
 *
 * Copyright (C) 2012-2025 Renesas Electronics Corporation and/or its affiliates.
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
 * INCLUDES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>     // standard definitions
#include <stdbool.h>    // boolean definition
#include "compiler.h"
#include "arch.h"
#include "arch_api.h"
#include "boot.h"       // boot definition
#include "rwip.h"       // BLE initialization
#include "syscntl.h"    // System control initialization
#include "em_map_ble.h"
#include "ke_mem.h"
#include "ke_event.h"
#include "user_periph_setup.h"

#include "uart.h"   // UART initialization
#include "nvds.h"   // NVDS initialization
#include "rf.h"     // RF initialization
#include "app.h"    // application functions
#include "dbg.h"    // For dbg_warning function

#include "datasheet.h"

#include "em_map_ble.h"

#include "lld_sleep.h"
#include "rwble.h"
#include "rf_585.h"
#include "gpio.h"
#include "hw_otpc.h"

#include "lld_evt.h"
#include "arch_console.h"

#include "arch_system.h"

#include "system_library.h"

#include "arch_wdg.h"

#include "user_callback_config.h"

#include "ea.h"

#include "arch_ram.h"

#if defined (__DA14531__)
#include "otp_cs.h"
#include "adc.h"
#endif

#if (USE_RANGE_EXT)
#include "range_ext_api.h"
#endif

#if (WLAN_COEX_ENABLED)
#include "wlan_coex.h"
#endif

/**
 * @addtogroup DRIVERS
 * @{
 */

/*
 * DEFINES
 ****************************************************************************************
 */

#if !defined (__DA14531__)
#if defined (CFG_RETAIN_RAM_1_BLOCK)
#define RAM_1_BLOCK                 (0x01)
#else
#define RAM_1_BLOCK                 (0)
#endif

#if defined (CFG_RETAIN_RAM_2_BLOCK)
#define RAM_2_BLOCK                 (0x02)
#else
#define RAM_2_BLOCK                 (0)
#endif

#if defined (CFG_RETAIN_RAM_3_BLOCK)
#define RAM_3_BLOCK                 (0x04)
#else
#define RAM_3_BLOCK                 (0)
#endif

#define RAM_4_BLOCK                 (0x08)
#endif

#if defined (__DA14531__)
#define ALL_RAM_BLOCKS              (0)
#define RAM1_DO_NOT_RETAIN          (1<<0)
#define RAM2_DO_NOT_RETAIN          (1<<2)
#define RAM3_DO_NOT_RETAIN          (1<<4)
#else
#define ALL_RAM_BLOCKS              (RAM_1_BLOCK | RAM_2_BLOCK | RAM_3_BLOCK | RAM_4_BLOCK)
#endif

/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

extern struct arch_sleep_env_tag sleep_env;

extern last_ble_evt arch_rwble_last_event;

#if defined (__DA14531__)
/// Structure containing info about debugger configuration and OTP CS registers
/// normally handled by the Booter - defined in otp_cs.c
extern otp_cs_booter_val_t booter_val;
#endif

#if defined (CFG_AUTO_DETECT_NON_RET_HEAP) && (DO_NOT_RETAIN_ALL_RAM_BLOCKS)
static uint32_t ret_mode_for_non_ret_heap       __SECTION_ZERO("retention_mem_area0");
#endif
#if (DO_NOT_RETAIN_ALL_RAM_BLOCKS)
static bool reinit_non_ret_heap            __SECTION_ZERO("retention_mem_area0");
#endif

/*
 * LOCAL FUNCTION DECLARATIONS
 ****************************************************************************************
 */
__STATIC_INLINE void otp_prepare(uint32_t code_size);
__STATIC_INLINE bool ble_is_powered(void);
__STATIC_INLINE void ble_turn_radio_off(void);
__STATIC_INLINE void schedule_while_ble_on(void);
__STATIC_INLINE void arch_turn_peripherals_off(sleep_mode_t current_sleep_mode);
__STATIC_INLINE void arch_goto_sleep(sleep_mode_t current_sleep_mode);
__STATIC_INLINE void arch_switch_clock_goto_sleep(sleep_mode_t current_sleep_mode);
__STATIC_INLINE void arch_resume_from_sleep(void);
__STATIC_INLINE sleep_mode_t rwip_power_down(void);
__STATIC_INLINE arch_main_loop_callback_ret_t app_asynch_trm(void);
__STATIC_INLINE arch_main_loop_callback_ret_t app_asynch_proc(void);
__STATIC_INLINE void app_asynch_sleep_proc(void);
__STATIC_INLINE void app_sleep_prepare_proc(sleep_mode_t *sleep_mode);
__STATIC_INLINE void app_sleep_exit_proc(void);
__STATIC_INLINE void app_sleep_entry_proc(sleep_mode_t sleep_mode);

__STATIC_INLINE void init_retention_mode(void);
#if !defined (__DA14531__)
__STATIC_INLINE void set_ldo_ret_trim(uint8_t retained_ram_blocks);
#endif
#if defined (CFG_AUTO_DETECT_NON_RET_HEAP) && (DO_NOT_RETAIN_ALL_RAM_BLOCKS)
__STATIC_INLINE uint32_t get_retention_mode_non_ret_heap(void);
#endif

#if (ENABLE_WAKEUP_METRICS) && defined (CFG_PRINTF)
#include "arch_console.h"

uint32_t print_cnt          __SECTION_ZERO("retention_mem_area0");
extern uint32_t wakeup_cnt;
extern uint32_t wakeup_delayed;
extern uint32_t stats_array[];
extern int max_diff_lp;
extern int max_diff_us;

#endif

#if ((UART_FLOW_CTRL_WAKEUP) && (!BLE_APP_PRESENT))
#include "ext_wakeup_uart.h"
extern bool ext_wakeup_pending_req_active(void);
extern void ext_wakeup_pending_req_sleep_prep(sleep_mode_t *sleep_mode);
#endif

/*
 * MAIN FUNCTION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief BLE main function.
 *        This function is called right after the booting process has completed.
 *        It contains the main function loop.
 ****************************************************************************************
 */
int main(void);

int main(void)
{
    sleep_mode_t sleep_mode;

    // initialize retention mode
    init_retention_mode();

    //global initialise
    system_init();

    /*
     ************************************************************************************
     * Platform initialization
     ************************************************************************************
     */

    while(1)
    {
        do {
            // schedule all pending events
            schedule_while_ble_on();
        }
        while (app_asynch_proc() != GOTO_SLEEP);    //grant control to the application, try to go to power down
                                                    //if the application returns GOTO_SLEEP

        //wait for interrupt and go to sleep if this is allowed
        if (((!BLE_APP_PRESENT) && (check_gtl_state())) || (BLE_APP_PRESENT))
        {
            //Disable the interrupts
            GLOBAL_INT_STOP();

            app_asynch_sleep_proc();

            // get the allowed sleep mode
            // time from rwip_power_down() to __WFI() must be kept as short as possible!!
            sleep_mode = rwip_power_down();

            if ((sleep_mode == mode_ext_sleep) || (sleep_mode == mode_ext_sleep_otp_copy))
            {
                //power down the radio and whatever is allowed
                arch_goto_sleep(sleep_mode);

                // In extended sleep mode the watchdog timer is disabled
                // (power domain PD_SYS is automatically OFF). However, if the debugger
                // is attached the watchdog timer remains enabled and must be explicitly
                // disabled.
                if ((GetWord16(SYS_STAT_REG) & DBG_IS_UP) == DBG_IS_UP)
                {
                    wdg_freeze();    // Stop watchdog timer
                }

                //wait for an interrupt to resume operation
                __WFI();

                if ((GetWord16(SYS_STAT_REG) & DBG_IS_UP) == DBG_IS_UP)
                {
                    wdg_resume();    // Resume watchdog timer
                }

                //resume operation
                arch_resume_from_sleep();
            }
            else if (sleep_mode == mode_idle)
            {
                if (((!BLE_APP_PRESENT) && check_gtl_state()) || (BLE_APP_PRESENT))
                {
                    //wait for an interrupt to resume operation
                    __WFI();
                }
            }
            // restore interrupts
            GLOBAL_INT_START();
        }
        wdg_reload(WATCHDOG_DEFAULT_PERIOD);
    }
}

/**
 ****************************************************************************************
 * @brief Power down the BLE Radio and whatever is allowed according to the sleep mode and
 *        the state of the system and application
 * @param[in] current_sleep_mode The current sleep mode proposed by the application.
 ****************************************************************************************
 */
__STATIC_INLINE void arch_goto_sleep(sleep_mode_t current_sleep_mode)
{
    sleep_mode_t sleep_mode = current_sleep_mode;
#if (USE_RANGE_EXT)
    // Disable range extender
    range_ext.disable(NULL);
#endif

#if (WLAN_COEX_ENABLED)
    // Drive to inactive state the pin used for the BLE event in progress signal
    wlan_coex_set_ble_eip_pin_inactive();
#endif

    // Turn the radio off
    ble_turn_radio_off();

    // Grant access to the application to check if we can go to sleep
    app_sleep_prepare_proc(&sleep_mode); // SDK Improvements for uniformity this one should be changed?

#if ((UART_FLOW_CTRL_WAKEUP) && (!BLE_APP_PRESENT))
    // Grant access to the external wake up mechanism if we can go to sleep
    ext_wakeup_pending_req_sleep_prep(&sleep_mode);
#endif
    
    // Turn the peripherals off according to the current sleep mode
    arch_turn_peripherals_off(sleep_mode);

    // Hook for app specific tasks just before sleeping
    app_sleep_entry_proc(sleep_mode);

#if defined (__DA14531__)
    // Buck mode: switch on LDO_LOW or DC/DC depending on used DA1453x
    if (!GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
#if defined (__DA14535__)
        /*
         * In buck mode make sure that the DC/DC will be enabled in sleep.
         * During sleep, the VBAT_LOW will be supplied by the DC/DC instead
         * of the LDO_LOW, as in DA14531, DA14531-01.
         */
        SetBits16(DCDC_SLP_CTRL_REG, DCDC_SLP_ENABLE, 1);

        /*
         * When waking up from sleep and using the DC/DC in sleep mode, there
         * will be a short time during which the VBAT_LOW is undriven
         * (buck mode). This is specifically the case when the lpclk is RCX.
         * To make sure this does not happen, the following register needs to
         * be programmed just before sleep.
         */
        SetBits16(POWER_CTRL_REG, LDO_LOW_CTRL_REG, 0);
#else
        /*
         * In buck mode make sure that LDO_LOW is on before going to sleep.
         * Set the LDO_LOW in low current mode.
         * The DC/DC will be automatically disabled in sleep.
         */
        SetBits16(POWER_CTRL_REG, LDO_LOW_CTRL_REG, 2);
#endif
    }

#if !defined (__DA14535__)
    // Set for proper RCX operation
    SetBits16(GP_DATA_REG, FORCE_RCX_VDD | FORCE_RCX_VREF, 2);
#endif
#endif

    // Do the last house keeping of the clocks and go to sleep
    arch_switch_clock_goto_sleep(sleep_mode);
}

/**
 ****************************************************************************************
 * @brief Manage the clocks and go to sleep.
 * @param[in] current_sleep_mode The current sleep mode proposed by the system so far
 ****************************************************************************************
 */
__STATIC_INLINE void arch_switch_clock_goto_sleep(sleep_mode_t current_sleep_mode)
{
    if ( (current_sleep_mode == mode_ext_sleep) || (current_sleep_mode == mode_ext_sleep_otp_copy) )
    {
#if !defined (__DA14531__)
        SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 0);      // Set BIAS to '0' if sleep has been decided
#endif

#if (USE_POWER_OPTIMIZATIONS)
#if defined (__DA14531__)
    SetBits16(CLK_RC32M_REG, RC32M_DISABLE, 0);             // Enable RC32M
#else
    SetBits16(CLK_16M_REG, RC16M_ENABLE, 1);                // Enable RC16M
#endif

    arch_asm_delay_us(13);

    SetBits16(CLK_CTRL_REG, SYS_CLK_SEL, 1);                // Switch to RC16M (DA14585/586) or RC32M (DA14531)

#if defined (__DA14531__)
  #if !defined (__DA14535__)
    while( (GetWord16(CLK_CTRL_REG) & RUNNING_AT_RC32M) == 0 );
    SetWord16(CLK_FREQ_TRIM_REG, 50);                       // Set value 50 to CLK_FREQ_TRIM_REG
  #endif
#else
    while( (GetWord16(CLK_CTRL_REG) & RUNNING_AT_RC16M) == 0 );
    SetWord16(CLK_FREQ_TRIM_REG, 0);                        // Set zero value to CLK_FREQ_TRIM_REG
#endif
    // Do not disable the XTAL16M (DA14585/586) or XTAL32M (DA14531, DA14531-01, DA14535).
    // It will be disabled when we sleep...
#endif // (USE_POWER_OPTIMIZATIONS)
    }
}

/**
 ****************************************************************************************
 * @brief  An interrupt came, resume from sleep.
 ****************************************************************************************
 */
__STATIC_INLINE void arch_resume_from_sleep(void)
{
#if defined (__DA14531__)

     // Turn off LDO_LOW (can be turned later by application code if required)
     SetBits16(POWER_CTRL_REG, LDO_LOW_CTRL_REG, 1);

#if !defined (__DA14535__)
    // Set for proper RCX operation
    SetBits16(GP_DATA_REG, FORCE_RCX_VDD | FORCE_RCX_VREF, 1);
#endif
#endif

#if (DO_NOT_RETAIN_ALL_RAM_BLOCKS)
    // Check if non retained heap should be re-initialized
    if (reinit_non_ret_heap)
    {
        ke_mem_init(KE_MEM_NON_RETENTION, (uint8_t*)(rom_cfg_table[rwip_heap_non_ret_pos]), rom_cfg_table[rwip_heap_non_ret_size]);
    }
#endif

#if defined (__DA14531__)
    // Re-initialize peripherals and pads
    periph_init();
#endif

    // Hook for app specific tasks just after waking up
    app_sleep_exit_proc( );

#if ((EXTERNAL_WAKEUP) && (!BLE_APP_PRESENT)) // external wake up, only in external processor designs
    ext_wakeup_resume_from_sleep();
#endif

#if defined (__DA14533__)
    // Setting extracted from `set_recommended_settings()`
    SetBits16(GP_ADC_CTRL3_REG, GP_ADC_EN_DEL, 0x50);
#endif

    // restore ARM Sleep mode
    // reset SCR[2]=SLEEPDEEP bit else the mode=idle __WFI() will cause a deep sleep
    // instead of a processor halt
    SCB->SCR &= ~(1<<2);
}

/**
 ****************************************************************************************
 * @brief Check if the BLE module is powered on.
 ****************************************************************************************
 */
__STATIC_INLINE bool ble_is_powered()
{
    return ((GetBits16(CLK_RADIO_REG, BLE_ENABLE) == 1) &&
            (GetBits32(BLE_DEEPSLCNTL_REG, DEEP_SLEEP_STAT) == 0) &&
            !(rwip_prevent_sleep_get() & RW_WAKE_UP_ONGOING));
}

/**
 ****************************************************************************************
 * @brief Call the scheduler if the BLE module is powered on.
 ****************************************************************************************
 */
__STATIC_INLINE void schedule_while_ble_on(void)
{
    // BLE clock is enabled
    while (ble_is_powered())
    {
        //execute messages and events
        rwip_schedule();
#if defined (__DA14531__)
        rcx20_read_freq(false);
#endif

#if !defined(__FPGA__)
        if (arch_rwble_last_event == BLE_EVT_END)
        {
#if defined (__DA14531__)
            rcx20_read_freq(true);
#else
            rcx20_read_freq();
#endif

            uint32_t sleep_duration = 0;
            //if you have enough time run a temperature calibration of the radio
            if (ea_sleep_check(&sleep_duration, 4)) //4 slots -> 2.5 ms
            {
                // check time and temperature to run radio calibrations.
                conditionally_run_radio_cals();
            }
        }
#endif

#if (ENABLE_WAKEUP_METRICS) && defined (CFG_PRINTF)
        if(wakeup_cnt == print_cnt)
        {
            arch_printf("W %d - F %d \n\r", wakeup_cnt, wakeup_delayed);
            print_cnt += PRINT_EVERY_X_EVENTS;
            
            arch_printf("1-500us %d, 500-1ms %d, 1-1.25ms %d, 1.25-1.5ms %d, 1.5-2ms %d, 2-2.5ms %d, 2.5-3ms %d, <3ms %d \n\r", stats_array[0], 
                                                                                                                                stats_array[1],
                                                                                                                                stats_array[2],
                                                                                                                                stats_array[3],
                                                                                                                                stats_array[4],
                                                                                                                                stats_array[5],
                                                                                                                                stats_array[6],
                                                                                                                                stats_array[7]);
            arch_printf("max lp: %d, max us: %d \n\r", max_diff_lp, max_diff_us);
        }
#endif // ENABLE_WAKEUP_METRICS

#if ((UART_FLOW_CTRL_WAKEUP) && (!BLE_APP_PRESENT))
        if (ext_wakeup_pending_req_active())
            continue; // keep the device actively awake due to pending request
#endif
        //grant control to the application, try to go to sleep
        //if the application returns GOTO_SLEEP
        if (app_asynch_trm() != GOTO_SLEEP)
        {
            continue; // so that rwip_schedule() is called again
        }
        else
        {
            arch_printf_process();
            break;
        }
    }
}

/**
 ****************************************************************************************
 * @brief Power down the ble ip if possible.
 * @return sleep_mode_t return the current sleep mode
 ****************************************************************************************
 */
__STATIC_INLINE sleep_mode_t rwip_power_down(void)
{
    sleep_mode_t sleep_mode;

#if ((EXTERNAL_WAKEUP) && (!BLE_APP_PRESENT)) // External wake up, only in external processor designs
    ext_wakeup_enable(EXTERNAL_WAKEUP_GPIO_PORT, EXTERNAL_WAKEUP_GPIO_PIN, EXTERNAL_WAKEUP_GPIO_POLARITY);
#endif

    // if app has turned sleep off, rwip_sleep() will act accordingly
    // time from rwip_sleep() to __WFI() must be kept as short as possible!
    sleep_mode = rwip_sleep();

    // BLE is sleeping ==> app defines the mode
    if (sleep_mode == mode_sleeping)
    {
        if (sleep_env.slp_state == ARCH_EXT_SLEEP_ON)
        {
            sleep_mode = mode_ext_sleep;
        }
        else
        {
            sleep_mode = mode_ext_sleep_otp_copy;
        }
    }
#if ((EXTERNAL_WAKEUP) && (!BLE_APP_PRESENT))
    else
    {
        // Disable external wake up
        ext_wakeup_disable();
    }
#endif

    return (sleep_mode);
}

/**
 ****************************************************************************************
 * @brief Turn the radio off.
 ****************************************************************************************
 */
__STATIC_INLINE void ble_turn_radio_off(void)
{
    SetBits16(PMU_CTRL_REG, RADIO_SLEEP, 1); // turn off radio
}

/**
 ****************************************************************************************
 * @brief Initialize retention mode.
 ****************************************************************************************
 */
__STATIC_INLINE void init_retention_mode(void)
{
#if (DO_NOT_RETAIN_ALL_RAM_BLOCKS)
#if defined (CFG_AUTO_DETECT_NON_RET_HEAP)
    // RAM retention mode for the non retainable heap data only
    ret_mode_for_non_ret_heap = get_retention_mode_non_ret_heap();
#endif
#endif
}

#if defined (CFG_AUTO_DETECT_NON_RET_HEAP) && (DO_NOT_RETAIN_ALL_RAM_BLOCKS)
/**
 ****************************************************************************************
 * @brief Find which RAM blocks are used by the non-retained heap, based on the
 *        non-retained heap base address and length, and return the respective
 *        RAM retention mode.
 * @return the retention mode (the RAM blocks to be retained)
 * @note The last RAM block will be always retained by SDK, since it holds the
 *       BLE data and ROM data.
 *       Hence, the function ignores the case where the not-retained heap is placed
 *       only at last RAM block.
  ****************************************************************************************
 */
__STATIC_INLINE uint32_t get_retention_mode_non_ret_heap()
{
    uint32_t non_ret_heap_base = NON_RET_HEAP_BASE;
    uint32_t non_ret_heap_length = NON_RET_HEAP_LENGTH;

#if defined (__DA14535__)
    if (non_ret_heap_base < RAM_2_BASE_ADDR)
    {
        return 0xFC;   // Retain RAM1
    }
    return 0x0F;
#elif defined (__DA14531__)
    if (non_ret_heap_base >= RAM_2_BASE_ADDR && non_ret_heap_base < RAM_3_BASE_ADDR)
    {
        return (0xF3);              // Retain RAM2
    }

    if (non_ret_heap_base < RAM_2_BASE_ADDR)
    {
        if (non_ret_heap_base + non_ret_heap_length > RAM_2_BASE_ADDR)
        {
            return (0xFC & 0xF3);   // Retain RAM1, RAM2
        }
        else
        {
            return 0xFC;            // Retain RAM1
        }
    }
    return 0x0F;
#else
    if (non_ret_heap_base >= RAM_4_BASE_ADDR)
    {
        return (RAM_4_BLOCK);
    }
    else if (non_ret_heap_base >= RAM_3_BASE_ADDR)
    {
        if (non_ret_heap_base + non_ret_heap_length < RAM_4_BASE_ADDR)
        {
            return (RAM_3_BLOCK);
        }
        else
        {
            return (RAM_3_BLOCK | RAM_4_BLOCK);
        }
    }
    else if (non_ret_heap_base >= RAM_2_BASE_ADDR)
    {
        if (non_ret_heap_base + non_ret_heap_length < RAM_3_BASE_ADDR)
        {
            return (RAM_2_BLOCK);
        }
        else if (non_ret_heap_base + non_ret_heap_length < RAM_4_BASE_ADDR)
        {
            return (RAM_2_BLOCK | RAM_3_BLOCK);
        }
        else
        {
            return (RAM_2_BLOCK | RAM_3_BLOCK | RAM_4_BLOCK);
        }
    }
    else
    {
        if (non_ret_heap_base + non_ret_heap_length < RAM_2_BASE_ADDR)
        {
            return (RAM_1_BLOCK);
        }
        else if (non_ret_heap_base + non_ret_heap_length < RAM_3_BASE_ADDR)
        {
            return (RAM_1_BLOCK | RAM_2_BLOCK);
        }
        else if (non_ret_heap_base + non_ret_heap_length < RAM_4_BASE_ADDR)
        {
            return (RAM_1_BLOCK | RAM_2_BLOCK | RAM_3_BLOCK);
        }
        else
        {
            return ALL_RAM_BLOCKS;
        }
    }
#endif
}
#endif // CFG_AUTO_DETECT_NON_RET_HEAP

#if !defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Set required LDO_RET_TRIM value in BANDGAP_REG. This value depends on the
 *        size of retained RAM and the temperature range device operation.
 * @param[in] retained_ram_blocks RAM blocks to be retained.
 * @note By default the [BANDGAP_REG.LDO_RET_TRIM] is loaded with the respective OTP
 *       header value (done by ROM booter).
 ****************************************************************************************
 */
__STATIC_INLINE void set_ldo_ret_trim(uint8_t retained_ram_blocks)
{
#if (USE_MID_TEMPERATURE)
    SetBits16(BANDGAP_REG, LDO_RET_TRIM, DEFAULT_LDO_SET);
#elif (USE_HIGH_TEMPERATURE || USE_EXT_TEMPERATURE)
    if ((retained_ram_blocks == RAM_SIZE_80KB_OPT1) || (retained_ram_blocks == RAM_SIZE_80KB_OPT2) || (retained_ram_blocks == RAM_SIZE_96KB_OPT1))
    {
            SetBits16(BANDGAP_REG, LDO_RET_TRIM, DEFAULT_LDO_SET_96K);  // LDO trim value up to 96KB retained RAM
    }
    else
    {
            SetBits16(BANDGAP_REG, LDO_RET_TRIM, DEFAULT_LDO_SET_64K);  // LDO trim value up to 64KB retained RAM
    }
#endif
}
#endif

/**
 ****************************************************************************************
 * @brief  Turn the peripherals off according to the current sleep mode.
 * @param[in] current_sleep_mode The current sleep mode proposed by the system so far
 ****************************************************************************************
 */
__STATIC_INLINE void arch_turn_peripherals_off(sleep_mode_t current_sleep_mode)
{
    if (current_sleep_mode == mode_ext_sleep || current_sleep_mode == mode_ext_sleep_otp_copy)
    {
#if (DO_NOT_RETAIN_ALL_RAM_BLOCKS)
#if defined (__DA14531__)
    #if defined (CFG_RETAIN_RAM_1_BLOCK)
        uint32_t retained_ram_blocks = RAM2_DO_NOT_RETAIN;  // Retain only RAM1 and RAM3
    #elif defined (CFG_RETAIN_RAM_2_BLOCK)
        uint32_t retained_ram_blocks = RAM1_DO_NOT_RETAIN;  // Retain only RAM2 and RAM3
    #else
        #if defined (__DA14535__)
        uint32_t retained_ram_blocks = RAM1_DO_NOT_RETAIN;                          // Retain only RAM2 (last RAM block)
        #else
        uint32_t retained_ram_blocks = RAM2_DO_NOT_RETAIN | RAM1_DO_NOT_RETAIN;     // Retain only RAM3 (last RAM block)
        #endif
    #endif
#else
        // DA14585/586 case
        uint32_t retained_ram_blocks = ALL_RAM_BLOCKS;      // Initialize retention mode
#endif
#endif
        SCB->SCR |= 1<<2; // enable deep sleep  mode bit in System Control Register (SCR[2]=SLEEPDEEP)

#if defined (__DA14531__)
        SetBits16(PAD_LATCH_REG, PAD_LATCH_EN, 0);
#else
        SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 0);          // activate PAD latches

        SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 1);          // turn off peripheral power domain
#endif
        if (current_sleep_mode == mode_ext_sleep)
        {
            /*
             * Sleep mode: EXTENDED - image kept at external resource
             *
             * The RAM blocks that hold the code and the retention data
             * must be retained.
             */

            // OTP copy and OTP copy emulation will be disabled
            SetBits16(SYS_CTRL_REG, OTP_COPY, 0);
        }
        else
        {
            /*
             * Sleep mode: EXTENDED - image kept at OTP (OTP mirroring takes place on wake-up)
             *
             * The RAM blocks that hold the retention data must be retained.
             */

#if (DEVELOPMENT_DEBUG)

            // enable OTP copy emulation
            SetBits16(SYS_CTRL_REG, OTP_COPY, 1);
            SetBits16(SYS_CTRL_REG, DEV_PHASE, 1);
#else
            // enable OTP copy
            SetBits16(SYS_CTRL_REG, DEV_PHASE, 0);
#endif

#if defined (CFG_CODE_SIZE_FOR_OTP_COPY_ON)
            ASSERT_WARNING(CFG_CODE_SIZE_FOR_OTP_COPY_ON > 0);
            ASSERT_WARNING(CFG_CODE_SIZE_FOR_OTP_COPY_ON <= MEMORY_OTP_SIZE);
            uint32_t code_size = CFG_CODE_SIZE_FOR_OTP_COPY_ON;
#else
            // The following equation calculates the code size and is strongly related to the
            // default SDK scatter file structure. The vector table and rom patches are included
            // in the code area calculations as well.
            uint32_t code_size = (CODE_AREA_BASE - RAM_1_BASE_ADDR) + CODE_AREA_LENGTH;
#endif
            otp_prepare((code_size + 3) >> 2);       // convert function argument from bytes to 32-bit words
        }

#if (DO_NOT_RETAIN_ALL_RAM_BLOCKS)
        // manage the non-retained heap
        // Note: non-retained heap is either empty or not. If it is not empty it must be retained.
        if (!ke_mem_is_empty(KE_MEM_NON_RETENTION))
        {
            reinit_non_ret_heap = false;
#if defined (CFG_AUTO_DETECT_NON_RET_HEAP)
#if defined (__DA14531__)
            retained_ram_blocks &= ret_mode_for_non_ret_heap;
#else
            retained_ram_blocks |= ret_mode_for_non_ret_heap;
#endif
#endif
        }
        else
        {
            reinit_non_ret_heap = true;
        }
#endif

        // set the RAM retention mode during extended sleep
#if (DO_NOT_RETAIN_ALL_RAM_BLOCKS)
        arch_ram_set_retention_mode(retained_ram_blocks);
#else
        arch_ram_set_retention_mode(ALL_RAM_BLOCKS);                    // Retain all RAM blocks
#endif

#if !defined (__DA14531__)
#if (DO_NOT_RETAIN_ALL_RAM_BLOCKS)
        set_ldo_ret_trim(retained_ram_blocks);
#else
        set_ldo_ret_trim(ALL_RAM_BLOCKS);
#endif
#endif
    }
}

/**
 ****************************************************************************************
 * @brief Prepare OTP Controller in order to be able to reload SysRAM at the next power-up.
 ****************************************************************************************
 */
__STATIC_INLINE void otp_prepare(uint32_t code_size)
{
    // Initialize OTP controller
    hw_otpc_init();

    SetBits16(SYS_CTRL_REG, OTP_COPY, 1);

    // Copy the size of software from the first word of the retention mem.
    SetWord32 (OTPC_NWORDS_REG, code_size - 1);

    // Close OTP controller
    hw_otpc_close();

#if defined (__DA14531__) && (!USE_POWER_MODE_BYPASS)
    // In Boost mode enable the DCDC converter to supply VBAT_HIGH for OTP when the system will
    // wake up.
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
        // Clear reservation status of DCDC converter
        hw_otpc_clear_dcdc_reserved();

        // Set DCDC converter voltage level for OTP read (required for OTP copy to RAM at wake up)
        syscntl_dcdc_set_level(SYSCNTL_DCDC_LEVEL_1V8);

        // Enable the DCDC converter (required for OTP copy to RAM at wake up)
        SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 1);
    }
#endif
}

/**
 ****************************************************************************************
 * @brief Used for sending messages to kernel tasks generated from
 *        asynchronous events that have been processed in app_asynch_proc.
 * @return KEEP_POWERED to force calling of schedule_while_ble_on(), else GOTO_SLEEP
 ****************************************************************************************
 */
__STATIC_INLINE arch_main_loop_callback_ret_t app_asynch_trm(void)
{
    if (user_app_main_loop_callbacks.app_on_ble_powered != NULL)
    {
        return user_app_main_loop_callbacks.app_on_ble_powered();
    }
    else
    {
        return GOTO_SLEEP;
    }
}

/**
 ****************************************************************************************
 * @brief Used for processing of asynchronous events at ?user? level. The
 *        corresponding ISRs should be kept as short as possible and the
 *        remaining processing should be done at this point.
 * @return KEEP_POWERED to force calling of schedule_while_ble_on(), else GOTO_SLEEP
 ****************************************************************************************
 */
__STATIC_INLINE arch_main_loop_callback_ret_t app_asynch_proc(void)
{
    if (user_app_main_loop_callbacks.app_on_system_powered != NULL)
    {
        return user_app_main_loop_callbacks.app_on_system_powered();
    }
    else
    {
        return GOTO_SLEEP;
    }
}

/**
 ****************************************************************************************
 * @brief Used for updating the state of the application just before sleep checking starts.
 ****************************************************************************************
 */
__STATIC_INLINE void app_asynch_sleep_proc(void)
{
    if (user_app_main_loop_callbacks.app_before_sleep != NULL)
        user_app_main_loop_callbacks.app_before_sleep();
}

/**
 ****************************************************************************************
 * @brief Used to override the slected extended sleep mode , based on the current
 *        application state.
 *        BLE and Radio are still powered off.
 * @param[in] sleep_mode     Sleep Mode
 ****************************************************************************************
 */
__STATIC_INLINE void app_sleep_prepare_proc(sleep_mode_t *sleep_mode)
{
    if (user_app_main_loop_callbacks.app_validate_sleep != NULL)
        (*sleep_mode) = user_app_main_loop_callbacks.app_validate_sleep(*sleep_mode);
}

/**
 ****************************************************************************************
 * @brief Used for application specific tasks just before entering the low power mode.
 * @param[in] sleep_mode     Sleep Mode
 ****************************************************************************************
 */
__STATIC_INLINE void app_sleep_entry_proc(sleep_mode_t sleep_mode)
{
    if (user_app_main_loop_callbacks.app_going_to_sleep != NULL)
        user_app_main_loop_callbacks.app_going_to_sleep(sleep_mode);
}

/**
 ****************************************************************************************
 * @brief Used for application specific tasks immediately after exiting the low power mode.
 * @param[in] sleep_mode     Sleep Mode
 ****************************************************************************************
 */
__STATIC_INLINE void app_sleep_exit_proc(void)
{
    if (user_app_main_loop_callbacks.app_resume_from_sleep != NULL)
        user_app_main_loop_callbacks.app_resume_from_sleep();
}

#ifdef __IS_SDK6_COMPILER_GCC__
// Defining these functions forces the linker to ignore unwind functions
void __aeabi_unwind_cpp_pr0(void) {};
void __aeabi_unwind_cpp_pr1(void) {};
void __aeabi_unwind_cpp_pr2(void) {};
void _close(void) {};
void _lseek(void) {};
void _read(void) {};
void _write(void) {};
#endif

/// @} DRIVERS

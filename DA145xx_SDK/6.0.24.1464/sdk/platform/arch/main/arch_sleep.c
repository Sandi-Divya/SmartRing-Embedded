/**
 ****************************************************************************************
 *
 * @file    arch_sleep.c
 *
 * @brief   Sleep control functions.
 *
 * Copyright (C) 2012-2023 Renesas Electronics Corporation and/or its affiliates.
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

#include <stdbool.h>
#include "arch.h"
#include "arch_api.h"
#include "app.h"
#include "rwip.h"
#include "gpio.h"

#if defined (__DA14531__)
#include "otp_cs.h"
#endif

#if (WLAN_COEX_ENABLED)
#include "wlan_coex.h"
#endif

/// Application Environment Structure
extern struct arch_sleep_env_tag    sleep_env;
static uint8_t sleep_md             __SECTION_ZERO("retention_mem_area0");
static uint8_t sleep_pend           __SECTION_ZERO("retention_mem_area0");
static uint8_t sleep_cnt            __SECTION_ZERO("retention_mem_area0");
static bool sleep_ext_force         __SECTION_ZERO("retention_mem_area0");

extern last_ble_evt arch_rwble_last_event;

#if defined (__DA14531__)
extern otp_cs_booter_val_t booter_val;
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void arch_disable_sleep(void)
{
    if (sleep_md == 0)
    {
        sleep_env.slp_state = ARCH_SLEEP_OFF;

        rwip_env.sleep_enable = false;

    }
    else
        sleep_pend = 0x80 | ARCH_SLEEP_OFF;
}

void arch_set_extended_sleep(bool otp_copy)
{
    if (sleep_md == 0)
    {
        sleep_env.slp_state = otp_copy ? ARCH_EXT_SLEEP_OTP_COPY_ON : ARCH_EXT_SLEEP_ON;

        rwip_env.sleep_enable = true;
    }
    else
    {
        sleep_pend = 0x80 | (otp_copy ? ARCH_EXT_SLEEP_OTP_COPY_ON : ARCH_EXT_SLEEP_ON);
    }
}

#if defined (__DA14531__)
void arch_set_deep_sleep(pd_sys_down_ram_t ram1,
                         pd_sys_down_ram_t ram2,
                         pd_sys_down_ram_t ram3,
                         bool pad_latch_en)
{
    // Stop watchdog timer
    wdg_freeze();

    // Disable Interrupts (CPSR)
    GLOBAL_INT_STOP()

#if (WLAN_COEX_ENABLED)
    // Drive to inactive state the pin used for the BLE event in progress signal
    wlan_coex_set_ble_eip_pin_inactive();
#endif

    // Disable radio
    SetBits16(PMU_CTRL_REG, RADIO_SLEEP, 1);

    // Disable the BLE core clocks
    SetBits16(CLK_RADIO_REG, BLE_ENABLE, 0);

    // Apply HW reset to BLE_Timers
    SetBits16(CLK_RADIO_REG, BLE_LP_RESET, 1);

    // Clear quadec pending interrupt - mask quadec interrupt
    SetWord16(QDEC_CTRL_REG, QDEC_IRQ_STATUS);

    // Close peripheral clock
    SetBits16(CLK_PER_REG, QUAD_ENABLE, 0);

    // Clear all pending interrupts
    NVIC->ICPR[0] = 0xFFFFFFFFUL;

    // Debugger must be disabled before entering a sleep mode. Wait until debugger has been disabled.
    while (GetBits16(SYS_STAT_REG, DBG_IS_UP) != 0);
    SetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE, 0);

    // Set deep sleep mode
    SetBits16(HIBERN_CTRL_REG, HIBERNATION_ENABLE, 0);
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);

    // Configure the state of RAM blocks during the deep sleep mode
    SetBits16(RAM_PWR_CTRL_REG, RAM1_PWR_CTRL, ram1);
    SetBits16(RAM_PWR_CTRL_REG, RAM2_PWR_CTRL, ram2);
#if !defined (__DA14535__)
    SetBits16(RAM_PWR_CTRL_REG, RAM3_PWR_CTRL, ram3);
#endif

    // Perform HW reset on wake-up
    SetBits16(PMU_CTRL_REG, RESET_ON_WAKEUP, 1);

    // Enable/Disable latching of pads state during sleep
    SetBits16(PAD_LATCH_REG, PAD_LATCH_EN, pad_latch_en);

#if defined (CFG_CHARACTERIZATION_SW) && defined (CHAR_SW_TRAP_DATA_FROM_EXT_TOOL)
    SetBits(POWER_AON_CTRL_REG, LDO_RET_TRIM, booter_val.char_sw_trap_data);
#else
    // Set default clamp voltage value
    SetBits16(POWER_AON_CTRL_REG, LDO_RET_TRIM, PREF_LDO_RET_TRIM_VAL);
#endif

    // Disable the testmode comparator
    SetBits(POWER_AON_CTRL_REG, FORCE_RUNNING_COMP_DIS, 1);

    // Perform the following steps when in boost (or bypass) mode
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
    {
        // Always LDO_LOW Off
        SetBits16(POWER_CTRL_REG, LDO_LOW_CTRL_REG, 1);

#if defined (__DA14535__)
        // Do not charge VBAT_HIGH in boost mode
        SetBits16(POWER_AON_CTRL_REG, CHARGE_VBAT_DISABLE, 1);

        // Connection between VBAT_HIGH and VBAT_LOW: Automatic connection control
        SetBits16(POWER_AON_CTRL_REG, VBAT_HL_CONNECT_RES_CTRL, 3);
#else
        // Force connection between VBAT_HIGH and VBAT_LOW
        SetBits(POWER_AON_CTRL_REG, VBAT_HL_CONNECT_RES_CTRL, 2);
#endif
    }
    // Perform the following steps, when in buck mode
    else
    {
#if defined (__DA14535__)
        // Enable DC/DC in sleep
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
        // High-current mode in active, Low-current mode in sleep
        SetBits16(POWER_CTRL_REG, LDO_LOW_CTRL_REG, 3);
#endif
        // Set automatic connection control between VBAT_HIGH and VBAT_LOW
        SetBits(POWER_AON_CTRL_REG, VBAT_HL_CONNECT_RES_CTRL, 3);
    }

    // Disable the POR triggered from VBAT_HIGH voltage level sensing
    SetBits(POWER_AON_CTRL_REG, POR_VBAT_HIGH_RST_MASK, 1);

    // Enable the POR triggered from VBAT_LOW voltage level sensing
    SetBits(POWER_AON_CTRL_REG, POR_VBAT_LOW_RST_MASK, 0);

#if !defined (__DA14535__)
    // Set for proper RCX operation
    SetBits16(GP_DATA_REG, FORCE_RCX_VDD | FORCE_RCX_VREF, 2);
#endif

    __NOP();
    __NOP();
    __NOP();

    // Enter Deep Sleep Mode
    __WFI();
}
#else
void arch_set_deep_sleep(bool ext_wakeup)
{
    wdg_freeze();                                               // Stop watchdog timer

    GLOBAL_INT_STOP()                                           // disable interrupts

#if (WLAN_COEX_ENABLED)
    // Drive to inactive state the pin used for the BLE event in progress signal
    wlan_coex_set_ble_eip_pin_inactive();
#endif

    SetBits16(PMU_CTRL_REG, RADIO_SLEEP, 1);                    // disable radio

    SetBits16(CLK_RADIO_REG, BLE_ENABLE, 0);                    // disable the BLE core clocks

    SetBits16(CLK_RADIO_REG, BLE_LP_RESET, 1);                  // apply HW reset to BLE_Timers

    SCB->SCR |= 1<<2;                                           // enable deep sleep

    SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 0);                   // activate PAD latches

    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 1);                   // disable peripherals

    SetBits16(PMU_CTRL_REG, RETENTION_MODE, 0x00);              // turn off all RAM blocks

    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 0);           // set BIAS to '0' if sleep has been decided

    SetBits16(CLK_32K_REG, XTAL32K_DISABLE_AMPREG, 0);          // enable the amplitude regulation of the XTAL32K osc

    if (ext_wakeup)
    {
        GPIO_DisablePorPin();                                   // Disable if any configured POR pins
        SetBits16(PMU_CTRL_REG, RESET_ON_WAKEUP, 1);            // enable boot after wake-up
    }
    else
    {
        SetBits16(PMU_CTRL_REG, RESET_ON_WAKEUP, 0);
        SetWord16(WKUP_CTRL_REG, 0x00);                         // disable wake-up interrupt
    }

    *((volatile unsigned long *)(0xE000E280)) = 0xFFFFFFFF;     // clear all interrupts pending status

    __NOP();
    __NOP();
    __NOP();

    __WFI();                                                      // enter deep sleep mode
}
#endif

void arch_set_sleep_mode(sleep_state_t sleep_state)
{
    switch (sleep_state)
    {
        case ARCH_SLEEP_OFF:
            arch_disable_sleep();
            break;
        case ARCH_EXT_SLEEP_ON:
            arch_set_extended_sleep(false);
            break;
        case ARCH_EXT_SLEEP_OTP_COPY_ON:
            arch_set_extended_sleep(true);
            break;
        default:
            break;
    }
}

uint8_t arch_get_sleep_mode(void)
{
    uint8_t ret = 0;

    switch(sleep_env.slp_state)
    {
        case ARCH_SLEEP_OFF:
            ret = 0; break;
        case ARCH_EXT_SLEEP_ON:
            ret = 1; break;
        case ARCH_EXT_SLEEP_OTP_COPY_ON:
            ret = 2; break;
    }

    return ret;
}

void arch_restore_sleep_mode(void)
{
    uint8_t cur_mode;

    if (sleep_cnt > 0)
        sleep_cnt--;

    if (sleep_cnt > 0)
        return;     // cannot restore it yet. someone else has requested active mode and we'll wait him to release it.

    if (sleep_pend != 0)
    {
        sleep_md = sleep_pend & 0x3;
        sleep_pend = 0;
    }
    else if (sleep_md)
        sleep_md--;

    cur_mode = sleep_md;
    sleep_md = 0;

    switch(cur_mode)
    {
       case 0:  break;
       case 1:  arch_set_extended_sleep(false); break;
       case 2:  arch_set_extended_sleep(true); break;
       default: break;
    }
}

void arch_force_active_mode(void)
{
    uint8_t cur_mode;

    sleep_cnt++;

    if (sleep_md == 0)  // add this check for safety! If it's called again before restore happens then sleep_md won't get overwritten
    {
        cur_mode = arch_get_sleep_mode();
        cur_mode++;     // 1: disabled, 2: extended sleep without OTP copy, 3: extended sleep with OTP copy (!=0 ==> sleep is in forced state)
        arch_disable_sleep();
        sleep_md = cur_mode;
    }
}

void arch_ble_ext_wakeup_on(void)
{
    sleep_ext_force = true;
}

void arch_ble_ext_wakeup_off(void)
{
    sleep_ext_force = false;
}

bool arch_ble_ext_wakeup_get(void)
{
    return sleep_ext_force;
}

bool arch_ble_force_wakeup(void)
{
    bool retval = false;

    // If BLE is sleeping, wake it up!
    GLOBAL_INT_DISABLE();
    if (GetBits16(CLK_RADIO_REG, BLE_ENABLE) == 0) { // BLE clock is off
        SetBits16(GP_CONTROL_REG, BLE_WAKEUP_REQ, 1);
        retval = true;
    }
    GLOBAL_INT_RESTORE();

    return retval;
}

last_ble_evt arch_last_rwble_evt_get(void)
{
    return arch_rwble_last_event;
}



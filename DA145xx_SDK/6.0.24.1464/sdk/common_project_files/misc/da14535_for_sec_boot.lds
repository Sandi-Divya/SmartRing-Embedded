/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
/*SDK6 symbols in DA14535AB ROM*/
/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/

/*system_DA14535.c (controlled by __EXCLUDE_ROM_SYSTEM_DA14535__)*/
SystemInit_ROM = 0x07f1ead9;

/*arch_console.c (controlled by __EXCLUDE_ROM_ARCH_CONSOLE__)*/
arch_printf_flush = 0x07f1edcd;
arch_vprintf = 0x07f1ee85;
arch_printf = 0x07f1eee5;
arch_puts = 0x07f1eef9;
arch_printf_process = 0x07f1ef09;

/*nvds.c (controlled by __EXCLUDE_ROM_NVDS__)*/
nvds_get_func = 0x07f1efb5;
nvds_init_func = 0x07f1f091;
nvds_del_func = 0x07f1f095;
nvds_put_func = 0x07f1f099;

/*arch_system.c (controlled by __EXCLUDE_ROM_ARCH_SYSTEM__)*/
init_pwr_and_clk_ble_part_start_ROM = 0x07f1f09d;
init_pwr_and_clk_ble_part_xtal32k_ROM = 0x07f1f0e9;
init_pwr_and_clk_ble_part_rcx_ROM = 0x07f1f129;
init_pwr_and_clk_ble_part_end_ROM = 0x07f1f161;
calc_rcx_freq_ROM = 0x07f1f195;
calc_rcx_freq_stas_ROM = 0x07f1f1d1;
check_sys_startup_period_ROM = 0x07f1f211;
set_xtal32m_trim_value_ROM = 0x07f1f241;

/*chacha20.c (controlled by __EXCLUDE_ROM_CHACHA20__)*/
csprng_seed = 0x07f1f329;
csprng_get_next_uint32 = 0x07f1f359;

/*hash.c (controlled by __EXCLUDE_ROM_HASH__)*/
hash = 0x07f1f401;

/*otp_cs.c (controlled by __EXCLUDE_ROM_OTP_CS__)*/
otp_cs_store_ROM = 0x07f1f46d;
otp_cs_unpack_pd_rad_enhanced_tx_pwr_ctrl_ROM = 0x07f1f615;
otp_cs_unpack_pd_adpll_enhanced_tx_pwr_ctrl_ROM = 0x07f1f66d;
otp_cs_load_pd_rad_ROM = 0x07f1f6a9;
otp_cs_load_pd_adpll_ROM = 0x07f1f711;

arch_asm_delay_us = 0x07f1f76d;

/*syscntl.c (controlled by __EXCLUDE_ROM_SYSCNTL__)*/
syscntl_use_lowest_amba_clocks = 0x07f1f78d;
syscntl_use_highest_amba_clocks = 0x07f1f7a3;
syscntl_cfg_xtal32m_amp_reg = 0x07f1f7b9;
syscntl_por_vbat_high_cfg = 0x07f1f7d1;
syscntl_por_vbat_low_cfg = 0x07f1f7f5;
syscntl_dcdc_set_level = 0x07f1f81d;
syscntl_dcdc_cfg = 0x07f1f845;
syscntl_dcdc_get_level = 0x07f1f865;
syscntl_dcdc_turn_off_ROM = 0x07f1f87d;
syscntl_dcdc_turn_on_in_boost_ROM = 0x07f1f8cd;
syscntl_dcdc_turn_on_in_buck_ROM = 0x07f1f929;

/*gpio.c (controlled by __EXCLUDE_ROM_GPIO__)*/
GPIO_SetPinFunction_ROM = 0x07f1f955;
GPIO_GetPinFunction_ROM = 0x07f1f965;
GPIO_SetInactive_ROM = 0x07f1f979;
GPIO_SetActive_ROM = 0x07f1f989;
GPIO_ConfigurePin_ROM = 0x07f1f999;
GPIO_ConfigurePinPower_ROM = 0x07f1f9bd;
GPIO_GetPinStatus_ROM = 0x07f1f9d5;
GPIO_is_valid_da14531_qfn_ROM = 0x07f1f9ed;
GPIO_is_valid_da14531_wlscp_ROM = 0x07f1f9f9;
GPIO_EnableIRQ_ROM = 0x07f1fa05;
GPIO_GetIRQInputLevel = 0x07f1fafd;
GPIO_SetIRQInputLevel = 0x07f1fb8d;
GPIO_ResetIRQ = 0x07f1fbb1;
GPIO_EnablePorPin_ROM = 0x07f1fbd5;
GPIO_DisablePorPin = 0x07f1fbf9;
GPIO_SetPorTime = 0x07f1fc05;
GPIO_GetPorTime = 0x07f1fc15;

/*wkupct_quadec.c (controlled by __EXCLUDE_ROM_WKUPCT_QUADEC__)*/
wkupct_set_deb_unit_time = 0x07f1fcc5;
wkupct_enable_irq_ROM = 0x07f1fce5;
wkupct2_enable_irq_ROM = 0x07f1fd2d;
wkupct_disable_irq = 0x07f1fd75;
wkupct2_disable_irq = 0x07f1fd91;
quad_decoder_init = 0x07f1fdad;
quad_decoder_disable_irq = 0x07f1fdf1;
quad_decoder_release = 0x07f1fe09;
quad_decoder_enable_irq = 0x07f1fe29;
quad_decoder_clear_irq = 0x07f1fe59;

/*TRNG implementation in ROM*/
trng_acquire = 0x07f1fea9;
trng_acquire_v2 = 0x07f1fed9;

/*spi_531.c (controlled by __EXCLUDE_ROM_SPI_531__)*/
/*spi_slave_send = 0x07f1ff05;*/
/*spi_slave_receive = 0x07f1ff95;*/
/*spi_slave_transfer = 0x07f20049;*/
/*spi_master_send = 0x07f200e1;*/
/*spi_master_receive = 0x07f20151;*/
/*spi_master_transfer = 0x07f201e9;*/
/*spi_set_speed = 0x07f202b1;*/
/*spi_enable = 0x07f20305;*/
/*spi_disable = 0x07f20329;*/
/*spi_set_cp_mode = 0x07f2034d;*/
/*spi_set_bitmode = 0x07f20361;*/
/*spi_initialize_ROM = 0x07f2038d;*/
/*spi_cs_high = 0x07f203f5;*/
/*spi_access = 0x07f20411;*/
/*spi_get_incr = 0x07f2047d;*/
/*spi_get_wsz = 0x07f20491;*/

/*i2c_eeprom.c (controlled by __EXCLUDE_ROM_I2C_EEPROM__)*/
i2c_eeprom_release = 0x07f20611;
i2c_write_last_byte = 0x07f20619;

/*uart.c (controlled by __EXCLUDE_ROM_UART__)*/
uart_enable = 0x07f20629;
uart_disable = 0x07f2065d;
uart_write_byte = 0x07f206c7;
uart_write_buffer = 0x07f206d5;
uart_wait_tx_finish = 0x07f206f3;
uart_read_byte = 0x07f206ff;
uart_read_buffer = 0x07f2070d;
uart_enable_flow_control = 0x07f20729;
uart_disable_flow_control_ROM = 0x07f2074d;

/*i2c.c (controlled by __EXCLUDE_ROM_I2C__)*/
i2c_setup_slave = 0x07f207ad;
i2c_setup_master = 0x07f207dd;
i2c_init = 0x07f20825;
i2c_release = 0x07f208f1;
i2c_master_transmit_buffer_sync = 0x07f20915;
i2c_master_receive_buffer_sync = 0x07f2098d;
i2c_slave_receive_buffer_sync = 0x07f20a05;
i2c_slave_transmit_buffer_sync = 0x07f20a2d;

/*hw_otpc_531.c (controlled by __EXCLUDE_ROM_HW_OTPC_531__)*/
otpc_dcdc_cfg = 0x07f20a89;
hw_otpc_enter_mode_ROM = 0x07f20abd;
hw_otpc_set_speed = 0x07f20b11;
hw_otpc_init_ROM = 0x07f20b2d;
hw_otpc_word_prog_and_verify = 0x07f20b95;
hw_otpc_word_read = 0x07f20bd9;
hw_otpc_prog = 0x07f20bf1;
hw_otpc_prog_and_verify = 0x07f20c25;
hw_otpc_read = 0x07f20c5b;
hw_otpc_disable_ROM = 0x07f20c85;

/*adc_531.c (controlled by __EXCLUDE_ROM_ADC_531__)*/
adc_set_diff_input = 0x07f20d35;
adc_set_se_input = 0x07f20d49;
adc_enable = 0x07f20e79;
adc_init_ROM = 0x07f20e89;
adc_reset = 0x07f20ed5;
adc_disable = 0x07f20f01;
adc_start = 0x07f20f11;
adc_get_sample = 0x07f20f21;
adc_offset_calibrate_ROM = 0x07f20f3d;
adc_correct_sample_ROM = 0x07f21019;
adc_get_temp_ROM = 0x07f21071;

/*rwble.c (controlled by __EXCLUDE_ROM_RWBLE__)*/
BLE_WAKEUP_LP_Handler_ROM = 0x07f2111d;
rwble_isr_ROM = 0x07f21171;

/*rwip.c (controlled by __EXCLUDE_ROM_RWIP__)*/
rwip_sleep_part1_ROM = 0x07f211a1;
rwip_sleep_part2_ROM = 0x07f2120d;
rwip_sleep_part3_ROM = 0x07f21251;

/*rf_531.c (controlled by __EXCLUDE_ROM_RF_531__)*/
dis_adpll_ROM = 0x07f212a1;
kdtc_cal_end_ROM = 0x07f212d1;
dis_kdtc_cal = 0x07f2131d;
en_hclk = 0x07f2132d;
en_adpll_tx_ROM = 0x07f21339;
dis_hclk = 0x07f21379;
en_kdtc_cal_mod0 = 0x07f21395;
en_kdtc_cal_mod1 = 0x07f213b1;
kdtc_cal_init_ROM = 0x07f213bd;
kdtc_calibration = 0x07f21421;
rf_calibration = 0x07f21561;
rf_adplldig_ldo_on = 0x07f21569;
rf_adplldig_txmod = 0x07f21575;
rf_ldo_cont_mode_en = 0x07f21585;
rf_adplldig_activate = 0x07f215d5;
rf_adplldig_deactivate = 0x07f21601;

/*ble_arp.c (controlled by __EXCLUDE_ROM_BLE_ARP__)*/
rf_sleep = 0x07f21635;
update_radiopwrupdn_pref_setting = 0x07f21651;
rf_regs = 0x07f21679;
ble_update_arp_adpll_calib_vals_ROM = 0x07f216b5;
ble_update_arp_adpll_div_val_ROM = 0x07f216d5;
ble_update_arp_ROM = 0x07f216f5;

/*attm_db_128.c (controlled by __EXCLUDE_ROM_ATTM_DB_128__)*/
attm_svc_create_db_128 = 0x07f21751;

/*prf.c (controlled by __EXCLUDE_ROM_PRF__)*/
prf_init_func = 0x07f2195d;
prf_add_profile_func = 0x07f219dd;
prf_create_func = 0x07f21a81;
prf_cleanup_func = 0x07f21abd;
prf_env_get = 0x07f21afd;
prf_src_task_get = 0x07f21b29;
prf_dst_task_get = 0x07f21b39;
prf_get_id_from_task_func = 0x07f21b4d;
prf_get_task_from_id_func = 0x07f21b85;
prf_reset_func = 0x07f21bbd;

/*prf_utils.c (controlled by __EXCLUDE_ROM_PRF_UTILS__)*/
prf_pack_char_pres_fmt = 0x07f21c2d;
prf_pack_date_time = 0x07f21c4b;
prf_unpack_date_time = 0x07f21c6b;

/*custs1.c (controlled by __EXCLUDE_ROM_CUSTS1__)*/
custs1_prf_itf_get = 0x07f21e3d;

/*custs1_task.c (controlled by __EXCLUDE_ROM_CUSTS1__)*/
custs1_init_ccc_values = 0x07f21eef;
custs1_set_ccc_value = 0x07f21f27;

/*custom_common.c (controlled by __EXCLUDE_ROM_CUSTOM_COMMON__)*/
check_client_char_cfg = 0x07f223ed;
get_value_handle = 0x07f22417;
get_cfg_handle = 0x07f22463;
custs1_get_att_handle = 0x07f224c5;
custs1_get_att_idx = 0x07f224e1;

/*suotar.c (controlled by __EXCLUDE_ROM_SUOTAR__)*/
suotar_prf_itf_get = 0x07f22845;

/*app.c (controlled by __EXCLUDE_ROM_APP_TASK__)*/
app_db_init_start = 0x07f22b35;
app_db_init = 0x07f22b51;
app_easy_gap_confirm = 0x07f22b5d;
append_device_name = 0x07f22b89;
app_easy_gap_update_adv_data = 0x07f22bad;
active_conidx_to_conhdl = 0x07f22bf5;
active_conhdl_to_conidx = 0x07f22c19;
app_easy_gap_disconnect = 0x07f22c55;
app_easy_gap_advertise_stop = 0x07f22c91;
app_timer_set = 0x07f22cad;
app_easy_gap_set_data_packet_length = 0x07f22cc9;
get_user_prf_srv_perm = 0x07f22d09;
app_set_prf_srv_perm = 0x07f22d31;
prf_init_srv_perm = 0x07f22d61;
app_gattc_svc_changed_cmd_send = 0x07f22d85;
app_gap_process_handler = 0x07f231fd;

/*app_entry_point.c (controlled by __EXCLUDE_ROM_APP_TASK__)*/
app_entry_point_handler = 0x07f23219;
app_std_process_event = 0x07f23261;

/*app_utils.c - (controlled by __EXCLUDE_ROM_APP_UTILS__)*/
app_get_address_type = 0x07f232a5;
app_fill_random_byte_array = 0x07f232df;

/*ARM library stuff*/
__aeabi_uldivmod = 0x07f23611;
__aeabi_llsr = 0x07f23671;
_ll_ushift_r = 0x07f23671;
__ARM_clz = 0x07f23693;
__aeabi_uidiv = 0x07f1d2bd;
__aeabi_uidivmod = 0x07f1d2bd;
__aeabi_idiv = 0x07f1d2e9;
__aeabi_idivmod = 0x07f1d2e9;
__aeabi_lmul = 0x07f1d311;
_ll_mul = 0x07f1d311;
__aeabi_memcpy = 0x07f1d3b1;
__aeabi_memcpy4 = 0x07f1d3b1;
__aeabi_memcpy8 = 0x07f1d3b1;
__aeabi_memset = 0x07f1d3d5;
__aeabi_memset4 = 0x07f1d3d5;
__aeabi_memset8 = 0x07f1d3d5;
__aeabi_memclr = 0x07f1d3e3;
__aeabi_memclr4 = 0x07f1d3e3;
__aeabi_memclr8 = 0x07f1d3e3;
_memset$wrapper = 0x07f1d3e7;
memcmp = 0x07f1d3f9;
__aeabi_uread4 = 0x07f1d413;
__rt_uread4 = 0x07f1d413;
_uread4 = 0x07f1d413;
__aeabi_uwrite4 = 0x07f1d427;
__rt_uwrite4 = 0x07f1d427;
_uwrite4 = 0x07f1d427;
__aeabi_llsl = 0x07f1d439;
_ll_shift_l = 0x07f1d439;
__ARM_common_switch8 = 0x07f1d459;

/*arch_system.c (controlled by __EXCLUDE_ROM_GAP_CFG_DATA__)*/
gap_cfg_user_var_struct = 0x07f236e0;

/*ble_arp.c (controlled by __EXCLUDE_ROM_BLE_ARP__)*/
arp_init_table = 0x07f2377c;

/*app_task.c (controlled by __EXCLUDE_ROM_APP_TASK__)*/
app_default_handler = 0x07f23b98;

/*RW _rand_state variable in stdlib/rand.c (microlib)*/
_rand_state_ROM_DATA = 0x07fcfa20;

/*uECC ROM symbols*/
uECC_verify = 0x07f1cee1;
uECC_secp256r1 = 0x07f1c0d9;
uECC_vli_mult = 0x07f02035;

/*rwip_schedule function symbol*/
rwip_schedule = 0x07f1b6b3;

#************************************************************
# THIS IS A WIZARD-GENERATED FILE.                           
#
# Version 13.1.4 Build 182 03/12/2014 SJ Full Version
#
#************************************************************

# Copyright (C) 1991-2014 Altera Corporation
# Your use of Altera Corporation's design tools, logic functions 
# and other software and tools, and its AMPP partner logic 
# functions, and any output files from any of the foregoing 
# (including device programming or simulation files), and any 
# associated documentation or information are expressly subject 
# to the terms and conditions of the Altera Program License 
# Subscription Agreement, Altera MegaCore Function License 
# Agreement, or other applicable license agreement, including, 
# without limitation, that your use is for the sole purpose of 
# programming logic devices manufactured by Altera and sold by 
#************************************************************
# THIS IS A WIZARD-GENERATED FILE.                           
#
# Version 11.1 Build 259 01/25/2012 Service Pack 2 SJ Full Version
#
#************************************************************

# Automatically calculate clock uncertainty to jitter and other effects.
derive_clock_uncertainty

# Automatically constrain PLL and other generated clocks
derive_pll_clocks -create_base_clocks

create_clock -name "clockfromadc" -period 125MHz [get_ports {adc_clka}]
create_clock -name "ref122880" -period 125MHz [get_ports {refclk_in}]

#create_generated_clock -name "i2s2_mck" -source [get_ports {refclk_in}] -divide_by 10 -duty_cycle 40.0 [get_ports {i2s2_mck}]

#create_clock -name "sclk_clock" -period 25MHz [get_ports {cpu_sclk}]
create_clock -name "fpga_ctl_cs_clock" -period 25MHz [get_ports {fpga_ctl_cs}]
create_clock -name "fpga_fir_cs_clock" -period 25MHz [get_ports {fpga_fir_cs}]
create_clock -name "i2smslk_clock" -period 25MHz [get_ports {i2smslk}]

#create_clock -name "sai1_sck_a_clock" -period 13MHz [get_ports {sai1_sck_a}]
#create_generated_clock -name "sai1_sck_a_clock" -source [get_ports {refclk_in}] -divide_by 10 -duty_cycle 40.0 [get_ports {sai1_sck_a}]

set_clock_groups -exclusive \
	-group { [get_clocks {clockfromadc}] [get_clocks {ref122880}] } \
	-group { [get_clocks {i2s2_mck}] } \
	-group { [get_clocks {sai1_sck_a_clock}] } \
	-group { [get_clocks {sclk_clock}] } \
	-group { [get_clocks {fpga_ctl_cs_clock}] } \
	
#	-group { [get_clocks {fpga_fir_cs_clock}] }

#set_false_path -from [get_clocks { ref122880    }] -to [get_ports {adc_dith adc_rand adc_shdn adc_pga dac_sleep}]
set_false_path -from [get_clocks { ref122880    }] -to [get_ports {led0 led1 led2 led3 fpga_ovf}]
set_false_path -from [get_clocks { ref122880    }] -to [get_ports {sai1_sd_b}]
#set_false_path -from [get_clocks { ref122880    }] -to [get_ports {i2s2_mck}]
set_false_path -from [get_clocks { ref122880    }] -to [get_ports {sai1_sck_a}]
set_false_path -from [get_clocks { ref122880    }] -to [get_ports {refclk_out}]

# tsu/th constraints

# tco constraints

# tpd constraints
#set_input_delay -clock  "clockfromadc"  -min [expr { - 0.8}] [get_ports {adc_data[*] adc_ovfl}]
#set_input_delay -clock  "clockfromadc"  -max [expr { + 0.8}] [get_ports {adc_data[*] adc_ovfl}]

#**************************************************************
# Set Clock Latency
#**************************************************************
set_output_delay -clock [get_clocks {ref122880}] -max 36ps [get_ports {dac_d[*]}]
set_output_delay -clock [get_clocks {ref122880}] -min 30ps [get_ports {dac_d[*]}]

#**************************************************************
# Set Input Delay
#**************************************************************
set_input_delay -clock clockfromadc -max 0ps [get_ports adc_data[*]]
set_input_delay -clock clockfromadc -min -66ps [get_ports adc_data[*]]
set_input_delay -clock clockfromadc -max 0ps [get_ports adc_ovfl]
set_input_delay -clock clockfromadc -min -66ps [get_ports adc_ovfl]


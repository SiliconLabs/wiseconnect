#!/bin/bash
now=$(date)
echo "$now"
#create array of peripheral list
PeripheralArray=("sl_si91x_calendar" "sl_si91x_gspi" "sl_si91x_watchdog_timer" "sl_si91x_usart" "sl_si91x_i2c_leader" "sl_si91x_i2c_follower" "sl_si91x_gpio" "sl_si91x_ssi_master" "sl_si91x_ssi_slave" "sl_si91x_dma" "sl_si91x_config_timer" "sl_si91x_sio" "sl_si91x_ulp_timer")
#Running the individual peripheral
for RunPripheral in "${PeripheralArray[@]}"; do
	printf "%s\n" "----------$RunPripheral-----------"
	commander flash slc_output/$RunPripheral/build/debug/$RunPripheral.rps
done
now=$(date)
echo "$now"

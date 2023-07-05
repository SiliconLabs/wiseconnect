set -e
mkdir -p /mnt/c/silabs/monorepo/gecko-sdk-dx/extension
ln -sf $(pwd)/output/wiseconnect3/ /mnt/c/silabs/monorepo/gecko-sdk-dx/extension/wiseconnect3
slc signature trust -extpath /mnt/c/silabs/monorepo/gecko-sdk-dx/extension/wiseconnect3
date
#Declare an array for all peripheral
PeripheralArray=("sl_si91x_calendar" "sl_si91x_ulp_timer" "sl_si91x_sio" "sl_si91x_watchdog_timer" "sl_si91x_config_timer" "sl_si91x_gspi" "sl_si91x_dma" "sl_si91x_i2c_follower" "sl_si91x_i2c_leader" "sl_si91x_ssi_master" "sl_si91x_ssi_slave" "sl_si91x_gpio" "sl_si91x_usart" "iostream_usart_baremetal" "sleeptimer")
for peripheral in "${PeripheralArray[@]}"; do
	printf "%s\n" "----------------------- $peripheral ------------------------------"
	#Command for generating make file in destination path
	if [ "$peripheral" == "sleeptimer" ]
	then
		slc generate /mnt/c/silabs/monorepo/gecko-sdk-dx/extension/wiseconnect3/examples/si91x_soc/peripheral/$peripheral/$peripheral.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d /mnt/c/silabs/sl_net/output/wiseconnect3/slc_output/$peripheral
	else
		slc generate /mnt/c/silabs/monorepo/gecko-sdk-dx/extension/wiseconnect3/examples/si91x_soc/peripheral/$peripheral/$peripheral.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d /mnt/c/silabs/sl_net/output/wiseconnect3/slc_output/$peripheral
	fi
	make -C /mnt/c/silabs/sl_net/output/wiseconnect3/slc_output/$peripheral -f $peripheral.Makefile -j
	commander rps create /mnt/c/silabs/sl_net/output/wiseconnect3/slc_output/$peripheral/build/debug/$peripheral.rps --app /mnt/c/silabs/sl_net/output/wiseconnect3/slc_output/$peripheral/build/debug/$peripheral.bin --address 0x8212000
done
date

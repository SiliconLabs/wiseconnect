set -e
/.yakka/repos/yq/linux/yq e --inplace '.steps[0].input="{{build_dir}}/{{project_name}}.out"' $(pwd)/output/wiseconnect3/utilities/postbuild_profile/wiseconnect_soc.slpb
mkdir -p /gsdk/gecko-sdk-dx-v3/extension
ln -sf $(pwd)/output/wiseconnect3/ /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3
/slc_cli/slc signature trust -extpath /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_central/ble_central.slcp --force --with "brd4180a" -np -d output/ble_central-brd4180a
make -C output/ble_central-brd4180a -f ble_central.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_central/ble_central.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_central-brd4325a
make -C output/ble_central-brd4325a -f ble_central.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_central/ble_central.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_central-brd4325c
make -C output/ble_central-brd4325c -f ble_central.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_central/ble_central.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_central-brd4325g
make -C output/ble_central-brd4325g -f ble_central.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/ble/ble_chat/ble_chat.slcp --force --with "brd4180a" -np -d output/ble_chat-brd4180a
make -C output/ble_chat-brd4180a -f ble_chat.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/ble/ble_chat/ble_chat.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_chat-brd4325a
make -C output/ble_chat-brd4325a -f ble_chat.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/ble/ble_chat/ble_chat.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_chat-brd4325c
make -C output/ble_chat-brd4325c -f ble_chat.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/ble/ble_chat/ble_chat.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_chat-brd4325g
make -C output/ble_chat-brd4325g -f ble_chat.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_datalength/ble_datalength.slcp --force --with "brd4180a" -np -d output/ble_datalength-brd4180a
make -C output/ble_datalength-brd4180a -f ble_datalength.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_datalength/ble_datalength.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_datalength-brd4325a
make -C output/ble_datalength-brd4325a -f ble_datalength.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_datalength/ble_datalength.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_datalength-brd4325c
make -C output/ble_datalength-brd4325c -f ble_datalength.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_datalength/ble_datalength.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_datalength-brd4325g
make -C output/ble_datalength-brd4325g -f ble_datalength.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_heart_rate_profile/ble_heart_rate_profile.slcp --force --with "brd4180a" -np -d output/ble_heart_rate_profile-brd4180a
make -C output/ble_heart_rate_profile-brd4180a -f ble_heart_rate_profile.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_heart_rate_profile/ble_heart_rate_profile.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_heart_rate_profile-brd4325a
make -C output/ble_heart_rate_profile-brd4325a -f ble_heart_rate_profile.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_heart_rate_profile/ble_heart_rate_profile.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_heart_rate_profile-brd4325c
make -C output/ble_heart_rate_profile-brd4325c -f ble_heart_rate_profile.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_heart_rate_profile/ble_heart_rate_profile.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_heart_rate_profile-brd4325g
make -C output/ble_heart_rate_profile-brd4325g -f ble_heart_rate_profile.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_hid_on_gatt/ble_hid_on_gatt.slcp --force --with "brd4180a" -np -d output/ble_hid_on_gatt-brd4180a
make -C output/ble_hid_on_gatt-brd4180a -f ble_hid_on_gatt.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_hid_on_gatt/ble_hid_on_gatt.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_hid_on_gatt-brd4325a
make -C output/ble_hid_on_gatt-brd4325a -f ble_hid_on_gatt.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_hid_on_gatt/ble_hid_on_gatt.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_hid_on_gatt-brd4325c
make -C output/ble_hid_on_gatt-brd4325c -f ble_hid_on_gatt.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_hid_on_gatt/ble_hid_on_gatt.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_hid_on_gatt-brd4325g
make -C output/ble_hid_on_gatt-brd4325g -f ble_hid_on_gatt.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_ibeacon/ble_ibeacon.slcp --force --with "brd4180a" -np -d output/ble_ibeacon-brd4180a
make -C output/ble_ibeacon-brd4180a -f ble_ibeacon.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_ibeacon/ble_ibeacon.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_ibeacon-brd4325a
make -C output/ble_ibeacon-brd4325a -f ble_ibeacon.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_ibeacon/ble_ibeacon.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_ibeacon-brd4325c
make -C output/ble_ibeacon-brd4325c -f ble_ibeacon.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_ibeacon/ble_ibeacon.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_ibeacon-brd4325g
make -C output/ble_ibeacon-brd4325g -f ble_ibeacon.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_longrange_2mbps/ble_longrange_2mbps.slcp --force --with "brd4180a" -np -d output/ble_longrange_2mbps-brd4180a
make -C output/ble_longrange_2mbps-brd4180a -f ble_longrange_2mbps.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_longrange_2mbps/ble_longrange_2mbps.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_longrange_2mbps-brd4325a
make -C output/ble_longrange_2mbps-brd4325a -f ble_longrange_2mbps.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_longrange_2mbps/ble_longrange_2mbps.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_longrange_2mbps-brd4325c
make -C output/ble_longrange_2mbps-brd4325c -f ble_longrange_2mbps.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_longrange_2mbps/ble_longrange_2mbps.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_longrange_2mbps-brd4325g
make -C output/ble_longrange_2mbps-brd4325g -f ble_longrange_2mbps.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_multiconnection_gatt_test/ble_multiconnection_gatt_test.slcp --force --with "brd4180a" -np -d output/ble_multiconnection_gatt_test-brd4180a
make -C output/ble_multiconnection_gatt_test-brd4180a -f ble_multiconnection_gatt_test.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_multiconnection_gatt_test/ble_multiconnection_gatt_test.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_multiconnection_gatt_test-brd4325a
make -C output/ble_multiconnection_gatt_test-brd4325a -f ble_multiconnection_gatt_test.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_multiconnection_gatt_test/ble_multiconnection_gatt_test.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_multiconnection_gatt_test-brd4325c
make -C output/ble_multiconnection_gatt_test-brd4325c -f ble_multiconnection_gatt_test.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_multiconnection_gatt_test/ble_multiconnection_gatt_test.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_multiconnection_gatt_test-brd4325g
make -C output/ble_multiconnection_gatt_test-brd4325g -f ble_multiconnection_gatt_test.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/ble_per/ble_per.slcp --force --with "brd4180a" -np -d output/ble_per-brd4180a
make -C output/ble_per-brd4180a -f ble_per.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/ble_per/ble_per.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_per-brd4325a
make -C output/ble_per-brd4325a -f ble_per.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/ble_per/ble_per.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_per-brd4325c
make -C output/ble_per-brd4325c -f ble_per.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/ble_per/ble_per.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_per-brd4325g
make -C output/ble_per-brd4325g -f ble_per.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/ble/ble_peripheral/ble_peripheral.slcp --force --with "brd4180a" -np -d output/ble_peripheral-brd4180a
make -C output/ble_peripheral-brd4180a -f ble_peripheral.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/ble/ble_peripheral/ble_peripheral.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_peripheral-brd4325a
make -C output/ble_peripheral-brd4325a -f ble_peripheral.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/ble/ble_peripheral/ble_peripheral.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_peripheral-brd4325c
make -C output/ble_peripheral-brd4325c -f ble_peripheral.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/ble/ble_peripheral/ble_peripheral.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_peripheral-brd4325g
make -C output/ble_peripheral-brd4325g -f ble_peripheral.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_power_save/ble_power_save.slcp --force --with "brd4180a" -np -d output/ble_power_save-brd4180a
make -C output/ble_power_save-brd4180a -f ble_power_save.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_power_save/ble_power_save.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_power_save-brd4325a
make -C output/ble_power_save-brd4325a -f ble_power_save.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_power_save/ble_power_save.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_power_save-brd4325c
make -C output/ble_power_save-brd4325c -f ble_power_save.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_power_save/ble_power_save.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_power_save-brd4325g
make -C output/ble_power_save-brd4325g -f ble_power_save.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_privacy/ble_privacy.slcp --force --with "brd4180a" -np -d output/ble_privacy-brd4180a
make -C output/ble_privacy-brd4180a -f ble_privacy.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_privacy/ble_privacy.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_privacy-brd4325a
make -C output/ble_privacy-brd4325a -f ble_privacy.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_privacy/ble_privacy.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_privacy-brd4325c
make -C output/ble_privacy-brd4325c -f ble_privacy.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_privacy/ble_privacy.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_privacy-brd4325g
make -C output/ble_privacy-brd4325g -f ble_privacy.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_secureconnection/ble_secureconnection.slcp --force --with "brd4180a" -np -d output/ble_secureconnection-brd4180a
make -C output/ble_secureconnection-brd4180a -f ble_secureconnection.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_secureconnection/ble_secureconnection.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_secureconnection-brd4325a
make -C output/ble_secureconnection-brd4325a -f ble_secureconnection.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_secureconnection/ble_secureconnection.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_secureconnection-brd4325c
make -C output/ble_secureconnection-brd4325c -f ble_secureconnection.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_secureconnection/ble_secureconnection.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_secureconnection-brd4325g
make -C output/ble_secureconnection-brd4325g -f ble_secureconnection.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/ble/ble_simple_smp/ble_simple_smp.slcp --force --with "brd4180a" -np -d output/ble_simple_smp-brd4180a
make -C output/ble_simple_smp-brd4180a -f ble_simple_smp.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/ble/ble_simple_smp/ble_simple_smp.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_simple_smp-brd4325a
make -C output/ble_simple_smp-brd4325a -f ble_simple_smp.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/ble/ble_simple_smp/ble_simple_smp.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_simple_smp-brd4325c
make -C output/ble_simple_smp-brd4325c -f ble_simple_smp.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/ble/ble_simple_smp/ble_simple_smp.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_simple_smp-brd4325g
make -C output/ble_simple_smp-brd4325g -f ble_simple_smp.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_throughput_app/ble_throughput_app.slcp --force --with "brd4180a" -np -d output/ble_throughput_app-brd4180a
make -C output/ble_throughput_app-brd4180a -f ble_throughput_app.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_throughput_app/ble_throughput_app.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_throughput_app-brd4325a
make -C output/ble_throughput_app-brd4325a -f ble_throughput_app.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_throughput_app/ble_throughput_app.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_throughput_app-brd4325c
make -C output/ble_throughput_app-brd4325c -f ble_throughput_app.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_throughput_app/ble_throughput_app.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_throughput_app-brd4325g
make -C output/ble_throughput_app-brd4325g -f ble_throughput_app.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_white_list/ble_white_list.slcp --force --with "brd4180a" -np -d output/ble_white_list-brd4180a
make -C output/ble_white_list-brd4180a -f ble_white_list.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_white_list/ble_white_list.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/ble_white_list-brd4325a
make -C output/ble_white_list-brd4325a -f ble_white_list.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_white_list/ble_white_list.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/ble_white_list-brd4325c
make -C output/ble_white_list-brd4325c -f ble_white_list.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/ble/ble_white_list/ble_white_list.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/ble_white_list-brd4325g
make -C output/ble_white_list-brd4325g -f ble_white_list.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/iostream_usart_baremetal/iostream_usart_baremetal.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/iostream_usart_baremetal-brd4325a
make -C output/iostream_usart_baremetal-brd4325a -f iostream_usart_baremetal.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/iostream_usart_baremetal/iostream_usart_baremetal.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/iostream_usart_baremetal-brd4325c
make -C output/iostream_usart_baremetal-brd4325c -f iostream_usart_baremetal.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/iostream_usart_baremetal/iostream_usart_baremetal.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/iostream_usart_baremetal-brd4325g
make -C output/iostream_usart_baremetal-brd4325g -f iostream_usart_baremetal.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_calendar/sl_si91x_calendar.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_calendar-brd4325a
make -C output/sl_si91x_calendar-brd4325a -f sl_si91x_calendar.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_calendar/sl_si91x_calendar.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_calendar-brd4325c
make -C output/sl_si91x_calendar-brd4325c -f sl_si91x_calendar.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_calendar/sl_si91x_calendar.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_calendar-brd4325g
make -C output/sl_si91x_calendar-brd4325g -f sl_si91x_calendar.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_config_timer/sl_si91x_config_timer.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_config_timer-brd4325a
make -C output/sl_si91x_config_timer-brd4325a -f sl_si91x_config_timer.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_config_timer/sl_si91x_config_timer.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_config_timer-brd4325c
make -C output/sl_si91x_config_timer-brd4325c -f sl_si91x_config_timer.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_config_timer/sl_si91x_config_timer.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_config_timer-brd4325g
make -C output/sl_si91x_config_timer-brd4325g -f sl_si91x_config_timer.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_dma/sl_si91x_dma.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_dma-brd4325a
make -C output/sl_si91x_dma-brd4325a -f sl_si91x_dma.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_dma/sl_si91x_dma.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_dma-brd4325c
make -C output/sl_si91x_dma-brd4325c -f sl_si91x_dma.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_dma/sl_si91x_dma.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_dma-brd4325g
make -C output/sl_si91x_dma-brd4325g -f sl_si91x_dma.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_gpio/sl_si91x_gpio.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_gpio-brd4325a
make -C output/sl_si91x_gpio-brd4325a -f sl_si91x_gpio.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_gpio/sl_si91x_gpio.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_gpio-brd4325c
make -C output/sl_si91x_gpio-brd4325c -f sl_si91x_gpio.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_gpio/sl_si91x_gpio.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_gpio-brd4325g
make -C output/sl_si91x_gpio-brd4325g -f sl_si91x_gpio.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_gspi/sl_si91x_gspi.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_gspi-brd4325a
make -C output/sl_si91x_gspi-brd4325a -f sl_si91x_gspi.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_gspi/sl_si91x_gspi.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_gspi-brd4325c
make -C output/sl_si91x_gspi-brd4325c -f sl_si91x_gspi.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_gspi/sl_si91x_gspi.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_gspi-brd4325g
make -C output/sl_si91x_gspi-brd4325g -f sl_si91x_gspi.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_i2c_follower/sl_si91x_i2c_follower.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_i2c_follower-brd4325a
make -C output/sl_si91x_i2c_follower-brd4325a -f sl_si91x_i2c_follower.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_i2c_follower/sl_si91x_i2c_follower.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_i2c_follower-brd4325c
make -C output/sl_si91x_i2c_follower-brd4325c -f sl_si91x_i2c_follower.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_i2c_follower/sl_si91x_i2c_follower.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_i2c_follower-brd4325g
make -C output/sl_si91x_i2c_follower-brd4325g -f sl_si91x_i2c_follower.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_i2c_leader/sl_si91x_i2c_leader.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_i2c_leader-brd4325a
make -C output/sl_si91x_i2c_leader-brd4325a -f sl_si91x_i2c_leader.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_i2c_leader/sl_si91x_i2c_leader.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_i2c_leader-brd4325c
make -C output/sl_si91x_i2c_leader-brd4325c -f sl_si91x_i2c_leader.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_i2c_leader/sl_si91x_i2c_leader.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_i2c_leader-brd4325g
make -C output/sl_si91x_i2c_leader-brd4325g -f sl_si91x_i2c_leader.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/sl_si91x_nvm3_common_flash/sl_si91x_nvm3_common_flash.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_nvm3_common_flash-brd4325a
make -C output/sl_si91x_nvm3_common_flash-brd4325a -f sl_si91x_nvm3_common_flash.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/sl_si91x_nvm3_common_flash/sl_si91x_nvm3_common_flash.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_nvm3_common_flash-brd4325c
make -C output/sl_si91x_nvm3_common_flash-brd4325c -f sl_si91x_nvm3_common_flash.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/sl_si91x_nvm3_common_flash/sl_si91x_nvm3_common_flash.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_nvm3_common_flash-brd4325g
make -C output/sl_si91x_nvm3_common_flash-brd4325g -f sl_si91x_nvm3_common_flash.Makefile -j


/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_sio/sl_si91x_sio.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_sio-brd4325a
make -C output/sl_si91x_sio-brd4325a -f sl_si91x_sio.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_sio/sl_si91x_sio.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_sio-brd4325c
make -C output/sl_si91x_sio-brd4325c -f sl_si91x_sio.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_sio/sl_si91x_sio.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_sio-brd4325g
make -C output/sl_si91x_sio-brd4325g -f sl_si91x_sio.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_ssi_master/sl_si91x_ssi_master.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_ssi_master-brd4325a
make -C output/sl_si91x_ssi_master-brd4325a -f sl_si91x_ssi_master.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_ssi_master/sl_si91x_ssi_master.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_ssi_master-brd4325c
make -C output/sl_si91x_ssi_master-brd4325c -f sl_si91x_ssi_master.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_ssi_master/sl_si91x_ssi_master.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_ssi_master-brd4325g
make -C output/sl_si91x_ssi_master-brd4325g -f sl_si91x_ssi_master.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_ssi_slave/sl_si91x_ssi_slave.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_ssi_slave-brd4325a
make -C output/sl_si91x_ssi_slave-brd4325a -f sl_si91x_ssi_slave.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_ssi_slave/sl_si91x_ssi_slave.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_ssi_slave-brd4325c
make -C output/sl_si91x_ssi_slave-brd4325c -f sl_si91x_ssi_slave.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_ssi_slave/sl_si91x_ssi_slave.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_ssi_slave-brd4325g
make -C output/sl_si91x_ssi_slave-brd4325g -f sl_si91x_ssi_slave.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_ulp_timer/sl_si91x_ulp_timer.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_ulp_timer-brd4325a
make -C output/sl_si91x_ulp_timer-brd4325a -f sl_si91x_ulp_timer.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_ulp_timer/sl_si91x_ulp_timer.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_ulp_timer-brd4325c
make -C output/sl_si91x_ulp_timer-brd4325c -f sl_si91x_ulp_timer.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_ulp_timer/sl_si91x_ulp_timer.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_ulp_timer-brd4325g
make -C output/sl_si91x_ulp_timer-brd4325g -f sl_si91x_ulp_timer.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_usart/sl_si91x_usart.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_usart-brd4325a
make -C output/sl_si91x_usart-brd4325a -f sl_si91x_usart.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_usart/sl_si91x_usart.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_usart-brd4325c
make -C output/sl_si91x_usart-brd4325c -f sl_si91x_usart.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_usart/sl_si91x_usart.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_usart-brd4325g
make -C output/sl_si91x_usart-brd4325g -f sl_si91x_usart.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_watchdog_timer/sl_si91x_watchdog_timer.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/sl_si91x_watchdog_timer-brd4325a
make -C output/sl_si91x_watchdog_timer-brd4325a -f sl_si91x_watchdog_timer.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_watchdog_timer/sl_si91x_watchdog_timer.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sl_si91x_watchdog_timer-brd4325c
make -C output/sl_si91x_watchdog_timer-brd4325c -f sl_si91x_watchdog_timer.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sl_si91x_watchdog_timer/sl_si91x_watchdog_timer.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sl_si91x_watchdog_timer-brd4325g
make -C output/sl_si91x_watchdog_timer-brd4325g -f sl_si91x_watchdog_timer.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sleeptimer/sleeptimer.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/sleeptimer-brd4325c
make -C output/sleeptimer-brd4325c -f sleeptimer.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/si91x_soc/peripheral/sleeptimer/sleeptimer.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/sleeptimer-brd4325g
make -C output/sleeptimer-brd4325g -f sleeptimer.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/wifi6_mimo/wifi6_mimo.slcp --force --with "brd4180a" -np -d output/wifi6_mimo-brd4180a
make -C output/wifi6_mimo-brd4180a -f wifi6_mimo.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/wifi6_mimo/wifi6_mimo.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi6_mimo-brd4325a
make -C output/wifi6_mimo-brd4325a -f wifi6_mimo.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/wifi6_mimo/wifi6_mimo.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi6_mimo-brd4325c
make -C output/wifi6_mimo-brd4325c -f wifi6_mimo.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/wifi6_mimo/wifi6_mimo.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi6_mimo-brd4325g
make -C output/wifi6_mimo-brd4325g -f wifi6_mimo.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/access_point/access_point.slcp --force --with "brd4180a" -np -d output/wifi_access_point-brd4180a
make -C output/wifi_access_point-brd4180a -f wifi_access_point.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/access_point/access_point.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_access_point-brd4325a
make -C output/wifi_access_point-brd4325a -f wifi_access_point.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/access_point/access_point.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_access_point-brd4325c
make -C output/wifi_access_point-brd4325c -f wifi_access_point.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/access_point/access_point.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_access_point-brd4325g
make -C output/wifi_access_point-brd4325g -f wifi_access_point.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/access_point_v6/access_point_v6.slcp --force --with "brd4180a" -np -d output/wifi_access_point_v6-brd4180a
make -C output/wifi_access_point_v6-brd4180a -f wifi_access_point_v6.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/access_point_v6/access_point_v6.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_access_point_v6-brd4325a
make -C output/wifi_access_point_v6-brd4325a -f wifi_access_point_v6.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/access_point_v6/access_point_v6.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_access_point_v6-brd4325c
make -C output/wifi_access_point_v6-brd4325c -f wifi_access_point_v6.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/access_point_v6/access_point_v6.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_access_point_v6-brd4325g
make -C output/wifi_access_point_v6-brd4325g -f wifi_access_point_v6.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/aws_device_shadow/aws_device_shadow.slcp --force --with "brd4180a" -np -d output/wifi_aws_device_shadow-brd4180a
make -C output/wifi_aws_device_shadow-brd4180a -f wifi_aws_device_shadow.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/aws_device_shadow/aws_device_shadow.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_aws_device_shadow-brd4325a
make -C output/wifi_aws_device_shadow-brd4325a -f wifi_aws_device_shadow.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/aws_device_shadow/aws_device_shadow.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_aws_device_shadow-brd4325c
make -C output/wifi_aws_device_shadow-brd4325c -f wifi_aws_device_shadow.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/aws_device_shadow/aws_device_shadow.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_aws_device_shadow-brd4325g
make -C output/wifi_aws_device_shadow-brd4325g -f wifi_aws_device_shadow.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/cloud_apps/aws/mqtt/aws_mqtt.slcp --force --with "brd4180a" -np -d output/wifi_aws_mqtt-brd4180a
make -C output/wifi_aws_mqtt-brd4180a -f wifi_aws_mqtt.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/cloud_apps/aws/mqtt/aws_mqtt.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_aws_mqtt-brd4325a
make -C output/wifi_aws_mqtt-brd4325a -f wifi_aws_mqtt.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/cloud_apps/aws/mqtt/aws_mqtt.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_aws_mqtt-brd4325c
make -C output/wifi_aws_mqtt-brd4325c -f wifi_aws_mqtt.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/cloud_apps/aws/mqtt/aws_mqtt.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_aws_mqtt-brd4325g
make -C output/wifi_aws_mqtt-brd4325g -f wifi_aws_mqtt.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan_ble/wifi_ble_power_save/wifi_ble_power_save.slcp --force --with "brd4180a" -np -d output/wifi_ble_power_save-brd4180a
make -C output/wifi_ble_power_save-brd4180a -f wifi_ble_power_save.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan_ble/wifi_ble_power_save/wifi_ble_power_save.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_ble_power_save-brd4325a
make -C output/wifi_ble_power_save-brd4325a -f wifi_ble_power_save.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan_ble/wifi_ble_power_save/wifi_ble_power_save.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_ble_power_save-brd4325c
make -C output/wifi_ble_power_save-brd4325c -f wifi_ble_power_save.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan_ble/wifi_ble_power_save/wifi_ble_power_save.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_ble_power_save-brd4325g
make -C output/wifi_ble_power_save-brd4325g -f wifi_ble_power_save.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/concurrent_mode/concurrent_mode.slcp --force --with "brd4180a" -np -d output/wifi_concurrent_mode-brd4180a
make -C output/wifi_concurrent_mode-brd4180a -f wifi_concurrent_mode.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/concurrent_mode/concurrent_mode.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_concurrent_mode-brd4325a
make -C output/wifi_concurrent_mode-brd4325a -f wifi_concurrent_mode.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/concurrent_mode/concurrent_mode.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_concurrent_mode-brd4325c
make -C output/wifi_concurrent_mode-brd4325c -f wifi_concurrent_mode.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/concurrent_mode/concurrent_mode.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_concurrent_mode-brd4325g
make -C output/wifi_concurrent_mode-brd4325g -f wifi_concurrent_mode.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/embedded_mqtt_client/embedded_mqtt_client.slcp --force --with "brd4180a" -np -d output/wifi_embedded_mqtt_client-brd4180a
make -C output/wifi_embedded_mqtt_client-brd4180a -f wifi_embedded_mqtt_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/embedded_mqtt_client/embedded_mqtt_client.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_embedded_mqtt_client-brd4325a
make -C output/wifi_embedded_mqtt_client-brd4325a -f wifi_embedded_mqtt_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/embedded_mqtt_client/embedded_mqtt_client.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_embedded_mqtt_client-brd4325c
make -C output/wifi_embedded_mqtt_client-brd4325c -f wifi_embedded_mqtt_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/embedded_mqtt_client/embedded_mqtt_client.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_embedded_mqtt_client-brd4325g
make -C output/wifi_embedded_mqtt_client-brd4325g -f wifi_embedded_mqtt_client.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/embedded_mqtt_client_twt/embedded_mqtt_client_twt.slcp --force --with "brd4180a" -np -d output/wifi_embedded_mqtt_client_twt-brd4180a
make -C output/wifi_embedded_mqtt_client_twt-brd4180a -f wifi_embedded_mqtt_client_twt.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/embedded_mqtt_client_twt/embedded_mqtt_client_twt.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_embedded_mqtt_client_twt-brd4325a
make -C output/wifi_embedded_mqtt_client_twt-brd4325a -f wifi_embedded_mqtt_client_twt.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/embedded_mqtt_client_twt/embedded_mqtt_client_twt.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_embedded_mqtt_client_twt-brd4325c
make -C output/wifi_embedded_mqtt_client_twt-brd4325c -f wifi_embedded_mqtt_client_twt.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/embedded_mqtt_client_twt/embedded_mqtt_client_twt.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_embedded_mqtt_client_twt-brd4325g
make -C output/wifi_embedded_mqtt_client_twt-brd4325g -f wifi_embedded_mqtt_client_twt.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/sntp_client/sntp_client.slcp --force --with "brd4180a" -np -d output/wifi_embedded_sntp_client-brd4180a
make -C output/wifi_embedded_sntp_client-brd4180a -f wifi_embedded_sntp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/sntp_client/sntp_client.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_embedded_sntp_client-brd4325a
make -C output/wifi_embedded_sntp_client-brd4325a -f wifi_embedded_sntp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/sntp_client/sntp_client.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_embedded_sntp_client-brd4325c
make -C output/wifi_embedded_sntp_client-brd4325c -f wifi_embedded_sntp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/sntp_client/sntp_client.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_embedded_sntp_client-brd4325g
make -C output/wifi_embedded_sntp_client-brd4325g -f wifi_embedded_sntp_client.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/enterprise_client/enterprise_client.slcp --force --with "brd4180a" -np -d output/wifi_enterprise_client-brd4180a
make -C output/wifi_enterprise_client-brd4180a -f wifi_enterprise_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/enterprise_client/enterprise_client.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_enterprise_client-brd4325a
make -C output/wifi_enterprise_client-brd4325a -f wifi_enterprise_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/enterprise_client/enterprise_client.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_enterprise_client-brd4325c
make -C output/wifi_enterprise_client-brd4325c -f wifi_enterprise_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/enterprise_client/enterprise_client.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_enterprise_client-brd4325g
make -C output/wifi_enterprise_client-brd4325g -f wifi_enterprise_client.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/firmware_update/firmware_update.slcp --force --with "brd4180a" -np -d output/wifi_firmware_update-brd4180a
make -C output/wifi_firmware_update-brd4180a -f wifi_firmware_update.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/firmware_update/firmware_update.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_firmware_update-brd4325a
make -C output/wifi_firmware_update-brd4325a -f wifi_firmware_update.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/firmware_update/firmware_update.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_firmware_update-brd4325c
make -C output/wifi_firmware_update-brd4325c -f wifi_firmware_update.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/firmware_update/firmware_update.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_firmware_update-brd4325g
make -C output/wifi_firmware_update-brd4325g -f wifi_firmware_update.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/http_client/http_client_app.slcp --force --with "brd4180a" -np -d output/wifi_http_client_app-brd4180a
make -C output/wifi_http_client_app-brd4180a -f wifi_http_client_app.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/http_client/http_client_app.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_http_client_app-brd4325a
make -C output/wifi_http_client_app-brd4325a -f wifi_http_client_app.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/http_client/http_client_app.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_http_client_app-brd4325c
make -C output/wifi_http_client_app-brd4325c -f wifi_http_client_app.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/http_client/http_client_app.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_http_client_app-brd4325g
make -C output/wifi_http_client_app-brd4325g -f wifi_http_client_app.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/http_client_post_data/http_client_post_data_app.slcp --force --with "brd4180a" -np -d output/wifi_http_client_post_data-brd4180a
make -C output/wifi_http_client_post_data-brd4180a -f wifi_http_client_post_data.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/http_client_post_data/http_client_post_data_app.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_http_client_post_data-brd4325a
make -C output/wifi_http_client_post_data-brd4325a -f wifi_http_client_post_data.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/http_client_post_data/http_client_post_data_app.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_http_client_post_data-brd4325c
make -C output/wifi_http_client_post_data-brd4325c -f wifi_http_client_post_data.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/http_client_post_data/http_client_post_data_app.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_http_client_post_data-brd4325g
make -C output/wifi_http_client_post_data-brd4325g -f wifi_http_client_post_data.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/http_otaf/http_otaf.slcp --force --with "brd4180a" -np -d output/wifi_http_otaf-brd4180a
make -C output/wifi_http_otaf-brd4180a -f wifi_http_otaf.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/http_otaf/http_otaf.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_http_otaf-brd4325a
make -C output/wifi_http_otaf-brd4325a -f wifi_http_otaf.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/http_otaf/http_otaf.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_http_otaf-brd4325c
make -C output/wifi_http_otaf-brd4325c -f wifi_http_otaf.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/http_otaf/http_otaf.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_http_otaf-brd4325g
make -C output/wifi_http_otaf-brd4325g -f wifi_http_otaf.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_https_ble_dual_role/wifi_https_ble_dual_role.slcp --force --with "brd4180a" -np -d output/wifi_https_ble_dual_role-brd4180a
make -C output/wifi_https_ble_dual_role-brd4180a -f wifi_https_ble_dual_role.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_https_ble_dual_role/wifi_https_ble_dual_role.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_https_ble_dual_role-brd4325a
make -C output/wifi_https_ble_dual_role-brd4325a -f wifi_https_ble_dual_role.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_https_ble_dual_role/wifi_https_ble_dual_role.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_https_ble_dual_role-brd4325c
make -C output/wifi_https_ble_dual_role-brd4325c -f wifi_https_ble_dual_role.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_https_ble_dual_role/wifi_https_ble_dual_role.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_https_ble_dual_role-brd4325g
make -C output/wifi_https_ble_dual_role-brd4325g -f wifi_https_ble_dual_role.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/instant_bgscan/instant_bgscan.slcp --force --with "brd4180a" -np -d output/wifi_instant_bgscan-brd4180a
make -C output/wifi_instant_bgscan-brd4180a -f wifi_instant_bgscan.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/instant_bgscan/instant_bgscan.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_instant_bgscan-brd4325a
make -C output/wifi_instant_bgscan-brd4325a -f wifi_instant_bgscan.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/instant_bgscan/instant_bgscan.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_instant_bgscan-brd4325c
make -C output/wifi_instant_bgscan-brd4325c -f wifi_instant_bgscan.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/instant_bgscan/instant_bgscan.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_instant_bgscan-brd4325g
make -C output/wifi_instant_bgscan-brd4325g -f wifi_instant_bgscan.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/lwip_tcp_client/lwip_tcp_client.slcp --force --with "brd4180a" -np -d output/wifi_lwip_tcp_client-brd4180a
make -C output/wifi_lwip_tcp_client-brd4180a -f wifi_lwip_tcp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/lwip_tcp_client/lwip_tcp_client.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_lwip_tcp_client-brd4325a
make -C output/wifi_lwip_tcp_client-brd4325a -f wifi_lwip_tcp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/lwip_tcp_client/lwip_tcp_client.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_lwip_tcp_client-brd4325c
make -C output/wifi_lwip_tcp_client-brd4325c -f wifi_lwip_tcp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/lwip_tcp_client/lwip_tcp_client.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_lwip_tcp_client-brd4325g
make -C output/wifi_lwip_tcp_client-brd4325g -f wifi_lwip_tcp_client.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/otaf/ota_firmware_update_app.slcp --force --with "brd4180a" -np -d output/wifi_ota_firmware_update_app-brd4180a
make -C output/wifi_ota_firmware_update_app-brd4180a -f wifi_ota_firmware_update_app.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/otaf/ota_firmware_update_app.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_ota_firmware_update_app-brd4325a
make -C output/wifi_ota_firmware_update_app-brd4325a -f wifi_ota_firmware_update_app.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/otaf/ota_firmware_update_app.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_ota_firmware_update_app-brd4325c
make -C output/wifi_ota_firmware_update_app-brd4325c -f wifi_ota_firmware_update_app.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/otaf/ota_firmware_update_app.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_ota_firmware_update_app-brd4325g
make -C output/wifi_ota_firmware_update_app-brd4325g -f wifi_ota_firmware_update_app.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/power_save_deep_sleep/power_save_deep_sleep.slcp --force --with "brd4180a" -np -d output/wifi_power_save_deep_sleep-brd4180a
make -C output/wifi_power_save_deep_sleep-brd4180a -f wifi_power_save_deep_sleep.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/power_save_deep_sleep/power_save_deep_sleep.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_power_save_deep_sleep-brd4325a
make -C output/wifi_power_save_deep_sleep-brd4325a -f wifi_power_save_deep_sleep.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/power_save_deep_sleep/power_save_deep_sleep.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_power_save_deep_sleep-brd4325c
make -C output/wifi_power_save_deep_sleep-brd4325c -f wifi_power_save_deep_sleep.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/power_save_deep_sleep/power_save_deep_sleep.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_power_save_deep_sleep-brd4325g
make -C output/wifi_power_save_deep_sleep-brd4325g -f wifi_power_save_deep_sleep.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/powersave_standby_associated/powersave_standby_associated.slcp --force --with "brd4180a" -np -d output/wifi_powersave_standby_associated-brd4180a
make -C output/wifi_powersave_standby_associated-brd4180a -f wifi_powersave_standby_associated.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/powersave_standby_associated/powersave_standby_associated.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_powersave_standby_associated-brd4325a
make -C output/wifi_powersave_standby_associated-brd4325a -f wifi_powersave_standby_associated.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/powersave_standby_associated/powersave_standby_associated.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_powersave_standby_associated-brd4325c
make -C output/wifi_powersave_standby_associated-brd4325c -f wifi_powersave_standby_associated.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/powersave_standby_associated/powersave_standby_associated.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_powersave_standby_associated-brd4325g
make -C output/wifi_powersave_standby_associated-brd4325g -f wifi_powersave_standby_associated.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/select_app/select_app.slcp --force --with "brd4180a" -np -d output/wifi_select_app-brd4180a
make -C output/wifi_select_app-brd4180a -f wifi_select_app.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/select_app/select_app.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_select_app-brd4325a
make -C output/wifi_select_app-brd4325a -f wifi_select_app.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/select_app/select_app.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_select_app-brd4325c
make -C output/wifi_select_app-brd4325c -f wifi_select_app.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/select_app/select_app.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_select_app-brd4325g
make -C output/wifi_select_app-brd4325g -f wifi_select_app.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/ssl_client/ssl_client.slcp --force --with "brd4180a" -np -d output/wifi_ssl_client-brd4180a
make -C output/wifi_ssl_client-brd4180a -f wifi_ssl_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/ssl_client/ssl_client.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_ssl_client-brd4325a
make -C output/wifi_ssl_client-brd4325a -f wifi_ssl_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/ssl_client/ssl_client.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_ssl_client-brd4325c
make -C output/wifi_ssl_client-brd4325c -f wifi_ssl_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/ssl_client/ssl_client.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_ssl_client-brd4325g
make -C output/wifi_ssl_client-brd4325g -f wifi_ssl_client.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_station_ble_provisioning/wifi_station_ble_provisioning.slcp --force --with "brd4180a" -np -d output/wifi_station_ble_provisioning-brd4180a
make -C output/wifi_station_ble_provisioning-brd4180a -f wifi_station_ble_provisioning.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_station_ble_provisioning/wifi_station_ble_provisioning.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_station_ble_provisioning-brd4325a
make -C output/wifi_station_ble_provisioning-brd4325a -f wifi_station_ble_provisioning.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_station_ble_provisioning/wifi_station_ble_provisioning.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_station_ble_provisioning-brd4325c
make -C output/wifi_station_ble_provisioning-brd4325c -f wifi_station_ble_provisioning.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_station_ble_provisioning/wifi_station_ble_provisioning.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_station_ble_provisioning-brd4325g
make -C output/wifi_station_ble_provisioning-brd4325g -f wifi_station_ble_provisioning.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_station_ble_provisioning_aws/wifi_station_ble_provisioning_aws.slcp --force --with "brd4180a" -np -d output/wifi_station_ble_provisioning_aws-brd4180a
make -C output/wifi_station_ble_provisioning_aws-brd4180a -f wifi_station_ble_provisioning_aws.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_station_ble_provisioning_aws/wifi_station_ble_provisioning_aws.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_station_ble_provisioning_aws-brd4325a
make -C output/wifi_station_ble_provisioning_aws-brd4325a -f wifi_station_ble_provisioning_aws.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_station_ble_provisioning_aws/wifi_station_ble_provisioning_aws.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_station_ble_provisioning_aws-brd4325c
make -C output/wifi_station_ble_provisioning_aws-brd4325c -f wifi_station_ble_provisioning_aws.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_station_ble_provisioning_aws/wifi_station_ble_provisioning_aws.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_station_ble_provisioning_aws-brd4325g
make -C output/wifi_station_ble_provisioning_aws-brd4325g -f wifi_station_ble_provisioning_aws.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/station_ping/station_ping.slcp --force --with "brd4180a" -np -d output/wifi_station_ping-brd4180a
make -C output/wifi_station_ping-brd4180a -f wifi_station_ping.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/station_ping/station_ping.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_station_ping-brd4325a
make -C output/wifi_station_ping-brd4325a -f wifi_station_ping.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/station_ping/station_ping.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_station_ping-brd4325c
make -C output/wifi_station_ping-brd4325c -f wifi_station_ping.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/station_ping/station_ping.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_station_ping-brd4325g
make -C output/wifi_station_ping-brd4325g -f wifi_station_ping.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/station_ping_v6/station_ping_v6.slcp --force --with "brd4180a" -np -d output/wifi_station_ping_v6-brd4180a
make -C output/wifi_station_ping_v6-brd4180a -f wifi_station_ping_v6.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/station_ping_v6/station_ping_v6.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_station_ping_v6-brd4325a
make -C output/wifi_station_ping_v6-brd4325a -f wifi_station_ping_v6.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/station_ping_v6/station_ping_v6.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_station_ping_v6-brd4325c
make -C output/wifi_station_ping_v6-brd4325c -f wifi_station_ping_v6.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/station_ping_v6/station_ping_v6.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_station_ping_v6-brd4325g
make -C output/wifi_station_ping_v6-brd4325g -f wifi_station_ping_v6.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/tcp_client/tcp_client.slcp --force --with "brd4180a" -np -d output/wifi_tcp_client-brd4180a
make -C output/wifi_tcp_client-brd4180a -f wifi_tcp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/tcp_client/tcp_client.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_tcp_client-brd4325a
make -C output/wifi_tcp_client-brd4325a -f wifi_tcp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/tcp_client/tcp_client.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_tcp_client-brd4325c
make -C output/wifi_tcp_client-brd4325c -f wifi_tcp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/tcp_client/tcp_client.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_tcp_client-brd4325g
make -C output/wifi_tcp_client-brd4325g -f wifi_tcp_client.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/three_ssl_concurrent_client_sockets/three_ssl_client_sockets.slcp --force --with "brd4180a" -np -d output/wifi_three_ssl_client_sockets-brd4180a
make -C output/wifi_three_ssl_client_sockets-brd4180a -f wifi_three_ssl_client_sockets.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/three_ssl_concurrent_client_sockets/three_ssl_client_sockets.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_three_ssl_client_sockets-brd4325a
make -C output/wifi_three_ssl_client_sockets-brd4325a -f wifi_three_ssl_client_sockets.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/three_ssl_concurrent_client_sockets/three_ssl_client_sockets.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_three_ssl_client_sockets-brd4325c
make -C output/wifi_three_ssl_client_sockets-brd4325c -f wifi_three_ssl_client_sockets.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/three_ssl_concurrent_client_sockets/three_ssl_client_sockets.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_three_ssl_client_sockets-brd4325g
make -C output/wifi_three_ssl_client_sockets-brd4325g -f wifi_three_ssl_client_sockets.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_throughput_ble_dual_role/wifi_throughput_ble_dual_role.slcp --force --with "brd4180a" -np -d output/wifi_throughput_ble_dual_role-brd4180a
make -C output/wifi_throughput_ble_dual_role-brd4180a -f wifi_throughput_ble_dual_role.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_throughput_ble_dual_role/wifi_throughput_ble_dual_role.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_throughput_ble_dual_role-brd4325a
make -C output/wifi_throughput_ble_dual_role-brd4325a -f wifi_throughput_ble_dual_role.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_throughput_ble_dual_role/wifi_throughput_ble_dual_role.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_throughput_ble_dual_role-brd4325c
make -C output/wifi_throughput_ble_dual_role-brd4325c -f wifi_throughput_ble_dual_role.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan_ble/wifi_throughput_ble_dual_role/wifi_throughput_ble_dual_role.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_throughput_ble_dual_role-brd4325g
make -C output/wifi_throughput_ble_dual_role-brd4325g -f wifi_throughput_ble_dual_role.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/tls_client/tls_client.slcp --force --with "brd4180a" -np -d output/wifi_tls_client-brd4180a
make -C output/wifi_tls_client-brd4180a -f wifi_tls_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/tls_client/tls_client.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_tls_client-brd4325a
make -C output/wifi_tls_client-brd4325a -f wifi_tls_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/tls_client/tls_client.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_tls_client-brd4325c
make -C output/wifi_tls_client-brd4325c -f wifi_tls_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/tls_client/tls_client.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_tls_client-brd4325g
make -C output/wifi_tls_client-brd4325g -f wifi_tls_client.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/twt_tcp_client/twt_tcp_client.slcp --force --with "brd4180a" -np -d output/wifi_twt_tcp_client-brd4180a
make -C output/wifi_twt_tcp_client-brd4180a -f wifi_twt_tcp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/twt_tcp_client/twt_tcp_client.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_twt_tcp_client-brd4325a
make -C output/wifi_twt_tcp_client-brd4325a -f wifi_twt_tcp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/twt_tcp_client/twt_tcp_client.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_twt_tcp_client-brd4325c
make -C output/wifi_twt_tcp_client-brd4325c -f wifi_twt_tcp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/twt_tcp_client/twt_tcp_client.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_twt_tcp_client-brd4325g
make -C output/wifi_twt_tcp_client-brd4325g -f wifi_twt_tcp_client.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/udp_client/udp_client.slcp --force --with "brd4180a" -np -d output/wifi_udp_client-brd4180a
make -C output/wifi_udp_client-brd4180a -f wifi_udp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/udp_client/udp_client.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_udp_client-brd4325a
make -C output/wifi_udp_client-brd4325a -f wifi_udp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/udp_client/udp_client.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_udp_client-brd4325c
make -C output/wifi_udp_client-brd4325c -f wifi_udp_client.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/_internal/Wireless_Examples/wlan/udp_client/udp_client.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_udp_client-brd4325g
make -C output/wifi_udp_client-brd4325g -f wifi_udp_client.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/wlan_throughput/wlan_throughput.slcp --force --with "brd4180a" -np -d output/wifi_wlan_throughput-brd4180a
make -C output/wifi_wlan_throughput-brd4180a -f wifi_wlan_throughput.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/wlan_throughput/wlan_throughput.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_wlan_throughput-brd4325a
make -C output/wifi_wlan_throughput-brd4325a -f wifi_wlan_throughput.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/wlan_throughput/wlan_throughput.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_wlan_throughput-brd4325c
make -C output/wifi_wlan_throughput-brd4325c -f wifi_wlan_throughput.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/featured/wlan_throughput/wlan_throughput.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_wlan_throughput-brd4325g
make -C output/wifi_wlan_throughput-brd4325g -f wifi_wlan_throughput.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/wlan_throughput_v6/wlan_throughput_v6.slcp --force --with "brd4180a" -np -d output/wifi_wlan_throughput_v6-brd4180a
make -C output/wifi_wlan_throughput_v6-brd4180a -f wifi_wlan_throughput_v6.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/wlan_throughput_v6/wlan_throughput_v6.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wifi_wlan_throughput_v6-brd4325a
make -C output/wifi_wlan_throughput_v6-brd4325a -f wifi_wlan_throughput_v6.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/wlan_throughput_v6/wlan_throughput_v6.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wifi_wlan_throughput_v6-brd4325c
make -C output/wifi_wlan_throughput_v6-brd4325c -f wifi_wlan_throughput_v6.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/wlan_throughput_v6/wlan_throughput_v6.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wifi_wlan_throughput_v6-brd4325g
make -C output/wifi_wlan_throughput_v6-brd4325g -f wifi_wlan_throughput_v6.Makefile -j

/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/wlan_rf_test/wlan_rf_test.slcp --force --with "brd4180a" -np -d output/wlan_rf_test-brd4180a
make -C output/wlan_rf_test-brd4180a -f wlan_rf_test.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/wlan_rf_test/wlan_rf_test.slcp --force --with "brd4325a;wiseconnect3_sdk" -np -d output/wlan_rf_test-brd4325a
make -C output/wlan_rf_test-brd4325a -f wlan_rf_test.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/wlan_rf_test/wlan_rf_test.slcp --force --with "brd4325c;wiseconnect3_sdk" -np -d output/wlan_rf_test-brd4325c
make -C output/wlan_rf_test-brd4325c -f wlan_rf_test.Makefile -j
/slc_cli/slc generate /gsdk/gecko-sdk-dx-v3/extension/wiseconnect3/examples/snippets/wlan/wlan_rf_test/wlan_rf_test.slcp --force --with "brd4325g;wiseconnect3_sdk" -np -d output/wlan_rf_test-brd4325g
make -C output/wlan_rf_test-brd4325g -f wlan_rf_test.Makefile -j

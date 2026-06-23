project(sl_net)

include_directories(./inc
                    ../inc
                    ../../inc
                    ../../../../../../../tests/unit_tests/inc
                    ../../../../../../common/inc
                    ../../../../../../device/silabs/si91x/wireless/socket/inc
                    ../../../../../../gsdk/common/inc
                    ../../../../../../service/bsd_socket/inc
                    ../../../../../../device/silabs/si91x/wireless/sl_net/inc
                    ../../../../../../service/network_manager/inc
                    ../../../../../../protocol/wifi/inc
                    ../../../../../../../third_party/fff
                    ../../../../../../device/silabs/si91x/wireless/inc
                    ../../../../../../device/silabs/si91x/wireless/sli_si91x_wifi_command_engine/inc
                    ../../../../../../gsdk/cmsis/RTOS2/Include
                    ../../../../../../freertos/cmsis/Include
                    ../../../../third_party/googletest/googlemock/include
                    ../../../../third_party/googletest/googletest/include
                    ../../../../../../device/silabs/si91x/wireless/firmware_upgrade
                    ../../../../../../sli_wifi/inc
                    ../../../../../../sli_wifi_per_mode/inc
                    ../../../../../../service/mdns/inc/
                    ../../../../../../sli_queue_manager/inc/
                    ../../../../../../sli_buffer_manager/inc/
                    ../../../../../../sli_routing_utility/inc/
                    ../../../../../../gsdk/service/mem_pool/inc
                    ../../../../../../sli_command_engine/inc
                    ../../../../../../sli_event_engine/inc
                    ../../sli_hal_si91x/inc
                    ../../../../../../device/silabs/si91x/mcu/drivers/service/sl_log/inc
                    ../../unit_tests/inc
)
# Add unit test cpp here
add_executable(${PROJECT_NAME}
                    src/sl_net_unit_tests.cpp
                    src/sl_net_fake_functions.c
                    ../src/sli_net_si91x_utility.c
                    ../src/sl_net_si91x_integration_handler.c
                    ../../../../../../../tests/unit_tests/src/stubs.c
)
# Add unit being tested here\
target_link_libraries(${PROJECT_NAME} PUBLIC 
                    fff
                    gtest
                    gtest_main
)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
target_link_libraries(${PROJECT_NAME} PUBLIC 
                    gcov
)
endif()
# Project name
project(sl_socket_utility)

# Include directories
include_directories(
    ./inc
    ../inc
    ../../../../../../../tests/unit_tests/inc
    ../../../../../../device/silabs/si91x/wireless/inc
    ../../../../../../device/silabs/si91x/wireless/sli_si91x_wifi_command_engine/inc
    ../../../../../../gsdk/common/inc
    ../../../../../../common/inc
    ../../../../../../protocol/wifi/inc
    ../../../../../../gsdk/cmsis/RTOS2/Include
    ../../../../../../../third_party/fff
    ../../../../../../service/bsd_socket/inc
    ../../../../third_party/googletest/googlemock/include
    ../../../../third_party/googletest/googletest/include
    ../../../../../../device/silabs/si91x/wireless/firmware_upgrade
    ../../../../../../sli_wifi/inc
    ../../../../../../sli_queue_manager/inc/
    ../../../../../../sli_buffer_manager/inc/
    ../../../../../../sli_routing_utility/inc/
    ../../../../../../gsdk/service/mem_pool/inc
    ../../../../../../sli_command_engine/inc
    ../../../../../../sli_event_engine/inc
    ../../../../../../device/silabs/si91x/wireless/sli_hal_si91x/inc
    ../../../../../../device/silabs/si91x/wireless/sli_si91x_wifi_event_handler/inc
    ../../../../../../device/silabs/si91x/wireless/sl_net/inc
    ../../../../../../service/network_manager/inc
    ../../../../../../device/silabs/si91x/mcu/drivers/service/sl_log/inc
    ../../unit_tests/inc
)
# Add source files for the test executable
add_executable(${PROJECT_NAME}
    src/sl_si91x_socket_utility_unit_tests.cpp
    src/sl_si91x_socket_utility_fake_function.c
    ../src/sl_si91x_socket_utility.c
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
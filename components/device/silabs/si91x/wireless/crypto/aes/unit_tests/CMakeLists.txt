# Project name
project(sl_aes_unit_tests)

# Include directories
include_directories(
    ../../../../../../../../tests/unit_tests/inc
    ../inc
    ../../../device/silabs/si91x/wireless/inc
    ../../../sli_si91x_wifi_command_engine/inc
    ../../../device/silabs/si91x/wireless/sl_net/inc
    ../../../firmware_upgrade
    ../../../device/silabs/si91x/wireless/ahb_interface/inc
    ../../../device/silabs/si91x/mcu/drivers/systemlevel/inc
    ../../../device/silabs/si91x/mcu/core/chip/inc
    ../../../../../../../gsdk/cmsis/RTOS2/Include
    ../../../../mcu/core/config
    ../../../board/silabs/config/brd4338a
    ../../../../resources/defaults
    ../../../gsdk/common/inc
    ../../../gsdk/cmsis/RTOS2/Include
    ../../../common/inc
    ../../../../third_party/googletest/googletest/include
    ../../../../third_party/googletest/googlemock/include
    ../../../../../../../../third_party/fff
    ../../../../../../stm32/silabs_utility/common/inc                          
    ../../../../../../../device/silabs/si91x/wireless/crypto/aes/inc
    ../../inc
    ../../../inc
    ../../../../../../../common/inc
    ../../../../../../../../components/sli_wifi/inc
    ../../../../../../../protocol/wifi/inc
    ../../../../../../../service/mdns/inc/
    ./inc 
    ../../../../../../../sli_queue_manager/inc/
    ../../../../../../../sli_buffer_manager/inc/
    ../../../../../../../sli_routing_utility/inc/
    ../../../../../../../gsdk/service/mem_pool/inc
    ../../../../../../../sli_command_engine/inc
    ../../../../../../../sli_event_engine/inc
    ../../../../../../../device/silabs/si91x/mcu/drivers/service/sl_log/inc
    ../../../unit_tests/inc
)

# Add source files for the test executable
add_executable(${PROJECT_NAME}
    src/sli_aes_fake_functions.c
    src/sli_aes_unit_tests.cpp
    ../../../../../../../device/silabs/si91x/wireless/crypto/aes/src/sl_si91x_aes.c
    ../../../../../../../../tests/unit_tests/src/stubs.c
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC 
                      gtest
                      gtest_main
                      fff
)

# Enable coverage for Clang/GCC
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_link_libraries(${PROJECT_NAME} PUBLIC gcov)
endif()

project(network_manager)

include_directories(../../../../tests/unit_tests/inc
                    ../inc
                    ../../../device/silabs/si91x/wireless/inc
                    ../../../device/silabs/si91x/wireless/sl_net/inc
                    ../../../../resources/defaults
                    ../../../gsdk/common/inc
                    ../../../common/inc
                    ../../../protocol/wifi/inc
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
add_compile_options("--coverage")
endif()

set(CMAKE_C_STANDARD 11)
add_library(network_manager 
            ../src/sl_net.c 
            ../si91x/sl_net_si91x.c
            ../../../../tests/unit_tests/src/stubs.c
)

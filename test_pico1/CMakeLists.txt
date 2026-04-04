# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico1_test C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_definitions(-DAPP_TITLE="Pico 1z")

# Add executable. Default name is the project name, version 0.1

add_executable(pico1_test 
    ../src/test.c 
    ../src/mhz19b.c 
    ../src/DEV_Config.c 
    ../src/EPD_1in54_V2.c 
    ../src/font24.c 
    ../src/eink.c 
    ../src/bme280_i2c.c 
    ../src/font16.c 
    ../src/font50.c
    ../src/font05.c
    ../src/printstream.c
    ../src/eink_plot.c
    ../src/button.c
    ../src/led.c
    )


pico_generate_pio_header(pico1_test ${CMAKE_CURRENT_LIST_DIR}/../src/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)


pico_set_program_name(pico1_test "pico1_test")
pico_set_program_version(pico1_test "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(pico1_test 1)
pico_enable_stdio_usb(pico1_test 0)

# Add the standard library to the build
target_link_libraries(pico1_test
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(pico1_test PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/generated
)

# Add any user requested libraries
target_link_libraries(pico1_test 
        hardware_spi
        hardware_i2c
		hardware_pio
        )

pico_add_extra_outputs(pico1_test)


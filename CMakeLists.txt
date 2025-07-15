cmake_minimum_required(VERSION 3.20)

set(NAME rp2350_test)

# if (NOT PICO_XOSC_STARTUP_DELAY_MULTIPLIER)
	# set(PICO_XOSC_STARTUP_DELAY_MULTIPLIER 32)
	# message(STATUS "XOSC delay defined")
# endif()

set(PICO_SDK_PATH "../../../../pico-sdk/")
set(PIMORONI_PICO_PATH "../../../../pimoroni/")

include(~/workspace/pico-sdk/pico_sdk_init.cmake)
include(~/workspace/pimoroni/pimoroni_pico_import.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(${NAME} C CXX ASM)

pico_sdk_init()

file(GLOB_RECURSE source_files "src/*.cpp")

add_executable(${NAME} ${source_files})

include(common/pimoroni_i2c)
include(common/pimoroni_bus)
include(drivers/st7789/st7789)
include(drivers/button/button)
include(libraries/bitmap_fonts/bitmap_fonts)
include(libraries/hershey_fonts/hershey_fonts)
include(libraries/pico_graphics/pico_graphics)

pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)

target_link_libraries(${NAME} pico_stdlib pimoroni_i2c pico_graphics st7789 button)

pico_add_extra_outputs(${NAME})

# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2 ${CMAKE_CURRENT_LIST_DIR}/README.md DESTINATION .)

# set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
# set(CPACK_GENERATOR "ZIP" "TGZ")
# include(CPack)

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
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(proj0302EmbarcaTech C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(proj0302EmbarcaTech
    proj0302EmbarcaTech.c
    src/ws2812b.pio.c
    src/ssd1306.c
    src/font.c
)

# Set program name and version
pico_set_program_name(proj0302EmbarcaTech "proj0302EmbarcaTech")
pico_set_program_version(proj0302EmbarcaTech "0.1")

# Generate PIO header for each .pio file (ws2812b and ssd1306)
pico_generate_pio_header(proj0302EmbarcaTech ${CMAKE_CURRENT_LIST_DIR}/ws2812b.pio)
pico_generate_pio_header(proj0302EmbarcaTech ${CMAKE_CURRENT_LIST_DIR}/ssd1306.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(proj0302EmbarcaTech 1)
pico_enable_stdio_usb(proj0302EmbarcaTech 0)

# Add the standard library to the build
target_link_libraries(proj0302EmbarcaTech
        pico_stdlib
        hardware_i2c
        hardware_pio
)

# Adicionar o diretório contendo o arquivo de cabeçalho
include_directories(K:/PROG/ProjectsUnity/proj0302EmbarcaTech)
include_directories(K:/PROG/ProjectsUnity/proj0302EmbarcaTech/inc)
target_link_libraries(proj0302EmbarcaTech PUBLIC ws2812b ssd1306)

# Add the standard include files to the build
target_include_directories(proj0302EmbarcaTech PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add extra outputs (UF2, BIN, etc)
pico_add_extra_outputs(proj0302EmbarcaTech)

cmake_minimum_required(VERSION 3.13)
# code for vscode pico extension, stolen from example cmakleists file
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

project(Capstone_Code C CXX ASM)

# include sdk file
include(pico_sdk_import.cmake)

# Initialize the SDK
pico_sdk_init()

# .c files are in src
file(GLOB SOURCES "src/*.c")

add_executable(Capstone_Code ${SOURCES})

# headers are in inc
target_include_directories(Capstone_Code PRIVATE inc)

# link libraries
target_link_libraries(Capstone_Code pico_stdlib)

# generate UF2/HEX files
pico_add_extra_outputs(Capstone_Code)
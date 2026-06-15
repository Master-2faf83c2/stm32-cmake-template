cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(DEFAULT_TARGET "app")
set(SCRIPTS_DIR "${CMAKE_CURRENT_LIST_DIR}/scripts")

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
if(DEFINED TARGET_NAME)
    set(CMAKE_PROJECT_NAME ${TARGET_NAME})
else()
    set(CMAKE_PROJECT_NAME ${DEFAULT_TARGET})
endif()


# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include("${CMAKE_SOURCE_DIR}/hal/cmake/gcc-arm-none-eabi.cmake")

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

set_property(GLOBAL APPEND PROPERTY ALL_SOURCES "")
set_property(GLOBAL APPEND PROPERTY ALL_LINK "")
set_property(GLOBAL APPEND PROPERTY ALL_INCLUDE "")
set_property(GLOBAL APPEND PROPERTY ALL_DEFINE "")

add_subdirectory(3rdparty)
add_subdirectory(hal)
add_subdirectory(src)

get_property(ALL_SOURCES GLOBAL PROPERTY ALL_SOURCES)
get_property(ALL_LINK GLOBAL PROPERTY ALL_LINK)
get_property(ALL_INCLUDE GLOBAL PROPERTY ALL_INCLUDE)
get_property(ALL_DEFINE  GLOBAL PROPERTY ALL_DEFINE)

message("宏定义: ${ALL_DEFINE}")
message("链接路径: ${ALL_LINK}")
message("头文件: ${ALL_INCLUDE}")
message("源文件: ${ALL_SOURCES}")

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${ALL_SOURCES}
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${ALL_INCLUDE}
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    ${ALL_DEFINE}
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    ${ALL_LINK}
)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
    COMMENT "Generating ${CMAKE_PROJECT_NAME}.bin"
)

add_custom_target(merge_compile_commands ALL
    COMMAND python ${SCRIPTS_DIR}/merge_compile_commands.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMENT "merge compile_commands.json file"
    DEPENDS ${CMAKE_PROJECT_NAME}
)

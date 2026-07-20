cmake_minimum_required(VERSION 3.22)

project(STM32H745_DualCore NONE)

include(ExternalProject)

set(TOOLCHAIN_FILE
    "${CMAKE_SOURCE_DIR}/hal/gcc-arm-none-eabi.cmake"
)

ExternalProject_Add(cm7_firmware
    SOURCE_DIR  "${CMAKE_SOURCE_DIR}/hal/CM7"
    BINARY_DIR  "${CMAKE_BINARY_DIR}/CM7"

    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}

    INSTALL_COMMAND ""
)

ExternalProject_Add(cm4_firmware
    SOURCE_DIR  "${CMAKE_SOURCE_DIR}/hal/CM4"
    BINARY_DIR  "${CMAKE_BINARY_DIR}/CM4"

    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}

    INSTALL_COMMAND ""
)

add_custom_target(firmware ALL
    DEPENDS cm7_firmware cm4_firmware
)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_custom_target(merge_compile_commands ALL
    COMMAND
        ${Python3_EXECUTABLE}
        "${CMAKE_SOURCE_DIR}/scripts/merge_compile_commands.py"

    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    DEPENDS cm7_firmware cm4_firmware
    COMMENT "Merging CM7 and CM4 compile_commands.json"
)
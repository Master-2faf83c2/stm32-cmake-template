cmake_minimum_required(VERSION 3.22)

#默认构建目标
set(DEFAULT_TARGET "app")
set(SCRIPTS_DIR "${CMAKE_CURRENT_LIST_DIR}/scripts")

set(CMAKE_C_STANDARD 11)            
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if(DEFINED TARGET_NAME)
    set(CMAKE_PROJECT_NAME ${TARGET_NAME})
else()
    set(CMAKE_PROJECT_NAME ${DEFAULT_TARGET})
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include("${CMAKE_SOURCE_DIR}/scripts/cmake/gcc-arm-none-eabi.cmake")

project(${CMAKE_PROJECT_NAME})
message("构建目标: ${CMAKE_PROJECT_NAME}, 编译类型: ${CMAKE_BUILD_TYPE}")

#告诉CMake 需要支持C 和 汇编
enable_language(C ASM)

set_property(GLOBAL APPEND PROPERTY ALL_SOURCES "")
set_property(GLOBAL APPEND PROPERTY ALL_INCLUDE "")
set_property(GLOBAL APPEND PROPERTY ALL_DEFINE "")

add_subdirectory(src)

get_property(ALL_SOURCES GLOBAL PROPERTY ALL_SOURCES)
get_property(ALL_INCLUDE GLOBAL PROPERTY ALL_INCLUDE)
get_property(ALL_DEFINE  GLOBAL PROPERTY ALL_DEFINE)

message("宏定义: ${ALL_DEFINE}")
message("头文件: ${ALL_INCLUDE}")
message("源文件: ${ALL_SOURCES}")

add_executable(${PROJECT_NAME} ${ALL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${ALL_INCLUDE})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${ALL_DEFINE})

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
    COMMENT "Generating ${CMAKE_PROJECT_NAME}.bin"
)

# add_custom_target(merge_compile_commands ALL
#     COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/scripts/merge_compile_commands.sh
#     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
#     COMMENT "Merging compile_commands.json from app and bootloader"
# )

add_custom_target(merge_compile_commands ALL
    COMMAND python ${SCRIPTS_DIR}/python/merge_compile_commands.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMENT "merge compile_commands.json file"
    DEPENDS ${CMAKE_PROJECT_NAME}
)

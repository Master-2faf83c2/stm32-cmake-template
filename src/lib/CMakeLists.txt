file(GLOB_RECURSE FIND_FILE
    ${CMAKE_CURRENT_LIST_DIR}/cmsis/startup/startup_stm32l1xx_md.s
    ${CMAKE_CURRENT_LIST_DIR}/cmsis/*.c
    ${CMAKE_CURRENT_LIST_DIR}/driver/src/*.c
)

set_property(GLOBAL APPEND PROPERTY ALL_DEFINE 
    STM32L1XX_MD
    USE_STDPERIPH_DRIVER
)

set_property(GLOBAL APPEND PROPERTY ALL_INCLUDE 
    ${CMAKE_CURRENT_LIST_DIR}/cmsis/startup
    ${CMAKE_CURRENT_LIST_DIR}/cmsis
    ${CMAKE_CURRENT_LIST_DIR}/driver/inc
)

set_property(
    GLOBAL APPEND PROPERTY ALL_SOURCES 
    ${FIND_FILE}
)

# message("源文件: ${ALL_SOURCES}, 头文件: ${ALL_INCLUDE}")
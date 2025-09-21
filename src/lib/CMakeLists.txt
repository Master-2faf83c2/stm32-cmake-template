file(GLOB_RECURSE FIND_FILE
    ${CMAKE_CURRENT_LIST_DIR}/cmsis/startup/startup_stm32f40_41xxx.s
    ${CMAKE_CURRENT_LIST_DIR}/cmsis/*.c
    ${CMAKE_CURRENT_LIST_DIR}/driver/src/*.c
)

set_property(GLOBAL APPEND PROPERTY ALL_DEFINE 
    STM32F40_41xxx 
    USE_STDPERIPH_DRIVER
)

# 排除不想要的文件
list(FILTER FIND_FILE EXCLUDE REGEX ".*stm32f4xx_(dma2d|fmc|ltdc)\\.c$")

set_property(GLOBAL APPEND PROPERTY ALL_INCLUDE 
    ${CMAKE_CURRENT_LIST_DIR}/cmsis/startup
    ${CMAKE_CURRENT_LIST_DIR}/cmsis
    ${CMAKE_CURRENT_LIST_DIR}/driver/inc
)

set_property(
    GLOBAL APPEND PROPERTY ALL_SOURCES 
    ${FIND_FILE}
)

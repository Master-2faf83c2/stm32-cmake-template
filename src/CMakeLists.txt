set_property(
    GLOBAL APPEND PROPERTY ALL_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/app.c
)

set_property(GLOBAL APPEND PROPERTY ALL_INCLUDE 
    ${CMAKE_CURRENT_LIST_DIR}
)

add_subdirectory(core)
add_subdirectory(sys)
add_subdirectory(utils)

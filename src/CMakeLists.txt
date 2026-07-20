function(add_project_sources TARGET CORE)

    set(SRC_ROOT "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")

    file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS
        "${SRC_ROOT}/common/*.c"
        "${SRC_ROOT}/drivers/*.c"
        "${SRC_ROOT}/utils/*.c"
    )

    target_sources(${TARGET} PRIVATE
        ${COMMON_SOURCES}
    )

    target_include_directories(${TARGET} PRIVATE
        "${SRC_ROOT}/common"
        "${SRC_ROOT}/drivers"
        "${SRC_ROOT}/utils"
    )

    if(CORE STREQUAL "CM7")
        file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
            "${SRC_ROOT}/cm7/*.c"
        )

        target_sources(${TARGET} PRIVATE
            ${CORE_SOURCES}
        )

        target_include_directories(${TARGET} PRIVATE
            "${SRC_ROOT}/cm7"
        )

        target_compile_definitions(${TARGET} PRIVATE
            APP_CORE_CM7
        )

    elseif(CORE STREQUAL "CM4")
        file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
            "${SRC_ROOT}/cm4/*.c"
        )

        target_sources(${TARGET} PRIVATE
            ${CORE_SOURCES}
        )

        target_include_directories(${TARGET} PRIVATE
            "${SRC_ROOT}/cm4"
        )

        target_compile_definitions(${TARGET} PRIVATE
            APP_CORE_CM4
        )

    else()
        message(FATAL_ERROR "Unsupported core: ${CORE}")
    endif()

endfunction()
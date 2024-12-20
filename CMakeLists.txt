#
# Created by An Inconspicuous Semicolon on 18/12/2024.
#

cmake_minimum_required(VERSION 3.5)
project(
        pch_generator
        VERSION 0.1.0
        DESCRIPTION "A C++20 executable for generating precompiled headers for a C++ project"
        HOMEPAGE_URL "https://github.com/an-inconspicuous-semicolon/pch_generator"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

add_executable(pch_generator 
        src/main.cpp
        include/ThreadPool.hpp src/ThreadPool.cpp
        include/util.hpp src/util.cpp
        )
target_include_directories(pch_generator
        PUBLIC include
        PRIVATE
)
target_link_libraries(pch_generator
        PUBLIC
        PRIVATE
)

function(generate_pch_for_target TARGET)
        if(NOT EXISTS "C:/Program Files/LLVM/bin/clang++.exe")
                message(STATUS "Precompiled headers require clang to be installed")
                return()
        endif()

        get_target_property(TARGET_SOURCES ${TARGET} SOURCES)

        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_sources.txt" "")
        foreach(SOURCE_FILE ${TARGET_SOURCES})
                file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_sources.txt" "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}\n")
        endforeach()

        get_target_property(TARGET_DEFINES ${TARGET} COMPILE_DEFINITIONS)

        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_defines.txt" "")
        foreach(DEFINE ${TARGET_DEFINES})
                file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_defines.txt" "${DEFINE}\n")
        endforeach()

        FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pch/${TARGET})

        add_custom_target(generate_pch_for_${TARGET}
                COMMAND $<TARGET_FILE:pch_generator>
                ${CMAKE_CURRENT_BINARY_DIR}/pch/${TARGET}
                ${CMAKE_CURRENT_BINARY_DIR}/pch/${TARGET}/__${TARGET}_pch.hpp
                ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_sources.txt
                ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_defines.txt
                "C:/Program Files/LLVM/bin/clang++.exe"
                COMMENT "Generating a precompiled header for ${TARGET}"
                VERBATIM
        )

        add_dependencies(${TARGET} generate_pch_for_${TARGET})
        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/pch/${TARGET}/__${TARGET}_pch.hpp" "")
        target_precompile_headers(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/pch/${TARGET}/__${TARGET}_pch.hpp)
endfunction()

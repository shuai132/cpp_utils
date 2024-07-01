cmake_minimum_required(VERSION 3.13)
project(cpp_utils)

option(cpp_utils_DISABLE_DETAIL_PRINT "" OFF)

if (MSVC)
    set(CMAKE_CXX_STANDARD 20)
    add_compile_options(/Zc:preprocessor)
    add_compile_options(/utf-8)
    add_compile_options(-DNOMINMAX)
    add_compile_options(-D_WIN32_WINNT=0x0601)
else ()
    set(CMAKE_CXX_STANDARD 14)
    add_compile_options(-Wall -Wunused-parameter)
endif ()

# for github actions/ci
if (cpp_utils_DISABLE_DETAIL_PRINT)
    add_definitions(-Dcpp_utils_DISABLE_DETAIL_PRINT)
endif ()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE .)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} INTERFACE pthread)
endif ()

# tests
link_libraries(${PROJECT_NAME})
file(GLOB_RECURSE CPP_SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/tests/*.cpp)
foreach (file ${CPP_SRC_LIST})
    string(REGEX REPLACE ".*tests|.cpp|/|\\\\" "" target_name ${file})

    set(unsupported_list Shell MutexSystemV MutexPosix make_unique thread_safe)
    if (WIN32 AND target_name IN_LIST unsupported_list)
        message(STATUS "Windows skip: ${target_name}")
        continue()
    endif ()

    add_executable(test_${target_name} ${file})
endforeach ()

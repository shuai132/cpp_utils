cmake_minimum_required(VERSION 3.13)
project(cpp_utils)

option(cpp_utils_DISABLE_DETAIL_PRINT "" OFF)

set(CMAKE_CXX_STANDARD 14)

# for github actions/ci
if (cpp_utils_DISABLE_DETAIL_PRINT)
    add_definitions(-Dcpp_utils_DISABLE_DETAIL_PRINT)
endif ()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE .)
target_link_libraries(${PROJECT_NAME} INTERFACE pthread)

# tests
link_libraries(${PROJECT_NAME})
file(GLOB_RECURSE CPP_SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/tests/*.cpp)
foreach (file ${CPP_SRC_LIST})
    string(REGEX REPLACE ".*tests|.cpp|/|\\\\" "" target_name ${file})
    add_executable(test_${target_name} ${file})
endforeach ()

cmake_minimum_required(VERSION 2.8)
project(cpp_utils)

set(CMAKE_CXX_STANDARD 11)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE .)

# tests
link_libraries(${PROJECT_NAME})
file(GLOB_RECURSE CPP_SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/tests/*.cpp)
foreach(file ${CPP_SRC_LIST})
    string(REGEX REPLACE ".*tests|.cpp|/|\\\\" "" target_name ${file})
    add_executable(test_${target_name} ${file})
endforeach()

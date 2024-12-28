project(test_lib)

add_library(${PROJECT_NAME}_so SHARED test_lib_so.cpp)
target_include_directories(${PROJECT_NAME}_so PUBLIC .)

add_library(${PROJECT_NAME}_a STATIC test_lib_a.cpp)
target_include_directories(${PROJECT_NAME}_a PUBLIC .)

add_library(${PROJECT_NAME} INTERFACE)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_so ${PROJECT_NAME}_a)

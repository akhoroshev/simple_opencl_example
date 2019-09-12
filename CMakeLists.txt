cmake_minimum_required(VERSION 3.1)
project(bitwise_matrix_and)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

file(COPY "${PROJECT_SOURCE_DIR}/kernel.cl" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(bitwise_matrix_and main.cpp)
target_link_libraries(bitwise_matrix_and OpenCL)

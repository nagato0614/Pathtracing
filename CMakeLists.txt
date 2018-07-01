cmake_minimum_required(VERSION 3.10)
project(pathtracing)

set(CMAKE_CXX_STANDARD 17)

# ターゲットファイル
file(GLOB src
        ${PROJECT_SOURCE_DIR}/*.h
        ${PROJECT_SOURCE_DIR}/*.hpp
        ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(pathtracing ${src})

# compile option
add_compile_options(-O2)
add_compile_options(-march=native)
add_compile_options(-Wall)

# openMP
find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif ()

message(status ${CMAKE_CXX_FLAGS})

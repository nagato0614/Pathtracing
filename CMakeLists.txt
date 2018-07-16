cmake_minimum_required(VERSION 3.10)
project(pathtracing)

set(CMAKE_CXX_STANDARD 17)

# ターゲットファイル
file(GLOB src
        ${PROJECT_SOURCE_DIR}/*.h
        ${PROJECT_SOURCE_DIR}/*.hpp
        ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(pathtracing ${src})
add_executable(matrixtest test/test.cpp tiny_obj_loader.h)
add_executable(test_tinyexr test/test_tinyexr.cc tinyexr.h tinyexr.cc)
add_executable(test_obj_loader test/loader_example.cc tiny_obj_loader.h)

add_subdirectory(ilmbase-2.2.1)

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

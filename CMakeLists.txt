cmake_minimum_required(VERSION 3.10)
project(pathtracing)

set(CMAKE_CXX_STANDARD 17)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)


file(GLOB src
        ${PROJECT_SOURCE_DIR}/src/core/*.cc
        ${PROJECT_SOURCE_DIR}/src/core/*.h
        ${PROJECT_SOURCE_DIR}/src/core/*.cpp
        ${PROJECT_SOURCE_DIR}/src/core/*.hpp
        ${PROJECT_SOURCE_DIR}/src/color/*.cpp
        ${PROJECT_SOURCE_DIR}/src/color/*.hpp
        ${PROJECT_SOURCE_DIR}/src/light/*.cpp
        ${PROJECT_SOURCE_DIR}/src/light/*.hpp
        ${PROJECT_SOURCE_DIR}/src/linearAlgebra/*.cpp
        ${PROJECT_SOURCE_DIR}/src/linearAlgebra/*.hpp
        ${PROJECT_SOURCE_DIR}/src/material/*.cpp
        ${PROJECT_SOURCE_DIR}/src/material/*.hpp
        ${PROJECT_SOURCE_DIR}/src/object/*.cpp
        ${PROJECT_SOURCE_DIR}/src/object/*.hpp
        ${PROJECT_SOURCE_DIR}/src/structure/*.cpp
        ${PROJECT_SOURCE_DIR}/src/structure/*.hpp)

# ライブラリ作成
add_library(pathtracingLibrary STATIC ${src})

#MESSAGE(STATUS "${src}")

add_executable(pathtracing main.cpp)

target_link_libraries(pathtracing pathtracingLibrary)

# compile option
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -mtune=native -march=native -mfpmath=both -flto -fwhole-program")


# openMP
find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenMP_CXX_FLAGS}")
endif ()

message(STATUS "## g++ options : ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "## g++ options : ${CMAKE_CXX_FLAGS_RELEASE}")



cmake_minimum_required(VERSION 3.10)
project(pathtracing)

set(CMAKE_CXX_STANDARD 17)

if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(googletest-distribution)
set(GOOGLETEST_VERSION 1.9.0)

enable_testing()

include(CMakeDependentOption)
include(GNUInstallDirs)

#Note that googlemock target already builds googletest
option(BUILD_GMOCK "Builds the googlemock subproject" ON)
option(INSTALL_GTEST "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" ON)

if(BUILD_GMOCK)
    add_subdirectory( googlemock )
else()
    add_subdirectory( googletest )
endif()

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
add_executable(bvh_test src/test/bvh_test.cpp)

target_link_libraries(pathtracing pathtracingLibrary)
target_link_libraries(bvh_test pathtracingLibrary)


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



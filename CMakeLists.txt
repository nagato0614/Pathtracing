cmake_minimum_required(VERSION 3.10)
project(pathtracing)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MACOSX_RPATH 1)

# google test download
include(${PROJECT_SOURCE_DIR}/cmake/DownloadProject/DownloadProject.cmake)
download_project(PROJ googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master
        UPDATE_DISCONNECTED 1
        )

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

include(GoogleTest)

# Google Test Definitions for macOS
if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

#openEXR
find_package(zlib)
find_package(boost)

SET(ILMBASE_NAMESPACE_VERSIONING OFF CACHE BOOL " " FORCE)
SET(OPENEXR_NAMESPACE_VERSIONING OFF CACHE BOOL " " FORCE)
SET(OPENEXR_BUILD_SHARED_LIBS    OFF CACHE BOOL " " FORCE)
SET(ILMBASE_BUILD_SHARED_LIBS    OFF CACHE BOOL " " FORCE)
SET(OPENEXR_BUILD_PYTHON_LIBS    OFF CACHE BOOL " " FORCE)

add_subdirectory(src/openEXR)


INCLUDE_DIRECTORIES (
        src/openEXR/IlmBase/Imath
        src/openEXR/IlmBase/Half
        src/openEXR/IlmBase/Iex
        src/openEXR/OpenEXR/IlmImf
        ${CMAKE_BINARY_DIR}/src/openEXR/IlmBase/config
        ${CMAKE_BINARY_DIR}/src/openEXR/OpenEXR/config
)

SET(OPENEXR_LIBS IlmImf Imath Half)

file(GLOB src
        ${PROJECT_SOURCE_DIR}/src/core/*.cc
        ${PROJECT_SOURCE_DIR}/src/core/*.h
        ${PROJECT_SOURCE_DIR}/src/core/*.cpp
        ${PROJECT_SOURCE_DIR}/src/core/*.hpp
        ${PROJECT_SOURCE_DIR}/src/color/*.cpp
        ${PROJECT_SOURCE_DIR}/src/color/*.hpp
        ${PROJECT_SOURCE_DIR}/src/linearAlgebra/*.cpp
        ${PROJECT_SOURCE_DIR}/src/linearAlgebra/*.hpp
        ${PROJECT_SOURCE_DIR}/src/material/*.cpp
        ${PROJECT_SOURCE_DIR}/src/material/*.hpp
        ${PROJECT_SOURCE_DIR}/src/object/*.cpp
        ${PROJECT_SOURCE_DIR}/src/object/*.hpp
        ${PROJECT_SOURCE_DIR}/src/structure/*.cpp
        ${PROJECT_SOURCE_DIR}/src/structure/*.hpp
        ${PROJECT_SOURCE_DIR}/src/BSDF/*.cpp
        ${PROJECT_SOURCE_DIR}/src/BSDF/*.hpp
        ${PROJECT_SOURCE_DIR}/src/film/*.hpp
        ${PROJECT_SOURCE_DIR}/src/film/*.cpp
        ${PROJECT_SOURCE_DIR}/src/camera/*.hpp
        ${PROJECT_SOURCE_DIR}/src/camera/*.cpp
        ${PROJECT_SOURCE_DIR}/src/render/*.cpp
        ${PROJECT_SOURCE_DIR}/src/render/*.hpp
        ${PROJECT_SOURCE_DIR}/src/sky/*.cpp
        ${PROJECT_SOURCE_DIR}/src/sky/*.hpp
        )

# ライブラリ作成
add_library(pathtracingLibrary STATIC ${src})
add_executable(progressbar src/tools/progressbar.cpp)
add_executable(pathtracing main.cpp)
add_executable(nagatoTest
        src/gtest/gtests.cpp)

target_link_libraries(pathtracing
        pathtracingLibrary
        ${OPENEXR_LIBS}
        )

target_link_libraries(
        nagatoTest
        pathtracingLibrary
        gmock
        gtest
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # compile option
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -Wall")
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fopenmp")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fopenmp")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # compile option
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -mtune=native -march=native -mfpmath=both -flto -fwhole-program")


    # openMP
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fopenmp")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fopenmp")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
endif()

#デバッグ時に定義
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DMY_DEBUG")

message(STATUS "## g++ options : ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "## g++ options : ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "## Using Compiler ID: ${CMAKE_CXX_COMPILER_ID}")

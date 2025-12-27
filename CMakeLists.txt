cmake_minimum_required(VERSION 3.12)
project(pathtracing)
set(CMAKE_CXX_STANDARD 20)

cmake_policy(SET CMP0074 NEW)
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
set(OpenMP_ROOT "/opt/homebrew/opt/libomp")

set(CMAKE_MACOSX_RPATH 1)

# google test download
include(${PROJECT_SOURCE_DIR}/cmake/DownloadProject/DownloadProject.cmake)
download_project(PROJ googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
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
find_package(ZLIB REQUIRED)
find_package(Imath REQUIRED)
find_package(OpenEXR REQUIRED)

include_directories(
        include
        /opt/homebrew/include
        /opt/homebrew/include/OpenEXR
        /opt/homebrew/include/Imath
)

link_directories(/opt/homebrew/lib)

file(GLOB src
        ${PROJECT_SOURCE_DIR}/src/core/*.cc
        ${PROJECT_SOURCE_DIR}/src/core/*.h
        ${PROJECT_SOURCE_DIR}/src/core/*.cpp
        ${PROJECT_SOURCE_DIR}/src/color/*.cpp
        ${PROJECT_SOURCE_DIR}/src/linearAlgebra/*.cpp
        ${PROJECT_SOURCE_DIR}/src/material/*.cpp
        ${PROJECT_SOURCE_DIR}/src/object/*.cpp
        ${PROJECT_SOURCE_DIR}/src/structure/*.cpp
        ${PROJECT_SOURCE_DIR}/src/BSDF/*.cpp
        ${PROJECT_SOURCE_DIR}/src/film/*.cpp
        ${PROJECT_SOURCE_DIR}/src/camera/*.cpp
        ${PROJECT_SOURCE_DIR}/src/render/*.cpp
        ${PROJECT_SOURCE_DIR}/src/sky/*.cpp
        )

file(GLOB hpp
        ${PROJECT_SOURCE_DIR}/include/core/*.hpp
        ${PROJECT_SOURCE_DIR}/include/color/*.hpp
        ${PROJECT_SOURCE_DIR}/include/linearAlgebra/*.hpp
        ${PROJECT_SOURCE_DIR}/include/material/*.hpp
        ${PROJECT_SOURCE_DIR}/include/object/*.hpp
        ${PROJECT_SOURCE_DIR}/include/structure/*.hpp
        ${PROJECT_SOURCE_DIR}/include/BSDF/*.hpp
        ${PROJECT_SOURCE_DIR}/include/film/*.hpp
        ${PROJECT_SOURCE_DIR}/include/camera/*.hpp
        ${PROJECT_SOURCE_DIR}/include/render/*.hpp
        ${PROJECT_SOURCE_DIR}/include/sky/*.hpp
        )

# SDL3
add_subdirectory(third_party/SDL)

# ライブラリ作成
add_library(pathtracingLibrary STATIC ${src} ${hpp})
target_include_directories(pathtracingLibrary PUBLIC include)
add_executable(progressbar src/tools/progressbar.cpp)
add_executable(pathtracing main.cpp)
add_executable(nagatoTest
        src/gtest/gtests.cpp)
add_executable(sdl_preview src/tools/sdl_preview.cpp)

target_link_libraries(pathtracing
        pathtracingLibrary
        OpenEXR
        Iex
        Imath
        )

target_link_libraries(sdl_preview
        pathtracingLibrary
        SDL3::SDL3
        )

target_link_libraries(
        nagatoTest
        pathtracingLibrary
        gmock
        gtest
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    # compile option
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -Wall")
    find_package(OpenMP)
    if (OpenMP_CXX_FOUND)
        link_libraries(OpenMP::OpenMP_CXX)
        include_directories(${OpenMP_CXX_INCLUDE_DIRS})
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

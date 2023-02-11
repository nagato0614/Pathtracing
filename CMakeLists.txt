cmake_minimum_required(VERSION 3.10)
project(pathtracing)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MACOSX_RPATH 1)


# Google Test Definitions for macOS
if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

SET(ILMBASE_NAMESPACE_VERSIONING OFF CACHE BOOL " " FORCE)
SET(OPENEXR_NAMESPACE_VERSIONING OFF CACHE BOOL " " FORCE)
SET(OPENEXR_BUILD_SHARED_LIBS    OFF CACHE BOOL " " FORCE)
SET(ILMBASE_BUILD_SHARED_LIBS    OFF CACHE BOOL " " FORCE)
SET(OPENEXR_BUILD_PYTHON_LIBS    OFF CACHE BOOL " " FORCE)

#add_subdirectory(src/openEXR)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB src
        ${PROJECT_SOURCE_DIR}/src/*/*.cpp
        )

# ライブラリ作成
add_library(pathtracingLibrary STATIC ${src})
add_executable(progressbar tools/progressbar.cpp)
add_executable(pathtracing main.cpp)
#add_executable(nagatoTest
#        src/gtest/gtests.cpp)

target_link_libraries(pathtracing
        "-pthread"
        pathtracingLibrary
        )

#target_link_libraries(
#        nagatoTest
#        pathtracingLibrary
#        gmock
#        gtest
#        )


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

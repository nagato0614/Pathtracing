cmake_minimum_required(VERSION 3.10)
project(pathtracing)

set(CMAKE_CXX_STANDARD 17)

file(GLOB src
        ${PROJECT_SOURCE_DIR}/src/*.cc
        ${PROJECT_SOURCE_DIR}/src/*.h
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        ${PROJECT_SOURCE_DIR}/src/*.hpp)

# ライブラリ作成
add_library(pathtracingLibrary STATIC ${src})

#MESSAGE(STATUS "${src}")

add_executable(pathtracing main.cpp)
add_executable(spectrum test/spectrum_test.cpp)
add_executable(wavept test/wave.cpp)
add_executable(csv_test test/csv_test.cpp)
add_executable(spectral_class_test test/spectral_class_test.cpp)

target_link_libraries(spectrum pathtracingLibrary)
target_link_libraries(pathtracing pathtracingLibrary)
target_link_libraries(csv_test pathtracingLibrary)
target_link_libraries(spectral_class_test pathtracingLibrary)


# compile option
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -mtune=native -march=native -mfpmath=both")


# openMP
find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenMP_CXX_FLAGS}")
endif ()

message(STATUS "## g++ options : ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "## g++ options : ${CMAKE_CXX_FLAGS_RELEASE}")


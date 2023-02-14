cmake_minimum_required(VERSION 3.23)
project(benchmark_shm LANGUAGES CXX)

#### Include section ####
include(FetchContent)

#### Settings ####
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BENCHMARK_ENABLE_TESTING NO)

enable_testing()

add_executable(benchmark_shm benchmark_shm.cpp)

FetchContent_Declare(
    install
    GIT_REPOSITORY git@github.com:MQCore/install.cmake.git
)
FetchContent_MakeAvailable(install)

find_package(xtcore QUIET)
if(NOT xtcore_FOUND)
    FetchContent_Declare(
        xtcore
        GIT_REPOSITORY git@github.com:MQCore/xt-core.git
        GIT_TAG dev
    )
    FetchContent_MakeAvailable(xtcore)
endif()


#### Dependencies ####
find_package(googlebenchmark QUIET)
if(NOT googlebenchmark_FOUND)
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG origin/main
)
    FetchContent_MakeAvailable(googlebenchmark)
endif()

target_include_directories(benchmark_shm PUBLIC
    $<BUILD_INTERFACE:${xtcore_SOURCE_DIR}/include>
)

target_link_libraries(benchmark_shm PRIVATE
    benchmark::benchmark
)




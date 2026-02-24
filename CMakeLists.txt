cmake_minimum_required(VERSION 4.1)
project(trading_ecosystem)

set(CMAKE_CXX_STANDARD 23)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2 /Oi /Ot)
    else()
        add_compile_options(-O3 -march=native -flto)
    endif()
endif()


add_executable(trading_engine main.cpp)

target_include_directories(trading_engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/hft-matching-engine
        ${CMAKE_CURRENT_SOURCE_DIR}/market-connectivity
)

add_executable(unit_tests tests/unit_tests.cpp)

target_include_directories(unit_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/hft-matching-engine
        ${CMAKE_CURRENT_SOURCE_DIR}/market-connectivity
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

add_executable(bench_engine bench/benchmark.cpp
)

target_include_directories(bench_engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/hft-matching-engine
        ${CMAKE_CURRENT_SOURCE_DIR}/market-connectivity
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
)
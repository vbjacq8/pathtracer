cmake_minimum_required(VERSION 3.20)
project(pathtracer_unit_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PATHTRACER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src")

find_package(GTest REQUIRED)

add_executable(pathtracer_unit_tests
    vec3_test.cpp
    hit_test.cpp
)

target_include_directories(pathtracer_unit_tests PRIVATE
    "${PATHTRACER_SRC_DIR}"
)

target_link_libraries(pathtracer_unit_tests PRIVATE
    GTest::gtest_main
)

include(GoogleTest)
# Cross-compiling discovery can miss tests on some hosts; run the binary directly in CI scripts.
gtest_discover_tests(pathtracer_unit_tests
    DISCOVERY_MODE PRE_TEST
)

cmake_minimum_required(VERSION 3.18)
project(pathtracer_cuda LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_ARCHITECTURES 80 CACHE STRING "CUDA arch")

# External package (like a prebuilt MathUtils from elsewhere).
find_package(mathdx REQUIRED COMPONENTS curanddx CONFIG
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../../../nvidia-mathdx-26.06.1-cuda13/nvidia/mathdx/26.06")

# 1. Our library target (header-only → INTERFACE, no .cu/.cpp to compile).
add_library(Pathtracer INTERFACE)

# 2. Headers for this library (+ pull in MathDx when something links Pathtracer).
target_include_directories(Pathtracer INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/host"
    "${CMAKE_CURRENT_SOURCE_DIR}/device"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src")
target_link_libraries(Pathtracer INTERFACE mathdx::curanddx)
target_compile_definitions(Pathtracer INTERFACE PATHTRACER_CUDA_RNG=1)

# 3–4. Executables that link our library (same idea as MyApp → MathUtils).
foreach(demo IN ITEMS red_sphere shaded_sphere two_spheres)
    add_executable(${demo} "${CMAKE_CURRENT_SOURCE_DIR}/../test/cu/${demo}.cu")
    target_link_libraries(${demo} PRIVATE Pathtracer)
    set_target_properties(${demo} PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endforeach()

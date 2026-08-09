cmake_minimum_required(VERSION 3.18)
project(pathtracer_cuda LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_ARCHITECTURES 80 CACHE STRING "CUDA arch")

# MathDx (cuRANDDx). Prefer -Dmathdx_ROOT=... / env; else sibling unpack next to the repo.
get_filename_component(_mathdx_sib
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../nvidia-mathdx-26.06.1-cuda13/nvidia/mathdx/26.06"
    ABSOLUTE)
if(NOT mathdx_ROOT AND DEFINED ENV{mathdx_ROOT})
    set(mathdx_ROOT "$ENV{mathdx_ROOT}")
endif()
if(NOT mathdx_ROOT AND EXISTS "${_mathdx_sib}/include")
    set(mathdx_ROOT "${_mathdx_sib}")
endif()
unset(_mathdx_sib)
if(NOT mathdx_ROOT)
    message(FATAL_ERROR
        "MathDx not found. Unpack nvidia-mathdx-*-cuda13 next to this repo, or:\n"
        "  cmake -Dmathdx_ROOT=/path/to/nvidia/mathdx/26.06")
endif()
find_package(mathdx REQUIRED COMPONENTS curanddx CONFIG PATHS "${mathdx_ROOT}")

# 1. Our library target (header-only → INTERFACE, no .cu/.cpp to compile).
add_library(Pathtracer INTERFACE)

# 2. Headers for this library (+ pull in MathDx when something links Pathtracer).
target_include_directories(Pathtracer INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/host"
    "${CMAKE_CURRENT_SOURCE_DIR}/device"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src")
target_link_libraries(Pathtracer INTERFACE mathdx::curanddx)

# 3–4. Executables that link our library.
foreach(demo IN ITEMS red_sphere shaded_sphere two_spheres)
    add_executable(${demo} "${CMAKE_CURRENT_SOURCE_DIR}/../test/cu/${demo}.cu")
    target_link_libraries(${demo} PRIVATE Pathtracer)
    set_target_properties(${demo} PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endforeach()

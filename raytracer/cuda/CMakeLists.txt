cmake_minimum_required(VERSION 3.18)
project(pathtracer_cuda LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# Longleaf GPUs vary (V100/A100/L40/…). Override at configure time if needed:
#   cmake -S . -B build -DCMAKE_CUDA_ARCHITECTURES=80
if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES OR CMAKE_CUDA_ARCHITECTURES STREQUAL "")
    set(CMAKE_CUDA_ARCHITECTURES "70;80;90" CACHE STRING "CUDA arches to build for" FORCE)
endif()

add_executable(cuda_smoke
    host/main.cu
    device/render_kernel.cu
)

target_include_directories(cuda_smoke PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/host"
    "${CMAKE_CURRENT_SOURCE_DIR}/device"
)

set_target_properties(cuda_smoke PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Useful warnings without being noisy on device code.
target_compile_options(cuda_smoke PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra>
    $<$<COMPILE_LANGUAGE:CUDA>:-Xcompiler=-Wall,-Wextra>
)

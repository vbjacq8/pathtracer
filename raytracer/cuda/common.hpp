#pragma once

// SM arch for cuRANDDx `SM<Arch>()`. Device passes use `__CUDA_ARCH__`;
// host / IntelliSense fall back to PATHTRACER_CUDA_ARCH (override via -D).
#ifndef PATHTRACER_CUDA_ARCH
#    if defined(__CUDA_ARCH__)
#        define PATHTRACER_CUDA_ARCH __CUDA_ARCH__
#    else
#        define PATHTRACER_CUDA_ARCH 700
#    endif
#endif

constexpr unsigned int Arch = PATHTRACER_CUDA_ARCH;

#pragma once

#include "hitable.h"
#include "parse.h"

#include <memory>

/**
 * \brief Batch renderer entry point: parses options, renders, and writes PPM to stdout.
 * \param world scene to trace (kept alive for the duration of the render)
 * \param background miss-ray radiance strategy (default sky gradient)
 * \returns process exit code
 */
int render(int argc, char** argv, HitablePtr world,
           BackgroundFn background = colorBlueWhiteGradient);

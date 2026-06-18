#pragma once

#include "hitable.h"
#include "parse.h"

/**
 * \brief Batch renderer entry point: parses options, renders, and writes PPM to stdout.
 * \param world scene to trace (caller retains ownership)
 * \returns process exit code
 */
int render(int argc, char** argv, Hitable* world);
